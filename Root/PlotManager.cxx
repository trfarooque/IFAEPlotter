#include "IFAEPlotter/Plotter_Options.h"
#include "IFAEPlotter/PlotManager.h"
#include "IFAETopFramework/HistManager.h"
#include "IFAEPlotter/PlotUtils.h"
#include "IFAEPlotter/StyleDictionary.h"
#include "IFAEPlotter/SampleAttributes.h"
#include "IFAEPlotter/VariableAttributes.h"
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
  m_filekey_map.clear(); 

  //m_filename_map.clear();
  //m_filekey_multi_map.clear();
  //m_filekey_samples.clear();
  //m_filescale_map.clear();
  //m_multi_extra.clear();

  m_new_sample_format = false;
  m_new_variable_format = false;
  m_new_style_format = false;
  m_new_filelist_format = false;

  bool opt_has_configformat = (m_opt->OptStr().find("--NEWCONFIG") != std::string::npos); 
  bool opt_has_sampleformat = (m_opt->OptStr().find("--NEWSAMPLECONFIG") != std::string::npos); 
  bool opt_has_variableformat = (m_opt->OptStr().find("--NEWVARIABLECONFIG") != std::string::npos); 
  bool opt_has_styleformat = (m_opt->OptStr().find("--NEWSTYLECONFIG") != std::string::npos); 
  bool opt_has_filelistformat = (m_opt->OptStr().find("--NEWFILELIST") != std::string::npos); 

  if(opt_has_sampleformat){ m_new_sample_format = m_opt->NewSampleFormat(); }
  else if(opt_has_configformat){ m_new_sample_format = m_opt->NewConfigFormat(); }

  if(opt_has_variableformat){ m_new_variable_format = m_opt->NewVariableFormat(); }
  else if(opt_has_configformat){ m_new_variable_format = m_opt->NewConfigFormat(); }

  if(opt_has_styleformat){ m_new_style_format = m_opt->NewStyleFormat(); }
  else if(opt_has_configformat){ m_new_style_format = m_opt->NewConfigFormat(); }

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

  if( m_opt->AllFromFile() ){
    const std::string& dist_file = (m_opt->DistributionsFile() != "") 
      ? m_opt->DistributionsFile() : (m_filekey_map.begin()->second)->FileList()->at(0);
    if(m_opt->MsgLevel() == Debug::DEBUG) std::cout << "Copying all distributions from file " << dist_file << std::endl;
    CopyVariableListFromFile( dist_file ); 
  }
  else{
    if( m_var_map.find("ALL") != m_var_map.end() ){
      std::cout<<" WARNING: ALL keyword in variable configuration will be ignored because ALLFROMFILE=FALSE in globa; options"<<std::endl;
      delete m_var_map["ALL"];
      m_var_map.erase( m_var_map.find("ALL") );
    }
  }
  if(stat < 0){return;}

  if(m_opt->MsgLevel() == Debug::DEBUG) std::cout<<"Parsing style library"<<std::endl; 
  m_styleDict = new StyleDictionary("test");
  stat = ParseStyleConfig( m_opt->StyleLib() );
  if(stat < 0){return;}

  //std::vector<std::string>* ptr_extra = (m_multi_extra.size() > 0) ? &m_multi_extra : NULL; 

  m_plotUtils = new PlotUtils(m_opt, m_hstMngr, m_attr_map, m_var_map, m_styleDict);

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


  if(m_opt->MsgLevel() == Debug::DEBUG) std::cout<<"PlotManager::Execute end"<<std::endl; 

  return;

}
void PlotManager::Terminate(){

  if(m_opt->MsgLevel() == Debug::DEBUG) std::cout<<"PlotManager::Terminate start"<<std::endl; 

  m_hstMngr->ClearAllTH1();
  m_hstMngr->ClearAllTH2();
  m_hstMngr->ClearAllTH3();
  m_plotUtils->Terminate();

  //m_filename_map.clear();
  //m_filekey_multi_map.clear();
  //m_filekey_samples.clear();
  //m_filescale_map.clear();

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
    if(m_opt->MsgLevel() == Debug::DEBUG) std::cout<<" nparam = "<<nparam<<" param = "<<param<<std::endl; 
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


int PlotManager::ParseSampleConfig(const std::string& config_sample, const std::string& delim ){
  
  int sc = 0; 
  vector<map<string, string> > parsed_map;
  int nline = ParseConfigFile(config_sample, parsed_map, delim, m_new_sample_format); 
  if(nline < 0){ std::cout<<"Sample configuration file could not be opened. Exiting"<<std::endl; return nline; }
  std::string name = "";
  /*
  //mandatory information
  std::string suffix = "";
  std::string leglabel = "";
  //optional information
  std::string drawopt = "";
  std::string resdrawopt = "";
  std::string legopt = "";
  std::string stylekey = "";
  std::string drawscale = "";
  std::string blinding = "NONE";
  std::string yield_format = "";
  bool draw_stack_sample = false;
  bool do_sum = false;
  int res_opt = -1;
  bool write = false;
  std::string outfile_name = "";
  std::string in_suffix = "";
  std::string in_prefix = "";
  bool no_shape = false;
  */
  for(int l = 0; l < nline; l++){
    name = "";
    /*
    suffix = "";
    leglabel = "";
    drawopt = "hist";
    resdrawopt="";
    legopt = "";
    stylekey = "";
    drawscale = "NORM";
    blinding = "NONE";
    yield_format = "";
    draw_stack_sample = false;
    do_sum = false;
    res_opt = -1;
    write = false;
    outfile_name = "";
    in_suffix = "";
    in_prefix = "";
    no_shape = false;
    */
    map<string, string> keymap = parsed_map.at(l);

    SampleAttributes* sampleObj = new SampleAttributes();
    if( keymap.find("NAME") != keymap.end() ){ 
      name = keymap["NAME"];
      sampleObj->SetName( name ); 
    }
    else{
      std::cout<<"Error : No name found for sample on line "<<l<<". Ignoring."<<std::endl; 
      delete sampleObj; 
      continue;
    }
    if( keymap.find("INSUFFIX") != keymap.end() ){ sampleObj->SetInSuffix(keymap["INSUFFIX"]); }
    if( keymap.find("INPREFIX") != keymap.end() ){ sampleObj->SetInPrefix(keymap["INPREFIX"]); }

    if( keymap.find("SUFFIX") != keymap.end() ){ sampleObj->SetSuffix(keymap["SUFFIX"]); }
    else if( sampleObj->InSuffix() != "" ){ sampleObj->SetSuffix( sampleObj->InSuffix() ); }
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
    //Make a SampleAttribute object and add it to the map
    //SampleAttributes* sampleObj = new SampleAttributes(name, suffix, leglabel, stylekey, drawopt, legopt, drawscale
    //						       , draw_stack_sample, do_sum, res_opt, resdrawopt, blinding, yield_format
    //						       , write, outfile_name, in_suffix, in_prefix, no_shape);
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
  std::cout<<" config_variable = "<<config_variable<<std::endl;
  int nline = ParseConfigFile(config_variable, parsed_map, delim, m_new_variable_format); 
  if(nline < 0){ std::cout<<"Variable configuration file could not be opened. Exiting"<<std::endl; return nline; }

  std::string name = "";
  /*
  std::string label = "";
  std::string ylabel = "";
  std::string reslabel = "";
  std::string extralabel = "";
  std::string blinding = "";

  bool draw_stack = false;
  std::string draw_res = "";
  std::string draw_res_err = "";
  std::string do_scale = "NONE";
  bool isLogY = false;
  bool isLogX = false;
  int rebin = 0;
  std::string rebinvar = "";
  bool do_width = false;
  std::string resdrawopt = "";
  double resmin = 0.5;
  double resmax = 1.5;
  bool has_resmin = false;
  bool has_resmax = false;
  double ymin = 0.;
  double ymax = 0.;
  bool has_ymin = false;
  bool has_ymax = false;
  double xmin = 0.;
  double xmax = 0.;
  bool has_xmin = false;
  bool has_xmax = false;

  double ttl_xmin = 0.;
  double ttl_ymin = 0.;
  double ttl_xmax = 0.;
  double ttl_ymax = 0.;
  bool has_ttl_xmin = false;
  bool has_ttl_ymin = false;
  bool has_ttl_xmax = false;
  bool has_ttl_ymax = false;
  */
  for(int l = 0; l < nline; l++){
    name = "";
    /*
    label = "";
    ylabel = "";
    reslabel = "";
    extralabel = "";
    blinding = "";

    draw_stack = 0;
    draw_res = "";
    draw_res_err = "";
    do_scale = "NONE";
    do_width = false;
    isLogY = false;
    isLogX = false;
    rebin = 0;
    rebinvar = "";
    resdrawopt = "";
    resmin = 0.5;
    resmax = 1.5;
    has_resmin = false;
    has_resmax = false;

    ymin = 0.;
    ymax = 0.;
    has_ymin = false;
    has_ymax = false;

    xmin = 0.;
    xmax = 0.;
    has_xmin = false;
    has_xmax = false;

    ttl_xmin = 0.;
    ttl_ymin = 0.;
    ttl_xmax = 0.;
    ttl_ymax = 0.;
    has_ttl_xmin = false;
    has_ttl_ymin = false;
    has_ttl_xmax = false;
    has_ttl_ymax = false;
    */

    VariableAttributes* varObj = new VariableAttributes();

    std::map<std::string, std::string> keymap = parsed_map.at(l);

    if( keymap.find("NAME") != keymap.end() ){ 
      name = keymap["NAME"];
      varObj->SetName(name);
    }
    else{
      std::cout<<"Error : No name found for variable"<<std::endl;
      delete varObj;
      continue;
    }
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

    /*
    VariableAttributes* varObj = new VariableAttributes(name, label, do_scale, do_width, draw_stack, draw_res, draw_res_err
							, isLogY, isLogX
							, ylabel, reslabel, has_resmin, has_resmax, resmin, resmax
							, has_ymin, has_ymax, ymin, ymax, has_xmin, has_xmax, xmin, xmax
							, has_ttl_xmin, has_ttl_xmax, ttl_xmin, ttl_xmax 
							, has_ttl_ymin, has_ttl_ymax, ttl_ymin, ttl_ymax 
							, resdrawopt, extralabel, rebin, rebinvar, blinding);
    */
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
  /*
  std::string linecolour = "";
  int linestyle = 1;
  int linewidth = 2;
  std::string fillcolour = "";
  int fillstyle = 0;
  std::string markercolour = "";
  int markerstyle = 1;
  int markersize = 1;
  */
  for(int l = 0; l < nline; l++){

    key = "";
    /*
    linecolour = "";
    linestyle = 1;
    linewidth = 2;
    fillcolour = "";
    fillstyle = 0;
    markercolour = "";
    markerstyle = 1;
    markersize = 1;
    */
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
    /*
    std::cout<<"nline = "<<l<<" key = "<<key<<" linecolour = "<<linecolour<<" linestyle = "<<linestyle<<" linewidth = "<<linewidth
	     <<" fillcolour = "<<fillcolour<<" fillstyle = "<<fillstyle
	     <<" markercolour = "<<markercolour<<" markerstyle = "<<markerstyle<<" markersize = "<<markersize<<std::endl;
    */
    //m_styleDict->AddStyle(key, linecolour, linestyle, linewidth, markercolour, markerstyle, markersize, fillcolour, fillstyle); 
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

  const std::string delim_in = ",";
  for(int l = 0; l < nline; l++){

    std::map<std::string, std::string> keymap = parsed_map.at(l);
    std::string keybase = "";
    std::string scalebase = "";
    if( keymap.find("KEY") != keymap.end() ){keybase = keymap["KEY"];}
    else{std::cout<<"Error : No sample key found for file name"<<std::endl; continue;}
    if( keymap.find("SCALE") != keymap.end() ){scalebase = keymap["SCALE"];}
    if( keymap.find("FILE") == keymap.end() ){std::cout<<"Error : No file name given for sample key "<<keymap["KEY"]<<std::endl; continue; }

    bool b_multi = (keybase.find(delim_in) != std::string::npos);      
    bool firstfile = (m_filekey_map.find(keymap["KEY"]) == m_filekey_map.end());

    FileKeyAttributes* fkey=NULL;

    if(firstfile){
      //Make a fileattribute and put it in 
      std::vector<std::string> *vfile = new std::vector<std::string>; vfile->clear();
      //std::vector<std::string>* vsample=NULL; 
      //if(b_multi){ vsample = new std::vector<std::string>; }
      fkey = new FileKeyAttributes(keybase, vfile, b_multi);
      m_filekey_map[keybase] = fkey;
   
      //Only check the sample key for multiple samples if it is the first file
      //Put this in a struct
      //m_filekey_multi_map[keybase] = b_multi;
      //m_filekey_multiname_map[keybase] = false;
      //std::vector<std::string>* vfile = new std::vector<std::string>; 
      //vfile->clear();
      //m_filename_map[ keybase ] = vfile; 
      
      if(b_multi){
	std::string parseString = keybase;
	std::string sparse = "";
	std::string::size_type pos = 0;
	bool b_multiname = false;

	std::vector< std::vector<double> >* vscales = new std::vector< std::vector<double> >; vscales->clear();

	SampleAttributesVector* v_samp = new SampleAttributesVector; v_samp->clear();
	do{
	  pos = AnalysisUtils::ParseString(parseString, sparse, delim_in);
	  if( m_attr_map.find(sparse) == m_attr_map.end() ){ 
	    std::cout<<"WARNING:: Sample "<<sparse<<" is not in sample list. Ignoring"<<std::endl; 
	    continue; 
	  }
	  SampleAttributes* cur_samp = m_attr_map[sparse];
	  v_samp->push_back( cur_samp );
	  std::vector<double> vscale_samp; vscale_samp.clear();
	  vscales->push_back( vscale_samp);

	  if( !b_multiname && ( (cur_samp->InSuffix() != "") || (cur_samp->InPrefix() != "") ) ){
	    b_multiname = true;
	    //m_filekey_map[keybase]=true;
	  }
	  //if(cur_samp->FileScales() == NULL){
	    //}
	  //m_filescale_map[ sparse ] = vscale; 
	} while(pos != std::string::npos); //subsamples

	fkey->SetSampleList(v_samp);
	fkey->SetSampleScales( vscales );
	
	//m_filekey_samples[keybase] = v_samp;
      }//if multi
      else{ 
	if( m_attr_map.find(keybase) == m_attr_map.end() ){ 
	  std::cout<<"WARNING:: Sample "<<keybase<<" is not in sample list. Ignoring"<<std::endl; continue; 
	  continue;
	}
	fkey->SetSingleSample(m_attr_map[keybase]);
	std::vector<double>* vscale = new std::vector<double>; vscale->clear();
	fkey->SetSingleSampleScales( vscale );

      }//if not multi

      m_filekey_map[keybase] = fkey;

    }//if first file of key
    else{ fkey = m_filekey_map[keybase]; }

    //m_filename_map[ keybase ].push_back( keymap["FILE"] );
    fkey->FileList()->push_back( keymap["FILE"] );

    if(scalebase != ""){
      if(b_multi){
	std::string parseString = scalebase;
	std::string sparse = ""; int nsamp = 0;
	std::string::size_type pos = 0;
	do{
	  pos = AnalysisUtils::ParseString(parseString, sparse, delim_in);
	  double sc = atof(sparse.c_str());
	  if(m_opt->OptStr().find("--GLOBALSCALE") != std::string::npos){ sc *= m_opt->GlobalScale(); }
	  fkey->SampleScales()->at(nsamp).push_back(sc);
	  //(fkey->GetSamples()->at(nsamp))->GetFileScales()->push_back(sc);
	  //m_attr_map[ m_filekey_samples[keybase]->GetSamples().at(nsamp) ]->GetFileScales()->push_back(sc); 
	  nsamp++;
	} while(pos != std::string::npos); //subsamples

      }//if multi
      else{ 
	double sc = atof(scalebase.c_str());
	if(m_opt->OptStr().find("--GLOBALSCALE") != std::string::npos){ sc *= m_opt->GlobalScale(); }
	fkey->SingleSampleScales()->push_back( sc );
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
      std::cout<<" samit->first = "<<samit->first<<" write = "<<samit->second->Write()<<std::endl; 
      if( !samit->second->Write() ){ continue; }
      const std::string& ds_suffix = samit->second->Suffix();
      const std::string& ds_outfile_name = (samit->second->OutFileName() != "" ) ? samit->second->OutFileName() : m_opt->OutputFile();
      TFile* ds_outfile = TFile::Open(ds_outfile_name.c_str(), "UPDATE");
      std::cout<<" samit->first = "<<samit->first<<" write = "<<samit->second->Write()<<" ds_outfile = "<<ds_outfile_name<<std::endl; 

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
      //if( b_var_isShape && (std::find(m_multi_extra.begin(), m_multi_extra.end(), samit->first) != m_multi_extra.end()) ){continue;}

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
  //std::map<std::string, std::vector<std::string> >::iterator fn_it = m_filename_map.begin();
  //for( ; fn_it != m_filename_map.end(); ++fn_it){
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
	hkey_seq_num->Scale( fn_it->second->SingleSampleScales()->at(fnum_int) ); //m_filescale_map[samp].at(fnum_int));
	hkey_num->Add(hkey_seq_num);

	hkey_den = m_hstMngr->GetTH1D(key_den);
	if(hkey_den == NULL){hkey_den = m_hstMngr->CloneTH1D(key_den, hkey_seq_den, true); }
	hkey_seq_den->Scale( fn_it->second->SingleSampleScales()->at(fnum_int) );//	hkey_seq_den->Scale(m_filescale_map[samp].at(fnum_int));
	hkey_den->Add(hkey_seq_den);

	key_num.clear(); key_den.clear();

	m_hstMngr->ClearTH1(key_seq_num);
	m_hstMngr->ClearTH1(key_seq_den);
	key_seq_num.clear();
	key_seq_den.clear();

      }//Variables

      infile->Close();
      delete infile; //infile_map.erase(infile_map.begin() + fnum);

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

/*
int PlotManager::ReadHistogramsFromFile(int dim){

  if(m_opt->MsgLevel() == Debug::DEBUG) std::cout<<"PlotManager::ReadHistogramsFromFile start"<<std::endl; 
  TFile* infile = NULL;
  int fnum = 0;
  bool makeSum = m_attr_map.find("SUM") != m_attr_map.end();

  for(FileKeyAttributesMap::iterator fn_it = m_filekey_map.begin(); fn_it != m_filekey_map.end(); ++fn_it){
    //bool b_multi = m_filekey_multi_map[fn_it->first];

	//1. Decide - does this key need different prefixes or suffixes?
    //if(b_multi){}




    std::vector<std::string>::iterator v_it = (fn_it->second->FileList())->begin();
    unsigned int fnum_int = 0;
    for( ; v_it != (fn_it->second->FileList())->end(); ++v_it){
      infile = TFile::Open( (*v_it).c_str(), "READ" );
      if(infile == NULL){ 
	std::cout<<"ERROR: File "<<(*v_it)<<" can not be found"<<std::endl; 
	continue;
      }
      if(m_opt->MsgLevel() == Debug::DEBUG) std::cout<<"PlotManager ::ReadHistogramsFromFile , opened file "<<(*v_it)<<std::endl;
      for( VariableAttributesMap::iterator varit = m_var_map.begin(); varit != m_var_map.end(); ++varit){






















      }//variable loop

    }//file loop

  }//key loop



}
*/

int PlotManager::ReadHistogramsFromFile(int dim){

  if(m_opt->MsgLevel() == Debug::DEBUG) std::cout<<"PlotManager::ReadHistogramsFromFile start"<<std::endl; 

  TFile* infile = NULL;
  int fnum = 0;
  bool makeSum = m_attr_map.find("SUM") != m_attr_map.end();

  //std::map<std::string, std::vector<std::string> >::iterator fn_it = m_filename_map.begin();
  //for( ; fn_it != m_filename_map.end(); ++fn_it){
  

  for(FileKeyAttributesMap::iterator fn_it = m_filekey_map.begin(); fn_it != m_filekey_map.end(); ++fn_it){
    bool b_multi = (fn_it->second)->MultiSample();//m_filekey_multi_map[fn_it->first];

    std::vector<std::string>::iterator v_it = (fn_it->second->FileList())->begin();
    unsigned int fnum_int = 0;
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


	TH1D* h1key_seq = NULL;
	TH1D* h1sum = NULL;
	TH1D* h1key = NULL;

	TH2D* h2key_seq = NULL;
	TH2D* h2sum = NULL;
	TH2D* h2key = NULL;
	
	//std::string key = var_name + "_TEMP";
	std::string key_seq = Form("%s_%i", var_name.c_str(), fnum);

	if(dim == 1){
	  m_hstMngr->ReadTH1D(var_name, infile, key_seq);
	  h1key_seq = m_hstMngr->GetTH1D(key_seq);
	}
	else if(dim == 2){
	  m_hstMngr->ReadTH2D(var_name, infile, key_seq);
	  h2key_seq = m_hstMngr->GetTH2D(key_seq);
	} 
	if(makeSum || (b_var_isShape && var_draw_stack)){
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

	if(b_multi){

	  SampleAttributesVector* vsamp = fn_it->second->SampleList();//m_filekey_samples[fn_it->first];
	  for(unsigned int js = 0; js < vsamp->size(); js++){
	    SampleAttributes* samp = vsamp->at(js);
	    bool b_samp_scale = (samp->DrawScale() != "NONE");
	    std::string key = var_name + "_" + samp->Suffix();
	    std::string key_seq_samp = Form("%s_%i_%i", var_name.c_str(), fnum, js);
	    TH1D* h1key_seq_samp = NULL; 
	    TH2D* h2key_seq_samp = NULL; 
	    double sc = 1;
	    if(b_var_scale && b_samp_scale){ 
	      sc = (fn_it->second)->SampleScales()->at(js).at(fnum_int);
	      /*
	      if( (m_filescale_map.find(samp) != m_filescale_map.end()) 
		  && (m_filescale_map[samp].size() > fnum_int) ) 
		{
		  sc = m_filescale_map[samp].at(fnum_int);
		}
	      */

	    }


	    if(dim == 1){
	      h1key = m_hstMngr->GetTH1D(key);
	      if(h1key == NULL){h1key = m_hstMngr->CloneTH1D(key, h1key_seq, true); }
	      h1key_seq_samp = m_hstMngr->CloneTH1D(key_seq_samp, h1key_seq);
	      if(b_var_scale && b_samp_scale){ h1key_seq_samp->Scale(sc); }

	      h1key->Add(h1key_seq_samp); 
	      if(h1sum && samp->DoSum()){ h1sum->Add(h1key_seq_samp); }
	      m_hstMngr->ClearTH1(key_seq_samp);
	    }
	    else if(dim == 2){
	      h2key = m_hstMngr->GetTH2D(key);
	      if(h2key == NULL){h2key = m_hstMngr->CloneTH2D(key, h2key_seq, true); }
	      h2key_seq_samp = m_hstMngr->CloneTH2D(key_seq_samp, h2key_seq);
	      if(b_var_scale && b_samp_scale){ h2key_seq_samp->Scale(sc); }

	      h2key->Add(h2key_seq_samp); 
	      if(h2sum && samp->DoSum()){ h2sum->Add(h2key_seq_samp); }
	      m_hstMngr->ClearTH2(key_seq_samp);
	    }

	    key_seq_samp.clear(); key.clear();

	  }//subsample loop

	}//if multi
	else{
	  SampleAttributes* samp = (fn_it->second)->SingleSample();
	  bool b_samp_scale = (samp->DrawScale() != "NONE");
	  double sc = (fn_it->second)->SingleSampleScales()->at(fnum_int); //m_filescale_map[samp].at(fnum_int);
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
	}

	if(dim == 1){ m_hstMngr->ClearTH1(key_seq); }
	else if(dim == 2){ m_hstMngr->ClearTH2(key_seq); }
	key_seq.clear();

      }//Variables

      infile->Close();
      delete infile; //infile_map.erase(infile_map.begin() + fnum);

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

