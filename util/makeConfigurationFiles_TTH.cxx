#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "TFile.h"
#include "TH1.h"
#include "TChain.h"
#include "IFAETopFramework/AnalysisUtils.h"
#include "IFAETopFramework/SampleInfo.h"

std::string m_dir_sel="";
std::string m_xsec_map_loc="";
std::string m_ds_map_loc="";
std::string m_MC15_TDP="";
std::string m_listloc="";
std::string m_hist_file_prefix="";
std::string m_sample_info = "";
std::string m_input_pattern = "";
std::string m_output_pattern = "";

int m_cutflow_weight=1;
bool m_make_file_list = false;
bool m_make_sample_config = false;
bool m_make_variable_config = false;

double GetXSec(const std::string& dsID);
double GetSumWgtFromTrees(const std::string& fileloc);
double GetSumWgtFromHists(const std::string& fileloc);

//void ParseSampleDat(const std::string& dsID, std::string& sample_name, std::string& sumwgt, std::string& xsec);

std::string GetSampleID(const std::string& dsID);
std::string GetSampleIDFromTDP(const std::string& dsID);

void makeSampleConfigurationFile();
void makeVariableConfigurationFile();
void makeFileLists(); //directly from sample_info.dat
void makeFileLists_OLD(); 
void makeFileLists_NEW(); //using the output list from TTHAnalysis
int main(int argc, char** argv){


  //std::string dsID = "";
  m_dir_sel = "ejets"; //input
  m_xsec_map_loc = "/afs/cern.ch/work/f/farooque/TTHNtupleAna/XSection-MC15-13TeV-fromSusyGrp.data.txt"; //input - fixed from TDP
  //m_ds_map_loc = "/afs/cern.ch/work/f/farooque/TTHNtupleAna/sample_DSID_map.txt"; //input - fixed by MC15.py from TDP (perhaps write script to make this)
  m_MC15_TDP = "/afs/cern.ch/work/f/farooque/TTHNtupleAna/MC15.py"; 
  m_listloc = "/afs/cern.ch/work/f/farooque/TTHNtupleAna/inputlist_TEST.txt"; //input
  m_hist_file_prefix = "/afs/cern.ch/work/f/farooque/TTHNtupleAna/HISTLISTS_NEW/HistListTESTDATMC"; //input
  m_sample_info = "/afs/cern.ch/work/f/farooque/RunIIArea/DEVAREA/samples_info_TTH.dat"; //input
  //m_input_pattern = "/nfs/at3/scratch2/rodina/TTHAnalysisOutput_2016_01_22_1518/outrunTTH_%DSID%._%SYS%_0Hist.root";
  m_input_pattern = "outrunTTH_%DSID%._%SYS%_0Hist.root";
  m_output_pattern = "ttH";

  int i_sample = 0;
  int i_variable = 0;
  int i_flist = 0;
  std::stringstream ss; std::string sw = "";
  int optind = 1;
  while ((optind < argc) && (argv[optind][0]=='-')) {
    ss << argv[optind]; sw = ss.str();
    //std::cout<<"Parsing '"<<argv[optind]<<" : "<<sw<<std::endl;
    if(sw == "--makeFileLists"){optind++;  ss.str(""); ss << argv[optind]; ss >> i_flist;}
    else if(sw == "--makeSampleConfig"){optind++;  ss.str(""); ss << argv[optind]; ss >> i_sample;}
    else if(sw == "--makeVariableConfig"){optind++;  ss.str(""); ss << argv[optind]; ss >> i_variable;}

    else if     (sw == "--sample_info"){optind++; ss.str(""); ss << argv[optind]; ss >> m_sample_info;}  
    else if     (sw == "--dir_sel"){optind++; ss.str(""); ss << argv[optind]; ss >> m_dir_sel;}
    else if     (sw == "--xsec_map_loc"){optind++; ss.str(""); ss << argv[optind]; ss >> m_xsec_map_loc;}
    else if     (sw == "--ds_map_loc"){optind++; ss.str(""); ss << argv[optind]; ss >> m_ds_map_loc;}
    else if     (sw == "--mc15_TDP"){optind++; ss.str(""); ss << argv[optind]; ss >> m_MC15_TDP;}
    else if     (sw == "--listloc"){optind++; ss.str(""); ss << argv[optind]; ss >> m_listloc;}
    else if     (sw == "--hist_file_prefix"){optind++; ss.str(""); ss << argv[optind]; ss >> m_hist_file_prefix;}
    else if     (sw == "--cutflow_weight"){optind++; ss.str(""); ss << argv[optind]; ss >> m_cutflow_weight;}
    else if     (sw == "--input_pattern"){optind++; ss.str(""); ss << argv[optind]; ss >> m_input_pattern;}
    else if     (sw == "--output_pattern"){optind++; ss.str(""); ss << argv[optind]; ss >> m_output_pattern;}
    //else if(sw == "--dsID"){optind++; ss.str(""); ss << argv[optind]; ss >> dsID;}
    else std::cout<<"Unknown switch "<<argv[optind]<<" sw = "<<sw<<std::endl;
    
    optind++; sw.clear();  ss.str(""); ss.clear();
  }


  m_make_file_list = (i_flist>0);
  m_make_sample_config = (i_sample>0);
  m_make_variable_config = (i_variable>0);

  std::cout<<" m_make_file_list = "<<m_make_file_list
	   <<" m_make_sample_config = "<<m_make_sample_config
	   <<" m_make_variable_config = "<<m_make_variable_config
	   <<std::endl;

  if(m_make_sample_config){ makeSampleConfigurationFile();}
  if(m_make_variable_config){ makeVariableConfigurationFile(); }
  if(m_make_file_list){ makeFileLists_NEW(); }
  
  return 0;
}


