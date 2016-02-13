#include "IFAEPlotter/Plotter_Options.h"
#include "IFAEPlotter/PlotManager.h"
#include "IFAETopFramework/HistManager.h"
#include "IFAEPlotter/PlotUtils.h"
#include "IFAEPlotter/StyleDictionary.h"
#include "IFAEPlotter/SampleAttributes.h"
#include "IFAEPlotter/VariableAttributes.h"
#include "IFAEPlotter/SystematicsAttributes.h"
#include "IFAEPlotter/FileKeyAttributes.h"

#include "TKey.h"
#include "TSystem.h"

#include <fstream>
#include <algorithm>


PlotManager::PlotManager(Plotter_Options* opt) : m_opt(opt){

  if(m_opt->MsgLevel() == Debug::DEBUG) std::cout<<"PlotManager constructor start"<<std::endl; 
  m_hstMngr = new HistManager();

  m_attr_map.clear();
  m_var_map.clear();
  m_sys_map.clear();
  m_filekey_map.clear(); 

  m_new_sample_format = false;
  m_new_variable_format = false;
  m_new_style_format = false;
  m_new_systematics_format = false;
  m_new_filelist_format = false;

  bool opt_has_configformat = (m_opt->OptStr().find("--NEWCONFIG") != std::string::npos); 
  bool opt_has_sampleformat = (m_opt->OptStr().find("--NEWSAMPLECONFIG") != std::string::npos); 
  bool opt_has_variableformat = (m_opt->OptStr().find("--NEWVARIABLECONFIG") != std::string::npos); 
  bool opt_has_styleformat = (m_opt->OptStr().find("--NEWSTYLECONFIG") != std::string::npos); 
  bool opt_has_systematicsformat = (m_opt->OptStr().find("--NEWSYSTEMATICSCONFIG") != std::string::npos); 
  bool opt_has_filelistformat = (m_opt->OptStr().find("--NEWFILELIST") != std::string::npos); 

  if(opt_has_sampleformat){ m_new_sample_format = m_opt->NewSampleFormat(); }
  else if(opt_has_configformat){ m_new_sample_format = m_opt->NewConfigFormat(); }

  if(opt_has_variableformat){ m_new_variable_format = m_opt->NewVariableFormat(); }
  else if(opt_has_configformat){ m_new_variable_format = m_opt->NewConfigFormat(); }

  if(opt_has_styleformat){ m_new_style_format = m_opt->NewStyleFormat(); }
  else if(opt_has_configformat){ m_new_style_format = m_opt->NewConfigFormat(); }

  if(opt_has_systematicsformat){ m_new_systematics_format = m_opt->NewSystematicsFormat(); }
  else if(opt_has_configformat){ m_new_systematics_format = m_opt->NewConfigFormat(); }

  if(opt_has_filelistformat){ m_new_filelist_format = m_opt->NewFileListFormat(); }
  else if(opt_has_configformat){ m_new_filelist_format = m_opt->NewConfigFormat(); }

  if(m_opt->OutputFolder() != ""){ gSystem->mkdir(m_opt->OutputFolder().c_str(), "TRUE"); }

  if(m_opt->MsgLevel() == Debug::DEBUG) std::cout<<"Parsing sample configuration"<<std::endl; 

  int stat = 0;
  stat = ParseSampleConfig( m_opt->ConfigSample() );
  if(stat < 0){return;}

  if(m_opt->MsgLevel() == Debug::DEBUG) std::cout<<"Parsing variable configuration"<<std::endl; 
  stat = ParseVariableConfig( m_opt->ConfigVariable() );
  if(stat < 0){return;}

  if(m_opt->MsgLevel() == Debug::DEBUG) std::cout<<"Parsing file list"<<std::endl; 
  stat = ParseFileList( m_opt->FileList() );
  if(stat < 0){return;}

  if( m_opt->DoSystematics() ){
    if(m_opt->MsgLevel() == Debug::DEBUG) std::cout<<"Parsing systematics list"<<std::endl; 
    stat = ParseSystematicsConfig( m_opt->SystematicsList() );
    if(stat < 0){return;}
  }

  if( m_opt->AllFromFile() ){
    const std::string& dist_file = (m_opt->DistributionsFile() != "") 
      ? m_opt->DistributionsFile() : (m_filekey_map.begin()->second)->FileList()->at(0);
    if(m_opt->MsgLevel() == Debug::DEBUG) std::cout << "Copying all distributions from file " << dist_file << std::endl;
    CopyVariableListFromFile( dist_file ); 
  }
  else{
    if( m_var_map.find("ALL") != m_var_map.end() ){
      std::cout<<" WARNING: ALL keyword in variable configuration will be ignored because ALLFROMFILE=FALSE in global options"<<std::endl;
      delete m_var_map["ALL"];
      m_var_map.erase( m_var_map.find("ALL") );
    }
  }
  if(stat < 0){return;}

  if(!m_opt->DoSystematics()){
    if(m_opt->MsgLevel() == Debug::DEBUG) std::cout<<"Parsing style library"<<std::endl; 
    m_styleDict = new StyleDictionary("test");
    stat = ParseStyleConfig( m_opt->StyleLib() );
    if(stat < 0){return;}
    m_plotUtils = new PlotUtils(m_opt, m_hstMngr, m_attr_map, m_var_map, m_styleDict);
  }
  if(m_opt->MsgLevel() == Debug::DEBUG) std::cout<<"PlotManager constructor end"<<std::endl; 

}

PlotManager::~PlotManager(){

  if(m_opt->MsgLevel() == Debug::DEBUG) std::cout<<"PlotManager destructor start"<<std::endl; 
  delete m_hstMngr;
  delete m_plotUtils;
  delete m_styleDict;

  for(SampleAttributesMap::iterator samit = m_attr_map.begin(); samit != m_attr_map.end(); ++samit){
    delete samit->second;
    m_attr_map.erase(samit);
  }

  for(VariableAttributesMap::iterator varit = m_var_map.begin(); varit != m_var_map.end(); ++varit){
    delete varit->second;
    m_var_map.erase(varit);
  }

  for(SystematicsAttributesMap::iterator sysit = m_sys_map.begin(); sysit != m_sys_map.end(); ++sysit){
    delete sysit->second;
    m_sys_map.erase(sysit);
  }

  for(FileKeyAttributesMap::iterator fkit = m_filekey_map.begin(); fkit != m_filekey_map.end(); ++fkit){
    delete fkit->second;
    m_filekey_map.erase(fkit);
  }


  if(m_opt->MsgLevel() == Debug::DEBUG) std::cout<<"PlotManager destructor end"<<std::endl; 

}

void PlotManager::Initialize(){ return; }

void PlotManager::Execute(){

  if(m_opt->MsgLevel() == Debug::DEBUG) std::cout<<"PlotManager::Execute start"<<std::endl; 

  if( m_opt->DoEff() ){ 
    makeEfficiencyHistograms(); 
    m_plotUtils->OverlayHists("EFF");
  }
  else if( m_opt->DoProjections() ){
    ProjectByBin(); 
    if( m_opt->ProjOpt().find("HIST") != std::string::npos){ m_plotUtils->OverlayHists("HIST");}
    if( m_opt->ProjOpt().find("MEAN") != std::string::npos){ m_plotUtils->OverlayHists("MEAN");}
    if( m_opt->ProjOpt().find("RMS") != std::string::npos){ m_plotUtils->OverlayHists("RMS");}
  }
  else if( m_opt->WriteHistos() || m_opt->Do1DPlots() ){
    FillHistManager(); 
    if( m_opt->Do1DPlots() ){ m_plotUtils->OverlayHists("NONE"); }
    if( m_opt->WriteHistos() ){ WriteHistogramsToFile(); }
  } 
  else if( m_opt->DoSystematics() ){

    ReadSystematicsFromFiles();
    PrintSystematics();
  }

  if(m_opt->MsgLevel() == Debug::DEBUG) std::cout<<"PlotManager::Execute end"<<std::endl; 

  return;

}
void PlotManager::Terminate(){

  if(m_opt->MsgLevel() == Debug::DEBUG) std::cout<<"PlotManager::Terminate start"<<std::endl; 

  m_hstMngr->ClearAllTH1();
  m_hstMngr->ClearAllTH2();
  m_hstMngr->ClearAllTH3();
  if(m_plotUtils) m_plotUtils->Terminate();

  if(m_opt->MsgLevel() == Debug::DEBUG) std::cout<<"PlotManager::Terminate end"<<std::endl; 

  return;
}

int PlotManager::ParseConfigFile(const std::string& config_file, std::vector<std::map<std::string, std::string> >& ret_map, const std::string& delim, bool newformat ){

  int stat = 1;
  if(newformat){ stat = ParseConfigFile_New(config_file, ret_map,  delim); }
  else{ stat = ParseConfigFile_Old(config_file, ret_map,  delim); }

  return stat;

}

int PlotManager::ParseConfigFile_New(const std::string& config_file, std::vector<std::map<std::string, std::string> >& ret_map, const std::string& delim ){

  ret_map.clear();
  std::ifstream conf_stream(config_file);
  if(!conf_stream){
    std::cout<<"Error : configuration file "<<config_file<<" not found. Exiting."<<std::endl;
    return -1;
  }

  std::string conf_line; 
  bool begun = false;
  int nset = -1;
  while( getline(conf_stream, conf_line) ){
    AnalysisUtils::TrimString(conf_line);
    if(conf_line == "BEGIN"){begun = true; continue;}
    if(!begun){continue;}
    if(conf_line == "END") break;
    if( conf_line.empty() || (conf_line.find("#") == 0) ) continue;

    if(conf_line == "NEW"){ 
      nset++; 
      std::map<std::string, std::string> varset;
      ret_map.push_back(varset);
    }
    else{
      std::string param; param.clear();
      std::string paramString = conf_line;
      std::string::size_type pos = AnalysisUtils::ParseString(paramString, param, delim);
      if(pos == std::string::npos){ 
	std::cout<<" Could not read parameter value from line "<<conf_line<<std::endl;
	std::cout<<" Format should be [PARAM] "<<delim<<" [VALUE]"<<std::endl;
	continue;
      }
      AnalysisUtils::TrimString(param);
      ret_map.at(nset)[param] = paramString;
      //if(m_opt->MsgLevel() == Debug::DEBUG) std::cout<<" nset = "<<nset<<" param = "<<param<<" paramString = "<<paramString<<std::endl;
    }
  }

  return nset+1;

}

