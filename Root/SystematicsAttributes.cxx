#include "IFAEPlotter/SystematicsAttributes.h"
#include "IFAEPlotter/Plotter_Options.h"
#include "IFAETopFramework/AnalysisUtils.h"
#include <iostream>


SystematicsAttributes::SystematicsAttributes(const std::string& name, const std::string& name_up, const std::string& name_down
					     , bool one_sided, bool newfile, bool newdir, const std::string& symmetrisation, const std::string& suffix) : 
  m_name(name),
  m_name_up(name_up),
  m_name_down(name_down),
  m_symmetrisation(symmetrisation),
  m_suffix(suffix),
  m_one_sided(one_sided),
  m_newfile(newfile),
  m_newdir(newdir)
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
  m_newdir           = q.m_newdir;
  
}

SystematicsAttributesMap SystematicsAttributes::ParseSystematicsConfig( Plotter_Options* opt ){

  const std::string& config_systematics = opt->SystematicsList(); 
  const std::string& delim = opt->ConfigDelim();
  bool block_format = false;
  if( opt->OptStr().find("--NEWSYSTEMATICSCONFIG") != std::string::npos ){ block_format = opt->NewSystematicsFormat(); }
  else if( opt->OptStr().find("--NEWCONFIG") != std::string::npos ){ block_format = opt->NewConfigFormat(); }

  SystematicsAttributesMap sys_map; sys_map.clear();
  std::vector<std::map<std::string, std::string> > parsed_map;
  int nline = AnalysisUtils::ParseConfigFile(config_systematics, parsed_map, delim, block_format); 
  if(nline < 0){ std::cout<<"Systematics configuration file could not be opened. Exiting"<<std::endl; return sys_map; }
  std::string name = "";

  for(int l = 0; l < nline; l++){
    name="";
    std::map<std::string, std::string> keymap = parsed_map.at(l);
    if(keymap.find("NAME") == keymap.end()){ 
      std::cout<<" Systematics in block "<<l<<" has no name. Ignoring."<<std::endl;
      continue; 
    }
    name = keymap["NAME"];
    SystematicsAttributes* sysObj = new SystematicsAttributes( name );
    if( keymap.find("ONESIDED") != keymap.end() ){ sysObj->SetOneSided(AnalysisUtils::BoolValue(keymap["ONESIDED"], "ONESIDED")); }
    if( keymap.find("NEWFILE") != keymap.end() ){ sysObj->SetNewFile(AnalysisUtils::BoolValue(keymap["NEWFILE"], "NEWFILE")); }
    if( keymap.find("NEWDIR") != keymap.end() ){ sysObj->SetNewFile(AnalysisUtils::BoolValue(keymap["NEWDIR"], "NEWDIR")); }
    if( keymap.find("NAMEUP") != keymap.end() ){ sysObj->SetNameUp(keymap["NAMEUP"]); }
    if( keymap.find("NAMEDOWN") != keymap.end() ){ sysObj->SetNameDown(keymap["NAMEDOWN"]); }
    if( keymap.find("SUFFIX") != keymap.end() ){ sysObj->SetSuffix(keymap["SUFFIX"]); }
    if( keymap.find("SYMMETRISATION") != keymap.end() ){ sysObj->SetSymmetrisation(keymap["SYMMETRISATION"]); }

    if(opt->MsgLevel() == Debug::DEBUG) std::cout<<" Adding systematic "<<name<<std::endl;
    sys_map[name] = sysObj;
    
    keymap.clear();
  }
  parsed_map.clear();


  if(opt->MsgLevel() == Debug::DEBUG) std::cout<<"SystematicsAttributes::ParseSystematicsConfig : nline = "<<nline<<" sys_map.size() = "<<sys_map.size()<<std::endl; 

  return sys_map;
}