void makeFileLists_OLD(){

  double nev_tot = 0.;
  double xsec = 0.;
  //double lumi_eff = 0.;
  double weight = 0.;

  std::ifstream flist(m_listloc.c_str());
  if(!flist){
    std::cout<<"Error : File containing Dataset-SampleID map "<<m_listloc<<" not found. Exiting."<<std::endl;
    return;
  }

  std::string flist_name = m_output_pattern + "_file_list.txt";

  std::ofstream file_list;
  if(AnalysisUtils::FileExists(flist_name)){std::cout<<"Error: File list already exists. Exiting"<<std::endl; return;}
  file_list.open(flist_name.c_str());
  file_list << std::endl << "BEGIN" << std::endl; 
  file_list << "KEY : FILE : SCALE" << std::endl;
  
  std::string sampleID="";
  std::string f_line="";
  std::string dsID="";
  std::string histFileName="";

  const std::string suffix = ".txt";
  const size_t sufsize = suffix.size();
  const size_t idsize = 6;

  while( getline(flist, f_line) ){

   //--------------   
   sampleID="";
   dsID="";
   histFileName="";

   nev_tot = 0.;
   xsec = 0.;
   //lumi_eff = 0.;
   weight = 0.;
   //--------------   
   
   AnalysisUtils::TrimString(f_line);
   if( f_line.empty() ){ continue; }
   size_t lnsize = f_line.size();
   int beg_pos = lnsize - sufsize - idsize;
   int end_pos = lnsize - sufsize - 1;

   if(f_line.substr(end_pos+1) != suffix){std::cout<<"Error: Check given file suffix"<<std::endl; break;}

   dsID = f_line.substr(beg_pos, idsize);
   sampleID = GetSampleIDFromTDP(dsID);
   histFileName = m_hist_file_prefix + "_" + dsID + ".root";

   //std::cout<<"dsID = "<<dsID<<" sampleID = "<<sampleID<<" f_line = "<<f_line<<" histFileName = "<<histFileName<<std::endl;
   
   //Now, once dsID is found, 
   xsec = GetXSec(dsID);
   nev_tot = (m_cutflow_weight > 0) ? GetSumWgtFromHists(f_line) : GetSumWgtFromTrees(f_line);

   //lumi_eff = (nev_tot > 0.) ? nev_tot/xsec : 0.;
   weight = (nev_tot > 0.) ? xsec/nev_tot : 0.;

   file_list << sampleID<<" : "<<histFileName<<" : "<<weight << std::endl;

   sampleID.clear();
   f_line.clear();
   dsID.clear();
   histFileName.clear();
   
   
  }
  file_list << std::endl << "END" << std::endl;
  file_list.close();
  flist_name.clear();

  return;
}


