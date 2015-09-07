#include "IFAEPlotter/SampleAttributes.h"


SampleAttributes::SampleAttributes(){
}

SampleAttributes::SampleAttributes(const std::string& name, const std::string& suffix, const std::string& leglabel, const std::string& stylekey
				   , const std::string& drawopt, const std::string& drawscale
				   , bool draw_stack, int res_opt) : 
  m_name(name),
  m_suffix(suffix),
  m_leglabel(leglabel),
  m_stylekey(stylekey),
  m_drawopt(drawopt),
  m_drawscale(drawscale),
  m_legopt("l"),
  m_draw_stack(draw_stack),
  m_res_opt(res_opt)
{

  if( (drawopt.find("hist") != std::string::npos) || (drawopt.find("HIST") != std::string::npos) 
      || (drawopt.find("e2") != std::string::npos) || (drawopt.find("E2") != std::string::npos)  ){ m_legopt += "f";}
  if( (drawopt.find("e") != std::string::npos) || (drawopt.find("E") != std::string::npos) ){ m_legopt += "p"; }

}