int PlotManager::ParseConfigFile_Old(const std::string& config_file, std::vector<std::map<std::string, std::string> >& ret_map, const std::string& delim ){

  ret_map.clear();

  std::map<int, std::string> paramSeq; paramSeq.clear();
  std::ifstream conf_stream(config_file);
  if(!conf_stream){
    std::cout<<"Error : configuration file "<<config_file<<" not found. Exiting."<<std::endl;
    return -1;
  }

  std::string conf_line;
  while( conf_line != "BEGIN" ){  
    getline(conf_stream, conf_line); 
    AnalysisUtils::TrimString(conf_line);
  }
  getline(conf_stream, conf_line);

  //Header gives the sequence of configuration variables
  std::string param; param.clear();
  std::string paramString = conf_line;
  int nparam = 0;
  std::string::size_type pos = 0;
  do{ 
    pos = AnalysisUtils::ParseString(paramString, param, delim);
    AnalysisUtils::TrimString(param);
    std::transform(param.begin(), param.end(), param.begin(), ::toupper);
    paramSeq[nparam] = param;
    //if(m_opt->MsgLevel() == Debug::DEBUG) std::cout<<" nparam = "<<nparam<<" param = "<<param<<std::endl; 
    nparam++;
  }while(pos != std::string::npos);


  int nkey = nparam;
  int nline = 0;

  map<string, string> keymap;
  while( getline(conf_stream, conf_line) ){
    AnalysisUtils::TrimString(conf_line);
    if(conf_line == "END") break;
    if( conf_line.empty() || (conf_line.find("#") == 0) ) continue;

    paramString = conf_line;
    param = "";
    nparam = 0;
    pos = 0;
    keymap.clear();

    do{
      pos = AnalysisUtils::ParseString(paramString, param, delim);
      AnalysisUtils::TrimString(param);
      if(nparam > nkey){
	std::cout<<"Error: Number of parameters on line "<<nline<<" exceeds number of keys given in header"<<std::endl; 
	break;
      }
      if(paramSeq.find(nparam) == paramSeq.end()){
	std::cout<<"WARNING: "<<nline<<" : param "<<nparam<<" not specified"<<std::endl;
	continue;
      } 
      keymap[ paramSeq[nparam] ] = param;
      nparam++;
    }while(pos != std::string::npos);//read all given parameters

    ret_map.push_back(keymap);
    nline++;
  }//line loop

  return nline;

}


int PlotManager::ParseSystematicsConfig(const std::string& config_systematics, const std::string& delim ){

  vector<map<string, string> > parsed_map;
  int nline = ParseConfigFile(config_systematics, parsed_map, delim, m_new_systematics_format); 
  if(nline < 0){ std::cout<<"Systematics configuration file could not be opened. Exiting"<<std::endl; return nline; }
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
    if( keymap.find("NAMEUP") != keymap.end() ){ sysObj->SetNameUp(keymap["NAMEUP"]); }
    if( keymap.find("NAMEDOWN") != keymap.end() ){ sysObj->SetNameDown(keymap["NAMEDOWN"]); }
    if( keymap.find("SUFFIX") != keymap.end() ){ sysObj->SetSuffix(keymap["SUFFIX"]); }
    if( keymap.find("SYMMETRISATION") != keymap.end() ){ sysObj->SetSuffix(keymap["SYMMETRISATION"]); }

    m_sys_map[name] = sysObj;
    
    keymap.clear();
  }
  parsed_map.clear();

  return 0;
}


int PlotManager::ParseSampleConfig(const std::string& config_sample, const std::string& delim ){
  
  int sc = 0; 
  vector<map<string, string> > parsed_map;
  int nline = ParseConfigFile(config_sample, parsed_map, delim, m_new_sample_format); 
  if(nline < 0){ std::cout<<"Sample configuration file could not be opened. Exiting"<<std::endl; return nline; }
  std::string name = "";

  for(int l = 0; l < nline; l++){
    name = "";

    map<string, string> keymap = parsed_map.at(l);

    if( keymap.find("NAME") == keymap.end() ){ 
      std::cout<<"Error : No name found for sample in block "<<l<<". Ignoring."<<std::endl; 
      continue;
    }
    SampleAttributes* sampleObj = new SampleAttributes();
    name = keymap["NAME"];
    sampleObj->SetName( name ); 


    if( keymap.find("INSUFFIX") != keymap.end() ){ sampleObj->SetInSuffix(keymap["INSUFFIX"]); }
    if( keymap.find("INPREFIX") != keymap.end() ){ sampleObj->SetInPrefix(keymap["INPREFIX"]); }

    if( keymap.find("SUFFIX") != keymap.end() ){ sampleObj->SetSuffix(keymap["SUFFIX"]); }
    else{ sampleObj->SetSuffix( name ); }

    if( keymap.find("LEGLABEL") != keymap.end() ){ sampleObj->SetLegLabel(keymap["LEGLABEL"]); }

    if( keymap.find("STYLEKEY") != keymap.end() ){ sampleObj->SetStyleKey(keymap["STYLEKEY"]); }
    if( keymap.find("DRAWOPT") != keymap.end() ){ sampleObj->SetDrawOpt(keymap["DRAWOPT"]); }
    if( keymap.find("RESDRAWOPT") != keymap.end() ){ sampleObj->SetResDrawOpt(keymap["RESDRAWOPT"]); }
    if( keymap.find("LEGOPT") != keymap.end() ){ sampleObj->SetLegOpt(keymap["LEGOPT"]); }
    if( keymap.find("DRAWSCALE") != keymap.end() ){ sampleObj->SetDrawScale(keymap["DRAWSCALE"]); }
    if( keymap.find("DRAWSTACK") != keymap.end() ){ sampleObj->SetDrawStack(AnalysisUtils::BoolValue(keymap["DRAWSTACK"], "DRAWSTACK")); }
    if( keymap.find("DOSUM") != keymap.end() && (keymap["DOSUM"] != "") ){ sampleObj->SetDoSum(AnalysisUtils::BoolValue(keymap["DOSUM"], "DOSUM")); }
    else{ sampleObj->SetDoSum( sampleObj->DrawStack() ); }

    if( keymap.find("RESOPT") != keymap.end() ){ 
      if(keymap["RESOPT"] == "SKIP"){sampleObj->SetResOpt(-1);}
      else if(keymap["RESOPT"] == "INC"){sampleObj->SetResOpt(0);}
      else if(keymap["RESOPT"] == "REF"){sampleObj->SetResOpt(1);}
      else{ std::cout<<"UNknown value "<<keymap["RESOPT"]<<" for RESOPT option"<<std::endl; }
    }
    if( keymap.find("BLINDING") != keymap.end() ){ sampleObj->SetBlinding(keymap["BLINDING"]); }
    if( keymap.find("YIELDFORMAT") != keymap.end() ){ sampleObj->SetYieldFormat(keymap["YIELDFORMAT"]); }
    if( keymap.find("WRITE") != keymap.end() ){ sampleObj->SetWrite(AnalysisUtils::BoolValue(keymap["WRITE"], "WRITE")); }
    if( keymap.find("OUTFILENAME") != keymap.end() ){ sampleObj->SetOutFileName(keymap["OUTFILENAME"]); }
    if( keymap.find("NOSHAPE") != keymap.end() ){ sampleObj->SetNoShape(AnalysisUtils::BoolValue(keymap["NOSHAPE"], "NOSHAPE")); }


    //---------- read all parameters------
    if(m_opt->MsgLevel() == Debug::DEBUG) std::cout<<" Adding sample "<<name<<std::endl;
    m_attr_map[name] = sampleObj;
    keymap.clear();
  }

  if(m_opt->MsgLevel() == Debug::DEBUG) std::cout<<"PlotManager::ParseSampleConfig : nline = "<<nline<<" m_attr_map.size() = "<<m_attr_map.size()<<std::endl; 

  parsed_map.clear();
  return sc;

}