void makeSampleConfigurationFile(){

  std::string samp_name = ""; //will change
  std::string samp_leglabel = ""; //will change
  std::string samp_stylekey = ""; //will change
  std::string samp_drawopt = "e0hist"; //will change for data
  std::string samp_legopt = "lpf"; //will change for data
  std::string samp_drawscale = "NORM"; //default - will change for data
  std::string samp_drawstack = "TRUE"; //will change to default for data
  std::string samp_resopt = "INC"; //will change for data
  std::string samp_blinding = "NONE"; //will DEFINITELY change for signal

  std::vector<std::string> samp_list; samp_list.clear();
  samp_list.push_back("0_Data");
  samp_list.push_back("1_ttbar");
  samp_list.push_back("2_singletop");
  samp_list.push_back("3_Wjets");
  samp_list.push_back("4_Zjets");
  samp_list.push_back("5_Diboson");
  samp_list.push_back("7_ttH");
  samp_list.push_back("SUM");

  std::map<std::string, std::string> samp_label_map; samp_label_map.clear();
  samp_label_map["0_Data"] = "Data";
  samp_label_map["1_ttbar"] = "t#bar{t}";
  samp_label_map["2_singletop"] = "single top";
  samp_label_map["3_Wjets"] = "W+jets";
  samp_label_map["4_Zjets"] = "Z+jets";
  samp_label_map["5_Diboson"] = "Dibosons";
  samp_label_map["7_ttH"] = "t#bar{t}H";
  samp_label_map["SUM"] = "SM Total ";


  std::ofstream sample_config; 
  std::string conf_name = m_output_pattern + "_sample_config.txt";
  if(AnalysisUtils::FileExists(conf_name)){std::cout<<"Error: Sample configuration already exists. Exiting"<<std::endl; return;}
  sample_config.open(conf_name.c_str());
  sample_config << std::endl << "BEGIN" <<std::endl; 

  for(unsigned int s = 0; s < samp_list.size(); s++){
    samp_name = samp_list.at(s);
    samp_leglabel = samp_label_map[samp_name];
    samp_stylekey = samp_name.substr(0,1);
    if(samp_name == "0_Data"){ 
      samp_drawopt = "e0"; 
      samp_legopt = "lpe";
      samp_drawscale = "NONE"; 
      samp_drawstack = "FALSE"; 
      samp_resopt = "REF";
      samp_blinding = "BLIND";
    }
    else if(samp_name == "SUM"){ 
      samp_drawopt = "e2"; 
      samp_legopt = "lef";
      samp_drawscale = "NONE"; 
      samp_drawstack = "FALSE"; 
      samp_resopt = "INC";
      samp_blinding = "NONE";
    }
    else{ 
      samp_drawopt = "e0hist"; 
      samp_legopt = "lpef"; 
      samp_drawscale = "NORM"; 
      samp_drawstack = "TRUE"; 
      samp_resopt = "INC";
      if(samp_name == "6_ttH"){ samp_blinding = "SIGNAL"; }
      else{ samp_blinding = "NONE"; }
    }

    sample_config << "NEW" <<std::endl;
    sample_config << "NAME : "<<samp_name<<std::endl;
    sample_config << "LEGLABEL : "<<samp_leglabel<<std::endl;
    sample_config << "STYLEKEY : "<<samp_stylekey<<std::endl;
    sample_config << "DRAWOPT : "<<samp_drawopt<<std::endl;
    sample_config << "LEGOPT : "<<samp_legopt<<std::endl;
    sample_config << "DRAWSCALE : "<<samp_drawscale<<std::endl;
    sample_config << "DRAWSTACK : "<<samp_drawstack<<std::endl;
    sample_config << "RESOPT : "<<samp_resopt<<std::endl;
    sample_config << "BLINDING : "<<samp_blinding<<std::endl;
    sample_config << std::endl;


  }

  sample_config << std::endl << "END" << std::endl;
  sample_config.close();
  conf_name.clear();

  return;
}

