#include "IFAEPlotter/PlotUtils.h"
#include "IFAEPlotter/StyleDictionary.h"
#include "IFAEPlotter/Plotter_Options.h"
#include "IFAETopFramework/HistManager.h"

#include "THStack.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TLegend.h"
#include "TPaveText.h"
#include "TLegendEntry.h"
//#include "TIter.h"
#include "TLatex.h"
#include "TLine.h"
#include <algorithm>

PlotUtils::PlotUtils(Plotter_Options* opt, HistManager* hstMngr, SampleAttributesMap& attrbt_map, VariableAttributesMap& var_map, StyleDictionary* style_dict ) : 
  m_opt(opt),
  m_hstMngr(hstMngr),
  m_attrbt_map(attrbt_map),
  m_var_map(var_map),
  m_style_dict(style_dict),
  m_outfile(NULL)
{ 

  if(m_opt->MsgLevel() == Debug::DEBUG) std::cout<<"PlotUtils::PlotUtils start"<<std::endl; 
  m_var_map_proj.clear(); 

  if( m_opt->DoProjections() && m_opt->ProjOpt().find("HIST") ){

    for( VariableAttributesMap::iterator varit = m_var_map.begin(); varit != m_var_map.end(); ++varit){
      for(int i = 1; i <= varit->second->NProjBin(); i++){
	
	VariableAttributes* vartemp = new VariableAttributes( *(varit->second) );
	std::string tempname = Form("%s_%i", varit->second->Name().c_str(), i);
	vartemp->SetName(tempname); 
	m_var_map_proj[tempname] = vartemp;
	tempname.clear();
      }
    }
  }

  m_drawSum = (m_attrbt_map.find("SUM") != m_attrbt_map.end());
  m_drawBlinder = (m_attrbt_map.find("BLINDER") != m_attrbt_map.end());

  if(m_opt->OutputFormat().find("ROOT") != std::string::npos){
    m_outfile = TFile::Open(Form("%s%s.root", m_opt->OutputFolder().c_str(), m_opt->OutputFile().c_str()), "RECREATE");
  }

  if(m_opt->MsgLevel() == Debug::DEBUG) std::cout<<"PlotUtils::PlotUtils end"<<std::endl; 

}

PlotUtils::~PlotUtils(){

  for(VariableAttributesMap::iterator varit = m_var_map_proj.begin(); varit != m_var_map_proj.end(); ++varit){
    delete varit->second;
    m_var_map_proj.erase(varit);
  }

  delete m_outfile;
}

void PlotUtils::Terminate(){
  if(m_outfile) m_outfile->Close();
}


