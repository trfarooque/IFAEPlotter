#include "IFAEPlotter/PlotManager.h"
#include "IFAETopFramework/HistManager.h"
#include "IFAEPlotter/PlotUtils.h"
#include "IFAEPlotter/StyleDictionary.h"

#include "TString.h"

#include <fstream>

//#include <vector>


PlotManager::PlotManager(const std::string& config_sample, const std::string& filelist, const std::string& config_variables, const std::string& config_style){

  m_hstMngr = new HistManager();

  m_attr_map.clear();
  m_var_map.clear();
  m_filename_map.clear();

  ParseSampleConfig(config_sample);
  ParseVariableConfig(config_variables);
  ParseFileList(filelist);

  m_styleDict = new StyleDictionary("test");
  ParseStyleConfig(config_style);

  m_plotUtils = new PlotUtils(m_hstMngr, m_attr_map, m_var_map, m_styleDict);

}


void PlotManager::Initialize(){

  return;
}

void PlotManager::Execute(){

  FillHistManager();
  m_plotUtils->OverlayHists();

  return;

}
void PlotManager::Terminate(){

  m_hstMngr->ClearAllTH1();
  m_hstMngr->ClearAllTH2();
  m_hstMngr->ClearAllTH3();

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
  int linecolour = -1;
  int fillcolour = -1;
  float scale = 1.;

  for(int l = 0; l < nline; l++){

    name = "";
    suffix = "";
    leglabel = "";
    drawopt = "hist";

    stylekey = "";
    linestyle = l;
    fillstyle = l;
    linecolour = l;
    fillcolour = l;
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
    if( keymap.find("LINECOLOUR") != keymap.end() ){ linecolour = atoi(keymap["LINECOLOUR"].c_str());}
    if( keymap.find("FILLCOLOUR") != keymap.end() ){ fillcolour = atoi(keymap["FILLCOLOUR"].c_str());}
    if( keymap.find("SCALE") != keymap.end() ){ scale = atof(keymap["SCALE"].c_str());}

    std::cout<<"nline = "<<l<<" name = "<<name<<" suffix = "<<suffix<<" leglabel = "<<leglabel
	     <<" drawopt = "<<drawopt<<" stylekey = "<<stylekey
	     <<" linestyle = "<<linestyle<<" fillstyle = "<<fillstyle
	     <<" linecolour = "<<linecolour<<" fillcolour = "<<fillcolour<<std::endl;  
    //---------- read all parameters------
    //Make a SampleAttribute object and add it to the map
    SampleAttributes* sampleObj = new SampleAttributes(name, suffix, leglabel, stylekey, drawopt, scale, draw_stack_sample, is_baseline
						       , linecolour, fillcolour, linestyle, fillstyle);
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


//-----------EDIT-------------------- 
int PlotManager::ParseStyleConfig(const std::string& config_style, const std::string& delim){

  int sc = 0; 
  std::vector<std::map<std::string, std::string> > parsed_map;
  int nline = ParseConfigFile(config_style, parsed_map, delim); 

  std::string key = "";
  std::string linecolour = "";
  int linestyle = 1;
  int linewidth = 2;
  std::string fillcolour = "";
  int fillstyle = 0;

  for(int l = 0; l < nline; l++){

    key = "";
    linecolour = "";
    linestyle = 1;
    linewidth = 2;
    fillcolour = "";
    fillstyle = 0;
    std::map<std::string, std::string> keymap = parsed_map.at(l);

    if( keymap.find("KEY") != keymap.end() ){ key = keymap["KEY"];}
    else{std::cout<<"Error : No key found for style"<<std::endl;}
    if( ( keymap.find("LINECOLOUR") == keymap.end() ) && ( keymap.find("FILLCOLOUR") == keymap.end() ) 
	&& ( keymap.find("COLOUR") == keymap.end() ) ){
      std::cout<<"Error: No colours found for style"<<std::endl;
    }

    if( keymap.find("LINECOLOUR") != keymap.end() ){ linecolour = keymap["LINECOLOUR"]; }
    else if( keymap.find("LINECOLOUR") != keymap.end() ){ linecolour = keymap["COLOUR"]; } 
    if( keymap.find("LINESTYLE") != keymap.end() ){ linestyle = atoi(keymap["LINESTYLE"].c_str());}
    if( keymap.find("LINEWIDTH") != keymap.end() ){ linewidth = atoi(keymap["LINEWIDTH"].c_str());}
    if( keymap.find("FILLCOLOUR") != keymap.end() ){ fillcolour = keymap["FILLCOLOUR"]; }
    else if( keymap.find("FILLCOLOUR") != keymap.end() ){ fillcolour = keymap["COLOUR"]; } 
    if( keymap.find("FILLSTYLE") != keymap.end() ){ fillstyle = atoi(keymap["FILLSTYLE"].c_str());}

    if( keymap.find("LINECOLOR") != keymap.end() ){ std::cout<<"Field LINECOLOR not recognised. Did you mean LINECOLOUR? #godsavethequeen"<<std::endl; }
    if( keymap.find("FILLCOLOR") != keymap.end() ){ std::cout<<"Field FILLCOLOR not recognised. Did you mean FILLCOLOUR? #godsavethequeen"<<std::endl; }
    if( keymap.find("COLOR") != keymap.end() ){ std::cout<<"Field COLOR not recognised. Did you mean COLOUR? #godsavethequeen"<<std::endl; }

    std::cout<<"nline = "<<l<<" key = "<<key<<" linecolour = "<<linecolour<<" linestyle = "<<linestyle<<" linewidth = "<<linewidth
	     <<" fillcolour = "<<fillcolour<<" fillstyle = "<<fillstyle<<std::endl;

    m_styleDict->AddStyle(key, linecolour, linestyle, linewidth, fillcolour, fillstyle); 
    keymap.clear();
  }

  parsed_map.clear();
  return sc;

}
//-----------EDIT END-------------------- 

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


  return sc;
}

void PlotManager::FillHistManager(){

  //Loop over m_filename_map
  //For every dataset, loop over the list of files 
  //read in the list of histograms specified in the variable map
  //the execute another loop over the variable maps, make the summed histograms
  //Remove the original histogram as soon as it has been added to the sum

  //std::map<std::string, std::vector<std::string> >::iterator mit = m_filename_map.begin();
  //for( ; mit != m_filename_map.end(); ++mit){


  for( SampleAttributesMap::iterator samit = m_attr_map.begin(); samit != m_attr_map.end(); ++samit){
    if( m_filename_map.find(samit->first) == m_filename_map.end() ){
      std::cout << "Error:: No files found for sample "<< samit->first << std::endl;
      continue;
    }

    const std::string& ds_suffix = samit->second->Suffix();
    
    unsigned int nfiles = (m_filename_map[samit->first]).size();
    std::cout<<"Sample : "<<samit->first<<" nfiles = "<<nfiles<<std::endl;
    for(unsigned int j = 0; j < nfiles; j++){
      std::cout<<(m_filename_map[samit->first]).at(j)<<std::endl;
      TFile* infile = TFile::Open( (m_filename_map[samit->first].at(j)).c_str(), "READ" );
      for( VariableAttributesMap::iterator varit = m_var_map.begin(); varit != m_var_map.end(); ++varit){
	
	const std::string& var_name = varit->second->Name();
	int var_rebin = varit->second->Rebin();
	
	std::string key = var_name + "_" + ds_suffix;
	TString tkeyseq = key + "_";
	tkeyseq += j;
	std::string key_seq = (std::string)tkeyseq;
	std::cout<<" 0 read"<<std::endl;
	m_hstMngr->ReadTH1D(var_name, infile, key_seq);
	std::cout<<" 1 read"<<std::endl;
	std::cout<<"0 j = "<<j<<" key_seq = "<<key_seq<<" getTH1 : "<<m_hstMngr->GetTH1D(key_seq)<<std::endl;
	TH1D* hkey = NULL;
	if(j == 0){
	  std::cout<<" 0 retrieve"<<std::endl;
	  hkey = (TH1D*)( m_hstMngr->GetTH1D(key_seq)->Clone() );
	  std::cout<<" 1 retrieve"<<std::endl;
	  std::cout<<" j = "<<j<<" key = "<<key<<" hkey = "<<hkey<<std::endl;
	  hkey->SetName(key.c_str());
	  m_hstMngr->SetTH1D(key, hkey);
	  std::cout<<"j = "<<j<<" setTH1 : "<<m_hstMngr->GetTH1D(key)<<std::endl;
	}
	else{
	  std::cout<<" 0 else retrieve"<<std::endl;
	  hkey = m_hstMngr->GetTH1D(key);
	  std::cout<<" 1 else retrieve"<<std::endl;
	  std::cout<<" j = "<<j<<" key = "<<key<<" hkey = "<<hkey<<" hkey_seq = "<<m_hstMngr->GetTH1D(key_seq)<<std::endl;
	  hkey->Add( m_hstMngr->GetTH1D(key_seq) ); 
	  std::cout<<"Added histogram "<<key_seq<<std::endl;
	}
	m_hstMngr->ClearTH1(key_seq);
	//std::cout<<"1 j = "<<j<<" key_seq = "<<key_seq<<" getTH1 : "<<m_hstMngr->GetTH1D(key_seq)<<std::endl;
	  
	key.clear();
	key_seq.clear();
	std::cout<<"End point of variable loop"<<std::endl;
      }//variable loop
      infile->Close();
      delete infile;
      std::cout<<"End point of file vector loop "<<std::endl;

    }//loop over file vector
    std::cout<<"End point of sample loop"<<std::endl;  
    std::cout<<std::endl;
  }//Loop over samples


  /*

	TH1D* hkey = m_hstMngr->GetTH1D(key);
	//Rebin and scale as necessary
	if(var_rebin > 0){ hkey->Rebin(var_rebin); }
	if(ds_scale < 0.){
	  double intgl_a = hkey->Integral();
	  double sc_a = (intgl_a > 0.) ? 1./intgl_a : 1.;
	  hkey->Scale(sc_a);
	}
	else if(ds_scale > 0.){hkey->Scale(ds_scale);}
  */


  for( VariableAttributesMap::iterator varit = m_var_map.begin(); varit != m_var_map.end(); ++varit){
    const std::string& var_name = varit->second->Name();
    int var_rebin = varit->second->Rebin();

    for( SampleAttributesMap::iterator samit = m_attr_map.begin(); samit != m_attr_map.end(); ++samit){

      const std::string& ds_suffix = samit->second->Suffix();
      float ds_scale = samit->second->Scale();

      std::string key = var_name + "_" + ds_suffix;
      TH1D* hsample = m_hstMngr->GetTH1D( key );
      if(ds_scale < 0.){
	double intgl_a = hsample->Integral();
	double sc_a = (intgl_a > 0.) ? 1./intgl_a : 1.;
	hsample->Scale(sc_a);
      }
      else if(ds_scale > 0.){hsample->Scale(ds_scale);}

      key.clear();
    }//sample loop
  
  }//variable loop


}
