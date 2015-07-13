#include "IFAEPlotter/PlotManager.h"
#include "IFAETopFramework/HistManager.h"
#include "IFAEPlotter/PlotUtils.h"
#include "IFAEPlotter/StyleDictionary.h"

#include <fstream>

//#include <vector>


PlotManager::PlotManager(const std::string& config_sample, const std::string& filelist, const std::string& config_variables){

  m_hstMngr = new HistManager();

  m_attr_map.clear();
  m_var_map.clear();
  m_filename_map.clear();

  ParseSampleConfig(config_sample);
  ParseVariableConfig(config_variables);
  ParseFileList(filelist);

  StyleDictionary* styleDict = new StyleDictionary("test");
  m_plotUtils = new PlotUtils(m_hstMngr, m_attr_map, m_var_map, styleDict);


}


void PlotManager::Initialize(){

  return;
}
void PlotManager::Execute(){
  return;
}
void PlotManager::Terminate(){
  return;
}


int PlotManager::ParseConfigFile(const std::string config_file, std::vector<std::map<string, string> >& ret_map, const std::string& delim ){

  ret_map.clear();

  std::map<int, std::string> paramSeq; paramSeq.clear();
  std::ifstream conf_stream(config_file);
  std::string conf_line;
  while( conf_line != "BEGIN" ){  getline(conf_stream, conf_line); }
  getline(conf_stream, conf_line);

  //Header gives the sequence of configuration variables
  std::string param; param.clear();
  std::string paramString = conf_line;
  int nparam = 0;
  while(paramString.find(delim) <= paramString.size()){
    param = paramString.substr(0, paramString.find(delim));
    std::cout<<"HEADER nparam = "<<nparam<<" param = "<<param<<std::endl;;
    paramString = paramString.substr(paramString.find(delim) + delim.size());
    paramSeq[nparam] = param;
    nparam++;
  }

  std::cout<<"HEADER nparam = "<<nparam<<" param = "<<param<<std::endl;;


  int nkey = nparam;
  int nline = 0;

  map<string, string> keymap;
  while( getline(conf_stream, conf_line) ){
    if(conf_line == "END") break;
    if( conf_line.find("#") == 0 ) continue;
    paramString = conf_line;
    param = "";
    nparam = 0;
    keymap.clear();

    while(paramString.find(delim) <= paramString.size()){
      param = paramString.substr(0, paramString.find(delim));
      std::cout<<"nline = "<<nline<<" nparam = "<<nparam<<" param = "<<param<<std::endl;;
      paramString = paramString.substr(paramString.find(delim) + delim.size());
      if(nparam > nkey){
	std::cout<<"Number of parameters on line "<<nline<<" exceeds number of keys given in header"<<std::endl; 
	break;
      }
      if(paramSeq.find(nparam) == paramSeq.end()){
	std::cout<<"Mismatch between number of parameters on line "<<nline<<" and number of keys given in header"<<std::endl;
	continue;
      } 
      keymap[ paramSeq[nparam] ] = param;
      nparam++;
    }//read all given parameters

    ret_map.push_back(keymap);
    nline++;
  }

  return nline;

}


int PlotManager::ParseSampleConfig(const std::string& config_sample, const std::string& delim ){
  
  int sc = 0; 
  vector<map<string, string> > parsed_map;
  int nline = ParseConfigFile(config_sample, parsed_map, delim); 
  
  //mandatory information
  std::string name = "";
  std::string suffix = "";
  std::string leglabel = "";
  //optional information
  std::string drawopt = "";
  std::string stylekey = "";
  bool draw_stack_sample = false;
  bool is_baseline = false;
  int linestyle = -1;
  int fillstyle = -1;
  int linecolor = -1;
  int fillcolor = -1;
  float scale = 1.;

  for(int l = 0; l < nline; l++){

    name = "";
    suffix = "";
    leglabel = "";
    drawopt = "hist";

    stylekey = "";
    linestyle = l;
    fillstyle = l;
    linecolor = l;
    fillcolor = l;
    scale = 1.;

    map<string, string> keymap = parsed_map.at(l);

    if( keymap.find("NAME") != keymap.end() ){ name = keymap["NAME"];}
    else{std::cout<<"Error : No name found for sample"<<std::endl;}
    if( keymap.find("SUFFIX") != keymap.end() ){ suffix = keymap["SUFFIX"];}
    else{std::cout<<"Error : No suffix found for sample"<<std::endl;}
    if( keymap.find("LEGLABEL") != keymap.end() ){ leglabel = keymap["LEGLABEL"];}
    else{std::cout<<"Error : No legend label for sample"<<std::endl;}

    if( keymap.find("DRAWOPT") != keymap.end() ){ drawopt = keymap["DRAWOPT"];}
    if( keymap.find("STYLEKEY") != keymap.end() ){ stylekey = keymap["STYLEKEY"];}
    if( keymap.find("DRAWSTACK") != keymap.end() ){ draw_stack_sample = (atoi(keymap["DRAWSTACK"].c_str()) > 0);}
    if( keymap.find("ISBASELINE") != keymap.end() ){ is_baseline = (atoi(keymap["ISBASELINE"].c_str()) > 0);}
    if( keymap.find("LINESTYLE") != keymap.end() ){ linestyle = atoi(keymap["LINESTYLE"].c_str());}
    if( keymap.find("FILLSTYLE") != keymap.end() ){ fillstyle = atoi(keymap["FILLSTYLE"].c_str());}
    if( keymap.find("LINECOLOR") != keymap.end() ){ linecolor = atoi(keymap["LINECOLOR"].c_str());}
    if( keymap.find("FILLCOLOR") != keymap.end() ){ fillcolor = atoi(keymap["FILLCOLOR"].c_str());}
    if( keymap.find("SCALE") != keymap.end() ){ scale = atof(keymap["SCALE"].c_str());}

    std::cout<<"nline = "<<l<<" name = "<<name<<" suffix = "<<suffix<<" leglabel = "<<leglabel
	     <<" drawopt = "<<drawopt<<" stylekey = "<<stylekey
	     <<" linestyle = "<<linestyle<<" fillstyle = "<<fillstyle
	     <<" linecolor = "<<linecolor<<" fillcolor = "<<fillcolor<<std::endl;  
    //---------- read all parameters------
    //Make a SampleAttribute object and add it to the map
    SampleAttributes* sampleObj = new SampleAttributes(name, suffix, leglabel, stylekey, drawopt, scale, draw_stack_sample, is_baseline
						       , linecolor, fillcolor, linestyle, fillstyle);
    m_attr_map[name] = sampleObj;
    keymap.clear();
  }

  parsed_map.clear();
  return sc;

}