int PlotManager::ParseVariableConfig(const std::string& config_variable, const std::string& delim){

  int sc = 0; 
  std::vector<std::map<std::string, std::string> > parsed_map;
  int nline = ParseConfigFile(config_variable, parsed_map, delim, m_new_variable_format); 
  if(nline < 0){ std::cout<<"Variable configuration file could not be opened. Exiting"<<std::endl; return nline; }

  std::string name = "";

  for(int l = 0; l < nline; l++){
    name = "";

    std::map<std::string, std::string> keymap = parsed_map.at(l);

    if( keymap.find("NAME") == keymap.end() ){ 
      std::cout<<"Error : No name found for variable in block "<<l<<". Ignoring."<<std::endl;
      continue;
    }
    VariableAttributes* varObj = new VariableAttributes();
    name = keymap["NAME"];
    varObj->SetName(name);

    if( keymap.find("LABEL") != keymap.end() ){ varObj->SetLabel(keymap["LABEL"]); }
    if( keymap.find("YLABEL") != keymap.end() ){ varObj->SetYLabel(keymap["YLABEL"]); }
    if( keymap.find("RESLABEL") != keymap.end() ){ varObj->SetResLabel(keymap["RESLABEL"]);}
    if( keymap.find("EXTRALABEL") != keymap.end() ){ varObj->SetExtraLabel(keymap["EXTRALABEL"]);}
    if( keymap.find("DRAWSTACK") != keymap.end() ){ varObj->SetDrawStack(AnalysisUtils::BoolValue(keymap["DRAWSTACK"], "DRAWSTACK")); }
    if( keymap.find("DRAWRES") != keymap.end() ){ varObj->SetDrawRes(keymap["DRAWRES"]); }
    if( keymap.find("DRAWRESERR") != keymap.end() ){ varObj->SetDrawResErr(keymap["DRAWRESERR"]);}
    if( keymap.find("ISLOGY") != keymap.end() ){ varObj->SetIsLogY(AnalysisUtils::BoolValue(keymap["ISLOGY"], "ISLOGY")); }
    if( keymap.find("ISLOGX") != keymap.end() ){ varObj->SetIsLogX(AnalysisUtils::BoolValue(keymap["ISLOGX"], "ISLOGX")); }
 

    if( keymap.find("REBIN") != keymap.end() ){ varObj->SetRebin(atoi(keymap["REBIN"].c_str())); }
    if( keymap.find("REBINVAR") != keymap.end() ){ varObj->SetRebinEdges(keymap["REBINVAR"]); }
    if( keymap.find("DOSCALE") != keymap.end() ){ varObj->SetDoScale(keymap["DOSCALE"]); }
    if( keymap.find("DOWIDTH") != keymap.end() ){ varObj->SetDoWidth(AnalysisUtils::BoolValue(keymap["DOWIDTH"], "DOWIDTH")); }
    if( keymap.find("RESDRAWOPT") != keymap.end() ){ varObj->SetResDrawOpt(keymap["RESDRAWOPT"]); }
    if( keymap.find("BLINDING") != keymap.end() ){ varObj->SetBlinding(keymap["BLINDING"]); }


    if( keymap.find("RESMIN") != keymap.end() && (keymap["RESMIN"] != "") ){ 
      varObj->SetResMin(atof(keymap["RESMIN"].c_str())); 
      varObj->SetHasResMin(true);
    }
    if( keymap.find("RESMAX") != keymap.end() && (keymap["RESMAX"] != "") ){ 
      varObj->SetResMax(atof(keymap["RESMAX"].c_str())); 
      varObj->SetHasResMax(true);
    }
    if( keymap.find("YMIN") != keymap.end() && (keymap["YMIN"] != "") ){ 
      varObj->SetYMin(atof(keymap["YMIN"].c_str())); 
      varObj->SetHasYMin(true);
    }
    if( keymap.find("YMAX") != keymap.end() && (keymap["YMAX"] != "") ){ 
      varObj->SetYMax(atof(keymap["YMAX"].c_str())); 
      varObj->SetHasYMax(true);
    }
    if( keymap.find("XMIN") != keymap.end() && (keymap["XMIN"] != "") ){ 
      varObj->SetXMin(atof(keymap["XMIN"].c_str())); 
      varObj->SetHasXMin(true);
    }
    if( keymap.find("XMAX") != keymap.end() && (keymap["XMAX"] != "") ){ 
      varObj->SetXMax(atof(keymap["XMAX"].c_str())); 
      varObj->SetHasXMax(true);
    }

    if( keymap.find("TITLEXMIN") != keymap.end() && (keymap["TITLEXMIN"] != "") ){ 
      varObj->SetTitleXMin(atof(keymap["TITLEXMIN"].c_str())); 
      varObj->SetHasTitleXMin(true);
    }
    if( keymap.find("TITLEXMAX") != keymap.end() && (keymap["TITLEXMAX"] != "") ){ 
      varObj->SetTitleXMax(atof(keymap["TITLEXMAX"].c_str())); 
      varObj->SetHasTitleXMax(true);
    }
    //
    if( keymap.find("TITLEYMIN") != keymap.end() && (keymap["TITLEYMIN"] != "") ){ 
      varObj->SetTitleYMin(atof(keymap["TITLEYMIN"].c_str())); 
      varObj->SetHasTitleYMin(true);
    }
    if( keymap.find("TITLEYMAX") != keymap.end() && (keymap["TITLEYMAX"] != "") ){ 
      varObj->SetTitleYMax(atof(keymap["TITLEYMAX"].c_str())); 
      varObj->SetHasTitleYMax(true);
    }

    if(m_opt->MsgLevel() == Debug::DEBUG) std::cout<<" Adding variable "<<name<<std::endl;
    m_var_map[name] = varObj;
    keymap.clear();
  }


  if(m_opt->MsgLevel() == Debug::DEBUG) std::cout<<"PlotManager::ParseVariableConfig : nline = "<<nline<<" m_var_map.size() = "<<m_var_map.size()<<std::endl; 

  parsed_map.clear();
  return sc;
  
}

int PlotManager::CopyVariableListFromFile(const std::string& dist_file){

  int sc = 0;
  VariableAttributes* varObj = (m_var_map.find("ALL") != m_var_map.end()) ? m_var_map["ALL"] : new VariableAttributes("ALL", "", "NONE");

  TFile* fget = TFile::Open(dist_file.c_str(), "READ");
  TIter next(fget->GetListOfKeys());
  TKey *key=NULL;
  std::string className = "";
  std::string name = "";
  while( (key = (TKey*)next()) ) {
    className = (std::string)(key->GetClassName());
    if( className.find("TH1") == std::string::npos ){ className.clear(); continue; }
    className.clear();
    VariableAttributes* varCopy = new VariableAttributes(*varObj);
    name = (std::string)(key->GetName());
    if(m_var_map.find(name) != m_var_map.end()){name.clear(); continue;}

    varCopy->SetName( name );
    m_var_map[name] = varCopy;
    name.clear();
  }
  delete varObj;
  if(m_var_map.find("ALL") != m_var_map.end()){ m_var_map.erase( m_var_map.find("ALL") ); }
  delete fget;

  return sc;
}




//-----------EDIT-------------------- 
int PlotManager::ParseStyleConfig(const std::string& config_style, const std::string& delim){

  int sc = 0; 
  std::vector<std::map<std::string, std::string> > parsed_map;
  int nline = ParseConfigFile(config_style, parsed_map, delim, m_new_style_format); 
  if(nline < 0){ std::cout<<"Style configuration file could not be opened. Exiting"<<std::endl; return nline; }

  std::string key = "";
  for(int l = 0; l < nline; l++){

    key = "";
    std::map<std::string, std::string> keymap = parsed_map.at(l);

    if( keymap.find("KEY") != keymap.end() ){ key = keymap["KEY"];}
    else{std::cout<<"Error : No key found for style on line "<<l<<". Ignoring."<<std::endl; continue;}
    m_styleDict->AddStyle(key);

    if( ( keymap.find("LINECOLOUR") == keymap.end() ) && ( keymap.find("FILLCOLOUR") == keymap.end() )  && ( keymap.find("MARKERCOLOUR") == keymap.end() ) 
	&& ( keymap.find("COLOUR") == keymap.end() ) ){
      std::cout<<"Error: No colours found for style on line "<<l<<" with key "<<key<<". Ignoring"<<std::endl;

    }

    if( keymap.find("LINECOLOUR") != keymap.end() ){ m_styleDict->SetLineColour(key, keymap["LINECOLOUR"]); }
    else if( keymap.find("LINECOLOUR") != keymap.end() ){ m_styleDict->SetLineColour(key, keymap["COLOUR"]); } 
    else if( keymap.find("LINECOLOR") != keymap.end() ){ std::cout<<"Field LINECOLOR not recognised. Did you mean LINECOLOUR? #GODSAVETHEQUEEN"<<std::endl; }

    if( keymap.find("LINESTYLE") != keymap.end() ){ m_styleDict->SetLineStyle( key, atoi(keymap["LINESTYLE"].c_str()) );}

    if( keymap.find("LINEWIDTH") != keymap.end() ){ m_styleDict->SetLineWidth( key, atoi(keymap["LINEWIDTH"].c_str()) );}

    if( keymap.find("FILLCOLOUR") != keymap.end() ){ m_styleDict->SetFillColour(key, keymap["FILLCOLOUR"]); }
    else if( keymap.find("FILLCOLOUR") != keymap.end() ){ m_styleDict->SetFillColour(key, keymap["COLOUR"]); } 
    else if( keymap.find("FILLCOLOR") != keymap.end() ){ std::cout<<"Field FILLCOLOR not recognised. Did you mean FILLCOLOUR? #GODSAVETHEQUEEN"<<std::endl; }

    if( keymap.find("FILLSTYLE") != keymap.end() ){ m_styleDict->SetFillStyle(key, atoi(keymap["FILLSTYLE"].c_str()) );}

    if( keymap.find("MARKERCOLOUR") != keymap.end() ){ m_styleDict->SetMarkerColour(key, keymap["MARKERCOLOUR"]); }
    else if( keymap.find("MARKERCOLOUR") != keymap.end() ){ m_styleDict->SetMarkerColour(key, keymap["COLOUR"]); } 
    else if( keymap.find("MARKERCOLOR") != keymap.end() ){ std::cout<<"Field MARKERCOLOR not recognised. Did you mean MARKERCOLOUR? #GODSAVETHEQUEEN"<<std::endl; }

    if( keymap.find("MARKERSTYLE") != keymap.end() ){ m_styleDict->SetMarkerStyle(key, atoi(keymap["MARKERSTYLE"].c_str()) );}

    if( keymap.find("MARKERSIZE") != keymap.end() ){ m_styleDict->SetMarkerSize(key, atoi(keymap["MARKERSIZE"].c_str()) );}


    if( keymap.find("COLOR") != keymap.end() ){ std::cout<<"Field COLOR not recognised. Did you mean COLOUR? #GODSAVETHEQUEEEN"<<std::endl; }
    keymap.clear();
  }

  if(m_opt->MsgLevel() == Debug::DEBUG) std::cout<<"PlotManager::ParseStyleConfig : nline = "<<nline<<" m_styleDict->NStyles() = "<<m_styleDict->NStyles()<<std::endl; 

  parsed_map.clear();
  return sc;

}
//-----------EDIT END-------------------- 