void PlotUtils::OverlayHists(const std::string& projopt){

  if(m_opt->MsgLevel() == Debug::DEBUG) std::cout<<"PlotUtils::OverlayHists start"<<std::endl; 

  bool doGraphs = ((projopt == "MEAN") || (projopt == "RMS") || (projopt == "EFF") );
  const std::string& glob_ttl = m_opt->Title();
  // int nsample = m_attrbt_map.size();
  //int ndist = m_var_map.size();

  bool opt_hasResMin = (m_opt->OptStr().find("--RESMIN") != std::string::npos); 
  bool opt_hasResMax = (m_opt->OptStr().find("--RESMAX") != std::string::npos);
  bool opt_hasYMin   = (m_opt->OptStr().find("--YMIN") != std::string::npos);
  bool opt_hasYMax   = (m_opt->OptStr().find("--YMAX") != std::string::npos);
  bool opt_hasXMin   = (m_opt->OptStr().find("--XMIN") != std::string::npos);
  bool opt_hasXMax   = (m_opt->OptStr().find("--XMAX") != std::string::npos);
  
  bool var_draw_stack = 0;
  bool var_isLogY = false;
  bool var_isLogX = false;
  bool var_isShape = false;
  //bool var_do_width = false;
  bool var_hasResMin = false;
  bool var_hasResMax = false;
  bool var_hasYMin = false;
  bool var_hasYMax = false;
  bool var_hasXMin = false;
  bool var_hasXMax = false;
  bool var_modXRange = false;
  //bool var_modYRange = false;
  int var_nbinx = 0;
  double var_xmin = 0.;
  double var_xmax = 0.;

  double var_ymin = 0.;
  double var_ymax = 0.;

  bool ds_draw_stack = false;
  int ds_res_opt = -1;
  std::string ds_res_erropt = "";

  //One preliminary loop to find the baseline sample
  std::string s_base_name = ""; 
  std::string s_base_suffix = "";
  for(SampleAttributesMap::iterator at_it = m_attrbt_map.begin(); at_it != m_attrbt_map.end(); ++at_it){
    if(at_it->second->ResOpt() == 1){
      if(s_base_name != ""){ std::cout<<"Error: More than one baseline specified for residual calculation"<<std::endl; }
      else{ 
	s_base_name = at_it->first;
	s_base_suffix = at_it->second->Suffix();
      }
    }
  } 

  std::string legopt = "";
  std::vector<TH1D*> v_hstack_a;
  v_hstack_a.clear();
  VariableAttributesMap* var_loop_map = (projopt == "HIST") ? &m_var_map_proj : &m_var_map;
  for(VariableAttributesMap::iterator va_it = var_loop_map->begin(); va_it != var_loop_map->end(); ++va_it){

    const std::string& var_blinding = (va_it->second->Blinding() != "") ?  va_it->second->Blinding() : m_opt->Blinding();
    bool var_drawBlinder = m_drawBlinder && (var_blinding.find("BIN") != std::string::npos);
    bool var_blind_yield = (var_blinding.find("YIELD") != std::string::npos);
    v_hstack_a.clear();
    std::string var_name       = va_it->second->Name();
    if(projopt == "MEAN"){var_name += "_MEAN";}
    else if(projopt == "RMS"){var_name += "_RMS";}
    else if(projopt == "EFF"){var_name += "_EFF";}
 
    const std::string& var_label         = va_it->second->Label();
    const std::string& var_draw_res      = va_it->second->DrawRes();
    const std::string& var_draw_res_err  = va_it->second->DrawResErr();
    const std::string& var_ylabel        = (va_it->second->YLabel() != "") ? va_it->second->YLabel() : m_opt->YLabel();
    const std::string& var_reslabel      = (va_it->second->ResLabel() != "") ? va_it->second->ResLabel() : m_opt->ResLabel();
    const std::string& var_resdrawopt    = (va_it->second->ResDrawOpt() != "") ? va_it->second->ResDrawOpt() : m_opt->ResDrawOpt();
    const std::string& var_extralabel    = va_it->second->ExtraLabel();

    var_isShape        = !doGraphs && (va_it->second->DoScale() == "SHAPE"); 
    //var_do_width       = !doGraphs && va_it->second->DoWidth();
    var_draw_stack     = !doGraphs && va_it->second->DrawStack();
    var_isLogY         = !doGraphs && va_it->second->IsLogY();
    var_isLogX         = !doGraphs && va_it->second->IsLogX();

    var_hasResMin      = va_it->second->HasResMin();
    var_hasResMax      = va_it->second->HasResMax();
    var_hasYMin        = va_it->second->HasYMin();
    var_hasYMax        = va_it->second->HasYMax();
    var_hasXMin        = va_it->second->HasXMin();
    var_hasXMax        = va_it->second->HasXMax();

    var_modXRange      = var_hasXMin || var_hasXMax || opt_hasXMin || opt_hasXMax;
    //var_modYRange      = var_hasYMin || var_hasYMax || opt_hasYMin || opt_hasYMax;

    var_xmin = 0.; 
    var_xmax = 0.; 
    var_ymin = 0.; 
    var_ymax = 0.; 

    var_nbinx = 0; 

    std::string hname_sum = m_drawSum ? var_name + "_" + m_attrbt_map["SUM"]->Suffix() : "";
    std::string hname_blinder = m_drawBlinder ? var_name + "_" + m_attrbt_map["BLINDER"]->Suffix() : "";
    bool drawRes = ( (var_draw_res != "") && (var_draw_res != "NONE") );
    if(drawRes && (s_base_name == "") ){std::cout<<"No reference sample specified for residual calculation"<<std::endl;}
    //NOTE:
    //Eventually, TCanvas and TLegend properties will be defined in the StyleDictionary
    //Make a TCanvas and set its attributes
    std::string canv_name = "canv_" + var_name;
    std::string hs_stack_name = "hs_stack_" + var_name;
    std::string hs_nostack_name = "hs_nostack_" + var_name;
    std::string hs_res_name = "hs_res_" + var_name;
    //std::string hs_res_ref_name = "hs_res_ref_" + var_name;

    TCanvas* canv_a = new TCanvas(canv_name.c_str(), "", 800, 800);
    SetStyleCanvas( *canv_a, drawRes );

    THStack* hs_res_a = drawRes ? new THStack(hs_res_name.c_str(), "") : NULL;
    //THStack* hs_res_ref_a = drawRes ? new THStack(hs_res_ref_name.c_str(), "") : NULL;
    std::string hbasename = var_name + "_" + s_base_suffix;
    //TH1D* h_base = drawRes ? m_hstMngr->GetTH1D(hbasename) : NULL;

    THStack* hs_nostack_a = new THStack(hs_nostack_name.c_str(), "");
    THStack* hs_stack_a = var_draw_stack ? new THStack(hs_stack_name.c_str(), "") : NULL;

    TLegend* leg_a = new TLegend();
    TLegend* leg_yield = (m_opt->ShowYields() && (!var_isShape || var_draw_stack) ) ? new TLegend() : NULL;

    double textsize=0.03;
    if(drawRes){textsize=0.03;}
    SetStyleLegend(*leg_a, textsize);
    leg_a->Clear();
    if(leg_yield){
      SetStyleLegend(*leg_yield, textsize, 42, 0.05);
      leg_yield->Clear();
    }

    TPaveText* ttlbox = NULL;
    if( (glob_ttl != "") || (var_extralabel != "") ){
      double ttl_xmin = 0.; double ttl_ymin = 0.; double ttl_xmax = 0.; double ttl_ymax = 0.;
      if(va_it->second->HasTitleXMin()){ ttl_xmin = va_it->second->TitleXMin(); }
      else if(m_opt->OptStr().find("TITLEXMIN") != std::string::npos){ ttl_xmin = m_opt->TitleXMin(); }
      else{ std::cout<<"Warning: No min. x coordinate give for title box"<<std::endl; } 

      if(va_it->second->HasTitleXMax()){ ttl_xmax = va_it->second->TitleXMax(); }
      else if(m_opt->OptStr().find("TITLEXMAX") != std::string::npos){ ttl_xmax = m_opt->TitleXMax(); }
      else{ std::cout<<"Warning: No max. x coordinate give for title box"<<std::endl; } 

      if(va_it->second->HasTitleYMin()){ ttl_ymin = va_it->second->TitleYMin(); }
      else if(m_opt->OptStr().find("TITLEYMIN") != std::string::npos){ ttl_ymin = m_opt->TitleYMin(); }
      else{ std::cout<<"Warning: No min. y coordinate give for title box"<<std::endl; } 

      if(va_it->second->HasTitleYMax()){ ttl_ymax = va_it->second->TitleYMax(); }
      else if(m_opt->OptStr().find("TITLEYMAX") != std::string::npos){ ttl_ymax = m_opt->TitleYMax(); }
      else{ std::cout<<"Warning: No max. y coordinate give for title box"<<std::endl; } 


      ttlbox = new TPaveText(ttl_xmin, ttl_ymin, ttl_xmax, ttl_ymax, "NBNDC");
      ttlbox->SetFillColor(0);
      ttlbox->SetFillStyle(0);
      ttlbox->SetLineColor(0);
      ttlbox->SetTextSize(0.03);
      ttlbox->SetTextFont(42);
      ttlbox->SetShadowColor(0);

      if(glob_ttl != ""){ ttlbox->AddText(glob_ttl.c_str()); }
      if(var_extralabel != ""){ ttlbox->AddText(var_extralabel.c_str());}
    }

    bool firstsample = true;
    for(SampleAttributesMap::iterator at_it = m_attrbt_map.begin(); at_it != m_attrbt_map.end(); ++at_it){
      if( var_isShape ){
	if( !var_draw_stack && ( (at_it->first == "SUM")  || at_it->second->NoShape() ) ){continue;}
      }

      //if( (m_samples_noshape != NULL) && var_isShape && (std::find(m_samples_noshape->begin(), m_samples_noshape->end(), at_it->first) != m_samples_noshape->end()) ){continue;}
      if(at_it->first == "BLINDER" && !var_drawBlinder){continue;}

      const std::string& ds_name = at_it->second->Name();
      const std::string& ds_suffix = at_it->second->Suffix();
      const std::string& ds_leglabel = at_it->second->LegLabel();
      const std::string& ds_stylekey = at_it->second->StyleKey();
      const std::string& ds_drawopt = at_it->second->DrawOpt();
      const std::string& ds_resdrawopt = at_it->second->ResDrawOpt();
      const std::string& ds_legopt =  at_it->second->LegOpt();
      const std::string& ds_yield_format = (at_it->second->YieldFormat() != "") ? at_it->second->YieldFormat() : m_opt->YieldFormat();

      ds_res_erropt = "";
      if( var_draw_res_err  == "REFBAND" ){ ds_res_erropt = "SCALE"; } //scale both REF and INC sample error by the factor used for the division
      else if( at_it->second->ResOpt() == 1 ){ ds_res_erropt = "ZERO"; } 

      bool ds_isShape = !doGraphs && (at_it->second->DrawScale() == "SHAPE"); 
      ds_draw_stack = !doGraphs && at_it->second->DrawStack();
      ds_res_opt = at_it->second->ResOpt();
      bool ds_isBlind = (ds_name == m_opt->BlindSample());
      std::string hist_name = var_name + "_" + ds_suffix;
      SetStyleHist(hist_name, ds_stylekey);
      TH1D* hist_a = m_hstMngr->GetTH1D(hist_name);

      if(firstsample){
	var_nbinx = hist_a->GetNbinsX();
	if(var_hasXMin){ var_xmin = va_it->second->XMin(); }
	else if(opt_hasXMin){ var_xmin = m_opt->XMin(); }
	else{ var_xmin = hist_a->GetXaxis()->GetBinLowEdge(1); }
	
	if(var_hasXMax){ var_xmax = va_it->second->XMax(); }
	else if(opt_hasXMax){ var_xmax = m_opt->XMax(); }
	else{ var_xmax = hist_a->GetXaxis()->GetBinUpEdge(var_nbinx); }
      }
      if(var_modXRange){
	hist_a->SetAxisRange(var_xmin, var_xmax);
      }

      if(ds_leglabel != ""){
	if(ds_legopt != ""){
	  leg_a->AddEntry(hist_a, ds_leglabel.c_str(), ds_legopt.c_str());
	}
	else if(m_opt->LegOpt() != ""){
	  leg_a->AddEntry(hist_a, ds_leglabel.c_str(), m_opt->LegOpt().c_str());
	}
	else{
	  legopt = "l";
	  if( (ds_drawopt.find("hist") != std::string::npos) || (ds_drawopt.find("HIST") != std::string::npos) 
	      || (ds_drawopt.find("e2") != std::string::npos) || (ds_drawopt.find("E2") != std::string::npos)  ){ legopt += "f";}
	  if( (ds_drawopt.find("e") != std::string::npos) || (ds_drawopt.find("E") != std::string::npos) ){ legopt += "p"; }
	  leg_a->AddEntry(hist_a, ds_leglabel.c_str(), legopt.c_str());
	  legopt.clear();
	}

	if(!doGraphs && leg_yield){ 
	  if( !(ds_isShape || (var_blind_yield && ds_isBlind)) ){leg_yield->AddEntry(hist_a, Form(ds_yield_format.c_str(),hist_a->Integral()), ""); }
	  else{ leg_yield->AddEntry(hist_a, " ", ""); }
	}

      }//if leglabel was not provided, then clearly the sample is not meant to be added to the legend

      if(at_it->first != "BLINDER"){
	if(var_draw_stack && ds_draw_stack){ 
	  v_hstack_a.push_back(hist_a);
	}
	else{ 
	  hs_nostack_a->Add(hist_a, ds_drawopt.c_str()); 
	}//if this sample is not be be stacked

	if(drawRes && ( (ds_res_opt == 0) || ((ds_res_opt == 1) && (var_draw_res_err == "REFBAND")) ) ){
	  std::string resname_a = var_name + "_" + ds_suffix + "_res_" + s_base_suffix;
	  TH1D* hist_res_a = makeResidual(resname_a, hist_name, hbasename, var_draw_res, ds_res_erropt);
	  string resdrawopt = ""; 
	  if(ds_resdrawopt != ""){ resdrawopt = ds_resdrawopt; }
	  else if( (ds_res_opt != 1) && (var_resdrawopt != "") ){ resdrawopt = var_resdrawopt; }
	  else if( (ds_res_opt == 1) && (var_draw_res_err == "REFBAND") ){ resdrawopt = "e2"; }
	    //if(var_draw_res_err == "REFBAND"){ resdrawopt = "e2";}
	    //else{ resdrawopt = "E0"; hist_res_a->SetFillStyle(0); }
	    //}
	  else{ resdrawopt = ds_drawopt; }
	  hs_res_a->Add(hist_res_a, resdrawopt.c_str());

	  resname_a.clear(); resdrawopt.clear();
	}//if residual histogram needed 

      }//if not a BLINDER

      //Clear strings
      hist_name.clear(); firstsample = false;
    }//sample loop

    if(hs_stack_a && v_hstack_a.size() > 0){

      for(int ro = v_hstack_a.size()-1; ro >= 0; ro--){
	hs_stack_a->Add(v_hstack_a.at(ro), "hist");
      }

    }


    //Draw the THStack in the top panel, set the x and y axis labels
    var_ymin = 0.; var_ymax = 0.;
    double stretch_min = 1.; double stretch_max = 1.;

    if(var_hasYMin){var_ymin = va_it->second->YMin();}
    else if(opt_hasYMin){var_ymin = m_opt->YMin();}
    else{ stretch_min = var_isLogY ? 1.E-2 : 0.5; }

    if(var_hasYMax){var_ymax = va_it->second->YMax();}
    else if(opt_hasYMax){var_ymax = m_opt->YMax();}
    else{ stretch_max = var_isLogY ? 1.E3 : 1.35; }

    TPad* curpad = (TPad*)(canv_a->cd(1));
    if(var_isLogY){curpad->SetLogy();}
    if(var_isLogX){curpad->SetLogx();}

    if(var_draw_stack && (hs_stack_a->GetNhists() > 0)){

      hs_stack_a->Draw();
      if(hs_nostack_a->GetNhists() > 0){hs_nostack_a->Draw("samenostack");}

      if(var_modXRange){ hs_stack_a->GetXaxis()->SetRangeUser(var_xmin, var_xmax); }

      if(!var_hasYMax && !opt_hasYMax){
	var_ymax = hs_stack_a->GetHistogram()->GetMaximum();
	if(hs_nostack_a->GetNhists() > 0){
	  var_ymax = max(var_ymax, hs_nostack_a->GetHistogram()->GetMaximum());
	}
	var_ymax *= stretch_max;
      }
      if(!var_hasYMin && !opt_hasYMin){
	var_ymin = hs_stack_a->GetHistogram()->GetMinimum();
	if(hs_nostack_a->GetNhists() > 0){
	  var_ymin = min(var_ymin, hs_nostack_a->GetHistogram()->GetMinimum());
	}
	var_ymin *= stretch_min;
      }
      if(!var_isLogY && var_ymin <= 1.e-5){var_ymin = 1.1e-5;}
      else if(var_isLogY && var_ymin <= 1.e-10){ var_ymin = 1.e-10; }
      hs_stack_a->SetMinimum(var_ymin);
      hs_stack_a->SetMaximum(var_ymax);

      if(var_ylabel != ""){ hs_stack_a->GetHistogram()->GetYaxis()->SetTitle(var_ylabel.c_str()); }
      else{ hs_stack_a->GetHistogram()->GetYaxis()->SetTitle( ((TH1D*)(hs_stack_a->GetStack()->First()))->GetYaxis()->GetTitle() ) ; }

      double yoff = 0.;
      double yttlsz = drawRes ? 0.05 : 0.04;
      double ylblsz = drawRes ? 0.05 : 0.04;

      int ndig = 0.;
      if( var_isLogY ){ndig = 2;}
      else if(var_ymin < 0.0001 || var_ymax > 100000.){ndig = 6; }
      else if(var_ymin < 0.01 || var_ymax > 1000.){ndig = 4; }
      else {ndig = 3; }

      if( ndig<=3 ){ yoff = 0.7; }
      else if( ndig>3 && ndig<6 ){ yoff = 1.0; }
      else if( ndig==6 ){ yoff = 1.2; }
      else{ yoff = 1.5; }


      hs_stack_a->GetHistogram()->GetYaxis()->SetTitleOffset(yoff);
      hs_stack_a->GetHistogram()->GetYaxis()->SetTitleSize(yttlsz);
      hs_stack_a->GetHistogram()->GetYaxis()->SetLabelSize(ylblsz);

      if(!drawRes){      
	if(var_label != ""){ hs_stack_a->GetHistogram()->GetXaxis()->SetTitle(var_label.c_str()); }
	else{ hs_stack_a->GetHistogram()->GetXaxis()->SetTitle( ((TH1D*)(hs_stack_a->GetStack()->First()))->GetXaxis()->GetTitle() ) ; }

	hs_stack_a->GetHistogram()->GetXaxis()->SetTitleOffset(1.);
	hs_stack_a->GetHistogram()->GetXaxis()->SetTitleSize(0.04);
	hs_stack_a->GetHistogram()->GetXaxis()->SetLabelSize(0.04);

      }

    }//if var_draw_stack
    else{
      hs_nostack_a->Draw("nostack"); 
      if(var_modXRange){ hs_nostack_a->GetXaxis()->SetRangeUser(var_xmin, var_xmax); }


      if(!var_hasYMax && !opt_hasYMax){
	var_ymax = hs_nostack_a->GetHistogram()->GetMaximum();
	var_ymax *= stretch_max;
      }
      if(!var_hasYMin && !opt_hasYMin){
	var_ymin = hs_nostack_a->GetHistogram()->GetMinimum();
	var_ymin *= stretch_min;
      }
      if(var_isLogY && var_ymin <= 1.e-10){ var_ymin = 1.e-10; }

      hs_nostack_a->SetMinimum(var_ymin);
      hs_nostack_a->SetMaximum(var_ymax);

      if(var_ylabel != ""){ hs_nostack_a->GetHistogram()->GetYaxis()->SetTitle(var_ylabel.c_str()); }
      else{ hs_nostack_a->GetHistogram()->GetYaxis()->SetTitle( ((TH1D*)(hs_nostack_a->GetStack()->First()))->GetYaxis()->GetTitle() ) ; }

      double yoff = 0.;
      double yttlsz = drawRes ? 0.05 : 0.04;
      double ylblsz = drawRes ? 0.05 : 0.04;

      int ndig = 0.;
      if( var_isLogY ){ndig = 2;}
      else if(var_ymin < 0.0001 || var_ymax > 100000.){ndig = 6; }
      else if(var_ymin < 0.01 || var_ymax > 1000.){ndig = 4; }
      else {ndig = 3; }

      if( ndig<=3 ){ yoff = 0.7; }
      else if( ndig>3 && ndig<6 ){ yoff = 1.0; }
      else if( ndig==6 ){ yoff = 1.2; }
      else{ yoff = 1.5; }

      hs_nostack_a->GetHistogram()->GetYaxis()->SetTitleOffset(yoff);
      hs_nostack_a->GetHistogram()->GetYaxis()->SetTitleSize(yttlsz);
      hs_nostack_a->GetHistogram()->GetYaxis()->SetLabelSize(ylblsz);


      if(!drawRes){      
	if(var_label != ""){ hs_nostack_a->GetHistogram()->GetXaxis()->SetTitle(var_label.c_str()); }
	else{ hs_nostack_a->GetHistogram()->GetXaxis()->SetTitle( ((TH1D*)(hs_nostack_a->GetStack()->First()))->GetXaxis()->GetTitle() ) ; }

	hs_nostack_a->GetHistogram()->GetXaxis()->SetTitleOffset(1.);
	hs_nostack_a->GetHistogram()->GetXaxis()->SetTitleSize(0.04);
	hs_nostack_a->GetHistogram()->GetXaxis()->SetLabelSize(0.04);

      }

    }//if !var_draw_stack

    if(var_drawBlinder){
      TH1D* h_blinder = m_hstMngr->GetTH1D(hname_blinder);
      for(int b = 1; b <= h_blinder->GetNbinsX(); b++){
	if(h_blinder->GetBinContent(b) > 0){h_blinder->SetBinContent(b, var_ymax);}
      }
      std::string blinder_drawopt = "same" + m_attrbt_map["BLINDER"]->DrawOpt();
      //canv_a->cd(1);
      h_blinder->Draw(blinder_drawopt.c_str());
      blinder_drawopt.clear();
    }

    if(leg_yield){
      ResizeLegend(*leg_yield, 0.89, 0.89);
      ResizeLegend(*leg_a, leg_yield->GetX1NDC(), leg_yield->GetY2NDC() );
    }
    else{ ResizeLegend(*leg_a, 0.89, 0.89 );}
    leg_a->DrawClone();
    if(leg_yield){ leg_yield->DrawClone(); }
    if(ttlbox){ ttlbox->Draw(); } 
    curpad->RedrawAxis();
    curpad->Update();
    curpad->Modified();

    if(drawRes){
      curpad = (TPad*)(canv_a->cd(2));
      if(var_isLogX){curpad->SetLogx();}

      //double r_min = 0.9*hs_res_a->GetHistogram()->GetMinimum();
      //double r_max = 1.1*hs_res_a->GetHistogram()->GetMaximum();

      double r_min = 0.; double r_max = 0.;
      if(var_hasResMin){r_min = va_it->second->ResMin();}
      else if(opt_hasResMin){r_min = m_opt->ResMin();}
      else if(var_draw_res == "RESIDUAL"){ r_min = -1.;}
      else if(var_draw_res == "FRACRES"){ r_min = -1.;}
      else if(var_draw_res == "RATIO"){ r_min = 0.5;}
      else if(var_draw_res == "INVRATIO"){ r_min = 0.5;}

      if(var_hasResMax){r_max = va_it->second->ResMax();}
      else if(opt_hasResMax){r_max = m_opt->ResMax();}
      else if(var_draw_res == "RESIDUAL"){ r_max = 1.;}
      else if(var_draw_res == "FRACRES"){ r_max = 1.;}
      else if(var_draw_res == "RATIO"){ r_max = 1.5;}
      else if(var_draw_res == "INVRATIO"){ r_max = 1.5;}

      hs_res_a->Draw("nostack");
      if(var_modXRange){ hs_res_a->GetXaxis()->SetRangeUser(var_xmin, var_xmax); }

      double ry = ( (var_draw_res == "RESIDUAL") || (var_draw_res == "DIFF") ) ? 0. : 1.;
      TLine* lnref = new TLine(var_xmin, ry, var_xmax, ry); // a reference line
      lnref->SetLineStyle(2); //dashed line

      if(var_label != ""){ hs_res_a->GetHistogram()->GetXaxis()->SetTitle(var_label.c_str()); }
      else{ hs_res_a->GetHistogram()->GetXaxis()->SetTitle( ((TH1D*)(hs_res_a->GetStack()->First()))->GetXaxis()->GetTitle() ) ; }


      if(var_reslabel != ""){ hs_res_a->GetHistogram()->GetYaxis()->SetTitle(var_reslabel.c_str()); }
      hs_res_a->SetMinimum(r_min);
      hs_res_a->SetMaximum(r_max);
      lnref->DrawClone("same");

      hs_res_a->GetHistogram()->GetXaxis()->SetTitleOffset(0.9);
      hs_res_a->GetHistogram()->GetYaxis()->SetTitleOffset(0.5);
      
      hs_res_a->GetHistogram()->GetXaxis()->SetTitleSize(0.09);
      hs_res_a->GetHistogram()->GetYaxis()->SetTitleSize(0.07);
 
      hs_res_a->GetHistogram()->GetXaxis()->SetLabelSize(0.09);
      hs_res_a->GetHistogram()->GetYaxis()->SetLabelSize(0.07);
      
      hs_res_a->GetHistogram()->GetYaxis()->CenterTitle();

      curpad->Update();
      curpad->Modified();

      //canv_a->cd(2)->Update();
      //canv_a->cd(2)->Modified();
      delete lnref;
    }

    //Write to output file/ print to a png
    if(m_opt->OutputFormat().find("PNG") != std::string::npos){ 
      canv_a->SaveAs(Form("%s/%s.png" ,m_opt->OutputFolder().c_str() ,canv_name.c_str())); 
      if(m_opt->MsgLevel() == Debug::DEBUG) std::cout<<"PlotUtils::OverlayHists printing "<<canv_name<<".png"<<std::endl;
    }
    if(m_opt->OutputFormat().find("EPS") != std::string::npos){ 
      canv_a->SaveAs(Form("%s/%s.eps" ,m_opt->OutputFolder().c_str() ,canv_name.c_str()));  
      if(m_opt->MsgLevel() == Debug::DEBUG) std::cout<<"PlotUtils::OverlayHists printing "<<canv_name<<".eps"<<std::endl;
   }
    if(m_opt->OutputFormat().find("PDF") != std::string::npos){
      canv_a->SaveAs(Form("%s/%s.pdf" ,m_opt->OutputFolder().c_str() ,canv_name.c_str()));
      if(m_opt->MsgLevel() == Debug::DEBUG) std::cout<<"PlotUtils::OverlayHists printing "<<canv_name<<".pdf"<<std::endl;
    }
    if(m_opt->OutputFormat().find("CPP") != std::string::npos){
      canv_a->SaveAs(Form("%s/%s.C" ,m_opt->OutputFolder().c_str() ,canv_name.c_str()));
      if(m_opt->MsgLevel() == Debug::DEBUG) std::cout<<"PlotUtils::OverlayHists writing "<<canv_name<<".C"<<std::endl;
    }
    if(m_opt->OutputFormat().find("ROOT") != std::string::npos){ 
      m_outfile->cd(); canv_a->Write();
      if(m_opt->MsgLevel() == Debug::DEBUG) std::cout<<"PlotUtils::OverlayHists writing "<<canv_name<<" into ROOT file"<<std::endl;
    }


    //Delete objects
    delete canv_a;
    delete hs_stack_a;
    delete hs_nostack_a;
    delete hs_res_a;
    delete ttlbox;
    curpad = NULL;
    //delete leg_a;
    //delete leg_yield;
    //Clear strings
    //var_name.clear();
    //var_label.clear();

    canv_name.clear();
    hs_stack_name.clear();
    hs_nostack_name.clear();
    hs_res_name.clear();
    hname_sum.clear();
    hbasename.clear();
  }//variables loop

  if(m_opt->MsgLevel() == Debug::DEBUG) std::cout<<"PlotUtils::OverlayHists end"<<std::endl; 

  return;

}