void makeVariableConfigurationFile(){

  std::string var_name = ""; //will change
  std::string var_drawstack = "TRUE"; //won't change 
  std::string var_drawres = "NONE"; //won't change
  std::string var_doscale = "NORM"; //won't change
  std::string var_blinding = "BIN"; //may change
  std::string var_dowidth = "FALSE"; //will change

  std::vector<std::string> var_list; var_list.clear();
  var_list.push_back("njet");
  var_list.push_back("nbjet");
  var_list.push_back("nljet");
  /*
  var_list.push_back("el_pt");
  var_list.push_back("el_eta");

  var_list.push_back("mu_pt");
  var_list.push_back("mu_eta");
  */
  var_list.push_back("jet_1_pt");
  var_list.push_back("jet_all_pt");
  /*
  var_list.push_back("jet_1_eta");
  var_list.push_back("jet_all_eta");

  var_list.push_back("ljet_1_pt");
  //var_list.push_back("ljet_2_pt");
  var_list.push_back("ljet_all_pt");

  var_list.push_back("ljet_1_eta");
  //var_list.push_back("ljet_2_eta");
  var_list.push_back("ljet_all_eta");

  var_list.push_back("ljet_1_mass");
  //var_list.push_back("ljet_2_mass");
  var_list.push_back("ljet_all_mass");

  var_list.push_back("ljet_1_SPLIT12");
  //var_list.push_back("ljet_2_SPLIT12");
  var_list.push_back("ljet_all_SPLIT12");

  var_list.push_back("ljet_1_SPLIT23");
  //var_list.push_back("ljet_2_SPLIT23");
  var_list.push_back("ljet_all_SPLIT23");

  var_list.push_back("ljet_1_Tau21");
  //var_list.push_back("ljet_2_Tau21");
  var_list.push_back("ljet_all_Tau21");

  var_list.push_back("ljet_1_Tau32");
  //var_list.push_back("ljet_2_Tau32");
  var_list.push_back("ljet_all_Tau32");

  var_list.push_back("nTopTag_loose");
  var_list.push_back("nTopTag_tight");
  */
  var_list.push_back("MET");
  var_list.push_back("HT_had");
  var_list.push_back("MTW");

  std::vector<std::string> reg_list; reg_list.clear();
  reg_list.push_back("reg_e_jets_6jin4bin");
  //reg_list.push_back("reg_mu_jets_6jin4bin");

  /*
  reg_list.push_back("reg_emu_jets");
  reg_list.push_back("reg_e_jets_4jex2bex");
  reg_list.push_back("reg_mu_jets_4jex2bex");
  */
  /*
  reg_list.push_back("reg_boosted_emu_jets");
  reg_list.push_back("reg_boosted_tag_emu_jets");
  reg_list.push_back("reg_boosted_emu_jets_2bin");
  reg_list.push_back("reg_boosted_tag_emu_jets_2bin");

  */
  /*
  reg_list.push_back("reg_boosted_emu_jets_2bin");
  reg_list.push_back("reg_boosted_e_jets_2bin");
  reg_list.push_back("reg_boosted_mu_jets_2bin");

  reg_list.push_back("reg_boosted_tag_emu_jets_2bin");
  reg_list.push_back("reg_boosted_tag_e_jets_2bin");
  reg_list.push_back("reg_boosted_tag_mu_jets_2bin");
  */
  std::ofstream var_config;
  std::string conf_name = m_output_pattern + "_variable_config.txt";
  if(AnalysisUtils::FileExists(conf_name)){std::cout<<"Error: Variable configuration already exists. Exiting"<<std::endl; return;}
  var_config.open(conf_name.c_str());
  var_config << std::endl << "BEGIN" <<std::endl; 

  for(unsigned int r = 0; r < reg_list.size(); r++){
    const std::string& region = reg_list.at(r);
    if( (region.find("6jin4bin") != std::string::npos) 
	|| (region.find("6jin3bex") != std::string::npos) 
	|| (region.find("5jin4bin") != std::string::npos) ){var_blinding = "BINYIELD";}
    else{var_blinding = "BIN";}
    for(unsigned int v = 0; v < var_list.size(); v++){

      const std::string& variable = var_list.at(v);
      var_name = region + "_" + variable + "_hist";
      var_config << "NEW" <<std::endl;
      var_config << "NAME : "<<var_name<<std::endl;
      var_config << "DRAWSTACK : "<<var_drawstack<<std::endl;
      var_config << "DRAWRES : "<<var_drawres<<std::endl;
      var_config << "DOSCALE : "<<var_doscale<<std::endl;
      if( (var_name.find("_pt") != std::string::npos) || (var_name == "MET") 
	  || (var_name == "HT_had") || (var_name == "MTW") ){var_config << "DOWIDTH : TRUE"<<std::endl;} 
      //var_config << "BLINDING : "<<var_blinding<<std::endl;
      var_config << std::endl;
    } //var loop

  } // reg loop

  var_config << std::endl << "END" << std::endl;
  var_config.close();
  conf_name.clear();

  return; 
}

