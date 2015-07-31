#include "IFAEPlotter/SampleAttributes.h"


SampleAttributes::SampleAttributes(){
}

SampleAttributes::SampleAttributes(std::string name, std::string suffix, std::string leglabel, std::string stylekey, std::string drawopt
				   , int do_scale, bool draw_stack, bool is_baseline, int linecolour, int fillcolour, int linestyle, int fillstyle) : 
  m_name(name),
  m_suffix(suffix),
  m_leglabel(leglabel),
  m_stylekey(stylekey),
  m_drawopt(drawopt),
  m_do_scale(do_scale),
  m_draw_stack(draw_stack),
  m_is_baseline(is_baseline),
  m_linecolour(linecolour),
  m_fillcolour(fillcolour),
  m_linestyle(linestyle),
  m_fillstyle(fillstyle){ }