int PlotManager::ParseFileList(const std::string& filelist, const std::string& delim){

  int sc = 0; 
  std::vector<std::map<std::string, std::string> > parsed_map;
  int nline = ParseConfigFile(filelist, parsed_map, delim, m_new_filelist_format); 
  if(nline < 0){ std::cout<<"File list "<<filelist<<" could not be opened. Exiting"<<std::endl; return nline; }

  std::string key = "";
  std::string fileloc = "";

  std::string keybase = "";
  std::string scalebase = "";
  std::string syst_name = "";
  const std::string delim_in = ",";

  for(int l = 0; l < nline; l++){

    std::map<std::string, std::string> keymap = parsed_map.at(l);
    keybase = "";
    scalebase = "";
    syst_name = "";
    if( keymap.find("KEY") != keymap.end() ){keybase = keymap["KEY"];}
    else{std::cout<<"Error : No sample key found for file name"<<std::endl; continue;}
    if( keymap.find("SCALE") != keymap.end() ){scalebase = keymap["SCALE"];}
    if( keymap.find("FILE") == keymap.end() ){std::cout<<"Error : No file name given for sample key "<<keymap["KEY"]<<std::endl; continue; }
    if( keymap.find("SYST") != keymap.end() ){syst_name = keymap["SYST"];}

    bool b_multi = (keybase.find(delim_in) != std::string::npos);      
    bool b_multiscale = (scalebase.find(delim_in) != std::string::npos);

    bool b_firstfile = (m_filekey_map.find(keymap["KEY"]) == m_filekey_map.end());

    bool b_sys = (syst_name != "");
    bool b_firstfile_systs = ( b_sys && ( b_firstfile || (m_filekey_map[keybase]->SystFileList()==NULL) ) );
    bool b_firstfile_syst_this = ( b_sys && ( b_firstfile_systs || 
					      (m_filekey_map[keybase]->SystFileList()->find(syst_name) 
					       == m_filekey_map[keybase]->SystFileList()->end()) ) );

    FileKeyAttributes* fkey=NULL;

    //===== FIRST FILE FOR KEY ==================
    if(b_firstfile){

      std::vector<std::string> *vfile = new std::vector<std::string>; vfile->clear();
      fkey = new FileKeyAttributes(keybase, vfile, b_multi, b_multiscale);
      m_filekey_map[keybase] = fkey;
   
      //Only check the sample key for multiple samples if it is the first file
      std::vector< std::vector<double> >* vscales = NULL; 
      if(b_multiscale){
	vscales = new std::vector< std::vector<double> >; vscales->clear();
	fkey->SetSampleScales( vscales );
      }
      else{ fkey->SetSingleSampleScales( new std::vector<double> ); }
      if(b_multi){
	std::string parseString = keybase;
	std::string sparse = "";
	std::string::size_type pos = 0;
	bool b_multiname = false;

	SampleAttributesVector* v_samp = new SampleAttributesVector; v_samp->clear();
	fkey->SetSampleList(v_samp);
	do{
	  pos = AnalysisUtils::ParseString(parseString, sparse, delim_in);
	  if( m_attr_map.find(sparse) == m_attr_map.end() ){ 
	    std::cout<<"WARNING:: Sample "<<sparse<<" is not in sample list. Ignoring"<<std::endl; 
	    continue; 
	  }
	  SampleAttributes* cur_samp = m_attr_map[sparse];
	  v_samp->push_back( cur_samp );
	  if(b_multiscale){
	    //std::vector<double> vscale_samp; vscale_samp.clear();
	    vscales->push_back( std::vector<double>() );
	  }
	  if( !b_multiname && ( (cur_samp->InSuffix() != "") || (cur_samp->InPrefix() != "") ) ){
	    b_multiname = true; fkey->SetMultiName(true);
	  }
	} while(pos != std::string::npos); //subsamples

	//fkey->SetSampleList(v_samp);
	//if(b_multiscale){ fkey->SetSampleScales( vscales ); }
	//else{ key->SetSingleSampleScales( new std::vector<double> );	}
      }//if multi
      else{ 
	if( m_attr_map.find(keybase) == m_attr_map.end() ){ 
	  std::cout<<"WARNING:: Sample "<<keybase<<" is not in sample list. Ignoring"<<std::endl; continue; 
	  continue;
	}

	fkey->SetSingleSample(m_attr_map[keybase]);
      }//if not multi

      m_filekey_map[keybase] = fkey;

    }//if first file of key
    else{ fkey = m_filekey_map[keybase]; }

    //===== FIRST SYST FILE ==================
    if(b_firstfile_systs){

      fkey->SetSystFileList( new std::map<std::string, std::vector<std::string> > );
      if(!b_multiscale){ fkey->SetSystSingleSampleScales( new std::map<std::string, std::vector<double> > ); }
      else{
	fkey->SetSystSampleScales( new std::map<std::string, std::vector<std::vector<double> > > ); 
      }

    }

    //===== FIRST SYST FILE FOR KEY ==================
    if(b_firstfile_syst_this){

      fkey->SystFileList() -> insert( std::pair<std::string, std::vector<std::string> >() );
      if(!b_multiscale){ fkey -> SystSingleSampleScales() -> insert( std::pair<std::string, std::vector<double> >( syst_name, std::vector<double>()  ) ); }
      else{   
	fkey -> SystSampleScales() -> insert( std::pair<std::string, std::vector< std::vector<double> > >( syst_name, std::vector<std::vector<double> >() ) ); 
	std::vector<std::vector<double> >&  syst_samp_scales = fkey->SystSampleScales(syst_name);
	//for( SampleAttributes*  : *(fkey ->SampleList()) ){ 
	for(SampleAttributesVector::iterator samp_it = fkey->SampleList()->begin()
	      ; samp_it != fkey->SampleList()->end(); ++samp_it) { syst_samp_scales.push_back( std::vector<double>() ); }

      }

    }// set up all the vectors for this systematic

    //=================================================

    //==========Add files to the nominal or systmatics list
    if(!b_sys){ fkey->FileList()->push_back( keymap["FILE"] ); }
    else{
      std::map<std::string, std::vector<std::string> >* syst_file_list = fkey->SystFileList();
      if( syst_file_list -> find(syst_name) == syst_file_list -> end() ){
	syst_file_list -> insert( std::pair<std::string, std::vector<std::string> >( syst_name, std::vector<std::string>() ) );
      }
      syst_file_list->at(syst_name).push_back(keymap["FILE"]);
    }

    //==========Add scales to the nominal or systematics list
    if(scalebase != ""){
      if( b_multi && b_multiscale){
	std::string parseString = scalebase;
	std::string sparse = ""; int nsamp = 0;
	std::string::size_type pos = 0;
	do{
	  pos = AnalysisUtils::ParseString(parseString, sparse, delim_in);
	  double sc = atof(sparse.c_str());
	  if(m_opt->OptStr().find("--GLOBALSCALE") != std::string::npos){ sc *= m_opt->GlobalScale(); }

	  if(b_sys){ fkey->SystSampleScales()->at(syst_name).at(nsamp).push_back(sc); }
	  else{ fkey->SampleScales()->at(nsamp).push_back(sc); }

	  nsamp++;
	} while(pos != std::string::npos); //subsamples

      }//if multiple scales given for each sample
      else{ 
	double sc = atof(scalebase.c_str());
	if(m_opt->OptStr().find("--GLOBALSCALE") != std::string::npos){ sc *= m_opt->GlobalScale(); }

	if(b_sys){ fkey->SystSingleSampleScales()->at(syst_name).push_back( sc ); }
	else{ fkey->SingleSampleScales()->push_back( sc ); }

      } 
    }//if scale
    
    keymap.clear();
  }//line loop
  
  parsed_map.clear();

  if(m_opt->MsgLevel() == Debug::DEBUG) 
    std::cout<<"PlotManager::ParseFileList : nline = "<<nline<<" m_filekey_map.size() = "<<m_filekey_map.size()<<std::endl; 

  return sc;
}

void PlotManager::WriteHistogramsToFile(){

    for(SampleAttributesMap::iterator samit = m_attr_map.begin(); samit != m_attr_map.end(); ++samit){
      if( !samit->second->Write() ){ continue; }
      const std::string& ds_suffix = samit->second->Suffix();
      const std::string& ds_outfile_name = (samit->second->OutFileName() != "" ) ? samit->second->OutFileName() : m_opt->OutputFile();
      TFile* ds_outfile = TFile::Open(ds_outfile_name.c_str(), "UPDATE");

      ds_outfile->cd();
      for( VariableAttributesMap::iterator varit = m_var_map.begin(); varit != m_var_map.end(); ++varit){
	const std::string& var_name = varit->second->Name();
	std::string key = var_name + "_" + ds_suffix;
	TH1D* hsample = (TH1D*)(m_hstMngr->GetTH1D( key )->Clone(var_name.c_str()));
	hsample->Write();
	hsample->SetDirectory(0);
	delete hsample;
      }//Variable loop
      ds_outfile->Close();
      delete ds_outfile;
    }//Sample loop
  return;
}


