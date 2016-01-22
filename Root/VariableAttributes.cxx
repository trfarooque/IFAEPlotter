#include "IFAEPlotter/VariableAttributes.h"

VariableAttributes::VariableAttributes(const std::string& name, const std::string& label, const std::string& do_scale
				       , bool do_width, bool draw_stack, const std::string& draw_res, const std::string& draw_res_err
				       , bool isLogY, bool isLogX
				       , const std::string& ylabel, const std::string& reslabel
				       , bool has_resmin, bool has_resmax, double resmin, double resmax
				       , bool has_ymin, bool has_ymax, double ymin, double ymax
				       , bool has_xmin, bool has_xmax, double xmin, double xmax
				       , bool has_ttl_xmin, bool has_ttl_xmax, double ttl_xmin, double ttl_xmax
				       , bool has_ttl_ymin, bool has_ttl_ymax, double ttl_ymin, double ttl_ymax 
				       , const std::string& resdrawopt, const std::string& extralabel, int rebin, const std::string& rebinedges, const std::string& blinding) : 
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
  m_has_ymin(has_ymin),
  m_has_ymax(has_ymax),
  m_xmin(xmin),
  m_xmax(xmax),
  m_has_xmin(has_xmin),
  m_has_xmax(has_xmax),
  m_ttl_xmin(ttl_xmin),
  m_ttl_ymin(ttl_ymin),
  m_ttl_xmax(ttl_xmax),
  m_ttl_ymax(ttl_ymax),
  m_has_ttl_xmin(has_ttl_xmin),
  m_has_ttl_ymin(has_ttl_ymin),
  m_has_ttl_xmax(has_ttl_xmax),
  m_has_ttl_ymax(has_ttl_ymax),
  m_nprojbin(-1),
  m_resdrawopt(resdrawopt),
  m_blinding(blinding),
  m_extralabel(extralabel)
{ }

VariableAttributes::VariableAttributes() : VariableAttributes("", "", "NONE"){}

VariableAttributes::VariableAttributes(VariableAttributes& q){
  m_name           = q.m_name;
  m_label          = q.m_label;
  m_ylabel         = q.m_ylabel;
  m_reslabel       = q.m_reslabel;
  m_draw_stack     = q.m_draw_stack;
  m_do_scale       = q.m_do_scale;
  m_draw_res       = q.m_draw_res;
  m_draw_res_err   = q.m_draw_res_err;
  m_is_logY        = q.m_is_logY;
  m_is_logX        = q.m_is_logX;
  m_rebin          = q.m_rebin;
  m_rebinedges     = q.m_rebinedges;
  m_do_width       = q.m_do_width;
  m_resmin         = q.m_resmin;
  m_resmax         = q.m_resmax;
  m_has_resmin     = q.m_has_resmin;
  m_has_resmax     = q.m_has_resmax;
  m_ymin           = q.m_ymin;
  m_ymax           = q.m_ymax;
  m_has_ymin       = q.m_has_ymin;
  m_has_ymax       = q.m_has_ymax;
  m_xmin           = q.m_xmin;
  m_xmax           = q.m_xmax;
  m_has_xmin       = q.m_has_xmin;
  m_has_xmax       = q.m_has_xmax;
  m_ttl_xmin       = q.m_ttl_xmin;
  m_ttl_ymin       = q.m_ttl_ymin;
  m_ttl_xmax       = q.m_ttl_xmax;
  m_ttl_ymax       = q.m_ttl_ymax;
  m_has_ttl_xmin   = q.m_has_ttl_xmin;
  m_has_ttl_ymin   = q.m_has_ttl_ymin;
  m_has_ttl_xmax   = q.m_has_ttl_xmax;
  m_has_ttl_ymax   = q.m_has_ttl_ymax;
  m_nprojbin       = q.m_nprojbin;
  m_resdrawopt     = q.m_resdrawopt;
  m_extralabel     = q.m_extralabel;
  m_blinding       = q.m_blinding;
}
