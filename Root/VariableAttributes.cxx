#include "IFAEPlotter/VariableAttributes.h"
#include "IFAEPlotter/Plotter_Options.h"
#include "IFAETopFramework/AnalysisUtils.h"
#include <iostream>

VariableAttributes::VariableAttributes(const std::string& name, const std::string& label, const std::string& do_scale
					, bool do_width, bool draw_stack, const std::string& draw_res, const std::string& draw_res_err
					, bool isLogY, bool isLogX
					, const std::string& ylabel, const std::string& reslabel
					, bool has_resmin, bool has_resmax, double resmin, double resmax
					, bool has_ymin, bool has_ymax, bool has_yscale
					, double ymin, double ymax, double yscale 
					, bool has_xmin, bool has_xmax, double xmin, double xmax
					
					, bool has_ttl_xmin, bool has_ttl_xmax, double ttl_xmin, double ttl_xmax
					, bool has_ttl_ymin, bool has_ttl_ymax, double ttl_ymin, double ttl_ymax   
					, bool has_ttl_textsize, double ttl_textsize 
					, bool has_leg_textsize, double leg_textsize 
					
					, bool has_leg_ymin, bool has_leg_ymax
					, double leg_ymin, double leg_ymax 
					, bool has_leg_xmin, bool has_leg_xmax
					, double leg_xmin, double leg_xmax 
					
					, bool has_xtitle_size, bool has_xtitle_offset
					, double xtitle_size, double xtitle_offset
					, bool has_ytitle_size, bool has_ytitle_offset
					, double ytitle_size, double ytitle_offset
					, bool has_restitle_size, bool has_restitle_offset
					, double restitle_size, double restitle_offset

					, bool has_xlabel_size, bool has_xlabel_offset
					, double xlabel_size, double xlabel_offset
					, bool has_ylabel_size, bool has_ylabel_offset
					, double ylabel_size, double ylabel_offset
					, bool has_reslabel_size, bool has_reslabel_offset
					, double reslabel_size, double reslabel_offset
				       
				       , bool has_xaxis_ndiv, int xaxis_ndiv
				       , bool has_yaxis_ndiv, int yaxis_ndiv
				       , bool has_resaxis_ndiv, int resaxis_ndiv
				       
					, bool has_bottom_margin, bool has_top_margin
					, bool has_left_margin, bool has_right_margin
					, double bottom_margin, double top_margin
					, double left_margin, double right_margin
					, bool is_count 
					, const std::string& resdrawopt, const std::string& extralabel
				       , int rebin, const std::string& rebinedges, double binshift, const std::string& output_folder, const std::string& blinding) :
  
  m_name(name),
  m_label(label),
  m_ylabel(ylabel),
  m_reslabel(reslabel),
  m_draw_stack(draw_stack),
  m_do_scale(do_scale),
  m_draw_res(draw_res),
  m_draw_res_err(draw_res_err),
  m_is_logY(isLogY),
  m_is_logX(isLogX),
  m_rebin(rebin),
  m_rebinedges(rebinedges),
  m_binshift(binshift),
  m_do_width(do_width),
  m_resmin(resmin),
  m_resmax(resmax),
  m_has_resmin(has_resmin),
  m_has_resmax(has_resmax),
  m_ymin(ymin),
  m_ymax(ymax),
  m_yscale(yscale),
  m_has_ymin(has_ymin),
  m_has_ymax(has_ymax),
  m_has_yscale(has_yscale),
  m_xmin(xmin),
  m_xmax(xmax),
  m_has_xmin(has_xmin),
  m_has_xmax(has_xmax),
  m_ttl_xmin(ttl_xmin),
  m_ttl_ymin(ttl_ymin),
  m_ttl_xmax(ttl_xmax),
  m_ttl_ymax(ttl_ymax),
  m_ttl_textsize(ttl_textsize),
  m_leg_textsize(leg_textsize),
  m_has_ttl_xmin(has_ttl_xmin),
  m_has_ttl_ymin(has_ttl_ymin),
  m_has_ttl_xmax(has_ttl_xmax),
  m_has_ttl_ymax(has_ttl_ymax),
  m_has_ttl_textsize(has_ttl_textsize),
  m_has_leg_textsize(has_leg_textsize),
  
  m_has_leg_xmin(has_leg_xmin),
  m_has_leg_xmax(has_leg_xmax),
  m_has_leg_ymin(has_leg_ymin),
  m_has_leg_ymax(has_leg_ymax),

  m_leg_xmin(leg_xmin),
  m_leg_xmax(leg_xmax),
  m_leg_ymin(leg_ymin),
  m_leg_ymax(leg_ymax),

  m_has_xtitle_size(has_xtitle_size),
  m_has_xtitle_offset(has_xtitle_offset),
  m_has_ytitle_size(has_ytitle_size),
  m_has_ytitle_offset(has_ytitle_offset),
  m_has_restitle_size(has_restitle_size),
  m_has_restitle_offset(has_restitle_offset),

  m_xtitle_size(xtitle_size),
  m_xtitle_offset(xtitle_offset),
  m_ytitle_size(ytitle_size),
  m_ytitle_offset(ytitle_offset),
  m_restitle_size(restitle_size),
  m_restitle_offset(restitle_offset),
 
  m_has_xlabel_size(has_xlabel_size),
  m_has_xlabel_offset(has_xlabel_offset),
  m_has_ylabel_size(has_ylabel_size),
  m_has_ylabel_offset(has_ylabel_offset),
  m_has_reslabel_size(has_reslabel_size),
  m_has_reslabel_offset(has_reslabel_offset),

  m_xlabel_size(xlabel_size),
  m_xlabel_offset(xlabel_offset),
  m_ylabel_size(ylabel_size),
  m_ylabel_offset(ylabel_offset),
  m_reslabel_size(reslabel_size),
  m_reslabel_offset(reslabel_offset),

  m_has_xaxis_ndiv(has_xaxis_ndiv),
  m_has_yaxis_ndiv(has_yaxis_ndiv),
  m_has_resaxis_ndiv(has_resaxis_ndiv),

  m_xaxis_ndiv(xaxis_ndiv),
  m_yaxis_ndiv(yaxis_ndiv),
  m_resaxis_ndiv(resaxis_ndiv),

  m_has_bottom_margin(has_bottom_margin),
  m_has_top_margin(has_top_margin),
  m_has_left_margin(has_left_margin),
  m_has_right_margin(has_right_margin),

  m_bottom_margin(bottom_margin),
  m_top_margin(top_margin),
  m_left_margin(left_margin),
  m_right_margin(right_margin),

  m_is_count(is_count),
  m_nprojbin(-1),
  m_resdrawopt(resdrawopt),
  m_blinding(blinding),
  m_extralabel(extralabel),
  m_output_folder(output_folder)
{ }