void PlotManager::FillHistManager(){

  if(m_opt->MsgLevel() == Debug::DEBUG) std::cout<<"PlotManager::FillHistManager start"<<std::endl; 

  ReadHistogramsFromFile(1);
  bool makeSum = m_attr_map.find("SUM") != m_attr_map.end();
  bool makeBlinder = m_attr_map.find("BLINDER") != m_attr_map.end();

  const std::string& g_blinding = m_opt->Blinding();
  const std::string& g_blind_crit = m_opt->BlindCriterion();
  const std::string& g_blind_sample = m_opt->BlindSample();
  double g_blind_thresh = m_opt->BlindThreshold();

  for( VariableAttributesMap::iterator varit = m_var_map.begin(); varit != m_var_map.end(); ++varit){
    const std::string& var_name = varit->second->Name();
    int var_rebin = varit->second->Rebin();
    const std::string& var_rebinedges = varit->second->RebinEdges();
    double* var_rebinedges_ptr = 0;
    if( (var_rebin > 0) && (var_rebinedges != "") ){ 
      var_rebinedges_ptr = new double[var_rebin]();
      ParseRebinEdges(var_rebin, var_rebinedges, var_rebinedges_ptr);
    }

    const std::string& var_doScale = varit->second->DoScale(); 
    bool b_var_isShape = (var_doScale == "SHAPE");
    bool var_draw_stack = varit->second->DrawStack();

    const std::string& var_blinding = (varit->second->Blinding() != "") ?  varit->second->Blinding() : g_blinding;
    bool var_do_blinding = ( (var_blinding != "") && (var_blinding != "NONE") );
    bool var_do_blind_threshold = var_blinding.find("THRESH") != std::string::npos;
    bool var_do_blind_bin = var_blinding.find("BIN") != std::string::npos;
  
    bool var_do_width = varit->second->DoWidth();

    //Get the blinded histogram, clone it to make a blinder
    TH1D* h_blind_sample = NULL; TH1D* h_blinder = NULL;
    std::string blinder_key = ""; 
    if(var_do_blinding){
      blinder_key = makeBlinder ? var_name + "_" + m_attr_map["BLINDER"]->Suffix() : var_name + "_blinder";
      if(g_blind_sample != ""){
	std::string blind_hist_key = var_name + "_" + m_attr_map[g_blind_sample]->Suffix();

	h_blind_sample = m_hstMngr->GetTH1D( blind_hist_key ); 
      }
    }

    TH1D* hsum = NULL;
    if(makeSum || (b_var_isShape && var_draw_stack)){
      std::string keysum = makeSum ? var_name + "_" + m_attr_map["SUM"]->Suffix() : var_name + "_sum";
      hsum = m_hstMngr->GetTH1D(keysum);
      keysum.clear();
    }
    double intgl_sum = (hsum) ? hsum->Integral() : 1.;
    if(var_do_blind_threshold && (hsum==NULL)){std::cout<<" Cannot find SUM histogram required to calculate blinding threshold. Program will crash"<<std::endl;}
    for(SampleAttributesMap::iterator samit = m_attr_map.begin(); samit != m_attr_map.end(); ++samit){
      if(m_opt->MsgLevel() == Debug::DEBUG) std::cout<<" Sample : "<<samit->first<<std::endl; 
      if(samit->first == "BLINDER"){continue;}


      const std::string& samp_blinding = samit->second->Blinding();
      const std::string& ds_suffix = samit->second->Suffix();
      const std::string& ds_drawScale = samit->second->DrawScale(); 

      std::string key = var_name + "_" + ds_suffix;
      TH1D* hsample = m_hstMngr->GetTH1D( key ); 
      if(var_do_width){
	int nbin = hsample->GetNbinsX();
	for(int nb = 0; nb <= nbin; nb++){
	  double bc = hsample->GetBinContent(nb)/hsample->GetXaxis()->GetBinWidth(nb);
	  double be = hsample->GetBinError(nb)/hsample->GetXaxis()->GetBinWidth(nb);
	  hsample->SetBinContent(nb, bc);
	  hsample->SetBinError(nb, be);
	}
      }

      if( var_do_blinding && (h_blinder==NULL) ){ h_blinder = m_hstMngr->CloneTH1D(blinder_key, hsample, true); }

      if(var_do_blind_threshold  && (samp_blinding == "SIGNAL")){
	if(var_do_blind_bin){
	  for( int b = 1; b <= hsample->GetNbinsX(); b++ ){
	    if( h_blinder->GetBinContent(b) > 0.){ continue; }
	  
	    double num = hsample->GetBinContent(b);
	    if( (g_blind_crit == "NSIG") && (num >= g_blind_thresh) ){ h_blinder->SetBinContent(b, 1.); }
	    else if(g_blind_crit == "SBYB"){
	      double den =  hsum->GetBinContent(b);
	      if( samit->second->DoSum() ){ den = den - num; }
	      double sbyb = (den > 0.) ? num/den : g_blind_thresh + 1; //blind any bin that does not have background
	      if( (num > 0) && sbyb >= g_blind_thresh ){ h_blinder->SetBinContent(b, 1.); }
	    }
	  
	  }//bin loop
	
	}//bin-by-bin blinding
	else{
	  if(h_blinder->Integral() > 0.){continue;}
	  double num = hsample->Integral();
	  if( (g_blind_crit == "NSIG") && (num >= g_blind_thresh) ){
	    for( int b = 1; b <= hsample->GetNbinsX(); b++ ){ h_blinder->SetBinContent(b, 1.); }
	  }
	  else if( g_blind_crit == "SBYB" ){
	    double den = hsum->Integral();
	    if( samit->second->DoSum() ){ den = den - num; }
	    double sbyb = (den > 0.) ? num/den : g_blind_thresh + 1.; 
	    if( (num > 0) && (sbyb >= g_blind_thresh) ){
	      for( int b = 1; b <= hsample->GetNbinsX(); b++ ){ h_blinder->SetBinContent(b, 1.); }
	    }
	  }
	}//yield blinding

      }//signal sample for blinding thresh

      //Scaling
      if( b_var_isShape && !var_draw_stack && samit->second->NoShape() ){continue;}
      if(var_rebin > 0){
	if(var_rebinedges_ptr != NULL){ VariableRebinning(key, hsample, var_rebin, var_rebinedges_ptr); }
	else{ hsample->Rebin(var_rebin); }
      }
      if( b_var_isShape || (ds_drawScale == "SHAPE") ){
	double intgl = hsample->Integral();
	double sc = 1.; 
	if(intgl > 0.){
	  sc = ( b_var_isShape && var_draw_stack && (samit->first != "SUM") && (ds_drawScale == "NORM") ) ? 1./intgl_sum : 1./intgl;
	}
	hsample->Scale(sc);
      }
      key.clear();
    }//sample loop

    //Blinding
    if( g_blind_sample != "" ){
      if(var_do_blind_bin){
	for( int b = 1; b <= h_blinder->GetNbinsX(); b++ ){
	  if(h_blinder->GetBinContent(b) < 1.){continue;}
	  else{ 
	    h_blind_sample->SetBinContent(b, 0.); 
	    h_blind_sample->SetBinError(b, 0.);
	  }
	}
      }
      else if(var_do_blinding){
	if(h_blinder->Integral() > 0. ){ h_blind_sample->Reset(); }
      }
    }

    if(!makeBlinder && (blinder_key != "")){m_hstMngr->ClearTH1(blinder_key);}
    blinder_key.clear();

    delete var_rebinedges_ptr;

  }//variable loop

  if(m_opt->MsgLevel() == Debug::DEBUG) std::cout<<"PlotManager::FillHistManager end"<<std::endl; 

  return;
}

void PlotManager::makeEfficiencyHistograms(){

  if(m_opt->MsgLevel() == Debug::DEBUG) std::cout<<"PlotManager::makeEfficiency start"<<std::endl; 

  TFile* infile = NULL;
  int fnum = 0;

  std::string var_name_num = ""; std::string var_name_den = "";

  for(FileKeyAttributesMap::iterator fn_it = m_filekey_map.begin(); fn_it != m_filekey_map.end(); ++fn_it){
    std::vector<std::string>::iterator v_it = (fn_it->second->FileList())->begin();
    int fnum_int = 0;
    for( ; v_it != (fn_it->second->FileList())->end(); ++v_it){
      infile = TFile::Open( (*v_it).c_str(), "READ" );
      if(infile == NULL){ 
	std::cout<<"ERROR: File "<<(*v_it)<<" can not be found"<<std::endl; 
	continue;
      }

      for( VariableAttributesMap::iterator varit = m_var_map.begin(); varit != m_var_map.end(); ++varit){
	
	const std::string& var_name = varit->second->Name();
	var_name_num.clear(); var_name_den.clear();
	if(m_opt->NumPrefix() != ""){ 
	  var_name_num += m_opt->NumPrefix() + "_";  
	}
	if(m_opt->DenPrefix() != ""){ 
	  var_name_den += m_opt->DenPrefix() + "_";  
	}
	var_name_num += var_name;
	var_name_den += var_name;
	if(m_opt->NumSuffix() != ""){ 
	  var_name_num += "_" + m_opt->NumSuffix();
	}
	if(m_opt->DenSuffix() != ""){ 
	  var_name_den += "_" + m_opt->DenSuffix();
	}

	std::string key_seq_num = Form("%s_%i", var_name_num.c_str(), fnum);
	m_hstMngr->ReadTH1D(var_name_num, infile, key_seq_num);
	TH1D* hkey_seq_num = m_hstMngr->GetTH1D(key_seq_num);

	std::string key_seq_den = Form("%s_%i", var_name_den.c_str(), fnum);
	m_hstMngr->ReadTH1D(var_name_den, infile, key_seq_den);
	TH1D* hkey_seq_den = m_hstMngr->GetTH1D(key_seq_den);

	TH1D* hkey_num = NULL;	TH1D* hkey_den = NULL;
	const std::string& samp = fn_it->first;
	std::string key_num = var_name_num + "_" + m_attr_map[samp]->Suffix();
	std::string key_den = var_name_den + "_" + m_attr_map[samp]->Suffix();

	hkey_num = m_hstMngr->GetTH1D(key_num);
	if(hkey_num == NULL){hkey_num = m_hstMngr->CloneTH1D(key_num, hkey_seq_num, true); }
	hkey_seq_num->Scale( fn_it->second->SingleSampleScales()->at(fnum_int) );
	hkey_num->Add(hkey_seq_num);

	hkey_den = m_hstMngr->GetTH1D(key_den);
	if(hkey_den == NULL){hkey_den = m_hstMngr->CloneTH1D(key_den, hkey_seq_den, true); }
	hkey_seq_den->Scale( fn_it->second->SingleSampleScales()->at(fnum_int) );
	hkey_den->Add(hkey_seq_den);

	key_num.clear(); key_den.clear();

	m_hstMngr->ClearTH1(key_seq_num);
	m_hstMngr->ClearTH1(key_seq_den);

	key_seq_num.clear();
	key_seq_den.clear();

      }//Variables

      infile->Close();
      delete infile;

      fnum++; fnum_int++;
    }//File loop

  }//Key loop

  std::string var_name_eff = "";
  for( VariableAttributesMap::iterator varit = m_var_map.begin(); varit != m_var_map.end(); ++varit){

    const std::string& var_name = varit->second->Name();
    var_name_num.clear(); var_name_den.clear();
    if(m_opt->NumPrefix() != ""){ 
      var_name_num += m_opt->NumPrefix() + "_";  
    }
    if(m_opt->DenPrefix() != ""){ 
      var_name_den += m_opt->DenPrefix() + "_";  
    }
    var_name_num += var_name;
    var_name_den += var_name;
    if(m_opt->NumSuffix() != ""){ 
      var_name_num += "_" + m_opt->NumSuffix();
    }
    if(m_opt->DenSuffix() != ""){ 
      var_name_den += "_" + m_opt->DenSuffix();
    }
    var_name_eff = var_name + "_EFF";

    for(SampleAttributesMap::iterator samit = m_attr_map.begin(); samit != m_attr_map.end(); ++samit){
      const std::string& ds_suffix = samit->second->Suffix();
      std::string key_num = var_name_num + "_" + ds_suffix;
      std::string key_den = var_name_den + "_" + ds_suffix;
      std::string key_eff = var_name_eff + "_" + ds_suffix;
      TH1D* hsample_num = m_hstMngr->GetTH1D( key_num ); 
      TH1D* hsample_den = m_hstMngr->GetTH1D( key_den ); 
      TH1D* hsample_eff = m_hstMngr->CloneTH1D(key_eff, key_num, true);
      hsample_eff->Divide(hsample_num, hsample_den, 1, 1, "b");

      key_num.clear(); key_den.clear(); key_eff.clear();
    }//sample loop

  
  }//variable loop

  if(m_opt->MsgLevel() == Debug::DEBUG) std::cout<<"PlotManager::makeEfficiencyHistograms end"<<std::endl; 

  return;
}