//TH1D* hist_res_a = makeResidual(resname_a, hist_name, hbasename, var_draw_res);
//erropt by default empty
//can change to "NONE" or "SCALE"
//so if one want the error on data to be zero, give "NONE" there.
//and if the error on MC sum should be scaled, give "SCALE"
//also change the drawing of the residuals so that REF is not skipped - this must be 
//the line that is drawn instead of an extra TLine

TH1D* PlotUtils::makeResidual(const std::string& resname, const std::string& s_hnum, const std::string& s_href
			      , const std::string& opt, const std::string& erropt){

  return makeResidual(resname, m_hstMngr->GetTH1D(s_hnum), m_hstMngr->GetTH1D(s_href), opt, erropt);

}

TH1D* PlotUtils::makeResidual(const std::string& resname, TH1D* hist, TH1D* href
			      , const std::string& opt, const std::string& erropt){

  bool b_selfdivide = (hist == href);

  if(opt == "NONE"){return NULL;}
  if( ! ( (opt == "RATIO") || (opt == "INVRATIO") || (opt == "DIFF") || (opt == "RESIDUAL") ) ){
    std::cout << " Error : Unknown option "<<opt<<" for residual calculation "<<std::endl;
    return NULL;
  }

  TH1D* hist_res = (TH1D*)(hist->Clone(resname.c_str()) );

  hist_res->SetStats(0);
  hist_res->SetName(resname.c_str());
  hist_res->SetTitle("");

  bool b_rootfn = true;
  bool b_def_err = (erropt == "");
  bool b_zero_err = (erropt == "ZERO");
  bool b_scale_err = (erropt == "SCALE");

  if( opt == "RATIO" ){ hist_res->Divide(hist_res, href); }
  else if(opt == "INVRATIO"){ hist_res->Divide(href, hist_res); }
  else if(opt == "DIFF"){ hist_res->Add(href, -1);}
  else{ b_rootfn = false; }

  if( b_selfdivide || !(b_rootfn && b_def_err) ){

    double res_bc = 0.; double res_be = 0.;
    double cont_c = 0.; double cont_e = 0.; double cont_fe = 0.;
    double ref_c = 0.; double ref_e = 0.; double ref_fe = 0.;

    for( int b = 1; b <=hist_res->GetNbinsX(); b++ ){
  
      cont_c = hist->GetBinContent(b);
      ref_c = href->GetBinContent(b);

      if(b_zero_err){ res_be = 0.; }
      else{
	cont_e = hist->GetBinError(b);
	ref_e = href->GetBinError(b);
      }

      if(opt == "RESIDUAL"){
	res_bc = (ref_c > 0.) ? (cont_c/ref_c - 1.) : 0.;

	if(b_def_err){
	  cont_fe = (cont_c > 0.) ? cont_e/cont_c : 0.;  
	  ref_fe = (ref_c > 0.) ? ref_e/cont_c : 0.;  
	  res_be = res_bc * sqrt(cont_fe*cont_fe + ref_fe*ref_fe); 
	}
	else if(b_scale_err){ res_be = (ref_c > 0.) ? cont_e/ref_c : 0.; }

      }
      else if( opt == "RATIO"){
	if(b_scale_err){ res_be = (ref_c > 0.) ? cont_e/ref_c : 0.; }
      }
      else if( opt == "DIFF"){
	if(b_scale_err){ res_be = cont_e; }
      }

      if( b_selfdivide ){
	if( (opt=="RATIO")|| (opt=="INVRATIO") || opt == ("RESIDUAL") ) { hist_res->SetBinContent(b, 1.); }
	else if(opt == "DIFF"){ hist_res->SetBinContent(b, 0.); }
      }
      else if(!(b_rootfn)){ hist_res->SetBinContent(b, res_bc); }  

      //if(!(b_rootfn || b_selfdivide) ){}
      //if(!b_selfdivide){ hist_res->SetBinContent(b, res_bc); }
      hist_res->SetBinError(b, res_be);

    }//bin loop

  }//not ROOT function or non-default error or self-division

  m_hstMngr->SetTH1D(resname, hist_res);
  return hist_res;

}

