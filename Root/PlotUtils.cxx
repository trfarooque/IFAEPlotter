/////////// THIS SIDE UP ///////////////////////////////////////////////////////
#include "IFAEPlotter/PlotUtils.h"
#include "IFAEPlotter/StyleDictionary.h"
#include "IFAEPlotter/Plotter_Options.h"
#include "IFAETopFramework/HistManager.h"
#include "IFAETopFramework/AnalysisUtils.h"

#include "TSystem.h"
#include "THStack.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TLegend.h"
#include "TPaveText.h"
#include "TLegendEntry.h"
//#include "TIter.h"
#include "TList.h"
#include "TLatex.h"
#include "TLine.h"
#include <algorithm>
#include <fstream>

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

  if(m_opt->OutputFolder() != ""){ 
    m_output_dir = m_opt->OutputFolder(); 
    if(m_output_dir.substr(m_output_dir.size()-1) != "/"){m_output_dir += "/";}
  }
  if(m_opt->OutputFormat().find("ROOT") != std::string::npos){
    m_outfile = TFile::Open(Form("%s%s.root", m_output_dir.c_str(), m_opt->OutputFile().c_str()), "RECREATE");
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

  bool doGraphs = ((projopt == "MEAN") || (projopt == "RMS") || (projopt == "FRACRMS") || (projopt == "EFF") );
  const std::string& glob_ttl        = m_opt->Title();

  bool opt_hasResMin = (m_opt->OptStr().find("--RESMIN") != std::string::npos); 
  bool opt_hasResMax = (m_opt->OptStr().find("--RESMAX") != std::string::npos);
  bool opt_hasYMin   = (m_opt->OptStr().find("--YMIN")   != std::string::npos);
  bool opt_hasYMax   = (m_opt->OptStr().find("--YMAX")   != std::string::npos);
  bool opt_hasYScale = (m_opt->OptStr().find("--YSCALE") != std::string::npos);
  bool opt_hasXMin   = (m_opt->OptStr().find("--XMIN")   != std::string::npos);
  bool opt_hasXMax   = (m_opt->OptStr().find("--XMAX")   != std::string::npos);

  //bool opt_hasLegendXMin = (m_opt->OptStr().find("--LEGENDXMIN") != std::string::npos);
  bool opt_hasLegendXMax = (m_opt->OptStr().find("--LEGENDXMAX") != std::string::npos);
  //bool opt_hasLegendYMin = (m_opt->OptStr().find("--LEGENDYMIN") != std::string::npos);
  bool opt_hasLegendYMax = (m_opt->OptStr().find("--LEGENDYMAX") != std::string::npos);

  bool opt_hasXTitleSize     = (m_opt->OptStr().find("--XTITLESIZE") != std::string::npos);
  bool opt_hasXTitleOffset   = (m_opt->OptStr().find("--XTITLEOFFSET") != std::string::npos);
  bool opt_hasYTitleSize     = (m_opt->OptStr().find("--YTITLESIZE") != std::string::npos);
  bool opt_hasYTitleOffset   = (m_opt->OptStr().find("--YTITLEOFFSET") != std::string::npos);
  bool opt_hasResTitleSize     = (m_opt->OptStr().find("--RESTITLESIZE") != std::string::npos);
  bool opt_hasResTitleOffset   = (m_opt->OptStr().find("--RESTITLEOFFSET") != std::string::npos);

  bool opt_hasXLabelSize     = (m_opt->OptStr().find("--XLABELSIZE") != std::string::npos);
  bool opt_hasXLabelOffset   = (m_opt->OptStr().find("--XLABELOFFSET") != std::string::npos);
  bool opt_hasYLabelSize     = (m_opt->OptStr().find("--YLABELSIZE") != std::string::npos);
  bool opt_hasYLabelOffset   = (m_opt->OptStr().find("--YLABELOFFSET") != std::string::npos);
  bool opt_hasResLabelSize     = (m_opt->OptStr().find("--RESLABELSIZE") != std::string::npos);
  bool opt_hasResLabelOffset   = (m_opt->OptStr().find("--RESLABELOFFSET") != std::string::npos);

  bool opt_hasBottomMargin   = (m_opt->OptStr().find("--BOTTOMMARGIN") != std::string::npos);
  bool opt_hasTopMargin      = (m_opt->OptStr().find("--TOPMARGIN") != std::string::npos);
  bool opt_hasLeftMargin     = (m_opt->OptStr().find("--LEFTMARGIN") != std::string::npos);
  bool opt_hasRightMargin    = (m_opt->OptStr().find("--RIGHTMARGIN") != std::string::npos);

  bool var_draw_stack = false;
  bool var_draw_res_stack = false;
  bool var_do_width = false;
  bool var_isLogY = false;
  bool var_isLogX = false;
  bool var_isLogRes = false;
  bool var_isShape = false;

  bool var_hasResMin = false;
  bool var_hasResMax = false;
  bool var_hasYMin   = false;
  bool var_hasYMax   = false;
  bool var_hasYScale = false;
  bool var_hasXMin   = false;
  bool var_hasXMax   = false;
  bool var_modXRange = false;

  bool var_hasResRefLine = false;
  //bool var_hasLegendXMin = false;
  bool var_hasLegendXMax = false;
  //bool var_hasLegendYMin = false;
  bool var_hasLegendYMax = false;

  bool var_hasXTitleSize   = false;
  bool var_hasXTitleOffset = false;
  bool var_hasYTitleSize   = false;
  bool var_hasYTitleOffset = false;
  bool var_hasResTitleSize   = false;
  bool var_hasResTitleOffset = false;

  bool var_hasXLabelSize   = false;
  bool var_hasXLabelOffset = false;
  bool var_hasYLabelSize   = false;
  bool var_hasYLabelOffset = false;
  bool var_hasResLabelSize   = false;
  bool var_hasResLabelOffset = false;

  bool var_has_xaxis_ndiv = false;
  bool var_has_yaxis_ndiv = false;
  bool var_has_resaxis_ndiv = false;

  bool var_hasBottomMargin = false;
  bool var_hasTopMargin    = false;
  bool var_hasLeftMargin   = false;
  bool var_hasRightMargin  = false;

  int var_nbinx = 0;
  double var_xmin = 0.;
  double var_xmax = 0.;

  double var_ymin = 0.;
  double var_ymax = 0.;
  double var_ymax_legrange = 0.;

  double var_legend_xmin = 0.;
  double var_legend_xmax = 0.;
  double var_legend_ymin = 0.;
  double var_legend_ymax = 0.;

  double var_xtitle_size = 0.;
  double var_xtitle_offset = 0.;
  double var_ytitle_size = 0.;
  double var_ytitle_offset = 0.;
  double var_restitle_size = 0.;
  double var_restitle_offset = 0.;

  double var_xlabel_size = 0.;
  double var_xlabel_offset = 0.;
  double var_ylabel_size = 0.;
  double var_ylabel_offset = 0.;
  double var_reslabel_size = 0.;
  double var_reslabel_offset = 0.;

  int var_xaxis_ndiv    = 0;
  int var_yaxis_ndiv    = 0;
  int var_resaxis_ndiv  = 0;

  double var_bottom_margin = 0.;
  double var_top_margin = 0.;
  double var_left_margin = 0.;
  double var_right_margin = 0.;

  bool var_isCount = false;

  std::string var_binlabels_str = "";
  std::map<int, std::string> *var_binlabels_map = NULL;

  bool ds_draw_stack = false;
  int ds_res_opt = -1;
  std::string ds_res_erropt = "";
  std::string ds_print_text = "";
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
  std::vector<TH1D*> v_hstack_a = {};
  std::vector<TH1D*> v_hstack_res_a = {};

  VariableAttributesMap* var_loop_map = (projopt == "HIST") ? &m_var_map_proj : &m_var_map;
  for(VariableAttributesMap::iterator va_it = var_loop_map->begin(); va_it != var_loop_map->end(); ++va_it){

    const std::string& var_blinding = (va_it->second->Blinding() != "") ?  va_it->second->Blinding() : m_opt->Blinding();
    bool var_drawBlinder = m_drawBlinder && (var_blinding.find("BIN") != std::string::npos);
    bool var_blind_yield = (var_blinding.find("YIELD") != std::string::npos);
    v_hstack_a.clear();
    v_hstack_res_a.clear();
    std::string var_name       = va_it->second->Name();
    if(projopt == "MEAN"){var_name += "_MEAN";}
    else if(projopt == "RMS"){var_name += "_RMS";}
    else if(projopt == "FRACRMS"){var_name += "_FRACRMS";}
    else if(projopt == "EFF"){ 
      var_name = AnalysisUtils::ReplaceString(var_name,"@EFF@","");
      var_name = AnalysisUtils::ReplaceString(var_name,"/","_");
      var_name += "_EFF";
    }
 
    const std::string& var_label         = va_it->second->Label();
    const std::string& var_draw_res      = va_it->second->DrawRes();
    const std::string& var_draw_res_err  = va_it->second->DrawResErr();
    const std::string& var_ylabel        = (va_it->second->YLabel() != "") ? va_it->second->YLabel() : m_opt->YLabel();
    const std::string& var_reslabel      = (va_it->second->ResLabel() != "") ? va_it->second->ResLabel() : m_opt->ResLabel();
    const std::string& var_resdrawopt    = (va_it->second->ResDrawOpt() != "") ? va_it->second->ResDrawOpt() : m_opt->ResDrawOpt();
    const std::string& var_extralabel    = va_it->second->ExtraLabel();

    var_isCount        = va_it->second->IsCount();
    var_isShape        = !doGraphs && (va_it->second->DoScale() == "SHAPE"); 
    var_do_width       = !doGraphs && va_it->second->DoWidth();
    var_draw_stack     = !doGraphs && va_it->second->DrawStack();
    var_draw_res_stack = !doGraphs && va_it->second->DrawResStack();
    var_isLogY         = !doGraphs && va_it->second->IsLogY();
    var_isLogX         = !doGraphs && va_it->second->IsLogX();
    var_isLogRes       = !doGraphs && va_it->second->IsLogRes();

    var_hasResRefLine  = va_it->second->HasResRefLine();
    var_hasResMin      = va_it->second->HasResMin();
    var_hasResMax      = va_it->second->HasResMax();
    var_hasYMin        = va_it->second->HasYMin();
    var_hasYMax        = va_it->second->HasYMax();
    var_hasYScale      = va_it->second->HasYScale();
    var_hasXMin        = va_it->second->HasXMin();
    var_hasXMax        = va_it->second->HasXMax();

    var_modXRange      = var_hasXMin || var_hasXMax || opt_hasXMin || opt_hasXMax;
    //var_modYRange      = var_hasYMin || var_hasYMax || opt_hasYMin || opt_hasYMax;

    //var_hasLegendXMin  = va_it->second->HasLegendXMin();
    var_hasLegendXMax  = va_it->second->HasLegendXMax();
    //var_hasLegendYMin  = va_it->second->HasLegendYMin();
    var_hasLegendYMax  = va_it->second->HasLegendYMax();

    var_hasXTitleSize   = va_it->second->HasXTitleSize();
    var_hasXTitleOffset = va_it->second->HasXTitleOffset();
    var_hasYTitleSize   = va_it->second->HasYTitleSize();
    var_hasYTitleOffset = va_it->second->HasYTitleOffset();
    var_hasResTitleSize   = va_it->second->HasResTitleSize();
    var_hasResTitleOffset = va_it->second->HasResTitleOffset();

    var_hasXLabelSize   = va_it->second->HasXLabelSize();
    var_hasXLabelOffset = va_it->second->HasXLabelOffset();
    var_hasYLabelSize   = va_it->second->HasYLabelSize();
    var_hasYLabelOffset = va_it->second->HasYLabelOffset();
    var_hasResLabelSize   = va_it->second->HasResLabelSize();
    var_hasResLabelOffset = va_it->second->HasResLabelOffset();

    var_has_xaxis_ndiv   = va_it->second->HasXAxisNdiv();
    var_has_yaxis_ndiv   = va_it->second->HasYAxisNdiv();
    var_has_resaxis_ndiv = va_it->second->HasResAxisNdiv();

    var_hasBottomMargin = va_it->second->HasBottomMargin();
    var_hasTopMargin    = va_it->second->HasTopMargin();
    var_hasLeftMargin   = va_it->second->HasLeftMargin();
    var_hasRightMargin  = va_it->second->HasRightMargin();

    var_xaxis_ndiv   = va_it->second->XAxisNdiv();
    var_yaxis_ndiv   = va_it->second->YAxisNdiv();
    var_resaxis_ndiv = va_it->second->ResAxisNdiv();

    var_binlabels_str = va_it->second->BinLabelsStr();
    var_binlabels_map = va_it->second->BinLabelsMap();
 
    //==================================================

    var_xmin = 0.; 
    var_xmax = 0.; 
    var_ymin = 0.; 
    var_ymax = 0.; 
    var_ymax_legrange = 0.; 

    var_legend_xmin = 0.;
    var_legend_xmax = 0.;
    var_legend_ymin = 0.;
    var_legend_ymax = 0.;

    var_xtitle_size = 0.;
    var_xtitle_offset = 0.;
    var_ytitle_size = 0.;
    var_ytitle_offset = 0.;
    var_restitle_size = 0.;
    var_restitle_offset = 0.;

    var_xlabel_size = 0.;
    var_xlabel_offset = 0.;
    var_ylabel_size = 0.;
    var_ylabel_offset = 0.;
    var_reslabel_size = 0.;
    var_reslabel_offset = 0.;

    var_bottom_margin = 0.;
    var_top_margin = 0.;
    var_left_margin = 0.;
    var_right_margin = 0.;

    var_nbinx = 0; 

    //================================================================================
    if(var_hasBottomMargin){ var_bottom_margin = va_it->second->BottomMargin(); }
    else if(opt_hasBottomMargin){ var_bottom_margin = m_opt->BottomMargin(); }
    else{ var_bottom_margin = 0.2;}

    if(var_hasTopMargin){ var_top_margin = va_it->second->TopMargin(); }
    else if(opt_hasTopMargin){ var_top_margin = m_opt->TopMargin(); }
    else{ var_top_margin = 0.05;}

    if(var_hasLeftMargin){ var_left_margin = va_it->second->LeftMargin(); }
    else if(opt_hasLeftMargin){ var_left_margin = m_opt->LeftMargin(); }
    else{ var_left_margin = 0.15;}

    if(var_hasRightMargin){ var_right_margin = va_it->second->RightMargin(); }
    else if(opt_hasRightMargin){ var_right_margin = m_opt->RightMargin(); }
    else{ var_right_margin = 0.05;}

    //================================================================================

    std::string hname_sum = m_drawSum ? var_name + "_" + m_attrbt_map["SUM"]->Suffix() : "";
    std::string hname_blinder = m_drawBlinder ? var_name + "_" + m_attrbt_map["BLINDER"]->Suffix() : "";
    bool drawRes = ( (var_draw_res != "") && (var_draw_res != "NONE") );
    if(drawRes && (s_base_name == "") ){std::cout<<"No reference sample specified for residual calculation"<<std::endl;}

    std::string canv_name = "canv_" + AnalysisUtils::ReplaceString(var_name,"*","");
    TCanvas* canv_a = new TCanvas(canv_name.c_str(), "", 800, 800);
    SetStyleCanvas( *canv_a, drawRes, var_bottom_margin, var_top_margin, var_left_margin, var_right_margin );
 
    //NOTE:
    //Eventually, TCanvas and TLegend properties will be defined in the StyleDictionary
    //Make a TCanvas and set its attributes
    std::string hs_stack_name = "hs_stack_" + var_name;
    std::string hs_nostack_name = "hs_nostack_" + var_name;
    std::string hs_res_stack_name = "hs_res_stack_" + var_name;
    std::string hs_res_nostack_name = "hs_res_nostack_" + var_name;
    //std::string hs_res_ref_name = "hs_res_ref_" + var_name;

    THStack* hs_res_nostack_a     = drawRes ? new THStack(hs_res_nostack_name.c_str(), "")       : NULL;
    THStack* hs_res_stack_a       = drawRes && var_draw_stack ? new THStack(hs_res_stack_name.c_str(), "")       : NULL;
    //THStack* hs_res_ref_a = drawRes ? new THStack(hs_res_ref_name.c_str(), "") : NULL;
    std::string hbasename = var_name + "_" + s_base_suffix;
    //TH1D* h_base = drawRes ? m_hstMngr->GetTH1D(hbasename) : NULL;

    THStack* hs_nostack_a = new THStack(hs_nostack_name.c_str(), "");
    THStack* hs_stack_a = var_draw_stack ? new THStack(hs_stack_name.c_str(), "") : NULL;

    //---------------------------------------------------------
    TLegend* leg_a = new TLegend();
    bool print_text = (m_opt->ShowYields() && (!var_isShape || var_draw_stack) ) || (m_opt->PrintValue() != "") ;
    TLegend* leg_text = print_text ? new TLegend() : NULL;

    double leg_textsize=0.;
    if(va_it->second->HasLegendTextSize()){ leg_textsize = va_it->second->LegendTextSize(); }
    else if(m_opt->OptStr().find("LEGENDTEXTSIZE") != std::string::npos){ leg_textsize = m_opt->LegendTextSize(); }
    else{ 
      leg_textsize = drawRes ? 0.045 : 0.035;
    }

    SetStyleLegend(*leg_a, leg_textsize);
    leg_a->Clear();
    if(leg_text){
      SetStyleLegend(*leg_text, leg_textsize, 42, 0.04);
      leg_text->Clear();
    }
    //---------------------------------------------------------------
    TPaveText* ttlbox = NULL;
    if( (glob_ttl != "") || (var_extralabel != "") ){
      double ttl_xmin = 0.; double ttl_ymin = 0.; double ttl_xmax = 0.; double ttl_ymax = 0.;
      double ttl_textsize = 0.;
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

      if(va_it->second->HasTitleTextSize()){ ttl_textsize = va_it->second->TitleTextSize(); }
      else if(m_opt->OptStr().find("TITLETEXTSIZE") != std::string::npos){ ttl_textsize = m_opt->TitleTextSize(); }
      else{ 
	ttl_textsize = drawRes ? 0.045 : 0.035;
      }

      ttlbox = new TPaveText(ttl_xmin, ttl_ymin, ttl_xmax, ttl_ymax, "NBNDC");
      ttlbox->SetTextAlign(12); //horizontally centred; top-aligned
      ttlbox->SetFillColor(0);
      ttlbox->SetFillStyle(0);
      ttlbox->SetLineColor(0);
      ttlbox->SetLineStyle(0);
      ttlbox->SetLineWidth(0);
      ttlbox->SetBorderSize(0);
      ttlbox->SetTextSize(ttl_textsize);
      ttlbox->SetTextFont(42);
      ttlbox->SetShadowColor(0);

      if(glob_ttl != ""){ ttlbox->AddText(glob_ttl.c_str()); }
      if(var_extralabel != ""){ ttlbox->AddText(var_extralabel.c_str());}
    }
    //---------------------------------------------------------------------------------------------------

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
      const std::string& ds_print_format = (at_it->second->PrintFormat() != "") ? at_it->second->PrintFormat() : m_opt->PrintFormat();
      const std::string& ds_print_value = m_opt->ShowYields() ? "YIELD" : 
	( (at_it->second->PrintValue() != "") ? at_it->second->PrintValue() : m_opt->PrintValue() );

      ds_res_erropt = "";
      if( var_draw_res_err  == "REFBAND" ){ ds_res_erropt = "SCALE"; } //scale both REF and INC sample error by the factor used for the division
      else if( at_it->second->ResOpt() == 1 ){ ds_res_erropt = "ZERO"; } 

      bool ds_isShape = !doGraphs && (at_it->second->DrawScale() == "SHAPE"); 
      ds_draw_stack = !doGraphs && at_it->second->DrawStack();
      ds_res_opt = at_it->second->ResOpt();
      bool ds_isBlind = (ds_name == m_opt->BlindSample());

      bool ds_leg_empty = (leg_text==NULL) || (var_blind_yield && ds_isBlind) || (ds_print_format == "NONE") 
	|| (ds_print_value == "NONE") || (ds_isShape && ds_print_value == "YIELD");

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

	ds_print_text = "";
	if(!doGraphs && leg_text){ 
	  if(ds_leg_empty){ leg_text->AddEntry(hist_a, " ", ""); }
	  else{
	    TH1D* _hist_print = NULL;
	    if(var_do_width){ _hist_print = GetHistTimesBinWidth(hist_a); }
	    else{ _hist_print = hist_a; }

	    ds_print_text = MakeMomentText(_hist_print, ds_print_value, ds_print_format);

	    /******************** ALERTRISHA *******************************/
	    /*
	    TH1D* _hist_ref_print = NULL;
	    if(var_do_width){ _hist_ref_print = GetHistTimesBinWidth(m_hstMngr->GetTH1D(hbasename)); }
	    else{ _hist_ref_print = m_hstMngr->GetTH1D(hbasename); }
	    ds_print_text = MakeResidualMomentText(_hist_print, _hist_ref_print, ds_print_value, ds_print_format); 

	    */
	    /******************** ALERTRISHA *******************************/

	    if(var_do_width){ delete _hist_print; }
	    leg_text->AddEntry(hist_a, ds_print_text.c_str(), "");
	  }
	}//If drawing a legend for text

	//==================================

      }//if leglabel was not provided, then clearly the sample is not meant to be added to the legend

      if(at_it->first != "BLINDER"){
	if( ds_drawopt != "DUMMY" ){
	  if(var_draw_stack && ds_draw_stack){ 
	    v_hstack_a.push_back(hist_a);
	  }
	  else{ 
	    hs_nostack_a->Add(hist_a, ds_drawopt.c_str()); 
	  }//if this sample is not be be stacked
	}

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

	  if(var_draw_res_stack && ds_draw_stack){ 
	    v_hstack_res_a.push_back(hist_res_a);
	  }
	  else{
	    hs_res_nostack_a->Add(hist_res_a, resdrawopt.c_str());
	  }
	  resname_a.clear(); resdrawopt.clear();
	}//if residual histogram needed 
	
      }//if not a BLINDER

      //Clear strings
      hist_name.clear(); firstsample = false;
    }//sample loop
    //================================================= AFTER SAMPLE LOOP ============================================

    if(hs_stack_a && v_hstack_a.size() > 0){

      for(int ro = v_hstack_a.size()-1; ro >= 0; ro--){
	hs_stack_a->Add(v_hstack_a.at(ro), "hist");
      }

    }

    if(hs_res_stack_a && v_hstack_res_a.size() > 0){

      for(int ro = v_hstack_res_a.size()-1; ro >= 0; ro--){
	hs_res_stack_a->Add(v_hstack_res_a.at(ro), "hist");
      }

    }

    //=================================================================
    if(var_hasLegendXMax){ var_legend_xmax = va_it->second->LegendXMax(); }
    else if(opt_hasLegendXMax){ var_legend_xmax = m_opt->LegendXMax(); }
    else{ var_legend_xmax = 0.9;}
    if(var_hasLegendYMax){ var_legend_ymax = va_it->second->LegendYMax(); }
    else if(opt_hasLegendYMax){ var_legend_ymax = m_opt->LegendYMax(); }
    else{ var_legend_ymax = 0.9;}

    /*
    if(var_hasLegendXMin){ var_legend_xmin = va_it->second->LegendXMin(); }
    else if(opt_hasLegendXMin){ var_legend_xmin = m_opt->LegendXMin(); }
    if(var_hasLegendYMin){ var_legend_ymin = va_it->second->LegendYMin(); }
    else if(opt_hasLegendYMin){ var_legend_ymin = m_opt->LegendYMin(); }
    */
    if(leg_text){
      ResizeLegend(*leg_text, var_legend_xmax, var_legend_ymax);
      ResizeLegend(*leg_a, leg_text->GetX1NDC(), leg_text->GetY2NDC() );
    }
    else{ ResizeLegend(*leg_a, var_legend_xmax, var_legend_ymax );}

    //=================================================================================

    //stretch_min = var_isLogY ? 1.E-2 : 0.5; 
    //stretch_max = var_isLogY ? 1.E3 : 1.35; 

    var_ymin = 0.; var_ymax = 0.;
    double stretch_min = 1.; double stretch_max = 1.;
    if(var_hasYMin){var_ymin = va_it->second->YMin();}
    else if(opt_hasYMin){var_ymin = m_opt->YMin();}
    else{
      stretch_min = var_isLogY ? 1.E-2 : 0.5; 
      var_ymin = (hs_nostack_a->GetNhists() > 0) ? hs_nostack_a->GetMinimum() : var_ymin;
      if(var_draw_stack && (hs_stack_a->GetNhists() > 0)){
	var_ymin = min(var_ymin, hs_stack_a->GetMinimum());
      }
      var_ymin = stretch_min*var_ymin;
    }

    if(var_hasYMax){var_ymax = va_it->second->YMax();}
    else if(opt_hasYMax){var_ymax = m_opt->YMax();}
    else{ 

      var_ymax = (hs_nostack_a->GetNhists() > 0) ? hs_nostack_a->GetMaximum("nostack") : var_ymax;
      if(var_draw_stack && (hs_stack_a->GetNhists() > 0)){
	var_ymax = max(var_ymax, hs_stack_a->GetMaximum());
      }

      if(var_hasYScale){stretch_max = va_it->second->YScale();}
      else if(opt_hasYScale){stretch_max = m_opt->YScale();}
      else{ 
	stretch_max = 1.35; 
	double stretch_max_leg = ( leg_a->GetY1NDC() > 0. ) ? 1./leg_a->GetY1NDC() : 1.35;

	if(stretch_max_leg > stretch_max){

	  //The x-range of the legend
	  double fmin_leg = leg_a->GetX1NDC(); double fmax_leg = leg_a->GetX2NDC();
	  //Find the maximum of the nostack histograms in the axis range
	  TList* hlist = hs_nostack_a->GetHists();

	  double hleg_low = fmin_leg*var_xmax + (1.-fmin_leg)*var_xmin;
	  double hleg_up = fmax_leg*var_xmax + (1.-fmax_leg)*var_xmin;

	  TH1D* hcur = NULL;

	  for(int i = 0; i < hs_nostack_a->GetNhists(); i++){
	    hcur = (TH1D*)(hlist->At(i));
	    hcur->SetAxisRange(hleg_low, hleg_up);
	    var_ymax_legrange = max(var_ymax_legrange, hcur->GetMaximum());
	    hcur->SetAxisRange(var_xmin, var_xmax);
	  }

	  if(var_draw_stack && (hs_stack_a->GetNhists() > 0)){
	    hcur = (TH1D*)(hs_stack_a->GetStack()->Last());
	    hcur->SetAxisRange(hleg_low, hleg_up);
	    var_ymax_legrange = max(var_ymax_legrange, hcur->GetMaximum());
	    hcur->SetAxisRange(var_xmin, var_xmax);
	  }

	  if(var_isLogY){ var_ymax_legrange = pow(var_ymax_legrange, stretch_max_leg)/pow(var_ymin, stretch_max_leg -1.); }
	  else{ var_ymax_legrange = stretch_max_leg*var_ymax_legrange - (1. - stretch_max_leg)*var_ymin; }

	}//if legend extent is more than default stretch factor

      } //if stretch factor is not provided

      if(var_isLogY){ var_ymax = pow(var_ymax, stretch_max)/pow(var_ymin, stretch_max -1.); }
      else{ var_ymax = stretch_max*var_ymax - (1. - stretch_max)*var_ymin; }
      var_ymax = max(var_ymax, var_ymax_legrange);

    }//if ymax coordinate not provided
    if(!var_hasYMin){
      if(drawRes && !var_isLogY && var_ymin <= 1.e-5){var_ymin = 1.1e-5;}
      else if(var_isLogY && var_ymin <= 1.e-10){ var_ymin = 1.e-10; }
    }
    //==================================================== Resizing done ======================================================================

    if(var_hasXTitleSize){ var_xtitle_size = va_it->second->XTitleSize(); }
    else if(opt_hasXTitleSize){ var_xtitle_size = m_opt->XTitleSize(); }
    else{ var_xtitle_size = drawRes? 0.1 : 0.04; }

    if(var_hasXTitleOffset){ var_xtitle_offset = va_it->second->XTitleOffset(); }
    else if(opt_hasXTitleOffset){ var_xtitle_offset = m_opt->XTitleOffset(); }
    else { var_xtitle_offset = drawRes ? 0.9 : 1.; }

    if(var_hasXLabelSize){ var_xlabel_size = va_it->second->XLabelSize(); }
    else if(opt_hasXLabelSize){ var_xlabel_size = m_opt->XLabelSize(); }
    else{ var_xlabel_size = drawRes? 0.09 : 0.04; }

    if(var_hasXLabelOffset){ var_xlabel_offset = va_it->second->XLabelOffset(); }
    else if(opt_hasXLabelOffset){ var_xlabel_offset = m_opt->XLabelOffset(); }
    else { var_xlabel_offset = 0.005; }

    if(var_hasYTitleSize){ var_ytitle_size = va_it->second->YTitleSize(); }
    else if(opt_hasYTitleSize){ var_ytitle_size = m_opt->YTitleSize(); }
    else{ var_ytitle_size = drawRes ? 0.07 : 0.04; }

    if(var_hasYTitleOffset){ var_ytitle_offset = va_it->second->YTitleOffset(); }
    else if(opt_hasYTitleOffset){ var_ytitle_offset = m_opt->YTitleOffset(); }
    else{

      int ndig = 0.;
      if( var_isLogY ){ndig = 3;}
      else if( var_ymax <=0.001 || var_ymax > 100000.){ndig = 6; }
      else if(var_ymax <=0.1 || var_ymax > 1000.){ndig = 4; }
      else {ndig = 3; }

      if( ndig<=3 ){ var_ytitle_offset = drawRes ? 0.7 : 1.2; }
      else if( ndig>3 && ndig<6 ){ var_ytitle_offset = drawRes? 1.0 : 1.4; }
      else if( ndig==6 ){ var_ytitle_offset = drawRes ? 1.2 : 1.6; }
      else{ var_ytitle_offset = drawRes ? 1.5 : 1.8; }

    }

    if(var_hasYLabelSize){ var_ylabel_size = va_it->second->YLabelSize(); }
    else if(opt_hasYLabelSize){ var_ylabel_size = m_opt->YLabelSize(); }
    else{ var_ylabel_size = drawRes ? 0.05 : 0.04; }

    if(var_hasYLabelOffset){ var_ylabel_offset = va_it->second->YLabelOffset(); }
    else if(opt_hasYLabelOffset){ var_ylabel_offset = m_opt->YLabelOffset(); }
    else{ var_ylabel_offset = 0.005; }

    if(var_hasResTitleSize){ var_restitle_size = va_it->second->ResTitleSize(); }
    else if(opt_hasResTitleSize){ var_restitle_size = m_opt->ResTitleSize(); }
    else{ var_restitle_size = 0.09; }

    if(var_hasResTitleOffset){ var_restitle_offset = va_it->second->ResTitleOffset(); }
    else if(opt_hasResTitleOffset){ var_restitle_offset = m_opt->ResTitleOffset(); }
    else { var_restitle_offset = 0.5; }

    if(var_hasResLabelSize){ var_reslabel_size = va_it->second->ResLabelSize(); }
    else if(opt_hasResLabelSize){ var_reslabel_size = m_opt->ResLabelSize(); }
    else{ var_reslabel_size = 0.07; }

    if(var_hasResLabelOffset){ var_reslabel_offset = va_it->second->ResLabelOffset(); }
    else if(opt_hasResLabelOffset){ var_reslabel_offset = m_opt->ResLabelOffset(); }
    else { var_reslabel_offset = 0.005; }

    //==================================================== Axis title parameters set ======================================================================

    TPad* curpad = (TPad*)(canv_a->cd(1));
    if(var_isLogY){curpad->SetLogy();}
    if(var_isLogX){curpad->SetLogx();}


    if(var_draw_stack && (hs_stack_a->GetNhists() > 0)){

      hs_stack_a->Draw();
      if(hs_nostack_a->GetNhists() > 0){hs_nostack_a->Draw("samenostack");}

      if(var_modXRange){ hs_stack_a->GetXaxis()->SetRangeUser(var_xmin, var_xmax); }
      //To protect empty histograms
      hs_stack_a->SetMinimum(var_ymin);
      if(var_ymax > var_ymin){
	hs_stack_a->SetMinimum(var_ymin);
	hs_stack_a->SetMaximum(var_ymax);
      }
      if(var_ylabel != ""){ hs_stack_a->GetHistogram()->GetYaxis()->SetTitle(var_ylabel.c_str()); }
      else{ hs_stack_a->GetHistogram()->GetYaxis()->SetTitle( ((TH1D*)(hs_stack_a->GetStack()->First()))->GetYaxis()->GetTitle() ) ; }

      if(var_has_xaxis_ndiv){ hs_stack_a->GetHistogram()->GetXaxis()->SetNdivisions(var_xaxis_ndiv, false); }
      else if(var_isCount)  { hs_stack_a->GetHistogram()->GetXaxis()->SetNdivisions(var_xmax - var_xmin, false); }
      if(var_has_yaxis_ndiv){ hs_stack_a->GetHistogram()->GetYaxis()->SetNdivisions(var_yaxis_ndiv, false); }

      if(var_isCount){ hs_stack_a->GetHistogram()->GetXaxis()->CenterLabels(); }

      hs_stack_a->GetHistogram()->GetYaxis()->SetTitleOffset(var_ytitle_offset);
      hs_stack_a->GetHistogram()->GetYaxis()->SetTitleSize(var_ytitle_size);
      hs_stack_a->GetHistogram()->GetYaxis()->SetLabelOffset(var_ylabel_offset);
      hs_stack_a->GetHistogram()->GetYaxis()->SetLabelSize(var_ylabel_size);

      if(!drawRes){      
	if(var_label != ""){ hs_stack_a->GetHistogram()->GetXaxis()->SetTitle(var_label.c_str()); }
	else{ hs_stack_a->GetHistogram()->GetXaxis()->SetTitle( ((TH1D*)(hs_stack_a->GetStack()->First()))->GetXaxis()->GetTitle() ) ; }

	hs_stack_a->GetHistogram()->GetXaxis()->SetTitleOffset(var_xtitle_offset);
	hs_stack_a->GetHistogram()->GetXaxis()->SetTitleSize(var_xtitle_size);
	hs_stack_a->GetHistogram()->GetXaxis()->SetLabelOffset(var_xlabel_offset);
	hs_stack_a->GetHistogram()->GetXaxis()->SetLabelSize(var_xlabel_size);

	//if have bin labels, set them here
	if( !var_binlabels_str.empty() ){
	  for( std::pair<int, std::string> label : *var_binlabels_map ){
	    hs_stack_a->GetHistogram()->GetXaxis()->SetBinLabel(label.first, label.second.c_str());
	  }
	}

      }

    }//if var_draw_stack
    else{
      hs_nostack_a->Draw("nostack"); 
      if(var_modXRange){ hs_nostack_a->GetXaxis()->SetRangeUser(var_xmin, var_xmax); }

      //To protect empty histograms
      hs_nostack_a->SetMinimum(var_ymin);
      if(var_ymax > var_ymin){
	hs_nostack_a->SetMinimum(var_ymin);
	hs_nostack_a->SetMaximum(var_ymax);
      }
      if(var_ylabel != ""){ hs_nostack_a->GetHistogram()->GetYaxis()->SetTitle(var_ylabel.c_str()); }
      else{ hs_nostack_a->GetHistogram()->GetYaxis()->SetTitle( ((TH1D*)(hs_nostack_a->GetStack()->First()))->GetYaxis()->GetTitle() ) ; }

      if(var_has_xaxis_ndiv){ hs_nostack_a->GetHistogram()->GetXaxis()->SetNdivisions(var_xaxis_ndiv, false); }
      else if(var_isCount)  { hs_nostack_a->GetHistogram()->GetXaxis()->SetNdivisions(var_xmax - var_xmin, false); }
      if(var_has_yaxis_ndiv){ hs_nostack_a->GetHistogram()->GetYaxis()->SetNdivisions(var_yaxis_ndiv, false); }

      if(var_isCount){ hs_nostack_a->GetHistogram()->GetXaxis()->CenterLabels(); }

      hs_nostack_a->GetHistogram()->GetYaxis()->SetTitleOffset(var_ytitle_offset);
      hs_nostack_a->GetHistogram()->GetYaxis()->SetTitleSize(var_ytitle_size);
      hs_nostack_a->GetHistogram()->GetYaxis()->SetLabelOffset(var_ylabel_offset);
      hs_nostack_a->GetHistogram()->GetYaxis()->SetLabelSize(var_ylabel_size);

      if(!drawRes){      
	if(var_label != ""){ hs_nostack_a->GetHistogram()->GetXaxis()->SetTitle(var_label.c_str()); }
	else{ hs_nostack_a->GetHistogram()->GetXaxis()->SetTitle( ((TH1D*)(hs_nostack_a->GetStack()->First()))->GetXaxis()->GetTitle() ) ; }

	hs_nostack_a->GetHistogram()->GetXaxis()->SetTitleOffset(var_xtitle_offset);
	hs_nostack_a->GetHistogram()->GetXaxis()->SetTitleSize(var_xtitle_size);
	hs_nostack_a->GetHistogram()->GetXaxis()->SetLabelOffset(var_xlabel_offset);
	hs_nostack_a->GetHistogram()->GetXaxis()->SetLabelSize(var_xlabel_size);

	//if have bin labels, set them here
	if( !var_binlabels_str.empty() ){
	  for( std::pair<int, std::string> label : *var_binlabels_map ){
	    hs_nostack_a->GetHistogram()->GetXaxis()->SetBinLabel(label.first, label.second.c_str());
	  }
	}

      }

    }//if !var_draw_stack
    //===================================================== THStacks drawn =====================================
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

    if(ttlbox){ ttlbox->Draw(); } 
    curpad->RedrawAxis();

    leg_a->Draw();
    if(leg_text){ leg_text->Draw(); }

    curpad->Update();
    curpad->Modified();
    //====================================================== Top panel completed ===============================================

    if(drawRes){
      curpad = (TPad*)(canv_a->cd(2));
      if(var_isLogRes){curpad->SetLogy();}
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
      else if(var_draw_res == "RATIOUNC"){ r_min = 0.5;}

      if(var_hasResMax){r_max = va_it->second->ResMax();}
      else if(opt_hasResMax){r_max = m_opt->ResMax();}
      else if(var_draw_res == "RESIDUAL"){ r_max = 1.;}
      else if(var_draw_res == "FRACRES"){ r_max = 1.;}
      else if(var_draw_res == "RATIO"){ r_max = 1.5;}
      else if(var_draw_res == "INVRATIO"){ r_max = 1.5;}
      else if(var_draw_res == "RATIOUNC"){ r_max = 1.5;}


      double ry = (var_hasResRefLine) ? va_it->second->ResRefLine() : ( ( (var_draw_res == "RESIDUAL") || (var_draw_res == "DIFF") ) ? 0. : 1. );
      TLine* lnref = new TLine(var_xmin, ry, var_xmax, ry); // a reference line
      lnref->SetLineStyle(2); //dashed line

      if(var_draw_res_stack && (hs_res_stack_a->GetNhists() > 0)){
	hs_res_stack_a->Draw();
	if(hs_res_nostack_a->GetNhists() > 0){
	  hs_res_nostack_a->Draw("samenostack");
	}
	if(var_modXRange){ hs_res_stack_a->GetXaxis()->SetRangeUser(var_xmin, var_xmax); }

	if(var_label != ""){ hs_res_stack_a->GetHistogram()->GetXaxis()->SetTitle(var_label.c_str()); }
	else{ hs_res_stack_a->GetHistogram()->GetXaxis()->SetTitle( ((TH1D*)(hs_res_stack_a->GetStack()->First()))->GetXaxis()->GetTitle() ) ; }

	if(var_reslabel != ""){ hs_res_stack_a->GetHistogram()->GetYaxis()->SetTitle(var_reslabel.c_str()); }
	hs_res_stack_a->SetMinimum(r_min);
	hs_res_stack_a->SetMaximum(r_max);
	lnref->DrawClone("same");

	if(var_has_xaxis_ndiv){ hs_res_stack_a->GetHistogram()->GetXaxis()->SetNdivisions(var_xaxis_ndiv, false); }
	else if(var_isCount)  { hs_res_stack_a->GetHistogram()->GetXaxis()->SetNdivisions(var_xmax - var_xmin, false); }
	if(var_has_resaxis_ndiv){ hs_res_stack_a->GetHistogram()->GetYaxis()->SetNdivisions(var_resaxis_ndiv, false); }

	if(var_isCount){ hs_res_stack_a->GetHistogram()->GetXaxis()->CenterLabels(); }

	hs_res_stack_a->GetHistogram()->GetXaxis()->SetTitleOffset(var_xtitle_offset);
	hs_res_stack_a->GetHistogram()->GetYaxis()->SetTitleOffset(var_restitle_offset);
      
	hs_res_stack_a->GetHistogram()->GetXaxis()->SetTitleSize(var_xtitle_size);
	hs_res_stack_a->GetHistogram()->GetYaxis()->SetTitleSize(var_restitle_size);
 
	hs_res_stack_a->GetHistogram()->GetXaxis()->SetLabelOffset(var_xlabel_offset);
	hs_res_stack_a->GetHistogram()->GetXaxis()->SetLabelSize(var_xlabel_size);
	hs_res_stack_a->GetHistogram()->GetYaxis()->SetLabelOffset(var_reslabel_offset);
	hs_res_stack_a->GetHistogram()->GetYaxis()->SetLabelSize(var_reslabel_size);
      
	hs_res_stack_a->GetHistogram()->GetYaxis()->CenterTitle();

	//if have bin labels, set them here
	if( !var_binlabels_str.empty() ){
	  for( std::pair<int, std::string> label : *var_binlabels_map ){
	    hs_res_stack_a->GetHistogram()->GetXaxis()->SetBinLabel(label.first, label.second.c_str());
	  }
	}

      }
      else{
	hs_res_nostack_a->Draw("nostack");
	if(var_modXRange){ hs_res_nostack_a->GetXaxis()->SetRangeUser(var_xmin, var_xmax); }

	if(var_label != ""){ hs_res_nostack_a->GetHistogram()->GetXaxis()->SetTitle(var_label.c_str()); }
	else{ hs_res_nostack_a->GetHistogram()->GetXaxis()->SetTitle( ((TH1D*)(hs_res_nostack_a->GetStack()->First()))->GetXaxis()->GetTitle() ) ; }

	if(var_reslabel != ""){ hs_res_nostack_a->GetHistogram()->GetYaxis()->SetTitle(var_reslabel.c_str()); }
	hs_res_nostack_a->SetMinimum(r_min);
	hs_res_nostack_a->SetMaximum(r_max);
	lnref->DrawClone("same");

	if(var_has_xaxis_ndiv){ hs_res_nostack_a->GetHistogram()->GetXaxis()->SetNdivisions(var_xaxis_ndiv, false); }
	else if(var_isCount)  { hs_res_nostack_a->GetHistogram()->GetXaxis()->SetNdivisions(var_xmax - var_xmin, false); }
	if(var_has_resaxis_ndiv){ hs_res_nostack_a->GetHistogram()->GetYaxis()->SetNdivisions(var_resaxis_ndiv, false); }

	if(var_isCount){ hs_res_nostack_a->GetHistogram()->GetXaxis()->CenterLabels(); }

	hs_res_nostack_a->GetHistogram()->GetXaxis()->SetTitleOffset(var_xtitle_offset);
	hs_res_nostack_a->GetHistogram()->GetYaxis()->SetTitleOffset(var_restitle_offset);
      
	hs_res_nostack_a->GetHistogram()->GetXaxis()->SetTitleSize(var_xtitle_size);
	hs_res_nostack_a->GetHistogram()->GetYaxis()->SetTitleSize(var_restitle_size);
 
	hs_res_nostack_a->GetHistogram()->GetXaxis()->SetLabelOffset(var_xlabel_offset);
	hs_res_nostack_a->GetHistogram()->GetXaxis()->SetLabelSize(var_xlabel_size);
	hs_res_nostack_a->GetHistogram()->GetYaxis()->SetLabelOffset(var_reslabel_offset);
	hs_res_nostack_a->GetHistogram()->GetYaxis()->SetLabelSize(var_reslabel_size);
      
	hs_res_nostack_a->GetHistogram()->GetYaxis()->CenterTitle();

	//if have bin labels, set them here
	if( !var_binlabels_str.empty() ){
	  for( std::pair<int, std::string> label : *var_binlabels_map ){
	    hs_res_nostack_a->GetHistogram()->GetXaxis()->SetBinLabel(label.first, label.second.c_str());
	  }
	}
      }//if not drawing stack of residuals


      curpad->Update();
      curpad->Modified();

      delete lnref;
    }


    //============================================= Bottom panel done ===========================================

    //Write to output file/ print to a png
    std::string var_outdir = va_it->second->OutputFolder();
    if (m_opt->MsgLevel() == Debug::DEBUG){ std::cout<< " Printing "<<canv_name << std::endl; }
    if(m_opt->OutputFormat().find("PNG") != std::string::npos){ 

      if(var_outdir != ""){
	if(var_outdir.substr(var_outdir.size()-1) != "/"){var_outdir += "/";}
	gSystem->mkdir(Form("IFP_PNG/%s/%s" ,m_output_dir.c_str(), var_outdir.c_str()), "TRUE");
      }

      canv_a->SaveAs(Form("IFP_PNG/%s/%s%s.png" ,m_output_dir.c_str(), var_outdir.c_str() ,canv_name.c_str())); 
      if(m_opt->MsgLevel() == Debug::DEBUG) std::cout<<"PlotUtils::OverlayHists printing "<<canv_name<<".png"<<std::endl;
    }
    if(m_opt->OutputFormat().find("EPS") != std::string::npos){ 

      if(var_outdir != ""){
	if(var_outdir.substr(var_outdir.size()-1) != "/"){var_outdir += "/";}
	gSystem->mkdir(Form("IFP_EPS/%s/%s" ,m_output_dir.c_str(), var_outdir.c_str()), "TRUE");
      }

      canv_a->SaveAs(Form("IFP_EPS/%s/%s%s.eps" ,m_output_dir.c_str() ,var_outdir.c_str() ,canv_name.c_str()));  
      if(m_opt->MsgLevel() == Debug::DEBUG) std::cout<<"PlotUtils::OverlayHists printing "<<canv_name<<".eps"<<std::endl;
   }
    if(m_opt->OutputFormat().find("PDF") != std::string::npos){
      if(var_outdir != ""){
	if(var_outdir.substr(var_outdir.size()-1) != "/"){var_outdir += "/";}
	gSystem->mkdir(Form("IFP_PDF/%s/%s" ,m_output_dir.c_str(), var_outdir.c_str()), "TRUE");
      }

      canv_a->SaveAs(Form("IFP_PDF/%s/%s%s.pdf" ,m_output_dir.c_str() ,var_outdir.c_str() ,canv_name.c_str()));
      if(m_opt->MsgLevel() == Debug::DEBUG) std::cout<<"PlotUtils::OverlayHists printing "<<canv_name<<".pdf"<<std::endl;
    }
    if(m_opt->OutputFormat().find("CPP") != std::string::npos){
      if(var_outdir != ""){
	if(var_outdir.substr(var_outdir.size()-1) != "/"){var_outdir += "/";}
	gSystem->mkdir(Form("IFP_CPP/%s/%s" ,m_output_dir.c_str(), var_outdir.c_str()), "TRUE");
      }

      canv_a->SaveAs(Form("IFP_CPP/%s/%s%s.C" ,m_output_dir.c_str() ,var_outdir.c_str() ,canv_name.c_str()));
      if(m_opt->MsgLevel() == Debug::DEBUG) std::cout<<"PlotUtils::OverlayHists writing "<<canv_name<<".C"<<std::endl;
    }
    if(m_opt->OutputFormat().find("ROOT") != std::string::npos){ 
      m_outfile->cd(); canv_a->Write();
      if(m_opt->MsgLevel() == Debug::DEBUG) std::cout<<"PlotUtils::OverlayHists writing "<<canv_name<<" into ROOT file"<<std::endl;
    }
    var_outdir.clear();

    //Delete objects
    delete canv_a;
    delete hs_stack_a;
    delete hs_nostack_a;
    delete hs_res_stack_a;
    delete hs_res_nostack_a;
    delete ttlbox;
    curpad = NULL;
    delete leg_a;
    delete leg_text;
    //Clear strings
    //var_name.clear();
    //var_label.clear();

    canv_name.clear();
    hs_stack_name.clear();
    hs_nostack_name.clear();
    hs_res_stack_name.clear();
    hs_res_nostack_name.clear();
    hname_sum.clear();
    hbasename.clear();
  }//variables loop

  if(m_opt->MsgLevel() == Debug::DEBUG) std::cout<<"PlotUtils::OverlayHists end"<<std::endl; 

  return;

}