//Only implemented right now for a SINGLE sample, SINGLE scale
//fk_att must be associated to ONLY ONE SampleAttribute
//and also have only one file
//Also assume that everything is scaled, there is no such thing as blinding, and no such thing as stacking
//Read all the histograms, and put them in the hist manager
//

//int PlotManager::WriteSystematicHistogramsToFile(){

  //Here, also print out the systematic variations

//}

int PlotManager::PrintSystematics(){
  TH1D* h_nominal = NULL;
  TH1D* h_up = NULL;
  TH1D* h_down = NULL;
  std::string var_nominal = "";
  std::string var_up = "";
  std::string var_down = "";

  for(std::pair<std::string, VariableAttributes*> var_pair : m_var_map){
    VariableAttributes* variable = var_pair.second;
    const std::string& var_name = variable->Name();
    //var_nominal = var_name + samp->Suffix();

    std::ofstream var_yield_stream(Form("Yields_SR_%s.txt", var_name.c_str()));
    std::ofstream var_syst_stream(Form("Syst_SR_%s.txt", var_name.c_str()));

    for(std::pair<std::string, SampleAttributes*> samp_pair : m_attr_map){
    
      SampleAttributes* sample = samp_pair.second;
      var_nominal = var_name + sample->Suffix();
      var_up = var_name + sample->Suffix() + "_" + "_SYSUP";
      var_down = var_name + sample->Suffix() + "_" + "_SYSDOWN";

      h_nominal = m_hstMngr->GetTH1D(var_nominal);
      h_up      = m_hstMngr->GetTH1D(var_up);
      h_down    = m_hstMngr->GetTH1D(var_down);

      var_yield_stream << sample->Name();
      var_syst_stream << sample->Name();
      for(int b = 1; b <= h_nominal->GetNbinsX(); b++){
	double bc_nominal = h_nominal->GetBinContent(b)*3029.;
	double bc_up = h_up->GetBinContent(b)*3029.;
	double bc_down = h_down->GetBinContent(b)*3029.;

 	double frac_up = bc_up/bc_nominal;
 	double frac_down = bc_down/bc_nominal;

	var_yield_stream << " & " << bc_nominal;// << " & " << bc_nominal+bc_up << " & " << bc_nominal+bc_down ;
	var_syst_stream << " & " << frac_up << " & " << frac_down ;

      }
      var_yield_stream << std::endl;
      var_syst_stream << std::endl;

      var_nominal.clear();
      var_up.clear();
      var_down.clear();
    }

    var_yield_stream.close();
    var_syst_stream.close();

  }

  return 0;
}

int PlotManager::ReadSystematicsFromFiles(){

  int stat = 0;
  for(FileKeyAttributesMap::iterator fn_it = m_filekey_map.begin(); fn_it != m_filekey_map.end(); ++fn_it){
    stat += ReadAllSystematics( fn_it->second );
  }
  return stat;
}

int PlotManager::ReadAllSystematics(FileKeyAttributes* fk_att){
  if( fk_att->MultiSample() || fk_att->MultiScale() || fk_att->MultiName() ){
    std::cout<<"Multiple samples for systematics calculations are not yet supported. Please try to be less fancy"<<std::endl;
    return -1;
  }
  if( fk_att->FileList()->size() != 1){ 
    std::cout<<" Key "<<fk_att->Key() << " has "<<fk_att->FileList()->size() << " files associated to it. "<<std::endl;
    std::cout<< "Please enter only one file for the nominal and weight systematics" << std::endl; 
    return -1;
  }

  const std::string& nom_fname = fk_att->FileList()->at(0);
  SampleAttributes* samp = fk_att->SingleSample();
  TFile* infile_nom  = TFile::Open(nom_fname.c_str(), "READ");

  //input names 
  std::string var_in_syst_up="";
  std::string var_in_syst_down="";

  //output names
  std::string var_nominal="";
  std::string var_syst_up="";
  std::string var_syst_down="";
  std::string var_up="";
  std::string var_down="";
  
  for(std::pair<std::string, VariableAttributes*> var_pair : m_var_map){
    VariableAttributes* variable = var_pair.second;
    const std::string& var_name = variable->Name();
    var_nominal = var_name + samp->Suffix();
    TH1D* h_nominal = m_hstMngr->ReadTH1D(var_name, infile_nom, var_nominal);

    var_up = var_name + samp->Suffix() + "_" + "_SYSUP";
    m_hstMngr->CloneTH1D(var_up, var_nominal, true);
    //TH1D* h_up = m_hstMngr->CloneTH1D(var_up, var_nominal, true);

    var_down = var_name + samp->Suffix() + "_" + "_SYSDOWN";
    m_hstMngr->CloneTH1D(var_down, var_nominal, true);
    //TH1D* h_down = m_hstMngr->CloneTH1D(var_down, var_nominal, true);

    for(std::pair<std::string, SystematicsAttributes*> syst_pair : m_sys_map){

      SystematicsAttributes* syst = syst_pair.second;
      if(syst->NewFile()) continue;

      if(syst->OneSided()){
	var_in_syst_up = var_name + "_" + syst->Name();
	var_syst_up = var_name + samp->Suffix() + syst->Name();
	m_hstMngr->ReadTH1D(var_in_syst_up, infile_nom, var_syst_up );
	m_hstMngr->GetTH1D(var_syst_up)->Add(h_nominal, -1.);
	QuadraticHistSum(var_up, var_syst_up);

	if(syst->Symmetrisation() != ""){
	  var_syst_down = var_name + samp->Suffix() + syst->Name() + "_OPP";
	  m_hstMngr->CloneTH1D(var_syst_down, var_syst_up);
	  QuadraticHistSum(var_down, var_syst_down);
	}
      }
      else{
	var_in_syst_up = var_name + "_" + syst->NameUp();
	var_syst_up = var_name + samp->Suffix() + syst->NameUp();
	m_hstMngr->ReadTH1D(var_in_syst_up, infile_nom, var_syst_up );
	m_hstMngr->GetTH1D(var_syst_up)->Add(h_nominal, -1.);
	QuadraticHistSum(var_up, var_syst_up);

	var_in_syst_down = var_name + "_" + syst->NameDown();
	var_syst_down = var_name + samp->Suffix() + syst->NameDown();
	m_hstMngr->ReadTH1D(var_in_syst_down, infile_nom, var_syst_down );
	m_hstMngr->GetTH1D(var_syst_down)->Add(h_nominal, -1.);
	QuadraticHistSum(var_down, var_syst_down);
      }

      //Clear the histograms
      m_hstMngr->ClearTH1(var_syst_up);
      m_hstMngr->ClearTH1(var_syst_down);

      var_in_syst_up.clear();
      var_in_syst_down.clear();

    }//weight systematics loop
    var_nominal.clear();

  }//variables loop

  infile_nom->Close();
  delete infile_nom;

  //--------------- Now read the object systematics
  TFile* infile_syst_up = NULL;
  TFile* infile_syst_down = NULL;

  //input names 
  var_in_syst_up="";
  var_in_syst_down="";

  //output names
  var_nominal="";
  var_syst_up="";
  var_syst_down="";
  var_up="";
  var_down="";
  
  for(std::pair<std::string, SystematicsAttributes*> syst_pair : m_sys_map){
    SystematicsAttributes* syst = syst_pair.second;
    if(!syst->NewFile()) continue;

    infile_syst_up = NULL;
    infile_syst_down = NULL;

    if(syst->OneSided()){

      if ( fk_att->SystFileList()->find(syst->Name()) == fk_att->SystFileList()->end() ) { 
	std::cout << "ERROR:: File list for systematic "<<syst->Name()<<" not found in FileAttribute "<<fk_att->Key()<<std::endl;
	continue;
      }
      if ( fk_att->SystFileList(syst->Name()).size() != 1 ){
	std::cout << "Please enter only one file for object systematic "<<syst->Name() << std::endl;
	continue;
      }
      
      infile_syst_up = TFile::Open( fk_att->SystFileList()->at(syst->Name()).at(0).c_str(), "READ" );

    }// One-sided
    else{

      if ( fk_att->SystFileList()->find(syst->NameUp()) == fk_att->SystFileList()->end() ) { 
	std::cout << "ERROR:: File list for systematic "<<syst->NameUp()<<" not found in FileAttribute "<<fk_att->Key()<<std::endl;
	continue;
      }
      if ( fk_att->SystFileList(syst->NameUp()).size() != 1 ){
	std::cout << "Please enter only one file for object systematic "<<syst->NameUp() << std::endl;
	continue;
      }

      if ( fk_att->SystFileList()->find(syst->NameDown()) == fk_att->SystFileList()->end() ) { 
	std::cout << "ERROR:: File list for systematic "<<syst->NameDown()<<" not found in FileAttribute "<<fk_att->Key()<<std::endl;
	continue;
      }
      if ( fk_att->SystFileList(syst->NameDown()).size() != 1 ){
	std::cout << "Please enter only one file for object systematic "<<syst->NameDown() << std::endl;
	continue;
      }
      
      infile_syst_up = TFile::Open( fk_att->SystFileList()->at(syst->NameUp()).at(0).c_str(), "READ" );
      infile_syst_down = TFile::Open( fk_att->SystFileList()->at(syst->NameDown()).at(0).c_str(), "READ" );

    }//Not one-sided


    for(std::pair<std::string, VariableAttributes*> var_pair : m_var_map){
      VariableAttributes* variable = var_pair.second;
      const std::string& var_name = variable->Name();
      var_nominal = var_name + samp->Suffix();
      TH1D* h_nom = m_hstMngr->GetTH1D(var_nominal);

      var_up = var_name + samp->Suffix() + "_" + "_SYSUP";
      m_hstMngr->GetTH1D(var_up);
      //TH1D* h_up = m_hstMngr->GetTH1D(var_up);
      
      var_down = var_name + samp->Suffix() + "_" + "_SYSDOWN";
      m_hstMngr->GetTH1D(var_down);
      //TH1D* h_down = m_hstMngr->GetTH1D(var_down);
      
      if(syst->OneSided()){
	var_syst_up = var_name + samp->Suffix() + syst->Name();
      }
      else{var_syst_up = var_name + samp->Suffix() + syst->NameUp();}
      m_hstMngr->ReadTH1D(var_name, infile_syst_up, var_syst_up );
      m_hstMngr->GetTH1D(var_syst_up)->Add(h_nom, -1.);
      QuadraticHistSum(var_up, var_syst_up);

      if(!syst->OneSided()){
	var_syst_down = var_name + samp->Suffix() + syst->NameDown();
	m_hstMngr->ReadTH1D(var_name, infile_syst_down, var_syst_down );
	m_hstMngr->GetTH1D(var_syst_down)->Add(h_nom, -1.);
	QuadraticHistSum(var_down, var_syst_down);
      }//Two-sided
      else{
	if(syst->Symmetrisation() != ""){
	  var_syst_down = var_name + samp->Suffix() + syst->Name() + "_OPP";
	  m_hstMngr->CloneTH1D(var_syst_down, var_syst_up);
	  QuadraticHistSum(var_down, var_syst_down);
	}
      }//One-sided


      //Clear the histograms
      m_hstMngr->ClearTH1(var_syst_up);
      m_hstMngr->ClearTH1(var_syst_down);
      
      //And the strings
      var_nominal.clear();
      var_up.clear();
      var_down.clear();
      var_syst_up.clear();
      var_syst_down.clear();
      
    }//Variable loop


    infile_syst_up->Close();
    if(infile_syst_down != NULL) infile_syst_down->Close();

    delete infile_syst_up;
    delete infile_syst_down;

  }//object systematics loop

  //At this point, should have the total systematic up and down variations for each variable for each 
  //sample.

  return 0;

}

