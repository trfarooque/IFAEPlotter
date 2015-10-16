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

PlotUtils::PlotUtils(Plotter_Options* opt, HistManager* hstMngr, SampleAttributesMap& attrbt_map, VariableAttributesMap& var_map, StyleDictionary* style_dict
		     , std::vector<std::string>* samples_noshape ) : 
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
  m_samples_noshape = samples_noshape;
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
  bool opt_hasYMin = (m_opt->OptStr().find("--YMIN") != std::string::npos);
  bool opt_hasYMax = (m_opt->OptStr().find("--YMAX") != std::string::npos);
  bool opt_hasXMin = (m_opt->OptStr().find("--XMIN") != std::string::npos);
  bool opt_hasXMax = (m_opt->OptStr().find("--XMAX") != std::string::npos);
  
  bool var_draw_stack = 0;
  bool var_isLog = false;
  bool var_isShape = false;
  bool var_do_width = false;
  bool var_hasResMin = false;
  bool var_hasResMax = false;
  bool var_hasYMin = false;
  bool var_hasYMax = false;
  bool var_hasXMin = false;
  bool var_hasXMax = false;

  bool ds_draw_stack = false;
  int ds_res_opt = -1;

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

    v_hstack_a.clear();
    std::string var_name       = va_it->second->Name();
    if(projopt == "MEAN"){var_name += "_MEAN";}
    else if(projopt == "RMS"){var_name += "_RMS";}
    else if(projopt == "EFF"){var_name += "_EFF";}
 
    const std::string& var_label      = va_it->second->Label();
    const std::string& var_draw_res   = va_it->second->DrawRes();
    const std::string& var_ylabel     = (va_it->second->YLabel() != "") ? va_it->second->YLabel() : m_opt->YLabel();
    const std::string& var_reslabel   = (va_it->second->ResLabel() != "") ? va_it->second->ResLabel() : m_opt->ResLabel();
    const std::string& var_resdrawopt = (va_it->second->ResDrawOpt() != "") ? va_it->second->ResDrawOpt() : m_opt->ResDrawOpt();
    const std::string& var_extralabel = va_it->second->ExtraLabel();

    var_isShape        = !doGraphs && (va_it->second->DoScale() == "SHAPE"); 
    var_do_width       = !doGraphs && va_it->second->DoWidth();
    var_draw_stack     = !doGraphs && va_it->second->DrawStack();
    var_isLog          = !doGraphs && va_it->second->IsLog();

    var_hasResMin      = va_it->second->HasResMin();
    var_hasResMax      = va_it->second->HasResMax();
    var_hasYMin        = va_it->second->HasYMin();
    var_hasYMax        = va_it->second->HasYMax();

    var_hasXMin        = va_it->second->HasXMin();
    var_hasXMax        = va_it->second->HasXMax();

    int nbinx = 0; 
    double xr_min = 0.; 
    double xr_max = 0.;

    std::string hname_sum = m_drawSum ? var_name + "_" + m_attrbt_map["SUM"]->Suffix() : "";

    bool drawRes = ( (var_draw_res != "") && (var_draw_res != "NONE") );
    if(drawRes && (s_base_name == "") ){std::cout<<"No reference sample specified for residual calculation"<<std::endl;}
    //NOTE:
    //Eventually, TCanvas and TLegend properties will be defined in the StyleDictionary
    //Make a TCanvas and set its attributes
    std::string canv_name = "canv_" + var_name;
    std::string hs_stack_name = "hs_stack_" + var_name;
    std::string hs_nostack_name = "hs_nostack_" + var_name;
    std::string hs_res_name = "hs_res_" + var_name;

    TCanvas* canv_a = new TCanvas(canv_name.c_str(), "", 800, 800);
    SetStyleCanvas( *canv_a, drawRes );

    THStack* hs_res_a = drawRes ? new THStack(hs_res_name.c_str(), "") : NULL;
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
      else{ std::cout<<"Warning: No min. y coordinate give for title boy"<<std::endl; } 

      if(va_it->second->HasTitleYMax()){ ttl_ymax = va_it->second->TitleYMax(); }
      else if(m_opt->OptStr().find("TITLEYMAX") != std::string::npos){ ttl_ymax = m_opt->TitleYMax(); }
      else{ std::cout<<"Warning: No max. y coordinate give for title boy"<<std::endl; } 


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
    /*
    v_label_stack.clear();
    v_drawopt_stack.clear();
    v_yield_stack.clear();

    v_label_nostack.clear();
    v_drawopt_nostack.clear();
    v_yield_nostack.clear();
    */
    for(SampleAttributesMap::iterator at_it = m_attrbt_map.begin(); at_it != m_attrbt_map.end(); ++at_it){

      if( var_isShape && !var_draw_stack && at_it->first == "SUM" ){continue;}
      if( (m_samples_noshape != NULL) && var_isShape && (std::find(m_samples_noshape->begin(), m_samples_noshape->end(), at_it->first) != m_samples_noshape->end()) ){continue;}

      const std::string& ds_name = at_it->second->Name();
      const std::string& ds_suffix = at_it->second->Suffix();
      const std::string& ds_leglabel = at_it->second->LegLabel();
      const std::string& ds_stylekey = at_it->second->StyleKey();
      const std::string& ds_drawopt = at_it->second->DrawOpt();
      const std::string& ds_legopt =  at_it->second->LegOpt();

      bool ds_isShape = !doGraphs && (at_it->second->DrawScale() == "SHAPE"); 
      ds_draw_stack = !doGraphs && at_it->second->DrawStack();
      ds_res_opt = at_it->second->ResOpt();

      std::string hist_name = var_name + "_" + ds_suffix;
      SetStyleHist(hist_name, ds_stylekey);
      TH1D* hist_a = m_hstMngr->GetTH1D(hist_name);

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
	if(!ds_isShape){leg_yield->AddEntry(hist_a, Form("%.4g",hist_a->Integral()), ""); }
	else{ leg_yield->AddEntry(hist_a, " ", ""); }
      }

      if(var_do_width){
	int nbin = hist_a->GetNbinsX();
	for(int nb = 0; nb <= nbin; nb++){
	  double bc = hist_a->GetBinContent(nb)/hist_a->GetXaxis()->GetBinWidth(nb);
	  double be = hist_a->GetBinError(nb)/hist_a->GetXaxis()->GetBinWidth(nb);
	  hist_a->SetBinContent(nb, bc);
	  hist_a->SetBinError(nb, be);
	}
      }

      if(var_draw_stack && ds_draw_stack){ 
	v_hstack_a.push_back(hist_a);
      }
      else{ 
	hs_nostack_a->Add(hist_a, ds_drawopt.c_str()); 

	if(drawRes && (ds_res_opt == 0) && (ds_name != "SUM") ){
	  std::string resname_a = var_name + "_" + ds_suffix + "_res_" + s_base_suffix;
	  TH1D* hist_res_a = makeResidual(resname_a, hist_name, hbasename, var_draw_res);
	  const std::string& resdrawopt = (var_resdrawopt != "") ? var_resdrawopt : ds_drawopt;
	  hs_res_a->Add(hist_res_a, resdrawopt.c_str());
	  resname_a.clear();
	} 

      }//if this sample is not be be stacked

      //Clear strings
      hist_name.clear();
    }//sample loop

    if(hs_stack_a && v_hstack_a.size() > 0){

      for(int ro = v_hstack_a.size()-1; ro >= 0; ro--){
	hs_stack_a->Add(v_hstack_a.at(ro), "hist");
      }

    }

    if( drawRes && var_draw_stack ){
      std::string resname_sum_a = var_name + "_sum_" + "_res_" + s_base_suffix;
      TH1D* hsum_res_a = makeResidual(resname_sum_a, hname_sum, hbasename, var_draw_res);
      hs_res_a->Add(hsum_res_a);
      resname_sum_a.clear();
    }
    //int size_res = hs_res_a ? hs_res_a->GetNhists() : 0;
    //std::cout<<" var_draw_stack = "<<var_draw_stack<<" drawRes = "<<drawRes<<" hs_res_a->size() = "<<size_res<<std::endl; 

    //Draw the THStack in the top panel, set the x and y axis labels
    double c_min = 0.; double c_max = 0.;
    double stretch_min = 1.; double stretch_max = 1.;

    if(var_hasYMin){c_min = va_it->second->YMin();}
    else if(opt_hasYMin){c_min = m_opt->YMin();}
    else{ stretch_min = var_isLog ? 1.E-2 : 0.5; }

    if(var_hasYMax){c_max = va_it->second->YMax();}
    else if(opt_hasYMax){c_max = m_opt->YMax();}
    else{ stretch_max = var_isLog ? 1.E3 : 1.3; }

    TPad* curpad = (TPad*)(canv_a->cd(1));
    if(var_isLog){curpad->SetLogy();}

    if(var_draw_stack){
      hs_stack_a->Draw();
      if(hs_nostack_a->GetNhists() > 0){
      	hs_nostack_a->Draw("samenostack");
      }

      if(!var_hasYMax && !opt_hasYMax){
	c_max = hs_stack_a->GetHistogram()->GetMaximum();
	if(hs_nostack_a->GetNhists() > 0){
	  c_max = max(c_max, hs_nostack_a->GetHistogram()->GetMaximum());
	}
	c_max *= stretch_max;
      }
      if(!var_hasYMin && !opt_hasYMin){
	c_min = hs_stack_a->GetHistogram()->GetMinimum();
	if(hs_nostack_a->GetNhists() > 0){
	  c_min = min(c_min, hs_nostack_a->GetHistogram()->GetMinimum());
	}
	c_min *= stretch_min;
      }
      hs_stack_a->SetMinimum(c_min);
      hs_stack_a->SetMaximum(c_max);

      nbinx = hs_stack_a->GetHistogram()->GetNbinsX();


      if(var_hasXMin || opt_hasXMin || var_hasXMax || opt_hasXMax){
	if(var_hasXMin){ xr_min = va_it->second->XMin(); }
	else if(opt_hasXMin){ xr_min = m_opt->XMin(); }
	else{ xr_min = hs_stack_a->GetHistogram()->GetXaxis()->GetBinLowEdge(1); }

	if(var_hasXMax){ xr_max = va_it->second->XMax(); }
	else if(opt_hasXMax){ xr_max = m_opt->XMax(); }
	else{ xr_max = hs_stack_a->GetHistogram()->GetXaxis()->GetBinUpEdge(nbinx); }

	hs_stack_a->GetXaxis()->SetRangeUser(xr_min, xr_max);
      }

      if(var_label != ""){ hs_stack_a->GetHistogram()->GetXaxis()->SetTitle(var_label.c_str()); }
      else{ hs_stack_a->GetHistogram()->GetXaxis()->SetTitle( ((TH1D*)(hs_stack_a->GetStack()->First()))->GetXaxis()->GetTitle() ) ; }

      if(var_ylabel != ""){ hs_stack_a->GetHistogram()->GetYaxis()->SetTitle(var_ylabel.c_str()); }
      else{ hs_stack_a->GetHistogram()->GetYaxis()->SetTitle( ((TH1D*)(hs_stack_a->GetStack()->First()))->GetYaxis()->GetTitle() ) ; }

    }
    else{
      hs_nostack_a->Draw("nostack"); 
      if(!var_hasYMax && !opt_hasYMax){
	c_max = hs_nostack_a->GetHistogram()->GetMaximum();
	c_max *= stretch_max;
      }
      if(!var_hasYMin && !opt_hasYMin){
	c_min = hs_nostack_a->GetHistogram()->GetMinimum();
	c_min *= stretch_min;
      }

      hs_nostack_a->SetMinimum(c_min);
      hs_nostack_a->SetMaximum(c_max);

      nbinx = hs_nostack_a->GetHistogram()->GetNbinsX();
      if(var_hasXMin || opt_hasXMin || var_hasXMax || opt_hasXMax){
	if(var_hasXMin){ xr_min = va_it->second->XMin(); }
	else if(opt_hasXMin){ xr_min = m_opt->XMin(); }
	else{ xr_min = hs_nostack_a->GetHistogram()->GetXaxis()->GetBinLowEdge(1); }

	if(var_hasXMax){ xr_max = va_it->second->XMax(); }
	else if(opt_hasXMax){ xr_max = m_opt->XMax(); }
	else{ xr_max = hs_nostack_a->GetHistogram()->GetXaxis()->GetBinUpEdge(nbinx); }

	hs_nostack_a->GetXaxis()->SetRangeUser(xr_min, xr_max);
      }

      if(var_label != ""){ hs_nostack_a->GetHistogram()->GetXaxis()->SetTitle(var_label.c_str()); }
      else{ hs_nostack_a->GetHistogram()->GetXaxis()->SetTitle( ((TH1D*)(hs_nostack_a->GetStack()->First()))->GetXaxis()->GetTitle() ) ; }

      if(var_ylabel != ""){ hs_nostack_a->GetHistogram()->GetYaxis()->SetTitle(var_ylabel.c_str()); }
      else{ hs_nostack_a->GetHistogram()->GetYaxis()->SetTitle( ((TH1D*)(hs_nostack_a->GetStack()->First()))->GetYaxis()->GetTitle() ) ; }

      //hs_nostack_a->SetMinimum(1e-11);

      //hs_nostack_a->GetXaxis()->SetRangeUser(0.,200.);

    }

    if(var_isLog && c_min <= 1.e-10){ c_min = 1.e-10; }
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
      canv_a->cd(2);

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
      if(var_hasXMin || opt_hasXMin || var_hasXMax || opt_hasXMax){
	hs_res_a->GetXaxis()->SetRangeUser(xr_min, xr_max);
      }
      else{
       nbinx = hs_res_a->GetHistogram()->GetNbinsX();
       xr_min = hs_res_a->GetHistogram()->GetXaxis()->GetBinLowEdge(1);
       xr_max = hs_res_a->GetHistogram()->GetXaxis()->GetBinUpEdge(nbinx);
      }
      double ry = ( (var_draw_res == "RESIDUAL") || (var_draw_res == "FRACRES") ) ? 0. : 1.;

      TLine* lnref = new TLine(xr_min, ry, xr_max, ry); // a reference line
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

      canv_a->cd(2)->Update();
      canv_a->cd(2)->Modified();
      delete lnref;
    }

    //Write to output file/ print to a png
    if(m_opt->OutputFormat().find("PNG") != std::string::npos){ canv_a->SaveAs(Form("%s%s.png" ,m_opt->OutputFolder().c_str() ,canv_name.c_str())); }
    if(m_opt->OutputFormat().find("EPS") != std::string::npos){ canv_a->SaveAs(Form("%s%s.eps" ,m_opt->OutputFolder().c_str() ,canv_name.c_str())); }
    if(m_opt->OutputFormat().find("PDF") != std::string::npos){ canv_a->SaveAs(Form("%s%s.pdf" ,m_opt->OutputFolder().c_str() ,canv_name.c_str())); }

    if(m_opt->OutputFormat().find("ROOT") != std::string::npos){ m_outfile->cd(); canv_a->Write(); }


    //Delete objects
    delete canv_a;
    delete hs_stack_a;
    delete hs_nostack_a;
    delete hs_res_a;
    delete ttlbox;
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