void makeFileLists_NEW(){

  std::string::size_type dsID_start = m_input_pattern.find("%DSID%");
  if(dsID_start == std::string::npos){std::cout<<" ERROR: Cannot find %DSID% in given file name name pattern."<<std::endl;}
  std::string prefix = m_input_pattern.substr(0, dsID_start);

  std::ofstream file_list;
  std::string flist_name = m_output_pattern + "_file_list.txt";
  if(AnalysisUtils::FileExists(flist_name)){std::cout<<"Error: File list already exists. Exiting"<<std::endl; return;}
  file_list.open(flist_name.c_str());
  file_list << std::endl << "BEGIN" << std::endl; 
  file_list << "KEY : FILE : SCALE" << std::endl;

  SampleInfo* sampleReader = new SampleInfo();
  std::string::size_type dsID_end = 0;
  std::string fname = "", dsID = "";
  std::ifstream dslist(m_listloc.c_str());
  while(getline(dslist, fname)){
    //find the dataset ID and pass into SampleInfo
    dsID_start = fname.find(prefix);
    if(dsID_start == std::string::npos){
      std::cout<<" ERROR : File path "<<fname<<" does not match given pattern "<<m_input_pattern<<std::endl;
      continue;
    }
    dsID_start += prefix.size();
    dsID = fname.substr( dsID_start );
    dsID_end = fname.find(".");
    if(dsID_end == std::string::npos){std::cout<<" ERROR : Dataset ID in the file name must end in '.'. Ask Loic why this must be. "<<std::endl;}
    dsID = dsID.substr(0, dsID.find("."));
    sampleReader->ReadSample(dsID, m_sample_info);
    if(!sampleReader->Ready()){std::cout<<" WARNING : DSID "<<dsID<<" could not be found in "<<m_sample_info<<". Ignoring"<<std::endl; continue;}
    file_list << sampleReader->SampleName() << " : " << fname << " : " << sampleReader->NormFactor() << std::endl;
  }
  file_list << std::endl << "END" << std::endl;
  file_list.close();
  delete sampleReader;
  flist_name.clear();

  return;
}

