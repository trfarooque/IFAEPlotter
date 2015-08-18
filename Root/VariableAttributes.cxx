#include "IFAEPlotter/VariableAttributes.h"

VariableAttributes::VariableAttributes(const std::string& name, const std::string& label, const std::string& do_scale
				       , bool draw_stack, std::string draw_res, bool isLog,  int rebin) : 
  m_name(name),
  m_label(label),
  m_do_scale(do_scale),
  m_draw_stack(draw_stack),
  m_draw_res(draw_res),
  m_is_log(isLog),
  m_rebin(rebin){ }