void PlotManager::QuadraticHistSum(const std::string& h_orig_name, const std::string& h_add_name){

  TH1D* h_orig = m_hstMngr->GetTH1D(h_orig_name);
  TH1D* h_add = m_hstMngr->GetTH1D(h_add_name);
  if( ( h_orig == NULL) || (h_add == NULL) ){ 
    std::cout << "ERROR: Histogram " << h_orig << " or " << h_add 
	      << " for quadratic sum not found. " << std::endl;
    return;
  }

  QuadraticHistSum(h_orig, h_add);

  return;
}

void PlotManager::QuadraticHistSum(TH1D* h_orig, TH1D* h_add){

  if(h_orig->GetNbinsX() != h_add->GetNbinsX()){
    std::cout << "ERROR : Different bins for h_orig and h_add" << std::endl;
    return;
  }

  for( int b = 1; b <= h_orig->GetNbinsX(); b++ ){

    double bc_orig = h_orig->GetBinContent(b);
    double bc_add = h_add->GetBinContent(b);

    h_orig -> SetBinContent( b, sqrt(bc_orig*bc_orig + bc_add*bc_add) ); 
    h_orig -> SetBinError( b, 0. );
  }

  return;

}


//--------------------------------------------------------------------------------------------------------------------
int PlotManager::ReadHistogramsFromFile(int dim){

  if(m_opt->MsgLevel() == Debug::DEBUG) std::cout<<"PlotManager::ReadHistogramsFromFile start"<<std::endl; 

  TFile* infile = NULL;
  int fnum = 0;
  bool makeSum = m_attr_map.find("SUM") != m_attr_map.end();

  for(FileKeyAttributesMap::iterator fn_it = m_filekey_map.begin(); fn_it != m_filekey_map.end(); ++fn_it){
    bool b_multi = (fn_it->second)->MultiSample();
    bool b_multiscale = (fn_it->second)->MultiScale();
    bool b_multiname = (fn_it->second)->MultiName();
    //bool b_syst = (fn_it->second)->HasSyst();

    std::vector<std::string>::iterator v_it = (fn_it->second->FileList())->begin();
    unsigned int fnum_int = 0;

    //============= NOMINAL FILES ========================================= 
    for( ; v_it != (fn_it->second->FileList())->end(); ++v_it){
      infile = TFile::Open( (*v_it).c_str(), "READ" );
      if(infile == NULL){ 
	std::cout<<"ERROR: File "<<(*v_it)<<" can not be found"<<std::endl; 
	continue;
      }
      if(m_opt->MsgLevel() == Debug::DEBUG) std::cout<<"PlotManager ::ReadHistogramsFromFile , opened file "<<(*v_it)<<std::endl;
      for( VariableAttributesMap::iterator varit = m_var_map.begin(); varit != m_var_map.end(); ++varit){

	const std::string& var_name = varit->second->Name();
	const std::string& var_doScale = varit->second->DoScale();
	if(m_opt->MsgLevel() == Debug::DEBUG) std::cout<<" var_name = "<<var_name<<std::endl;

	bool b_var_scale = (var_doScale != "NONE");
	bool b_var_isShape = (var_doScale == "SHAPE");
	bool var_draw_stack = varit->second->DrawStack();

	//Even if makeSum is false, the sum histogram will be required for normalisation when
	//drawing a stacked shape plot
	bool var_makesum = makeSum || (b_var_isShape && var_draw_stack);

	std::string keysum = "";
	if(var_makesum){
	  keysum = makeSum ? var_name + "_" + m_attr_map["SUM"]->Suffix() : var_name + "_sum";
	}

	TH1D* h1key_seq = NULL;
	TH1D* h1sum = NULL;
	TH1D* h1key = NULL;

	TH2D* h2key_seq = NULL;
	TH2D* h2sum = NULL;
	TH2D* h2key = NULL;
	
	std::string key_seq = Form("%s_%i", var_name.c_str(), fnum);
	if(!b_multiname){
	  if(dim == 1){ h1key_seq = m_hstMngr->ReadTH1D(var_name, infile, key_seq); }
	  else if(dim == 2){ h2key_seq = m_hstMngr->ReadTH2D(var_name, infile, key_seq); }
	  if(var_makesum){
	    std::string keysum = makeSum ? var_name + "_" + m_attr_map["SUM"]->Suffix() : var_name + "_sum";
	    if(dim == 1){
	      h1sum = m_hstMngr->GetTH1D(keysum);
	      if(h1sum == NULL){h1sum = m_hstMngr->CloneTH1D(keysum, h1key_seq, true); }
	    }
	    else if(dim == 2){
	      h2sum = m_hstMngr->GetTH2D(keysum);
	      if(h2sum == NULL){h2sum = m_hstMngr->CloneTH2D(keysum, h2key_seq, true); }
	    }
	    keysum.clear();
	  }//Sum histogram created
	}
	if(b_multi){

	  SampleAttributesVector* vsamp = fn_it->second->SampleList();
	  for(unsigned int js = 0; js < vsamp->size(); js++){
	    SampleAttributes* samp = vsamp->at(js);
	    bool b_samp_scale = (samp->DrawScale() != "NONE");
	    std::string key = var_name + "_" + samp->Suffix();
	    std::string key_seq_samp = Form("%s_%i_%i", var_name.c_str(), fnum, js);
	    TH1D* h1key_seq_samp = NULL; 
	    TH2D* h2key_seq_samp = NULL; 
	    double sc = 1;

	    if(b_var_scale && b_samp_scale){ 
	      if(b_multiscale){ sc = (fn_it->second)->SampleScales()->at(js).at(fnum_int); }
	      else{ sc = (fn_it->second)->SingleSampleScales()->at(fnum_int); }
	    }

	    if(dim == 1){

	      if(b_multiname){ h1key_seq_samp = m_hstMngr->ReadTH1D( samp->InPrefix() + var_name + samp->InSuffix(), infile, key_seq_samp); }
	      else{ h1key_seq_samp = m_hstMngr->CloneTH1D(key_seq_samp, h1key_seq); }

	      h1key = m_hstMngr->GetTH1D(key);
	      if(h1key == NULL){h1key = m_hstMngr->CloneTH1D(key, h1key_seq_samp, true); }
	      if(b_var_scale && b_samp_scale){ h1key_seq_samp->Scale(sc); }

	      h1key->Add(h1key_seq_samp); 
	      if(h1sum && samp->DoSum()){ h1sum->Add(h1key_seq_samp); }

	      m_hstMngr->ClearTH1(key_seq_samp);

	    }
	    else if(dim == 2){
	      if(b_multiname){ h2key_seq_samp = m_hstMngr->ReadTH2D( samp->InPrefix() + var_name + samp->InSuffix(), infile, key_seq_samp); }
	      else{ h2key_seq_samp = m_hstMngr->CloneTH2D(key_seq_samp, h2key_seq); }

	      h2key = m_hstMngr->GetTH2D(key);
	      if(h2key == NULL){h2key = m_hstMngr->CloneTH2D(key, h2key_seq_samp, true); }
	      if(b_var_scale && b_samp_scale){ h2key_seq_samp->Scale(sc); }

	      h2key->Add(h2key_seq_samp); 
	      if(h2sum && samp->DoSum()){ h2sum->Add(h2key_seq_samp); }
	      m_hstMngr->ClearTH2(key_seq_samp);
	    }

	    key_seq_samp.clear(); key.clear();

	  }//subsample loop

	  if(!b_multiname){
	    if(dim == 1){ m_hstMngr->ClearTH1(key_seq); }
	    else if(dim == 2){ m_hstMngr->ClearTH2(key_seq); }
	  }

	}//if multi
	else{

	  SampleAttributes* samp = (fn_it->second)->SingleSample();
	  bool b_samp_scale = (samp->DrawScale() != "NONE");
	  double sc = (fn_it->second)->SingleSampleScales()->at(fnum_int);
	  std::string key = var_name + "_" + samp->Suffix();
	  if(dim == 1){
	    h1key = m_hstMngr->GetTH1D(key);
	    if(h1key == NULL){h1key = m_hstMngr->CloneTH1D(key, h1key_seq, true); }

	    if(b_var_scale && b_samp_scale){ h1key_seq->Scale(sc); }
	    h1key->Add(h1key_seq);
	    if(h1sum && samp->DoSum()){ h1sum->Add(h1key_seq); }
	  }
	  else if(dim == 2){
	    h2key = m_hstMngr->GetTH2D(key);
	    if(h2key == NULL){h2key = m_hstMngr->CloneTH2D(key, h2key_seq, true); }

	    if(b_var_scale && b_samp_scale){ h2key_seq->Scale(sc); }
	    h2key->Add(h2key_seq);
	    if(h2sum && samp->DoSum()){ h2sum->Add(h2key_seq); }
	  }
	  key.clear();

	}//not multi
	key_seq.clear();

      }//Variables

      infile->Close();
      delete infile;

      fnum++; fnum_int++;
    }//File loop

  }//Key loop

  if(m_opt->MsgLevel() == Debug::DEBUG) std::cout<<"PlotManager::ReadHistogramsFromFile end"<<std::endl; 

  return 0;

}