void makeFileLists(){

  std::ofstream file_list;
  std::string flist_name = m_output_pattern + "_file_list.txt";
  if(AnalysisUtils::FileExists(flist_name)){std::cout<<"Error: File list already exists. Exiting"<<std::endl; return;}
  file_list.open(flist_name.c_str());
  file_list << std::endl << "BEGIN" << std::endl; 
  file_list << "KEY : FILE : SCALE" << std::endl;
  
  //opposite direction, it has to be. loop over samples_info.dat and build your file names.
  //m_input_pattern; //--> How to make a list out of this in C++? DO we even need to?
  //Also, how do we list systematics and read them in? 


  std::ifstream dslist(m_sample_info.c_str());
  if(!dslist){
    std::cout<<"Error : File containing sample information "<<m_sample_info<<" not found. Exiting."<<std::endl;
    return;
  }

  std::string f_line = "";

  std::string sampleID = "";
  std::string dsID = "";
  std::string histFileName="";

  double dswgt = 0.;
  double xsec = 0.;
  double sumwgt = 0.;

  std::string paramString="";
  std::string param="";
  int nparam = 0;
  std::vector<std::string> vparam; vparam.clear();
  std::string::size_type pos = 0;

  while( getline(dslist, f_line) ){

    sampleID.clear();
    dsID.clear();
    histFileName.clear();

    dswgt = 0.;
    xsec = 0.;
    sumwgt = 0.;
 
    paramString.clear();
    param.clear();
    nparam = 0;
    pos = 0;
    vparam.clear();

    AnalysisUtils::TrimString(f_line);

    if( f_line.empty() ){ continue; }
   //--------------   

    paramString = f_line;
    do{
      pos = AnalysisUtils::ParseString(paramString, param, " ");
      AnalysisUtils::TrimString(param);
      if(!param.empty()){
	vparam.push_back(param);
	nparam++;
      }
    }while( (!paramString.empty()) && (pos != std::string::npos) );//read all given parameters

    if(nparam != 4 ){
      std::cout<<"Given parameter line :" << f_line << " : does not conform to expectation of four parts"<<std::endl;
      std::cout<<" Number of parts = "<<nparam<<std::endl; 
      for(unsigned int t = 0; t < vparam.size(); t++){
	std::cout<<" vparam.at("<<t<<") = "<<vparam.at(t)<<"-E"<<std::endl; 
      }
    }
    else{
      //void ParseSampleDat(const std::string& dsID, std::string& sample_name, std::string& sumwgt, std::string& xsec);
      dsID = vparam.at(0);
      sumwgt = atof(vparam.at(1).c_str());
      xsec = atof(vparam.at(2).c_str());
      sampleID = vparam.at(3);
    }

    histFileName = AnalysisUtils::ReplaceString(m_input_pattern, "%DSID%", dsID);
    histFileName = AnalysisUtils::ReplaceString(m_input_pattern, "%SYS%", "nominal");

    dswgt = (sumwgt > 0.) ? xsec / sumwgt : 0.;

    file_list << sampleID <<" : "<<histFileName<<" : "<<dswgt << std::endl;

  }//loop over lines in sample.dat file

  file_list << std::endl << "END" << std::endl;
  file_list.close();
  flist_name.clear();

  return;
}