//========================================================= 
void PlotUtils::MakeTableFromHists (const bool opt_bin){

  //Draw stack now has no meaning or purpose, since the SUM histogram will have already been created
  //Better to use the DoSum option for a sample to determine the order in which it should appear

  //Summed histogram contributions
  //SUM
  //non-stacked contributions

  if(m_opt->MsgLevel() == Debug::DEBUG) std::cout<<"PlotUtils::MakeTableFromHists start"<<std::endl; 

  bool var_draw_stack = 0;
  bool var_do_width = false;
  bool var_isShape = false;

  std::string var_binlabels_str = "";
  std::map<int, std::string> *var_binlabels_map = NULL;

  bool ds_do_sum = false;
  int ds_res_opt = -1;
  std::string ds_res_erropt = "";
  std::string ds_print_text = "";

  std::vector<std::string> glob_moments_list = ParseMomentsTableHeader(m_opt->PrintValue());
  std::string str_moments_header = "";
  if(!m_opt->PrintValue().empty()){
    for( std::string moment : glob_moments_list){
      if(!str_moments_header.empty()){ str_moments_header += " & "; }
      str_moments_header += moment;
    }
    str_moments_header += " \\\\";
  }

  std::vector<std::string> ds_moments_list; ds_moments_list.clear();

  //One preliminary loop to find the baseline sample
  std::string s_base_name = ""; 
  std::string s_base_suffix = "";
  std::string s_base_leglabel = "";

  for(SampleAttributesMap::iterator at_it = m_attrbt_map.begin(); at_it != m_attrbt_map.end(); ++at_it){
    if(at_it->second->ResOpt() == 1){
      if(s_base_name != ""){ std::cout<<"Error: More than one baseline specified for residual calculation"<<std::endl; }
      else{ 
	s_base_name = at_it->first;
	s_base_suffix = at_it->second->Suffix();
	s_base_leglabel = at_it->second->LegLabel();
      }
    }
  } 

  std::vector<std::string> v_str_sum_a;   v_str_sum_a.clear();
  std::vector<std::string> v_str_nosum_a; v_str_nosum_a.clear();
  std::vector<std::string> v_str_res_a;   v_str_res_a.clear();
  bool line_start = true;

  std::string header = " Samples";
  VariableAttributesMap* var_loop_map =  &m_var_map;
  for(VariableAttributesMap::iterator va_it = var_loop_map->begin(); va_it != var_loop_map->end(); ++va_it){

    const std::string& var_blinding = (va_it->second->Blinding() != "") ?  va_it->second->Blinding() : m_opt->Blinding();
    bool var_blind_yield = (var_blinding.find("YIELD") != std::string::npos);

    std::string var_name       = va_it->second->Name();
 
    const std::string& var_label         = ( va_it->second->Label() != "" ) ? va_it->second->Label() : va_it->second->Name();
    const std::string& var_draw_res      = va_it->second->DrawRes();
    const std::string& var_draw_res_err  = va_it->second->DrawResErr();

    var_isShape        = (va_it->second->DoScale() == "SHAPE"); 
    var_do_width       = va_it->second->DoWidth();
    var_draw_stack     = va_it->second->DrawStack();

    var_binlabels_str = va_it->second->BinLabelsStr();
    var_binlabels_map = va_it->second->BinLabelsMap();

    v_str_sum_a.clear();
    v_str_nosum_a.clear();
    v_str_res_a.clear();
    line_start = true;

    //==================================================

    std::string hname_sum = m_drawSum ? var_name + "_" + m_attrbt_map["SUM"]->Suffix() : "";
    bool drawRes = ( (var_draw_res != "") && (var_draw_res != "NONE") );
    if(drawRes && (s_base_name == "") ){std::cout<<"No reference sample specified for residual calculation"<<std::endl;}

    std::string hbasename = var_name + "_" + s_base_suffix;
    TH1D* hist_base = m_hstMngr->GetTH1D(hbasename);

    //---------------------------------------------------------

    for(SampleAttributesMap::iterator at_it = m_attrbt_map.begin(); at_it != m_attrbt_map.end(); ++at_it){
      if( var_isShape ){
	if( !var_draw_stack && ( (at_it->first == "SUM")  || at_it->second->NoShape() ) ){continue;}
      }

      if(at_it->first == "BLINDER"){continue;}

      const std::string& ds_name = at_it->second->Name();
      const std::string& ds_suffix = at_it->second->Suffix();
      const std::string& ds_leglabel = at_it->second->LegLabel();
      const std::string& ds_print_format = (at_it->second->PrintFormat() != "") ? at_it->second->PrintFormat() : m_opt->PrintFormat();
      const std::string& ds_print_value =  at_it->second->PrintValue(); 
      ds_moments_list.clear();
      ds_moments_list = !(ds_print_value.empty()) ? ParseMomentsTableHeader(ds_print_value) : ParseMomentsTableHeader(m_opt->PrintValue());

      ds_res_erropt = "";
      //bool ds_isShape = (at_it->second->DrawScale() == "SHAPE"); 
      ds_do_sum = at_it->second->DoSum();
      ds_res_opt = at_it->second->ResOpt();
      bool ds_isBlind = (ds_name == m_opt->BlindSample());

      std::string hist_name = var_name + "_" + ds_suffix;
      TH1D* hist_a = m_hstMngr->GetTH1D(hist_name);

      ds_print_text = "";
      if( opt_bin ){ ds_print_text += MakeHistTableRow(hist_a, ds_print_format, true, var_binlabels_map); }
      else{  
	if(var_blind_yield && ds_isBlind){
	  for(unsigned int i = 0; i < ds_moments_list.size(); i++){
	    if(!ds_print_text.empty()){ ds_print_text += " &"; }
	    ds_print_text += " "; 
	  }
	}
	else{ ds_print_text += MakeMomentsTableRow(hist_a, ds_moments_list, ds_print_format, var_do_width); }
      }
      
      ds_print_text = (line_start) ? ds_leglabel + "  &  " + ds_print_text : "  &  " + ds_print_text;
      std::cout << " ds_print_text = " << ds_print_text << std::endl;
      //============================================================================================
      if(var_draw_stack && ds_do_sum){ v_str_sum_a.push_back(ds_print_text); }
      else{ v_str_nosum_a.push_back(ds_print_text); }
      
      if(drawRes && ( (ds_res_opt == 0) || ((ds_res_opt == 1) && (var_draw_res_err == "REFBAND")) ) ){
	std::string resname_a = var_name + "_" + ds_suffix + "_res_tab_" + s_base_suffix;
	TH1D* hist_res_a = makeResidual(resname_a, hist_name, hbasename, var_draw_res, ds_res_erropt);
	std::string res_print_text = ""; 
	if( opt_bin ){  res_print_text = MakeHistTableRow(hist_res_a, ds_print_format, true, var_binlabels_map); }
	else{ res_print_text = MakeResidualMomentsTableRow(hist_a, hist_base, ds_moments_list, ds_print_format, var_do_width);	}
	res_print_text = (line_start) ? ds_leglabel+"/"+s_base_leglabel + "  &  " + res_print_text : "  &  " + res_print_text;

	v_str_res_a.push_back(res_print_text);
	//delete hist_res_a;
	resname_a.clear(); res_print_text.clear();
      }//if residual histogram needed 
      
      //Clear strings
      hist_name.clear();
    }//sample loop
    line_start = false;

    //=================================== PRINT THE TABLE =========================================================
    std::string s_IFP_DIR = (opt_bin) ? "IFP_TABLES_BINS" : "IFP_TABLES_MOMENTS";
    std::string var_outdir = va_it->second->OutputFolder();
    if(var_outdir != ""){
      if(var_outdir.substr(var_outdir.size()-1) != "/"){var_outdir += "/";}
      gSystem->mkdir(Form("%s/%s/%s" , s_IFP_DIR.c_str(),m_output_dir.c_str(), var_outdir.c_str()), "TRUE");
    }
    std::ofstream var_tab_file; 
    var_tab_file.open(Form("%s/%s/%s%s.txt" , s_IFP_DIR.c_str(),m_output_dir.c_str(), var_outdir.c_str() ,var_name.c_str())); 
    var_tab_file << "============== " << var_label << "============== " << std::endl;
    if(v_str_sum_a.size() > 0){
      for(std::string row : v_str_sum_a){ var_tab_file << row << " \\\\"<< std::endl; }
      var_tab_file << "\\hline \\\\"<<std::endl;
    }
    if(v_str_nosum_a.size() > 0){
      for(std::string row : v_str_nosum_a){ var_tab_file << row << " \\\\" << std::endl; }
    }
    if(v_str_res_a.size() > 0){
      var_tab_file << "\\hline\\hline \\\\"<<std::endl;
      for(std::string row : v_str_res_a){ var_tab_file << row << " \\\\" << std::endl; }
    }

    v_str_sum_a.clear();
    v_str_sum_a.clear();
    v_str_sum_a.clear();
      
    var_outdir.clear();


    hname_sum.clear();
    hbasename.clear();
  }//variables loop

  if(m_opt->MsgLevel() == Debug::DEBUG) std::cout<<"PlotUtils::OverlayHists end"<<std::endl; 

  return;

}
//================================================================================

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
  if( ! ( (opt == "RATIO") || (opt == "INVRATIO") || (opt == "DIFF") || (opt == "RESIDUAL") || (opt == "RATIOUNC") ) ){
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
      else if(opt == "RATIOUNC"){
	cont_fe = (cont_c > 0.) ? cont_e/cont_c : 0.;
	ref_fe = (ref_c > 0.) ? ref_e/ref_c : 0.;
	res_bc = (ref_fe>0.) ? cont_fe/ref_fe : 0.;
	res_be = 0.;
      }

      else if( opt == "RATIO"){
	if(b_scale_err){ res_be = (ref_c > 0.) ? cont_e/ref_c : 0.; }
      }
      else if( opt == "DIFF"){
	if(b_scale_err){ res_be = cont_e; }
      }

      if( b_selfdivide ){
	if( (opt=="RATIO")|| (opt=="INVRATIO") || opt == ("RESIDUAL") || (opt=="RATIOUNC") ) { hist_res->SetBinContent(b, 1.); }
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

int PlotUtils::SetStyleCanvas(TCanvas& canv, bool divide, double bottommargin, double topmargin, double leftmargin, double rightmargin){

  if(divide){canv.Divide(1,2);
    canv.cd(1)->SetPad(0.,0.35,0.95,0.95);
    canv.cd(1)->SetTicks();
    canv.cd(1)->SetBorderMode(0);
    canv.cd(1)->SetTopMargin(topmargin);
    canv.cd(1)->SetBottomMargin(0.0001);
    canv.cd(1)->SetRightMargin(rightmargin);
    canv.cd(1)->SetLeftMargin(leftmargin);

    canv.cd(2)->SetPad(0.,0.,0.95,0.35);
    canv.cd(2)->SetTicks();
    canv.cd(2)->SetBorderMode(0);
    canv.cd(2)->SetTopMargin(0.0001);
    canv.cd(2)->SetBottomMargin(bottommargin);
    canv.cd(2)->SetRightMargin(rightmargin);
    canv.cd(2)->SetLeftMargin(leftmargin);
  }
  else{
    canv.cd()->SetTicks();
    canv.cd()->SetTopMargin(topmargin);
    canv.cd()->SetBottomMargin(bottommargin);
    canv.cd()->SetRightMargin(rightmargin);
    canv.cd()->SetLeftMargin(leftmargin);
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
  leg.SetLineStyle(0);
  leg.SetLineWidth(0);
  leg.SetMargin(margin);
  leg.SetBorderSize(0);
  leg.SetTextSize(textsize);
  leg.SetTextFont(textfont);
  leg.SetShadowColor(0);

  return 0;
}

int PlotUtils::ResizeLegend(TLegend& leg, double xpt, double ypt, const std::string& justify){

  float textsize = leg.GetTextSize();
  float nrows = (float)leg.GetNRows();
  //float margin = leg.GetMargin();
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
  float delX = maxlsize;//*(1.+margin);
  //float delX = maxlsize*(1.+margin+0.1);//*textsize;
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


std::vector<std::string> PlotUtils::ParseMomentsTableHeader(const std::string& header_line, const std::string& delim ){

  std::vector<std::string> moment_list; moment_list.clear();

  //Header gives the sequence of configuration variables
  std::string moment; moment.clear();
  std::string lineString = header_line;
  std::string::size_type pos = 0;
  do{ 
    pos = AnalysisUtils::ParseString(lineString, moment, delim);
    AnalysisUtils::TrimString(moment);
    moment_list.push_back(moment);
  }while(pos != std::string::npos);

  return moment_list;

}

std::string PlotUtils::MakeMomentText(TH1D* hist, const std::string& moment, const std::string& print_format){

  std::string moment_text = "";
  if(moment == "YIELD"){
    moment_text = Form(print_format.c_str(), hist->Integral());
  }
  else if(moment == "ENTRIES"){
    moment_text = Form(print_format.c_str(), hist->GetEntries());
  }
  else if(moment == "MEAN"){
    moment_text = Form(print_format.c_str(), hist->GetMean());
  }
  else if(moment == "RMS"){
    moment_text = Form(print_format.c_str(), hist->GetRMS());
  }

  else if(moment == "YIELDANDERROR"){
    double err = 0.;
    double intgl = hist->IntegralAndError(0.,-1.,err);
    moment_text = Form(print_format.c_str(), intgl, err);
  }
  else if(moment == "MEANANDERROR"){
    moment_text = Form(print_format.c_str(), hist->GetMean(), hist->GetMeanError());
  }

  return moment_text;
}


std::string PlotUtils::MakeResidualMomentText(TH1D* hist, TH1D* href, const std::string& moment, const std::string& print_format){

  std::string moment_text = "";

  if(moment == "YIELD"){
    moment_text = (href->Integral() != 0.) ? Form(print_format.c_str(), hist->Integral()/href->Integral()) : Form(print_format.c_str(), 0.);
  }
  else if(moment == "ENTRIES"){
    moment_text = (href->GetEntries() != 0.) ? Form(print_format.c_str(), hist->GetEntries()/href->GetEntries()) : Form(print_format.c_str(), 0.);
  }
  else if(moment == "MEAN"){
    moment_text = (href->GetMean() != 0.) ? Form(print_format.c_str(), hist->GetMean()/href->GetMean()) : Form(print_format.c_str(), 0.);
  }
  else if(moment == "RMS"){
    moment_text = (href->GetRMS() != 0.) ? Form(print_format.c_str(), hist->GetRMS()/href->GetRMS()) : Form(print_format.c_str(), 0.);
  }

  else if(moment == "YIELDANDERROR"){
    double err = 0.; double err_ref = 0.;
    double intgl = hist->IntegralAndError(0.,-1.,err);
    double intgl_ref = href->IntegralAndError(0.,-1.,err_ref);

    double intgl_ratio = (intgl_ref != 0.) ? intgl/intgl_ref : 0.;

    double ferr = (intgl != 0 ) ? err/intgl : 0.;
    double ferr_ref = (intgl_ref != 0 ) ? err_ref/intgl_ref : 0.;

    double err_ratio = intgl_ratio*sqrt(ferr*ferr + ferr_ref*ferr_ref);

    moment_text = Form(print_format.c_str(), intgl_ratio, err_ratio);
  }
  else if(moment == "MEANANDERROR"){
    double mean = hist->GetMean();
    double err = hist->GetMeanError();

    double mean_ref = href->GetMean();
    double err_ref = href->GetMeanError();

    double mean_ratio = (mean_ref != 0.) ? mean/mean_ref : 0.;

    double ferr = (mean != 0 ) ? err/mean : 0.;
    double ferr_ref = (mean_ref != 0 ) ? err_ref/mean_ref : 0.;

    double err_ratio = mean_ratio*sqrt(ferr*ferr + ferr_ref*ferr_ref);

    moment_text = Form(print_format.c_str(), mean_ratio, err_ratio);
  }

  return moment_text;
}

 

std::string PlotUtils::MakeMomentsTableRow(TH1D* hist, const std::vector<std::string>& moment_list, const std::string& print_format, const bool use_width){

  TH1D* _hist = NULL;
  if(use_width){ _hist = GetHistTimesBinWidth(hist); }
  else{ _hist = hist; }

  std::string row_string="";
  for( std::string moment : moment_list){
    if(!row_string.empty()){ row_string += " & "; } 
    row_string += MakeMomentText(_hist, moment, print_format); 
  }
  //row_string += "\\\\";
  if(use_width){ delete _hist; }

  return row_string;

}

 std::string PlotUtils::MakeResidualMomentsTableRow(TH1D* hist, TH1D* href, const std::vector<std::string>& moment_list, const std::string& print_format, const bool /*use_width*/){

  std::string row_string="";
  for( std::string moment : moment_list){
    if(!row_string.empty()){ row_string += " & "; } 
    row_string += MakeResidualMomentText(hist, href, moment, print_format); 
  }
  //row_string += "\\\\";

  return row_string;

}


//Given bin labels, use them
//If not given, use the labels already set in the bins
std::string PlotUtils::MakeHistTableRow(TH1D* hist, const std::string& print_format, const bool print_error, const std::map<int, std::string> *bin_labels){

  std::string row_string="";
  for(int ibin = 1; ibin < hist->GetNbinsX(); ibin++){
    if( bin_labels  && (bin_labels->size() > 0) && (bin_labels->find(ibin)==bin_labels->end()) ){ continue; }

    if(!row_string.empty()){ row_string += " & "; }
    if(print_error){
      row_string += Form(print_format.c_str(), hist->GetBinContent(ibin), hist->GetBinError(ibin));
    }
    else{
      row_string += Form(print_format.c_str(), hist->GetBinContent(ibin));
    }
  }
  row_string += "\\\\";

  return row_string;

}

/*
TH1D* PlotUtils::ShiftHistBins(TH1D* hist, double var_binshift){

  int nbin = hist->GetNbinsX();
  double oldmin = hist->GetBinLowEdge(1);
  double oldmax = hist->GetBinLowEdge(nbin) + hist->GetBinWidth(nbin);
  TH1D* hist_shifted = new TH1D(hist->GetName(), hist->GetTitle(), nbin, oldmin + var_binshift, oldmax+var_binshift);
  for(int i = 1; i <= nbin; i++){
    hist_shifted->SetBinContent( i, hist->GetBinContent(i) );
    hist_shifted->SetBinError( i, hist->GetBinError(i) );
  }
  hist_shifted->SetEntries( hist->GetEntries() );
  m_hstMngr->ReplaceTH1D(keysum, hist_shifted);
  hist = hist_shifted;

}//shift bin edges
*/

TH1D* PlotUtils::GetHistByBinWidth(TH1D* hist){

  TH1D* _hist = NULL;
  std::string _histname = hist->GetName();
  _histname += "_by_width";
  _hist = (TH1D*)(hist->Clone(_histname.c_str()));
  for(int i = 1; i<= hist->GetNbinsX(); i++){
    _hist->SetBinContent(i, hist->GetBinContent(i)/hist->GetBinWidth(i));
    _hist->SetBinError(i, hist->GetBinError(i)/hist->GetBinWidth(i));
  }
  _hist->SetEntries(hist->GetEntries());

  return _hist;
}

TH1D* PlotUtils::GetHistTimesBinWidth(TH1D* hist){

  TH1D* _hist = NULL;
  std::string _histname = hist->GetName();
  _histname += "_times_weighted";
  _hist = (TH1D*)(hist->Clone(_histname.c_str()));
  for(int i = 1; i<= hist->GetNbinsX(); i++){
    _hist->SetBinContent(i, hist->GetBinContent(i)*hist->GetBinWidth(i));
    _hist->SetBinError(i, hist->GetBinError(i)*hist->GetBinWidth(i));
  }
  _hist->SetEntries(hist->GetEntries());

  return _hist;
}
