#include "IFAEPlotter/SystematicsAttributes.h"



SystematicsAttributes::SystematicsAttributes(const std::string& name, const std::string& name_up, const std::string& name_down
					     , bool one_sided, bool newfile, const std::string& symmetrisation, const std::string& suffix) : 
  m_name(name),
  m_name_up(name_up),
  m_name_down(name_down),
  m_symmetrisation(symmetrisation),
  m_suffix(suffix),
  m_one_sided(one_sided),
  m_newfile(newfile)
{}

SystematicsAttributes::SystematicsAttributes() : SystematicsAttributes(""){}

SystematicsAttributes::SystematicsAttributes(SystematicsAttributes& q){

  m_name             = q.m_name;
  m_name_up          = q.m_name_up;
  m_name_down        = q.m_name_down;
  m_symmetrisation   = q.m_symmetrisation;
  m_suffix           = q.m_suffix;
  m_one_sided        = q.m_one_sided;
  m_newfile          = q.m_newfile;
  
}
