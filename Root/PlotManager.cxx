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
  m_filescale_map.clear();

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
  vector<string> hlist = m_hstMngr->GetTH1KeyList();
  //for(string hname : hlist){ std::cout<<hname<<std::endl; }
  //std::cout<<"End PlotManager m_hstMngr"<<std::endl;
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
    paramString = paramString.substr(paramString.find(delim) + delim.size());
    paramSeq[nparam] = param;
    nparam++;
  }

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
      paramString = paramString.substr(paramString.find(delim) + delim.size());
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
  int do_scale = 0;

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
    do_scale = 0;

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
    if( keymap.find("DOSCALE") != keymap.end() ){ do_scale = atoi(keymap["DOSCALE"].c_str());}

    std::cout<<"nline = "<<l<<" name = "<<name<<" suffix = "<<suffix<<" leglabel = "<<leglabel
	     <<" drawopt = "<<drawopt<<" stylekey = "<<stylekey
	     <<" linestyle = "<<linestyle<<" fillstyle = "<<fillstyle
	     <<" linecolour = "<<linecolour<<" fillcolour = "<<fillcolour<<std::endl;  
    //---------- read all parameters------
    //Make a SampleAttribute object and add it to the map
    SampleAttributes* sampleObj = new SampleAttributes(name, suffix, leglabel, stylekey, drawopt, do_scale, draw_stack_sample, is_baseline
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
      if(m_filescale_map.find(keymap["KEY"]) == m_filescale_map.end()){ 
	std::vector<double> vscale;
	m_filescale_map[ keymap["KEY"] ] = vscale; 
      }
    }
    else{std::cout<<"Error : No sample key found for file name"<<std::endl;}

    if( keymap.find("FILE") != keymap.end() ){
      m_filename_map[ keymap["KEY"] ].push_back( keymap["FILE"] );
    }
    else{std::cout<<"Error : No file name given"<<std::endl;}
    std::cout<<" File parsing key = "<<keymap["KEY"]<<" scale = "<<keymap["SCALE"]<<std::endl;
    if( keymap.find("SCALE") != keymap.end() ){
      m_filescale_map[ keymap["KEY"] ].push_back( atof(keymap["SCALE"].c_str()) );
    }
    else{ m_filescale_map[ keymap["KEY"] ].push_back( 1. ); }

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

  vector<TFile*> infile_map; infile_map.clear();
  //TFile* infile = NULL;
  int fnum = 0;
  double fscale = 1.;
  for( SampleAttributesMap::iterator samit = m_attr_map.begin(); samit != m_attr_map.end(); ++samit){
    if( m_filename_map.find(samit->first) == m_filename_map.end() ){
      if(samit->first != "SUM"){ 
	std::cout << "Error:: No files found for sample "<< samit->first << std::endl; 
      }
      continue;
    }

    const std::string& ds_suffix = samit->second->Suffix();
    int ds_doScale = samit->second->DoScale();
    
    unsigned int nfiles = (m_filename_map[samit->first]).size();
    std::cout<<"Sample : "<<samit->first<<" nfiles = "<<nfiles<<std::endl;
    for(unsigned int j = 0; j < nfiles; j++){

      std::cout<<(m_filename_map[samit->first]).at(j)<<std::endl;
      infile_map.push_back( TFile::Open( (m_filename_map[samit->first].at(j)).c_str(), "READ" ) );
      //infile = TFile::Open( (m_filename_map[samit->first].at(j)).c_str(), "READ" ) ;
      fscale = m_filescale_map[samit->first].at(j);

      for( VariableAttributesMap::iterator varit = m_var_map.begin(); varit != m_var_map.end(); ++varit){
	
	const std::string& var_name = varit->second->Name();
	std::string key = var_name + "_" + ds_suffix;

	TString tkeyseq = key + "_";
	tkeyseq += j;
	std::string key_seq = (std::string)tkeyseq;
	m_hstMngr->ReadTH1D(var_name, infile_map.at(fnum), key_seq);
	//m_hstMngr->ReadTH1D(var_name, infile, key_seq);
	TH1D* hkey_seq = m_hstMngr->GetTH1D(key_seq);
	//std::cout<<"BEFORE SCALING fscale = "<<fscale<<" key_seq = "<<key_seq<<" integral = "<<hkey_seq->Integral()<<std::endl;
	//Scale if needed
	if(ds_doScale < 0){
	  double intgl_a = hkey_seq->Integral();
	  double sc_a = (intgl_a > 0.) ? 1./intgl_a : 1.;
	  hkey_seq->Scale(sc_a);
	}
	else if(ds_doScale > 0){
	  hkey_seq->Scale(fscale);
	}
	else{;}
	//std::cout<<"AFTER SCALING fscale = "<<fscale<<" key_seq = "<<key_seq<<" integral = "<<hkey_seq->Integral()<<std::endl;

	TH1D* hkey = NULL;
	if(j == 0){
	  hkey = (TH1D*)( m_hstMngr->GetTH1D(key_seq)->Clone() );
	  hkey->SetName(key.c_str());
	  m_hstMngr->SetTH1D(key, hkey);
	}
	else{
	  hkey = m_hstMngr->GetTH1D(key);
	  hkey->Add( m_hstMngr->GetTH1D(key_seq) ); 
	}
	m_hstMngr->ClearTH1(key_seq);
	key.clear();
	key_seq.clear();
      }//variable loop
      //infile->Close();
      //delete infile;
      fnum++;
    }//loop over file vector
  }//Loop over samples

  bool makeSum = m_attr_map.find("SUM") != m_attr_map.end();
  for( VariableAttributesMap::iterator varit = m_var_map.begin(); varit != m_var_map.end(); ++varit){
    const std::string& var_name = varit->second->Name();
    int var_rebin = varit->second->Rebin();
    SampleAttributesMap::iterator samit = m_attr_map.begin();
    //Make the sum of histograms if needed
    TH1D* hsum = NULL;
    if(makeSum){
      std::string keybeg = var_name + "_" + samit->second->Suffix();
      std::string keysum = var_name + "_" + m_attr_map["SUM"]->Suffix();
      hsum = (TH1D*)(m_hstMngr->GetTH1D(keybeg)->Clone());
      hsum->Reset();
      hsum->SetName(keysum.c_str());
      m_hstMngr->SetTH1D(keysum, hsum);
      keysum.clear();
      keybeg.clear();
    }

    for(; samit != m_attr_map.end(); ++samit){
      const std::string& ds_suffix = samit->second->Suffix();
      std::string key = var_name + "_" + ds_suffix;
      TH1D* hsample = m_hstMngr->GetTH1D( key ); 
      if(var_rebin > 0){hsample->Rebin(var_rebin);}

      if( (!samit->second->DrawStack()) || (samit->first == "SUM") ){continue;}
      if(hsum){hsum->Add(hsample);}

      key.clear();
    }//sample loop
  
  }//variable loop


}