TH1D* PlotUtils::makeResidual(const std::string& resname, const std::string& s_hnum, const std::string& s_href, const std::string& opt){

  return makeResidual(resname, m_hstMngr->GetTH1D(s_hnum), m_hstMngr->GetTH1D(s_href), opt);

}

TH1D* PlotUtils::makeResidual(const std::string& resname, TH1D* hist, TH1D* href, const std::string& opt){

  TH1D* hist_res = (TH1D*)(hist->Clone(resname.c_str()) );

  hist_res->SetStats(0);
  hist_res->SetName(resname.c_str());
  hist_res->SetTitle("");
  if(opt == "RATIO"){ hist_res->Divide(href, hist_res); }
  else if(opt == "INVRATIO"){ hist_res->Divide(hist_res, href); }
  else if(opt == "RESIDUAL"){ hist_res->Add(href, -1);}
  else{ std::cout<<"Unknown option for residual calculation : "<<opt<<std::endl;}
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

    canv.cd(2)->SetPad(0.,0.,0.95,0.35);
    canv.cd(2)->SetBorderMode(0);
    canv.cd(2)->SetTopMargin(0.0001);
    canv.cd(2)->SetBottomMargin(0.2);
    canv.cd(2)->SetRightMargin(0.05);
  }
  else{
    canv.cd()->SetTopMargin(0.05);
    canv.cd()->SetBottomMargin(0.2);
    canv.cd()->SetRightMargin(0.05);
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
  leg.SetFillColor(0);
  leg.SetFillStyle(0);
  leg.SetLineColor(0);
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
