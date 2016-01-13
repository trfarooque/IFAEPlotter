#include "IFAEPlotter/SampleAttributes.h"


SampleAttributes::SampleAttributes(){
}

SampleAttributes::SampleAttributes(const std::string& name, const std::string& suffix, const std::string& leglabel, const std::string& stylekey
				   , const std::string& drawopt, const std::string& legopt, const std::string& drawscale
				   , bool draw_stack, bool do_sum, int res_opt, const std::string& resdrawopt
				   , const std::string& blinding, const std::string& yield_format
				   , bool write, const std::string& outfile_name
				   , const std::string& in_suffix, const std::string& in_prefix) : 
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
  m_in_suffix(in_suffix),
  m_in_prefix(in_prefix),
  m_draw_stack(draw_stack),
  m_do_sum(do_sum),
  m_res_opt(res_opt),
  m_write(write)
{}

SampleAttributes::SampleAttributes(SampleAttributes& q){

  m_name             = q.m_name;
  m_suffix           = q.m_suffix;
  m_leglabel         = q.m_leglabel;
  m_stylekey         = q.m_stylekey;
  m_drawopt          = q.m_drawopt;
  m_res_drawopt      = q.m_res_drawopt;
  m_drawscale        = q.m_drawscale;
  m_legopt           = q.m_legopt;
  m_blinding         = q.m_blinding;
  m_yield_format     = q.m_yield_format;
  m_outfile_name     = q.m_outfile_name;
  m_in_suffix        = q.m_in_suffix;
  m_in_prefix        = q.m_in_prefix;
  m_draw_stack       = q.m_draw_stack;
  m_do_sum           = q.m_do_sum;
  m_res_opt          = q.m_res_opt;
  m_write            = q.m_write;


}
