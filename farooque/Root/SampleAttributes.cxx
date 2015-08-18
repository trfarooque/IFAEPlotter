#include "IFAEPlotter/SampleAttributes.h"


SampleAttributes::SampleAttributes(){
}

SampleAttributes::SampleAttributes(const std::string& name, const std::string& suffix, const std::string& leglabel, const std::string& stylekey
				   , const std::string& drawopt, const std::string& drawscale
				   , bool draw_stack, bool is_baseline, int linecolour, int fillcolour, int linestyle, int fillstyle) : 
  m_name(name),
  m_suffix(suffix),
  m_leglabel(leglabel),
  m_stylekey(stylekey),
  m_drawopt(drawopt),
  m_drawscale(drawscale),
  m_draw_stack(draw_stack),
  m_is_baseline(is_baseline),
  m_linecolour(linecolour),
  m_fillcolour(fillcolour),
  m_linestyle(linestyle),
  m_fillstyle(fillstyle){ }