void ParseSampleDat(const std::string& dsID, std::string& sample_name, std::string& sumwgt, std::string& xsec){
  
  std::string conf_line="";
  std::string paramString="";
  std::string param="";
  int nparam = 0;
  std::vector<std::string> vparam; vparam.clear();
  std::string::size_type pos = 0;

  std::ifstream dslist(m_sample_info.c_str());
  if(!dslist){
    std::cout<<"Error : File containing sample information "<<m_sample_info<<" not found. Exiting."<<std::endl;
    return;
  }

  while( getline(dslist, conf_line) ){

    paramString.clear();
    param.clear();
    nparam = 0;
    pos = 0;
    vparam.clear();

    AnalysisUtils::TrimString(conf_line);

    if( conf_line.empty() || (conf_line.find(dsID) == std::string::npos) ){ continue; }
    if(conf_line.find(dsID) != std::string::npos){
      //1st part - dsID
      //2nd part - sumwgt
      //3rd part - xsec
      //4th part - sample name
      paramString = conf_line;
      do{
	pos = AnalysisUtils::ParseString(paramString, param, " ");
	AnalysisUtils::TrimString(param);
	if(!param.empty()){
	  vparam.push_back(param);
	  nparam++;
	}

      }while( (!paramString.empty()) && (pos != std::string::npos) );//read all given parameters

      if(nparam != 4 || (vparam.at(0) != dsID) ){
	std::cout<<"Given parameter line :" << conf_line << " : does not conform to expectation of four parts"<<std::endl;
	std::cout<<" Number of parts = "<<nparam<<std::endl; 
	for(unsigned int t = 0; t < vparam.size(); t++){
	  std::cout<<" vparam.at("<<t<<") = "<<vparam.at(t)<<"-E"<<std::endl; 
	}
      }
      else{
	//void ParseSampleDat(const std::string& dsID, std::string& sample_name, std::string& sumwgt, std::string& xsec);
	//dsID = vparam.at(0);
	sample_name = vparam.at(3);
	sumwgt = vparam.at(1);
	xsec = vparam.at(2);
      }
      break;
    }//if found dsID on line

  }//loop over lines

  dslist.close();
  return;

}

double GetXSec(const std::string& dsID){

  double xsec = 0.;

  std::ifstream xsec_file(m_xsec_map_loc.c_str());
  if(!xsec_file){
    std::cout<<"Error : cross-section file "<< m_xsec_map_loc.c_str() <<" not found. Exiting."<<std::endl;
    return -1;
  }

  std::string conf_line="";
  std::string paramString="";
  std::string param="";
  int nparam = 0;
  std::vector<std::string> vparam; vparam.clear();
  std::string::size_type pos = 0;

  while( getline(xsec_file, conf_line) ){

    paramString.clear();
    param.clear();
    nparam = 0;
    pos = 0;
    vparam.clear();

    AnalysisUtils::TrimString(conf_line);

    if( conf_line.empty() || (conf_line.find(dsID) == std::string::npos) ){ continue; }
    if(conf_line.find(dsID) != std::string::npos){
      //1st part - dsID
      //2nd part - xsec
      //3rd part - kfactor
      paramString = conf_line;
      do{
	pos = AnalysisUtils::ParseString(paramString, param, " ");
	AnalysisUtils::TrimString(param);
	if(!param.empty()){
	  vparam.push_back(param);
	  nparam++;
	}

      }while( (!paramString.empty()) && (pos != std::string::npos) );//read all given parameters

      if(nparam != 3 || (vparam.at(0) != dsID) ){
	std::cout<<"Given parameter line :" << conf_line << " : does not conform to expectation of three parts"<<std::endl;
	std::cout<<" Number of parts = "<<nparam<<std::endl; 
	for(unsigned int t = 0; t < vparam.size(); t++){
	  std::cout<<" vparam.at("<<t<<") = "<<vparam.at(t)<<"-E"<<std::endl; 
	}
      }
      else{
	xsec = atof(vparam.at(1).c_str())*atof(vparam.at(2).c_str());
      }
      break;
    }//if found dsID on line

  }//loop over lines

  return xsec;
}


