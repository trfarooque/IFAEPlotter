#include "IFAEPlotter/PlotManager.h"
#include "IFAETopFramework/HistManager.h"
#include "IFAEPlotter/PlotUtils.h"
#include "IFAEPlotter/StyleDictionary.h"

//#include "TString.h"

#include <fstream>

//#include <vector>


PlotManager::PlotManager(const std::string& config_sample, const std::string& filelist, const std::string& config_variables, const std::string& config_style){

  m_hstMngr = new HistManager();

  m_attr_map.clear();
  m_var_map.clear();
  m_filename_map.clear();
  m_filekey_multi_map.clear();
  m_filekey_samples.clear();
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

std::string::size_type PlotManager::ParseString(std::string& base, std::string& piece, const std::string& delim ){

  std::string::size_type pos = base.find(delim);
  if(pos != std::string::npos ){
    piece = base.substr(0, pos);
    base = base.substr(pos + delim.size());
  }
  else {piece = base;}
  return pos;

}

int PlotManager::ParseConfigFile(const std::string& config_file, std::vector<std::map<string, string> >& ret_map, const std::string& delim ){

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
  std::string::size_type pos = 0;
  do{ 
    pos = ParseString(paramString, param, delim);
    paramSeq[nparam] = param;
    nparam++;
  }while(pos != std::string::npos);

  int nkey = nparam;
  int nline = 0;

  map<string, string> keymap;
  while( getline(conf_stream, conf_line) ){
    if(conf_line == "END") break;
    if( conf_line.find("#") == 0 ) continue;
    paramString = conf_line;
    param = "";
    nparam = 0;
    pos = 0;
    keymap.clear();

    do{
      pos = ParseString(paramString, param, delim);
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
  int nline = ParseConfigFile(config_sample, parsed_map, delim); 
  
  //mandatory information
  std::string name = "";
  std::string suffix = "";
  std::string leglabel = "";
  //optional information
  std::string drawopt = "";
  std::string stylekey = "";
  std::string drawscale = "";
  bool draw_stack_sample = false;
  bool is_baseline = false;
  int linestyle = -1;
  int fillstyle = -1;
  int linecolour = -1;
  int fillcolour = -1;

  for(int l = 0; l < nline; l++){

    name = "";
    suffix = "";
    leglabel = "";
    drawopt = "hist";
    drawscale = "NORM";

    stylekey = "";
    linestyle = l;
    fillstyle = l;
    linecolour = l;
    fillcolour = l;

    map<string, string> keymap = parsed_map.at(l);

    if( keymap.find("NAME") != keymap.end() ){ name = keymap["NAME"];}
    else{std::cout<<"Error : No name found for sample"<<std::endl;}
    if( keymap.find("SUFFIX") != keymap.end() ){ suffix = keymap["SUFFIX"];}
    else{std::cout<<"Error : No suffix found for sample"<<std::endl;}
    if( keymap.find("LEGLABEL") != keymap.end() ){ leglabel = keymap["LEGLABEL"];}
    else{std::cout<<"Error : No legend label for sample"<<std::endl;}

    if( keymap.find("STYLEKEY") != keymap.end() ){ stylekey = keymap["STYLEKEY"];}
    if( keymap.find("DRAWOPT") != keymap.end() ){ drawopt = keymap["DRAWOPT"];}
    if( keymap.find("DRAWSCALE") != keymap.end() ){ drawscale = keymap["DRAWSCALE"];}
    if( keymap.find("DRAWSTACK") != keymap.end() ){ draw_stack_sample = (atoi(keymap["DRAWSTACK"].c_str()) > 0);}
    if( keymap.find("ISBASELINE") != keymap.end() ){ is_baseline = (atoi(keymap["ISBASELINE"].c_str()) > 0);}
    if( keymap.find("LINESTYLE") != keymap.end() ){ linestyle = atoi(keymap["LINESTYLE"].c_str());}
    if( keymap.find("FILLSTYLE") != keymap.end() ){ fillstyle = atoi(keymap["FILLSTYLE"].c_str());}
    if( keymap.find("LINECOLOUR") != keymap.end() ){ linecolour = atoi(keymap["LINECOLOUR"].c_str());}
    if( keymap.find("FILLCOLOUR") != keymap.end() ){ fillcolour = atoi(keymap["FILLCOLOUR"].c_str());}

    std::cout<<"nline = "<<l<<" name = "<<name<<" suffix = "<<suffix<<" leglabel = "<<leglabel
	     <<" drawopt = "<<drawopt<<" stylekey = "<<stylekey<<" drawscale = "<<drawscale
	     <<" linestyle = "<<linestyle<<" fillstyle = "<<fillstyle
	     <<" linecolour = "<<linecolour<<" fillcolour = "<<fillcolour<<std::endl;  
    //---------- read all parameters------
    //Make a SampleAttribute object and add it to the map
    SampleAttributes* sampleObj = new SampleAttributes(name, suffix, leglabel, stylekey, drawopt, drawscale, draw_stack_sample, is_baseline
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
  std::string do_scale = "NONE";
  bool isLog = false;
  int rebin = 0;

  for(int l = 0; l < nline; l++){

    name = "";
    label = "";
    draw_stack = 0;
    draw_res = "";
    do_scale = "NONE";
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
    if( keymap.find("DOSCALE") != keymap.end() ){ do_scale = keymap["DOSCALE"].c_str();}

    std::cout<<"nline = "<<l<<" name = "<<name<<" label = "<<label<<" draw_stack = "<<draw_stack<<" draw_res = "<<draw_res<<" isLog = "<<isLog<<" rebin = "<<rebin<<std::endl;  

    VariableAttributes* varObj = new VariableAttributes(name, label, do_scale, draw_stack, draw_res, isLog, rebin);
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
    bool firstfile = (m_filename_map.find(keymap["KEY"]) == m_filename_map.end());
    if(firstfile){
      //Only check the sample key for multiple samples if it is the first file
      
      m_filekey_multi_map[keybase] = b_multi;
      
      std::vector<std::string> vfile;
      m_filename_map[ keybase ] = vfile; 
      
      if(b_multi){
	std::string parseString = keybase;
	std::string sparse = "";
	int pos = 0;
	vector<std::string> v_samp; v_samp.clear();
	do{
	  pos = ParseString(parseString, sparse, delim_in);
	  v_samp.push_back(sparse);
	  
	  std::vector<double> vscale; vscale.clear();
	  m_filescale_map[ sparse ] = vscale; 
	} while(pos != std::string::npos); //subsamples
	m_filekey_samples[keybase] = v_samp;
      }//if multi
    }//if first file of key
    m_filename_map[ keybase ].push_back( keymap["FILE"] );
    
    if(scalebase != ""){
      if(b_multi){
	std::string parseString = scalebase;
	std::string sparse = ""; int nsamp = 0;
	int pos = 0;
	do{
	  pos = ParseString(parseString, sparse, delim_in);
	  double sc = atof(sparse.c_str());
	  m_filescale_map[ m_filekey_samples[keybase].at(nsamp) ].push_back(sc); 
	  nsamp++;
	} while(pos != std::string::npos); //subsamples

      }//if multi
      else{ m_filescale_map[ keybase ].push_back( atof(scalebase.c_str()) );} 
	
    }//if scale
    
    keymap.clear();
  }//line loop
  
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
  //double fscale = 1.;

  std::map<std::string, std::vector<std::string> >::iterator fn_it = m_filename_map.begin();
  for( ; fn_it != m_filename_map.end(); ++fn_it){
    bool b_multi = m_filekey_multi_map[fn_it->first];

    std::vector<std::string>::iterator v_it = (fn_it->second).begin();
    int fnum_int = 0;
    for( ; v_it != (fn_it->second).end(); ++v_it){
      bool b_firstfile = (v_it == (fn_it->second).begin());
      infile_map.push_back( TFile::Open( (*v_it).c_str(), "READ" ) );

      for( VariableAttributesMap::iterator varit = m_var_map.begin(); varit != m_var_map.end(); ++varit){
	
	const std::string& var_name = varit->second->Name();
	const std::string& var_doScale = varit->second->DoScale();
	//std::string key = var_name + "_TEMP";
	std::string key_seq = Form("%s_%i", var_name.c_str(), fnum);
	m_hstMngr->ReadTH1D(var_name, infile_map.at(fnum), key_seq);
	TH1D* hkey_seq = m_hstMngr->GetTH1D(key_seq);

	TH1D* hkey = NULL;
	if(b_multi){

	  std::vector<std::string>& vsamp = m_filekey_samples[fn_it->first];
	  for(unsigned int js = 0; js < vsamp.size(); js++){
	    const std::string& samp = vsamp.at(js);
	    //Make the histogram name to which hkey_seq is to be added
	    //And also decide whether or not to scale 
	    //const std::string& ds_suffix = vsamp.at(js)->Suffix();
	    //const std::string& ds_drawScale = m_attr_map[samp]->DrawScale();

	    std::string key = var_name + "_" + m_attr_map[samp]->Suffix();
	    if(b_firstfile){ hkey = m_hstMngr->CloneTH1D(key, hkey_seq, true); }//if first file
	    else{ hkey = m_hstMngr->GetTH1D(key); }

	    std::string key_seq_samp = Form("%s_%i_%i", var_name.c_str(), fnum, js);
	    TH1D* hkey_seq_samp = m_hstMngr->CloneTH1D(key_seq_samp, hkey_seq);
	    if(var_doScale != "NONE"){ 
	      double sc = 1.;
	      if( (m_filescale_map.find(samp) != m_filescale_map.end()) 
		  && (m_filescale_map[samp].size() > fnum_int) ) 
		{
		  sc = m_filescale_map[samp].at(fnum_int);
		}

	      hkey_seq_samp->Scale(sc); 
	    }
	    hkey->Add(hkey_seq_samp);
	    m_hstMngr->ClearTH1(key_seq_samp);
	    key_seq_samp.clear(); key.clear();

	  }//subsample loop

	}//if multi
	else{
	  const std::string& samp = fn_it->first;
	  std::string key = var_name + "_" + m_attr_map[samp]->Suffix();
	  if(b_firstfile){ hkey = m_hstMngr->CloneTH1D(key, hkey_seq, true); }//if first file
	  else{ hkey = m_hstMngr->GetTH1D(key); }

	  if(var_doScale != "NONE"){ hkey_seq->Scale(m_filescale_map[samp].at(fnum_int)); }
	  hkey->Add(hkey_seq);
	  key.clear();
	}

	m_hstMngr->ClearTH1(key_seq);
	key_seq.clear();

      }//Variables
      infile_map[fnum]->Close();
      delete infile_map[fnum]; //infile_map.erase(infile_map.begin() + fnum);

      fnum++; fnum_int++;
    }//File loop

  }//Key loop

  bool makeSum = m_attr_map.find("SUM") != m_attr_map.end();
  for( VariableAttributesMap::iterator varit = m_var_map.begin(); varit != m_var_map.end(); ++varit){
    const std::string& var_name = varit->second->Name();
    int var_rebin = varit->second->Rebin();
    const std::string& var_doScale = varit->second->DoScale(); 
    SampleAttributesMap::iterator samit = m_attr_map.begin();
    //Make the sum of histograms if needed
    TH1D* hsum = NULL;
    if( (var_doScale != "SHAPE") && makeSum){
      std::string keybeg = var_name + "_" + samit->second->Suffix();
      std::string keysum = var_name + "_" + m_attr_map["SUM"]->Suffix();
      hsum = m_hstMngr->CloneTH1D(keysum, m_hstMngr->GetTH1D(keybeg), true);
      keysum.clear();
      keybeg.clear();
    }

    for(; samit != m_attr_map.end(); ++samit){
      if( (var_doScale == "SHAPE") && (samit->first == "SUM") ){continue;}
      const std::string& ds_suffix = samit->second->Suffix();
      const std::string& ds_drawScale = samit->second->DrawScale(); 
      std::string key = var_name + "_" + ds_suffix;
      TH1D* hsample = m_hstMngr->GetTH1D( key ); 
      if(var_rebin > 0){hsample->Rebin(var_rebin);}
      if( (var_doScale == "SHAPE") || (ds_drawScale == "SHAPE") ){
	double intgl = hsample->Integral();
	double sc = (intgl > 0) ? 1./intgl : 1.;
	hsample->Scale(sc);
      }
      if( (!samit->second->DrawStack()) || (samit->first == "SUM") ){continue;}
      if(hsum){hsum->Add(hsample);}

      key.clear();
    }//sample loop
  
  }//variable loop


}
