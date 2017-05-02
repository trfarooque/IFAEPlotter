#include "IFAEPlotter/FileKeyAttributes.h"
#include "IFAEPlotter/SampleAttributes.h"
#include "IFAEPlotter/Plotter_Options.h"
FileKeyAttributes::FileKeyAttributes(const std::string& key, std::vector<std::string>* file_list, bool multi_sample
				     , bool multi_scale, bool multi_name, bool has_syst
				     , const SampleAttributes* single_sample, SampleAttributesConstVector* sample_list
				     , std::map<std::string, std::vector<std::string> >* syst_file_list) : 
  m_key(key),
  m_multi_sample(multi_sample),
  m_multi_name(multi_name),
  m_multi_scale(multi_scale),
  m_has_syst(has_syst),
  m_file_list(file_list),
  m_syst_file_list(syst_file_list),
  m_single_sample(single_sample),
  m_sample_list(sample_list),
  m_single_sample_scales(NULL),
  m_sample_scales(NULL),
  m_syst_single_sample_scales(NULL),
  m_syst_sample_scales(NULL)
{}

FileKeyAttributes::FileKeyAttributes(): FileKeyAttributes("",NULL){ }

FileKeyAttributes::FileKeyAttributes( FileKeyAttributes& q){
  m_key                       = q.m_key;
  m_multi_sample              = q.m_multi_sample;
  m_multi_name                = q.m_multi_name;
  m_multi_scale               = q.m_multi_scale;
  m_has_syst                  = q.m_has_syst;
  m_file_list                 = q.m_file_list;
  m_syst_file_list            = q.m_syst_file_list;
  m_single_sample             = q.m_single_sample;
  m_sample_list               = q.m_sample_list;
  m_single_sample_scales      = q.m_single_sample_scales;
  m_sample_scales             = q.m_sample_scales;
  m_syst_single_sample_scales = q.m_syst_single_sample_scales;
  m_syst_sample_scales        = q.m_syst_sample_scales;
}

FileKeyAttributes::~FileKeyAttributes(){

  delete m_file_list;
  delete m_syst_file_list;
  delete m_sample_list;
  delete m_single_sample_scales;
  delete m_sample_scales;
  delete m_syst_single_sample_scales;
  delete m_syst_sample_scales;

}

FileKeyAttributesMap FileKeyAttributes::ParseFileList(Plotter_Options* opt, const SampleAttributesMap& attr_map){

  const std::string& filelist = opt->FileList(); 
  const std::string& delim = opt->ConfigDelim();
  bool block_format = false;
  if( opt->OptStr().find("--NEWFILELIST") != std::string::npos ){ block_format = opt->NewFileListFormat(); }
  else if( opt->OptStr().find("--NEWCONFIG") != std::string::npos ){ block_format = opt->NewConfigFormat(); }

  FileKeyAttributesMap filekey_map; filekey_map.clear();
  std::vector<std::map<std::string, std::string> > parsed_map;
  int nline = AnalysisUtils::ParseConfigFile(filelist, parsed_map, delim, block_format); 
  if(nline < 0){ std::cout<<"File list "<<filelist<<" could not be opened. Exiting"<<std::endl; return filekey_map; }

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

    bool b_firstfile = (filekey_map.find(keymap["KEY"]) == filekey_map.end());

    bool b_sys = (syst_name != "");
    bool b_firstfile_systs = ( b_sys && ( b_firstfile || (filekey_map[keybase]->SystFileList()==NULL) ) );
    bool b_firstfile_syst_this = ( b_sys && ( b_firstfile_systs || 
					      (filekey_map[keybase]->SystFileList()->find(syst_name) 
					       == filekey_map[keybase]->SystFileList()->end()) ) );

    FileKeyAttributes* fkey=NULL;

    //===== FIRST FILE FOR KEY ==================
    if(b_firstfile){

      std::vector<std::string> *vfile = new std::vector<std::string>; vfile->clear();
      fkey = new FileKeyAttributes(keybase, vfile, b_multi, b_multiscale);
      filekey_map[keybase] = fkey;
   
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

	SampleAttributesConstVector* v_samp = new SampleAttributesConstVector; v_samp->clear();
	fkey->SetSampleList(v_samp);
	do{
	  pos = AnalysisUtils::ParseString(parseString, sparse, delim_in);
	  if( attr_map.find(sparse) == attr_map.end() ){ 
	    std::cout<<"WARNING:: Sample "<<sparse<<" is not in sample list. Ignoring"<<std::endl; 
	    continue; 
	  }
	  const SampleAttributes* cur_samp = attr_map.at(sparse);
	  v_samp->push_back( cur_samp );
	  if(b_multiscale){
	    vscales->push_back( std::vector<double>() );
	  }
	  if( !b_multiname && ( (cur_samp->InPattern() != "") || (cur_samp->InSuffix() != "") || (cur_samp->InPrefix() != "") ) ){
	    b_multiname = true; fkey->SetMultiName(true);
	  }
	} while(pos != std::string::npos); //subsamples

      }//if multi
      else{ 
	if( attr_map.find(keybase) == attr_map.end() ){ 
	  std::cout<<"WARNING:: Sample "<<keybase<<" is not in sample list. Ignoring"<<std::endl; continue; 
	  continue;
	}

	fkey->SetSingleSample(attr_map.at(keybase));
      }//if not multi

      filekey_map[keybase] = fkey;

    }//if first file of key
    else{ fkey = filekey_map[keybase]; }

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
	for(SampleAttributesConstVector::iterator samp_it = fkey->SampleList()->begin()
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
	  if(opt->OptStr().find("--GLOBALSCALE") != std::string::npos){ sc *= opt->GlobalScale(); }

	  if(b_sys){ fkey->SystSampleScales()->at(syst_name).at(nsamp).push_back(sc); }
	  else{ fkey->SampleScales()->at(nsamp).push_back(sc); }

	  nsamp++;
	} while(pos != std::string::npos); //subsamples

      }//if multiple scales given for each sample
      else{ 
	double sc = atof(scalebase.c_str());
	if(opt->OptStr().find("--GLOBALSCALE") != std::string::npos){ sc *= opt->GlobalScale(); }

	if(b_sys){ fkey->SystSingleSampleScales()->at(syst_name).push_back( sc ); }
	else{ fkey->SingleSampleScales()->push_back( sc ); }

      } 
    }//if scale
    
    keymap.clear();
  }//line loop
  
  parsed_map.clear();

  if(opt->MsgLevel() == Debug::DEBUG) 
    std::cout<<"PlotManager::ParseFileList : nline = "<<nline<<" m_filekey_map.size() = "<<filekey_map.size()<<std::endl; 

  return filekey_map;
}