int PlotManager::ParseVariableConfig(const std::string& config_variable, const std::string& delim){

  int sc = 0; 
  std::vector<std::map<std::string, std::string> > parsed_map;
  int nline = ParseConfigFile(config_variable, parsed_map, delim); 

  std::string name = "";
  std::string label = "";
  bool draw_stack = 0;
  std::string draw_res = "";
  bool isLog = false;
  int rebin = 0;

  for(int l = 0; l < nline; l++){

    name = "";
    label = "";
    draw_stack = 0;
    draw_res = "";
    isLog = false;
    rebin = 0;
    std::map<std::string, std::string> keymap = parsed_map.at(l);

    if( keymap.find("NAME") != keymap.end() ){ name = keymap["NAME"];}
    else{std::cout<<"Error : No name found for variable"<<std::endl;}
    if( keymap.find("LABEL") != keymap.end() ){ label = keymap["LABEL"];}
    else{std::cout<<"Error : No axis label for variable"<<std::endl;}

    if( keymap.find("DRAWSTACK") != keymap.end() ){ draw_stack = (atoi(keymap["DRAWSTACK"].c_str()) >0);}
    if( keymap.find("DRAWRES") != keymap.end() ){ draw_res = keymap["DRAWRES"].c_str();}
    if( keymap.find("ISLOG") != keymap.end() ){ isLog = (atoi(keymap["ISLOG"].c_str()) > 0) ;}
    if( keymap.find("REBIN") != keymap.end() ){ rebin = atoi(keymap["REBIN"].c_str());}

    std::cout<<"nline = "<<l<<" name = "<<name<<" label = "<<label<<" draw_stack = "<<draw_stack<<" draw_res = "<<draw_res<<" isLog = "<<isLog<<" rebin = "<<rebin<<std::endl;  

    VariableAttributes* varObj = new VariableAttributes(name, label, draw_stack, draw_res, isLog, rebin);
    m_var_map[name] = varObj;
    keymap.clear();
  }

  parsed_map.clear();
  return sc;

}
int PlotManager::ParseFileList(const std::string& filelist, const std::string& delim){
  int sc = 0; 
  std::vector<std::map<std::string, std::string> > parsed_map;
  int nline = ParseConfigFile(filelist, parsed_map, delim); 

  std::string key = "";
  std::string fileloc = "";

  //m_filename_map;  
  for(int l = 0; l < nline; l++){

    std::map<std::string, std::string> keymap = parsed_map.at(l);
    if( keymap.find("KEY") != keymap.end() ){
      if(m_filename_map.find(keymap["KEY"]) == m_filename_map.end()){ 
	std::vector<std::string> vfile;
	m_filename_map[ keymap["KEY"] ] = vfile; 
      }
    }
    else{std::cout<<"Error : No sample key found for file name"<<std::endl;}

    if( keymap.find("FILE") != keymap.end() ){
      m_filename_map[ keymap["KEY"] ].push_back( keymap["FILE"] );
    }
    else{std::cout<<"Error : No file name given"<<std::endl;}

    keymap.clear();
  }

  parsed_map.clear();

  //Loop over the m_filename_map to check that everything is in order
  std::map<std::string, std::vector<std::string> >::iterator mit = m_filename_map.begin();
  for( ; mit != m_filename_map.end(); ++mit){
    unsigned int nfiles = (mit->second).size();
    std::cout<<"Sample : "<<mit->first<<" nfiles = "<<nfiles<<std::endl;
    for(unsigned int j = 0; j < nfiles; j++){
      std::cout<<(mit->second).at(j)<<std::endl;
    }
    std::cout<<std::endl;
  }

  return sc;
}