int PlotUtils::SetStyleCanvas(TCanvas& canv, bool divide){

  if(divide){canv.Divide(1,2);
    canv.cd(1)->SetPad(0.,0.35,0.95,0.95);
    canv.cd(1)->SetBorderMode(0);
    canv.cd(1)->SetTopMargin(0.05);
    canv.cd(1)->SetBottomMargin(0.0001);
    canv.cd(1)->SetRightMargin(0.05);
    canv.cd(1)->SetLeftMargin(0.15);

    canv.cd(2)->SetPad(0.,0.,0.95,0.35);
    canv.cd(2)->SetBorderMode(0);
    canv.cd(2)->SetTopMargin(0.0001);
    canv.cd(2)->SetBottomMargin(0.2);
    canv.cd(2)->SetRightMargin(0.05);
    canv.cd(2)->SetLeftMargin(0.15);
  }
  else{
    canv.cd()->SetTopMargin(0.05);
    canv.cd()->SetBottomMargin(0.2);
    canv.cd()->SetRightMargin(0.05);
    canv.cd()->SetLeftMargin(0.15);
  }

  return 0;
}

int PlotUtils::SetStyleHist(std::string hname, std::string style_key){

  TH1D* hist = m_hstMngr->GetTH1D(hname);
  if(hist == NULL){ std::cout<<"SetStyleHist : Histogram "<<hname<<" not found"<<std::endl; return -1;}
  hist->SetLineColor( m_style_dict->LineColour(style_key) );
  hist->SetLineStyle( m_style_dict->LineStyle(style_key) );
  hist->SetLineWidth( m_style_dict->LineWidth(style_key) );
  hist->SetFillColor( m_style_dict->FillColour(style_key) );
  hist->SetFillStyle( m_style_dict->FillStyle(style_key) );
  hist->SetMarkerColor( m_style_dict->MarkerColour(style_key) );
  hist->SetMarkerStyle( m_style_dict->MarkerStyle(style_key) );
  hist->SetMarkerSize( m_style_dict->MarkerSize(style_key) );

  return 0;
}