void PlotManager::ProjectByBin(){

  bool drawHists = ( m_opt->ProjOpt().find("HIST") != std::string::npos );
  bool drawMean = ( m_opt->ProjOpt().find("MEAN") != std::string::npos );
  bool drawRMS = ( m_opt->ProjOpt().find("RMS") != std::string::npos );

  if(m_opt->MsgLevel() == Debug::DEBUG) std::cout<<"PlotManager::ProjectByBin start"<<std::endl; 

  ReadHistogramsFromFile(2);
  bool makeSum = m_attr_map.find("SUM") != m_attr_map.end();

  for( VariableAttributesMap::iterator varit = m_var_map.begin(); varit != m_var_map.end(); ++varit){

    const std::string& var_name = varit->second->Name();
    //const std::string& var_label = varit->second->Label();
    int var_rebin = varit->second->Rebin();
    const std::string& var_doScale = varit->second->DoScale(); 
    bool b_var_isShape = (var_doScale == "SHAPE");
    bool var_draw_stack = varit->second->DrawStack();

    TH2D* hsum = NULL;
    if(makeSum || (b_var_isShape && var_draw_stack)){
      std::string keysum = makeSum ? var_name + "_" + m_attr_map["SUM"]->Suffix() : var_name + "_sum";
      hsum = m_hstMngr->GetTH2D(keysum);
      keysum.clear();
    }

    int nbinx = -1;
    for(SampleAttributesMap::iterator samit = m_attr_map.begin(); samit != m_attr_map.end(); ++samit){

      if( b_var_isShape && samit->second->NoShape() ){continue;}
      const std::string& ds_suffix = samit->second->Suffix();
      const std::string& ds_drawScale = samit->second->DrawScale(); 
      std::string key = var_name + "_" + ds_suffix;
      TH2D* hsample = m_hstMngr->GetTH2D( key ); 
      TH1D* hmean = NULL; TH1D* hrms = NULL;
      if(drawMean || drawRMS){
	TH1D* projtemp = hsample->ProjectionX(Form("%s_px", key.c_str()));
	if(drawMean){
	  std::string keymean = var_name + "_MEAN_" + ds_suffix;
	  hmean = m_hstMngr->CloneTH1D(keymean, projtemp, true);
	  keymean.clear();
	}
	if(drawRMS){
	  std::string keyrms = var_name + "_RMS_" + ds_suffix;
	  hrms = m_hstMngr->CloneTH1D(keyrms, projtemp, true);
	  keyrms.clear();
	}
      }

      //Project each bin in the x-axis onto a new histogram
      if(nbinx < 0){nbinx = hsample->GetXaxis()->GetNbins();;}
      double rmin = 0.; double rmax = 0.;
      std::string projname = ""; std::string sumname = "";
      for(int nb = 1; nb<nbinx; nb++){
	std::string var_name_nb = Form("%s_%i", var_name.c_str(), nb);

	rmin = hsample->GetXaxis()->GetBinLowEdge(nb);
	rmax = hsample->GetXaxis()->GetBinLowEdge(nb);
	projname = var_name_nb + "_" + ds_suffix;
	TH1D* hproj = m_hstMngr->GetTH1D(projname);
	if(hproj == NULL){
	  hproj = hsample->ProjectionY(projname.c_str(), nb, nb);
	  hproj->SetTitle(Form("%0.f < %s < %0.f", rmin, hsample->GetXaxis()->GetTitle(), rmax));
	  m_hstMngr->SetTH1D(projname, hproj);
	}

	if(drawMean){
	  hmean->SetBinContent(nb, hproj->GetMean());
	  hmean->SetBinError(nb, hproj->GetMeanError());
	}
	if(drawRMS){
	  hrms->SetBinContent(nb, hproj->GetRMS());
	  hrms->SetBinError(nb, hproj->GetRMSError());
	}
	if(!drawHists){
	  m_hstMngr->ClearTH1(projname); 
	  continue;
	}
	if(var_rebin > 0){hproj->Rebin(var_rebin);}
	if( b_var_isShape || (ds_drawScale == "SHAPE") ){
	  double sc = 1.; 
	  if( b_var_isShape && var_draw_stack && (samit->first != "SUM") && (ds_drawScale == "NORM") ){

	    sumname = makeSum ? var_name_nb + "_" + m_attr_map["SUM"]->Suffix() : var_name_nb + "_sum";
	    TH1D* hsum_proj = m_hstMngr->GetTH1D(sumname);
	    if(hsum_proj == NULL){ 
	      hsum_proj = hsum->ProjectionY(sumname.c_str(),nb,nb);
	      m_hstMngr->SetTH1D(sumname, hsum_proj);
	    }
	    double intgl_sum = hsum_proj->Integral();
	    sc = (intgl_sum > 0.) ? 1./intgl_sum : 1.;
	  } 
	  else{ 
	    double intgl = hproj->Integral();
	    sc = (intgl > 0.) ? 1./intgl : 1.;
	  }
	  hproj->Scale(sc);
	}//if unit normalisation
	
      }//loop over bins
      
      key.clear();
    }//sample loop

    varit->second->SetNProjBin(nbinx);

  }//Variable loop

  if(m_opt->MsgLevel() == Debug::DEBUG) std::cout<<"PlotManager::ProjectByBin end"<<std::endl; 

  return;
}

void PlotManager::ParseRebinEdges(const int nbin, const std::string& bindef, double* xbins_new_ptr){

  //Parse bindef
  std::string parseString = bindef;
  std::string sparse = ""; int nedge = 0;
  std::string::size_type pos = 0;
  std::string delim_in = ",";
  do{
    pos = AnalysisUtils::ParseString(parseString, sparse, delim_in);
    double xedge = atof(sparse.c_str());
    xbins_new_ptr[nedge] = xedge;
    nedge++;
  } while(pos != std::string::npos); //all edges
  
  if(nedge != nbin+1){
    return;
  }

  return;

}

TH1D* PlotManager::VariableRebinning(const std::string& histname, int nbin, const double* binedges){

  TH1D* hnew = VariableRebinning( histname, m_hstMngr->GetTH1D(histname), nbin, binedges);
  return hnew;

}

TH1D* PlotManager::VariableRebinning(const std::string& histname, TH1D* horig, int nbin, const double* binedges){

  std::string histname_temp = histname + "_temp_rebin";
  horig->Rebin(nbin, histname_temp.c_str(), binedges);

  TH1D* hnew = (TH1D*)(gDirectory->Get(histname_temp.c_str()));
  m_hstMngr->ReplaceTH1D(histname, hnew);
  hnew->SetName(histname.c_str());

  histname_temp.clear();
  return 0;

}