VariableAttributes::VariableAttributes() : VariableAttributes("", "", "NONE"){}

VariableAttributes::VariableAttributes(VariableAttributes& q){
  m_name               = q.m_name;
  m_label              = q.m_label;
  m_ylabel             = q.m_ylabel;
  m_reslabel           = q.m_reslabel;
  m_draw_stack         = q.m_draw_stack;
  m_do_scale           = q.m_do_scale;
  m_draw_res           = q.m_draw_res;
  m_draw_res_err       = q.m_draw_res_err;
  m_is_logY            = q.m_is_logY;
  m_is_logX            = q.m_is_logX;
  m_rebin              = q.m_rebin;
  m_rebinedges         = q.m_rebinedges;
  m_has_binshift       = q.m_has_binshift;
  m_binshift           = q.m_binshift;
  m_do_width           = q.m_do_width;
  m_resmin             = q.m_resmin;
  m_resmax             = q.m_resmax;
  m_has_resmin         = q.m_has_resmin;
  m_has_resmax         = q.m_has_resmax;
  m_ymin               = q.m_ymin;
  m_ymax               = q.m_ymax;
  m_yscale             = q.m_yscale;
  m_has_ymin           = q.m_has_ymin;
  m_has_ymax           = q.m_has_ymax;
  m_has_yscale         = q.m_has_yscale;
  m_xmin               = q.m_xmin;
  m_xmax               = q.m_xmax;
  m_has_xmin           = q.m_has_xmin;
  m_has_xmax           = q.m_has_xmax;
  m_ttl_xmin           = q.m_ttl_xmin;
  m_ttl_ymin           = q.m_ttl_ymin;
  m_ttl_xmax           = q.m_ttl_xmax;
  m_ttl_ymax           = q.m_ttl_ymax;
  m_ttl_textsize       = q.m_ttl_textsize;
  m_leg_textsize       = q.m_leg_textsize;
  m_has_ttl_xmin       = q.m_has_ttl_xmin;
  m_has_ttl_ymin       = q.m_has_ttl_ymin;
  m_has_ttl_xmax       = q.m_has_ttl_xmax;
  m_has_ttl_ymax       = q.m_has_ttl_ymax;
  m_has_ttl_textsize   = q.m_has_ttl_textsize;
  m_has_leg_textsize   = q.m_has_leg_textsize;

  m_has_leg_xmin       = q.m_has_leg_xmin;
  m_has_leg_xmax       = q.m_has_leg_xmax;
  m_has_leg_ymin       = q.m_has_leg_ymin;
  m_has_leg_ymax       = q.m_has_leg_ymax;

  m_leg_xmin           = q.m_leg_xmin;
  m_leg_xmax           = q.m_leg_xmax;
  m_leg_ymin           = q.m_leg_ymin;
  m_leg_ymax           = q.m_leg_ymax;

  m_has_xtitle_size    = q.m_has_xtitle_size;
  m_has_xtitle_offset  = q.m_has_xtitle_offset;
  m_has_ytitle_size    = q.m_has_ytitle_size;
  m_has_ytitle_offset  = q.m_has_ytitle_offset;
  m_has_restitle_size  = q.m_has_restitle_size;
  m_has_restitle_offset= q.m_has_restitle_offset;

  m_xtitle_size        = q.m_xtitle_size;
  m_xtitle_offset      = q.m_xtitle_offset;
  m_ytitle_size        = q.m_ytitle_size;
  m_ytitle_offset      = q.m_ytitle_offset;
  m_restitle_size      = q.m_restitle_size;
  m_restitle_offset    = q.m_restitle_offset;

  m_has_xlabel_size    = q.m_has_xlabel_size;
  m_has_xlabel_offset  = q.m_has_xlabel_offset;
  m_has_ylabel_size    = q.m_has_ylabel_size;
  m_has_ylabel_offset  = q.m_has_ylabel_offset;
  m_has_reslabel_size  = q.m_has_reslabel_size;
  m_has_reslabel_offset= q.m_has_reslabel_offset;

  m_xlabel_size        = q.m_xlabel_size;
  m_xlabel_offset      = q.m_xlabel_offset;
  m_ylabel_size        = q.m_ylabel_size;
  m_ylabel_offset      = q.m_ylabel_offset;
  m_reslabel_size      = q.m_reslabel_size;
  m_reslabel_offset    = q.m_reslabel_offset;

  m_has_bottom_margin  = q.m_has_bottom_margin;
  m_has_top_margin     = q.m_has_top_margin;
  m_has_left_margin    = q.m_has_left_margin;
  m_has_right_margin   = q.m_has_right_margin;

  m_has_xaxis_ndiv     = q.m_has_xaxis_ndiv;
  m_has_yaxis_ndiv     = q.m_has_yaxis_ndiv;
  m_has_resaxis_ndiv   = q.m_has_resaxis_ndiv;

  m_xaxis_ndiv         = q.m_xaxis_ndiv;
  m_yaxis_ndiv         = q.m_yaxis_ndiv;
  m_resaxis_ndiv       = q.m_resaxis_ndiv;

  m_bottom_margin      = q.m_bottom_margin;
  m_top_margin         = q.m_top_margin;
  m_left_margin        = q.m_left_margin;
  m_right_margin       = q.m_right_margin;

  m_is_count           = q.m_is_count;
  m_nprojbin           = q.m_nprojbin;
  m_resdrawopt         = q.m_resdrawopt;
  m_blinding           = q.m_blinding;
  m_extralabel         = q.m_extralabel;
  m_output_folder      = q.m_output_folder;

}