int PlotUtils::SetStyleLegend(TLegend &leg, double textsize, int textfont, double margin){
  leg.SetFillColor(kWhite);
  leg.SetFillStyle(1001);
  leg.SetLineColor(kWhite);
  leg.SetMargin(margin);
  leg.SetTextSize(textsize);
  leg.SetTextFont(textfont);
  leg.SetShadowColor(0);

  return 0;
}

int PlotUtils::ResizeLegend(TLegend& leg, double xpt, double ypt, const std::string& justify){

  float textsize = leg.GetTextSize();
  float nrows = (float)leg.GetNRows();
  float margin = leg.GetMargin();
  TIter liter(leg.GetListOfPrimitives());
  float maxlsize = 0.;
  int nrow = 0;
  TCanvas* canv_test = new TCanvas("canv_test", "canv_test", 800, 800); canv_test->cd();

  while( TLegendEntry* lentry = (TLegendEntry*)( liter() ) ){
    TLatex entrytex(0., 0., lentry->GetLabel());
    entrytex.SetNDC(kTRUE);
    entrytex.SetTextSize(textsize);
    entrytex.Draw();
    if( entrytex.GetXsize() > maxlsize ){maxlsize = entrytex.GetXsize();}
    nrow++;
  }
  delete canv_test;

  float X1 = 0., X2 = 0., Y1 = 0., Y2 = 0.;
  float delX = maxlsize*(1.+margin+0.1);//*textsize;
  float delY = nrows * textsize;

  if(justify == "r"){
    X2 = (xpt > 0.) ? xpt : 0.89; 
    Y2 = (ypt > 0.) ? ypt : 0.89;
    X1 = X2 - delX;
    Y1 = Y2 - delY;
  }
  else if(justify == "l"){
    X1 = (xpt > 0.) ? xpt : 0.01; 
    Y1 = (ypt > 0.) ? ypt : 0.01;
    X2 = X1 + delX; 
    Y2 = Y1 + delY;
  }
  else{std::cout<<"Error : Unknown option for legend justification"<<std::endl;}

  leg.SetX1NDC(X1);
  leg.SetX2NDC(X2);
  leg.SetY1NDC(Y1);
  leg.SetY2NDC(Y2);

  return 0;
}
