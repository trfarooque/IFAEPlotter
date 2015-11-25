#include "IFAEPlotter/SampleAttributes.h"


SampleAttributes::SampleAttributes(){
}

SampleAttributes::SampleAttributes(const std::string& name, const std::string& suffix, const std::string& leglabel, const std::string& stylekey
				   , const std::string& drawopt, const std::string& legopt, const std::string& drawscale
				   , bool draw_stack, bool do_sum, int res_opt, const std::string& resdrawopt
				   , const std::string& blinding, const std::string& yield_format
				   , bool write, const std::string& outfile_name) : 
  m_name(name),
  m_suffix(suffix),
  m_leglabel(leglabel),
  m_stylekey(stylekey),
  m_drawopt(drawopt),
  m_res_drawopt(resdrawopt),
  m_drawscale(drawscale),
  m_legopt(legopt),
  m_blinding(blinding),
  m_yield_format(yield_format),
  m_outfile_name(outfile_name),
  m_draw_stack(draw_stack),
  m_do_sum(do_sum),
  m_res_opt(res_opt),
  m_write(write)
{}
