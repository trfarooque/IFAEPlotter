#include "IFAEPlotter/VariableAttributes.h"

VariableAttributes::VariableAttributes(const std::string& name, const std::string& label, const std::string& do_scale
				       , bool do_width, bool draw_stack, std::string draw_res, bool isLog
				       , const std::string& ylabel, const std::string& reslabel
				       , bool has_resmin, bool has_resmax
				       , double resmin, double resmax, const std::string& resdrawopt
				       , int rebin) : 
  m_name(name),
  m_label(label),
  m_ylabel(ylabel),
  m_reslabel(reslabel),
  m_draw_stack(draw_stack),
  m_do_scale(do_scale),
  m_draw_res(draw_res),
  m_is_log(isLog),
  m_rebin(rebin),
  m_do_width(do_width),
  m_resmin(resmin),
  m_resmax(resmax),
  m_resdrawopt(resdrawopt),
  m_has_resmin(has_resmin),
  m_has_resmax(has_resmax)
{ }
