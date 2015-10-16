#include "IFAEPlotter/SampleAttributes.h"


SampleAttributes::SampleAttributes(){
}

SampleAttributes::SampleAttributes(const std::string& name, const std::string& suffix, const std::string& leglabel, const std::string& stylekey
				   , const std::string& drawopt, const std::string& legopt, const std::string& drawscale
				   , bool draw_stack, int res_opt) : 
  m_name(name),
  m_suffix(suffix),
  m_leglabel(leglabel),
  m_stylekey(stylekey),
  m_drawopt(drawopt),
  m_drawscale(drawscale),
  m_legopt(legopt),
  m_draw_stack(draw_stack),
  m_res_opt(res_opt)
{}
