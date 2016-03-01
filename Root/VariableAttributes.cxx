#include "IFAEPlotter/VariableAttributes.h"

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
					
					, bool has_bottom_margin, bool has_top_margin
					, bool has_left_margin, bool has_right_margin
					, double bottom_margin, double top_margin
					, double left_margin, double right_margin
					
					, bool is_count 
					, const std::string& resdrawopt, const std::string& extralabel
					, int rebin, const std::string& rebinedges, const std::string& blinding) :
  
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
  m_extralabel(extralabel)
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

  m_bottom_margin      = q.m_bottom_margin;
  m_top_margin         = q.m_top_margin;
  m_left_margin        = q.m_left_margin;
  m_right_margin       = q.m_right_margin;

  m_is_count           = q.m_is_count;
  m_nprojbin           = q.m_nprojbin;
  m_resdrawopt         = q.m_resdrawopt;
  m_extralabel         = q.m_extralabel;
  m_blinding           = q.m_blinding;
}