std::string GetSampleIDFromTDP(const std::string& dsID){

  std::string sampleID = "";
  std::string descr_line = "";
  bool foundDSID = false; 
  std::ifstream sampleMap(m_MC15_TDP.c_str());
  if(!sampleMap){
    std::cout<<"Error : TDP MC15.py file : "<< m_MC15_TDP.c_str() <<" not found. Exiting."<<std::endl;
    return "";
  }

  std::string conf_line="";
  while( getline(sampleMap, conf_line) ){
    if(conf_line.find("TopExamples.grid.Add(") != std::string::npos){ descr_line = conf_line; continue;}
    else if(conf_line.find(dsID) != std::string::npos ){ foundDSID = true; break; }
    else{continue;}
  }

  if(!foundDSID){std::cout<<"Could not find DSID "<<dsID<<" in MC15.py"<<std::endl;}
  else{
    if(descr_line.find("ttbar") != std::string::npos){ sampleID = "1_ttbar"; }
    else if(descr_line.find("singletop") != std::string::npos){ sampleID = "2_singletop"; }
    else if(descr_line.find("W_Sherpa") != std::string::npos){ sampleID = "3_Wjets"; }
    else if(descr_line.find("Z_Sherpa") != std::string::npos){ sampleID = "4_Zjets"; }
    else if(descr_line.find("Dibosons") != std::string::npos){ sampleID = "5_Dibosons"; }
    else if(descr_line.find("ttX") != std::string::npos){ sampleID = "6_ttEW"; }
    else if(descr_line.find("ttH") != std::string::npos){ sampleID = "7_ttH"; }

  }

  return sampleID;
}

std::string GetSampleID(const std::string& dsID){

  std::string sampleID = "";

  std::ifstream sampleMap(m_ds_map_loc.c_str());
  if(!sampleMap){
    std::cout<<"Error : sample mapping file "<< m_ds_map_loc.c_str() <<" not found. Exiting."<<std::endl;
    return "";
  }

  std::string conf_line="";
  std::string paramString="";
  std::string param="";
  int nparam = 0;
  std::vector<std::string> vparam; vparam.clear();
  std::string::size_type pos = 0;
  while( getline(sampleMap, conf_line) ){
    paramString.clear();
    param.clear();
    nparam = 0;
    pos = 0;
    vparam.clear();

    AnalysisUtils::TrimString(conf_line);
    if( conf_line.empty() || (conf_line.find(dsID) == std::string::npos) ){ continue; }
    if(conf_line.find(dsID) != std::string::npos){
      //1st part - sampleID
      //2nd part - dsID
      paramString = conf_line;
      do{
	pos = AnalysisUtils::ParseString(paramString, param, " ");
	AnalysisUtils::TrimString(param);
	vparam.push_back(param);
	nparam++;
      }while(pos != std::string::npos);//read all given parameters
      if(nparam != 2 || (vparam.at(1) != dsID) ){std::cout<<"Given parameter line does not conform to expectation of two parts"<<std::endl;}
      else{
	sampleID = vparam.at(0);
      }
      break;
    }

  }
  
  return sampleID;
}

double GetSumWgtFromHists(const std::string& fileloc){
  
  double sumwgt = 0.;
  
  //Sum up the total number of events
  std::ifstream flist(fileloc.c_str()); 
  std::string fname="";
  if(flist.is_open()){
    while( getline(flist, fname) ){
      TFile* fcur = TFile::Open(fname.c_str(), "READ");
      sumwgt += ( (TH1D*)( fcur->Get(Form("%s/cutflow_mc_pu_zvtx", m_dir_sel.c_str())) ) )->GetBinContent(1);
      fcur->Close();
      delete fcur;
    }
  }
  else{std::cout<<"Cannot open file"<<std::endl;}

  return sumwgt;
}
 

double GetSumWgtFromTrees(const std::string& fileloc){

  double nev_tot = 0.;

  TChain* fChain = new TChain("sumWeights");
  float totalEventsWeighted;
  TBranch* b_totalEventsWeighted = 0;
  fChain->SetBranchAddress("totalEventsWeighted", &totalEventsWeighted, &b_totalEventsWeighted);

  //Sum up the total number of events
  std::ifstream flist(fileloc.c_str()); 
  std::string fname="";
  if(flist.is_open()){
    while( getline(flist, fname) ){
      fChain->Add(fname.c_str());
    }
  }
  else{std::cout<<"Cannot open file"<<std::endl;}

  int nenetries = (int)fChain->GetEntries();

  for(int n = 0; n < nenetries; n++){
    fChain->GetEntry(n);
    nev_tot += (double)totalEventsWeighted;
  }

  delete fChain;

  return nev_tot;
}