VariableAttributesMap VariableAttributes::ParseVariableConfig( Plotter_Options* opt ){

  const std::string& config_variable = opt->ConfigVariable(); 
  const std::string& delim = opt->ConfigDelim();
  bool block_format = false;
  if( opt->OptStr().find("--NEWVARIABLECONFIG") != std::string::npos ){ block_format = opt->NewVariableFormat(); }
  else if( opt->OptStr().find("--NEWCONFIG") != std::string::npos ){ block_format = opt->NewConfigFormat(); }

  VariableAttributesMap var_map; var_map.clear();
  std::vector<std::map<std::string, std::string> > parsed_map;
  int nline = AnalysisUtils::ParseConfigFile(config_variable, parsed_map, delim, block_format); 
  if(nline < 0){ std::cout<<"Variable configuration file could not be opened. Exiting"<<std::endl; return var_map; }

  std::string name = "";

  for(int l = 0; l < nline; l++){
    name = "";

    std::map<std::string, std::string>& keymap = parsed_map.at(l);

    if( keymap.find("NAME") == keymap.end() ){ 
      std::cout<<"Error : No name found for variable in block "<<l<<". Ignoring."<<std::endl;
      continue;
    }
    VariableAttributes* varObj = new VariableAttributes();
    name = keymap["NAME"];
    varObj->SetName(name);

    if( keymap.find("LABEL") != keymap.end() ){ varObj->SetLabel(keymap["LABEL"]); }
    if( keymap.find("YLABEL") != keymap.end() ){ varObj->SetYLabel(keymap["YLABEL"]); }
    if( keymap.find("RESLABEL") != keymap.end() ){ varObj->SetResLabel(keymap["RESLABEL"]);}
    if( keymap.find("EXTRALABEL") != keymap.end() ){ varObj->SetExtraLabel(keymap["EXTRALABEL"]);}
    if( keymap.find("DRAWSTACK") != keymap.end() ){ varObj->SetDrawStack(AnalysisUtils::BoolValue(keymap["DRAWSTACK"], "DRAWSTACK")); }
    if( keymap.find("DRAWRES") != keymap.end() ){ varObj->SetDrawRes(keymap["DRAWRES"]); }
    if( keymap.find("DRAWRESERR") != keymap.end() ){ varObj->SetDrawResErr(keymap["DRAWRESERR"]);}
    if( keymap.find("ISLOGY") != keymap.end() ){ varObj->SetIsLogY(AnalysisUtils::BoolValue(keymap["ISLOGY"], "ISLOGY")); }
    if( keymap.find("ISLOGX") != keymap.end() ){ varObj->SetIsLogX(AnalysisUtils::BoolValue(keymap["ISLOGX"], "ISLOGX")); }
 

    if( keymap.find("REBIN") != keymap.end() ){ varObj->SetRebin(atoi(keymap["REBIN"].c_str())); }
    if( keymap.find("REBINVAR") != keymap.end() ){ varObj->SetRebinEdges(keymap["REBINVAR"]); }
    if( keymap.find("DOSCALE") != keymap.end() ){ varObj->SetDoScale(keymap["DOSCALE"]); }
    if( keymap.find("DOWIDTH") != keymap.end() ){ varObj->SetDoWidth(AnalysisUtils::BoolValue(keymap["DOWIDTH"], "DOWIDTH")); }
    if( keymap.find("RESDRAWOPT") != keymap.end() ){ varObj->SetResDrawOpt(keymap["RESDRAWOPT"]); }
    if( keymap.find("BLINDING") != keymap.end() ){ varObj->SetBlinding(keymap["BLINDING"]); }
    if( keymap.find("OUTPUTFOLDER") != keymap.end() ){ varObj->SetOutputFolder(keymap["OUTPUTFOLDER"]); }

    if( keymap.find("BINSHIFT") != keymap.end() && (keymap["BINSHIFT"] != "") ){ 
      varObj->SetBinShift(atof(keymap["BINSHIFT"].c_str())); 
      varObj->SetHasBinShift(true);
    }

    if( keymap.find("RESMIN") != keymap.end() && (keymap["RESMIN"] != "") ){ 
      varObj->SetResMin(atof(keymap["RESMIN"].c_str())); 
      varObj->SetHasResMin(true);
    }
    if( keymap.find("RESMAX") != keymap.end() && (keymap["RESMAX"] != "") ){ 
      varObj->SetResMax(atof(keymap["RESMAX"].c_str())); 
      varObj->SetHasResMax(true);
    }
    if( keymap.find("YMIN") != keymap.end() && (keymap["YMIN"] != "") ){ 
      varObj->SetYMin(atof(keymap["YMIN"].c_str())); 
      varObj->SetHasYMin(true);
    }
    if( keymap.find("YMAX") != keymap.end() && (keymap["YMAX"] != "") ){ 
      varObj->SetYMax(atof(keymap["YMAX"].c_str())); 
      varObj->SetHasYMax(true);
    }
    if( keymap.find("YSCALE") != keymap.end() && (keymap["YSCALE"] != "") ){ 
      varObj->SetYScale(atof(keymap["YSCALE"].c_str())); 
      varObj->SetHasYScale(true);
    }
    if( keymap.find("XMIN") != keymap.end() && (keymap["XMIN"] != "") ){ 
      varObj->SetXMin(atof(keymap["XMIN"].c_str())); 
      varObj->SetHasXMin(true);
    }
    if( keymap.find("XMAX") != keymap.end() && (keymap["XMAX"] != "") ){ 
      varObj->SetXMax(atof(keymap["XMAX"].c_str())); 
      varObj->SetHasXMax(true);
    }

    if( keymap.find("TITLEXMIN") != keymap.end() && (keymap["TITLEXMIN"] != "") ){ 
      varObj->SetTitleXMin(atof(keymap["TITLEXMIN"].c_str())); 
      varObj->SetHasTitleXMin(true);
    }
    if( keymap.find("TITLEXMAX") != keymap.end() && (keymap["TITLEXMAX"] != "") ){ 
      varObj->SetTitleXMax(atof(keymap["TITLEXMAX"].c_str())); 
      varObj->SetHasTitleXMax(true);
    }
    //
    if( keymap.find("TITLEYMIN") != keymap.end() && (keymap["TITLEYMIN"] != "") ){ 
      varObj->SetTitleYMin(atof(keymap["TITLEYMIN"].c_str())); 
      varObj->SetHasTitleYMin(true);
    }
    if( keymap.find("TITLEYMAX") != keymap.end() && (keymap["TITLEYMAX"] != "") ){ 
      varObj->SetTitleYMax(atof(keymap["TITLEYMAX"].c_str())); 
      varObj->SetHasTitleYMax(true);
    }
    //
    if( keymap.find("TITLETEXTSIZE") != keymap.end() && (keymap["TITLETEXTSIZE"] != "") ){ 
      varObj->SetTitleTextSize(atof(keymap["TITLETEXTSIZE"].c_str())); 
      varObj->SetHasTitleTextSize(true);
    }
    if( keymap.find("LEGENDTEXTSIZE") != keymap.end() && (keymap["LEGENDTEXTSIZE"] != "") ){ 
      varObj->SetLegendTextSize(atof(keymap["LEGENDTEXTSIZE"].c_str())); 
      varObj->SetHasLegendTextSize(true);
    }

    if( keymap.find("LEGENDXMIN") != keymap.end() && (keymap["LEGENDXMIN"] != "") ){ 
      varObj->SetLegendXMin(atof(keymap["LEGENDXMIN"].c_str())); 
      varObj->SetHasLegendXMin(true);
    }
    if( keymap.find("LEGENDXMAX") != keymap.end() && (keymap["LEGENDXMAX"] != "") ){ 
      varObj->SetLegendXMax(atof(keymap["LEGENDXMAX"].c_str())); 
      varObj->SetHasLegendXMax(true);
    }
    if( keymap.find("LEGENDYMIN") != keymap.end() && (keymap["LEGENDYMIN"] != "") ){ 
      varObj->SetLegendYMin(atof(keymap["LEGENDYMIN"].c_str())); 
      varObj->SetHasLegendYMin(true);
    }
    if( keymap.find("LEGENDYMAX") != keymap.end() && (keymap["LEGENDYMAX"] != "") ){ 
      varObj->SetLegendYMax(atof(keymap["LEGENDYMAX"].c_str())); 
      varObj->SetHasLegendYMax(true);
    }

    //---
    if( keymap.find("XTITLESIZE") != keymap.end() && (keymap["XTITLESIZE"] != "") ){ 
      varObj->SetXTitleSize(atof(keymap["XTITLESIZE"].c_str())); 
      varObj->SetHasXTitleSize(true);
    }
    if( keymap.find("XTITLEOFFSET") != keymap.end() && (keymap["XTITLEOFFSET"] != "") ){ 
      varObj->SetXTitleOffset(atof(keymap["XTITLEOFFSET"].c_str())); 
      varObj->SetHasXTitleOffset(true);
    }
    if( keymap.find("YTITLESIZE") != keymap.end() && (keymap["YTITLESIZE"] != "") ){ 
      varObj->SetYTitleSize(atof(keymap["YTITLESIZE"].c_str())); 
      varObj->SetHasYTitleSize(true);
    }
    if( keymap.find("YTITLEOFFSET") != keymap.end() && (keymap["YTITLEOFFSET"] != "") ){ 
      varObj->SetYTitleOffset(atof(keymap["YTITLEOFFSET"].c_str())); 
      varObj->SetHasYTitleOffset(true);
    }
    if( keymap.find("RESTITLESIZE") != keymap.end() && (keymap["RESTITLESIZE"] != "") ){ 
      varObj->SetResTitleSize(atof(keymap["RESTITLESIZE"].c_str())); 
      varObj->SetHasResTitleSize(true);
    }
    if( keymap.find("RESTITLEOFFSET") != keymap.end() && (keymap["RESTITLEOFFSET"] != "") ){ 
      varObj->SetResTitleOffset(atof(keymap["RESTITLEOFFSET"].c_str())); 
      varObj->SetHasResTitleOffset(true);
    }
    //---
    if( keymap.find("XLABELSIZE") != keymap.end() && (keymap["XLABELSIZE"] != "") ){ 
      varObj->SetXLabelSize(atof(keymap["XLABELSIZE"].c_str())); 
      varObj->SetHasXLabelSize(true);
    }
    if( keymap.find("XLABELOFFSET") != keymap.end() && (keymap["XLABELOFFSET"] != "") ){ 
      varObj->SetXLabelOffset(atof(keymap["XLABELOFFSET"].c_str())); 
      varObj->SetHasXLabelOffset(true);
    }
    if( keymap.find("YLABELSIZE") != keymap.end() && (keymap["YLABELSIZE"] != "") ){ 
      varObj->SetYLabelSize(atof(keymap["YLABELSIZE"].c_str())); 
      varObj->SetHasYLabelSize(true);
    }
    if( keymap.find("YLABELOFFSET") != keymap.end() && (keymap["YLABELOFFSET"] != "") ){ 
      varObj->SetYLabelOffset(atof(keymap["YLABELOFFSET"].c_str())); 
      varObj->SetHasYLabelOffset(true);
    }
    if( keymap.find("RESLABELSIZE") != keymap.end() && (keymap["RESLABELSIZE"] != "") ){ 
      varObj->SetResLabelSize(atof(keymap["RESLABELSIZE"].c_str())); 
      varObj->SetHasResLabelSize(true);
    }
    if( keymap.find("RESLABELOFFSET") != keymap.end() && (keymap["RESLABELOFFSET"] != "") ){ 
      varObj->SetResLabelOffset(atof(keymap["RESLABELOFFSET"].c_str())); 
      varObj->SetHasResLabelOffset(true);
    }

    //------------
    if( keymap.find("XAXISNDIV") != keymap.end() && (keymap["XAXISNDIV"] != "") ){ 
      varObj->SetXAxisNdiv(atoi(keymap["XAXISNDIV"].c_str())); 
      varObj->SetHasXAxisNdiv(true);
    }
    if( keymap.find("YAXISNDIV") != keymap.end() && (keymap["YAXISNDIV"] != "") ){ 
      varObj->SetYAxisNdiv(atoi(keymap["YAXISNDIV"].c_str())); 
      varObj->SetHasYAxisNdiv(true);
    }
    if( keymap.find("RESAXISNDIV") != keymap.end() && (keymap["RESAXISNDIV"] != "") ){ 
      varObj->SetResAxisNdiv(atoi(keymap["RESAXISNDIV"].c_str())); 
      varObj->SetHasResAxisNdiv(true);
    }

    //------------
    if( keymap.find("BOTTOMMARGIN") != keymap.end() && (keymap["BOTTOMMARGIN"] != "") ){ 
      varObj->SetBottomMargin(atof(keymap["BOTTOMMARGIN"].c_str())); 
      varObj->SetHasBottomMargin(true);
    }
    if( keymap.find("TOPMARGIN") != keymap.end() && (keymap["TOPMARGIN"] != "") ){ 
      varObj->SetTopMargin(atof(keymap["TOPMARGIN"].c_str())); 
      varObj->SetHasTopMargin(true);
    }
    if( keymap.find("LEFTMARGIN") != keymap.end() && (keymap["LEFTMARGIN"] != "") ){ 
      varObj->SetLeftMargin(atof(keymap["LEFTMARGIN"].c_str())); 
      varObj->SetHasLeftMargin(true);
    }
    if( keymap.find("RIGHTMARGIN") != keymap.end() && (keymap["RIGHTMARGIN"] != "") ){ 
      varObj->SetRightMargin(atof(keymap["RIGHTMARGIN"].c_str())); 
      varObj->SetHasRightMargin(true);
    }

    if( keymap.find("ISCOUNT") != keymap.end() ){ varObj->SetIsCount(AnalysisUtils::BoolValue(keymap["ISCOUNT"], "ISCOUNT")); }

    if(opt->MsgLevel() == Debug::DEBUG) std::cout<<" Adding variable "<<name<<std::endl;
    var_map[name] = varObj;
    keymap.clear();
  }


  if(opt->MsgLevel() == Debug::DEBUG) std::cout<<"VariableAttributes::ParseVariableConfig : nline = "<<nline<<" var_map.size() = "<<var_map.size()<<std::endl; 

  parsed_map.clear();
  return var_map;
  

}
