#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "TFile.h"
#include "TH1.h"
#include "TChain.h"





std::string m_hist_file_prefix = "/afs/cern.ch/work/f/farooque/RunIIArea/DEVAREA/SUSYOUTPUTS/test_bkgdstudy";
std::string m_hist_file_suffix = "full.root";
bool m_make_file_list = false;
bool m_make_sample_config = false;
bool m_make_variable_config = false;

std::vector<std::string> m_sample_list{ "ttbar", "singletop", "Wjets", "Zjets", "diboson", "topEW" };

std::vector<std::string> m_variable_list{ "RCTTMass_jets_n", "RCTTMass_jet0_nbjets", "RCTTMass_jet0_nconsts", "RCTTMass_jet0_pt"
    , "RCTTMass_jets_nbjets", "RCTTMass_jets_nconsts", "RCTTMass_jets_pt"
    , "bjets_insidetop_RCTTMass_truthflavour", "bjets_outsidetop_RCTTMass_truthflavour", "bjets_n_outsidetop_RCTTMass"
    , "bjets_min_dr", "jets_n", "bjets85_n", "bjets70_n" };


std::vector<std::string> m_prefix_list{ "c0l", "Gtt0L_SR1", "Gtt0L_SR2", "Gtt0L_SR3", "Gtt0L_SR4", "Gtt0L_CR1", "Gtt0L_CR2", "Gtt0L_CR3", "Gtt0L_CR4"
    , "Gtt0L_VR1_0L", "Gtt0L_VR2_0L","Gtt0L_VR3_0L","Gtt0L_VR4_0L", "Gtt0L_VR1_1L", "Gtt0L_VR2_1L","Gtt0L_VR3_1L","Gtt0L_VR4_1L" };

void makeSampleConfigurationFile();
void makeVariableConfigurationFile();

int main(int argc, char** argv){

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

    else if     (sw == "--hist_file_prefix"){optind++; ss.str(""); ss << argv[optind]; ss >> m_hist_file_prefix;}
    else if     (sw == "--hist_file_suffix"){optind++; ss.str(""); ss << argv[optind]; ss >> m_hist_file_suffix;}
    else std::cout<<"Unknown switch "<<argv[optind]<<" sw = "<<sw<<std::endl;
    
    optind++; sw.clear();  ss.str(""); ss.clear();
  }

  m_make_file_list = (i_flist>0);
  m_make_sample_config = (i_sample>0);
  m_make_variable_config = (i_variable>0);

  std::cout<<" m_make_file_list = "<<m_make_file_list<<" m_make_sample_config = "<<m_make_sample_config<<" m_make_variable_config = "<<m_make_variable_config<<std::endl;

  if(m_make_sample_config){ makeSampleConfigurationFile();}
  if(m_make_variable_config){ makeVariableConfigurationFile(); }
  
  return 0;
}


void makeSampleConfigurationFile(){

  std::string samp_name = ""; //will change
  std::string samp_key = ""; //will change
  std::string samp_fkey = ""; //will change
  std::string samp_leglabel = ""; //will change
  std::string samp_stylekey = ""; //will change
  std::string samp_drawopt = "e0hist"; //will change for data
  std::string samp_legopt = "lpf"; //will change for data
  std::string samp_drawscale = "NORM"; //default - will change for data
  std::string samp_drawstack = "TRUE"; //will change to default for data
  std::string samp_resopt = "INC"; //will change for data
  std::string samp_blinding = "NONE"; //will DEFINITELY change for data

  std::string samp_filename = "";
  //std::vector<std::string> m_sample_list{ "ttbar", "singletop", "Wjets", "Zjets", "diboson", "topEW" };

  std::map<std::string, std::string> samp_fkey_map; samp_fkey_map.clear();
  samp_fkey_map["Data"] = "Data";
  samp_fkey_map["ttbar"] = "ttbar";
  samp_fkey_map["singletop"] = "singletop";
  samp_fkey_map["Wjets"] = "W_sherpa";
  samp_fkey_map["Zjets"] = "Z_sherpa";
  samp_fkey_map["diboson"] = "diboson";
  samp_fkey_map["topEW"] = "topEW";


  std::map<std::string, std::string> samp_key_map; samp_key_map.clear();
  samp_key_map["Data"] = "0_Data";
  samp_key_map["ttbar"] = "1_ttbar";
  samp_key_map["singletop"] = "2_singletop";
  samp_key_map["Wjets"] = "3_Wjets";
  samp_key_map["Zjets"] = "4_Zjets";
  samp_key_map["diboson"] = "5_dibosons";
  samp_key_map["topEW"] = "6_topEW";

  std::map<std::string, std::string> samp_label_map; samp_label_map.clear();
  samp_label_map["Data"] = "Data";
  samp_label_map["ttbar"] = "t#bar{t}";
  samp_label_map["singletop"] = "single top";
  samp_label_map["Wjets"] = "W+jets";
  samp_label_map["Zjets"] = "Z+jets";
  samp_label_map["diboson"] = "Dibosons";
  samp_label_map["topEW"] = "TopEW";

  std::ofstream sample_config; sample_config.open("SUSY_sample_config.txt");
  sample_config << std::endl << "BEGIN" <<std::endl; 

  std::ofstream file_list; 
  if(m_make_file_list){ 
    file_list.open("SUSY_file_list.txt"); 
    file_list << std::endl << "BEGIN" << std::endl;
    file_list << "KEY : FILE : SCALE" << std::endl;
  }
  for(unsigned int s = 0; s < m_sample_list.size(); s++){
    samp_name = m_sample_list.at(s);
    samp_key = samp_key_map[samp_name];
    samp_fkey = samp_fkey_map[samp_name];
    samp_leglabel = samp_label_map[samp_name];
    samp_stylekey = samp_key.substr(0,1);
    if(samp_name == "Data"){ 
      samp_drawopt = "e0"; 
      samp_legopt = "lpe";
      samp_drawscale = "NONE"; 
      samp_drawstack = "FALSE"; 
      samp_resopt = "REF";
      samp_blinding = "BLIND";
    }
    else{ 
      samp_drawopt = "e0hist"; 
      samp_legopt = "lpe"; 
      samp_drawscale = "NORM"; 
      samp_drawstack = "TRUE"; 
      samp_resopt = "INC";
      if(samp_name == "ttH"){ samp_blinding = "SIGNAL"; }
      else{ samp_blinding = "NONE"; }
    }

    sample_config << "NEW" <<std::endl;
    sample_config << "NAME : "<<samp_key<<std::endl;
    sample_config << "LEGLABEL : "<<samp_leglabel<<std::endl;
    sample_config << "STYLEKEY : "<<samp_stylekey<<std::endl;
    sample_config << "DRAWOPT : "<<samp_drawopt<<std::endl;
    sample_config << "LEGOPT : "<<samp_legopt<<std::endl;
    sample_config << "DRAWSCALE : "<<samp_drawscale<<std::endl;
    sample_config << "DRAWSTACK : "<<samp_drawstack<<std::endl;
    sample_config << "RESOPT : "<<samp_resopt<<std::endl;
    //sample_config << "BLINDING : "<<samp_blinding<<std::endl;
    sample_config << std::endl;

    if(m_make_file_list){ 
      samp_filename = m_hist_file_prefix + "_" + samp_fkey + "_" + m_hist_file_suffix;
      file_list << samp_key << " : " << samp_filename << " : " << "1." <<std::endl; 
    }

  }

  sample_config << std::endl << "END" << std::endl;
  sample_config.close();

  if(m_make_file_list){ 
  file_list << std::endl << "END" << std::endl;
  file_list.close();

  }

  return;
}

void makeVariableConfigurationFile(){
  /*
  std::string var_name = ""; //will change
  std::string var_drawstack = "TRUE"; //won't change 
  std::string var_drawres = "RATIO"; //won't change
  std::string var_doscale = "NORM"; //won't change
  std::string var_blinding = "BIN"; //may change
  */
  std::string var_name = ""; //will change
  std::string var_drawstack = "FALSE"; //won't change 
  std::string var_drawres = "NONE"; //won't change
  std::string var_doscale = "SHAPE"; //won't change
  std::string var_blinding = "BIN"; //may change
  std::string var_label = ""; //will change
  std::string var_title = ""; //will change for each region


  std::ofstream var_config; var_config.open("SUSY_variable_config.txt");
  var_config << std::endl << "BEGIN" <<std::endl; 

  for(unsigned int r = 0; r < m_prefix_list.size(); r++){
    const std::string& region = m_prefix_list.at(r);

    if(region == "c0l"){ var_title = "#splitline{#splitline{#scale[1.2]{#bf{#it{ATLAS}} Internal}}{0 lepton, 4 jets, 2-bjets}}{#splitline{MET > 200 GeV}{#Delta#phi_{min}(jet, MET) > 0.4}}";}
    else if(region == "Gtt0L_SR1"){var_title = "#splitline{#scale[1.2]{#bf{#it{ATLAS}} Internal}}{Gtt_0L_SR-A}";}
    else if(region == "Gtt0L_SR2"){var_title = "#splitline{#scale[1.2]{#bf{#it{ATLAS}} Internal}}{Gtt_0L_SR-B}";}
    else if(region == "Gtt0L_SR3"){var_title = "#splitline{#scale[1.2]{#bf{#it{ATLAS}} Internal}}{Gtt_0L_SR-C}";}
    else if(region == "Gtt0L_SR4"){var_title = "#splitline{#scale[1.2]{#bf{#it{ATLAS}} Internal}}{Gtt_0L_SR-D}";}

    else if(region == "Gtt0L_CR1"){var_title = "#splitline{#scale[1.2]{#bf{#it{ATLAS}} Internal}}{Gtt_0L_CR-A}";}
    else if(region == "Gtt0L_CR2"){var_title = "#splitline{#scale[1.2]{#bf{#it{ATLAS}} Internal}}{Gtt_0L_CR-B}";}
    else if(region == "Gtt0L_CR3"){var_title = "#splitline{#scale[1.2]{#bf{#it{ATLAS}} Internal}}{Gtt_0L_CR-C}";}
    else if(region == "Gtt0L_CR4"){var_title = "#splitline{#scale[1.2]{#bf{#it{ATLAS}} Internal}}{Gtt_0L_CR-D}";}

    else if(region == "Gtt0L_VR1_0L"){var_title = "#splitline{#scale[1.2]{#bf{#it{ATLAS}} Internal}}{Gtt_0L_VR0L-A}";}
    else if(region == "Gtt0L_VR2_0L"){var_title = "#splitline{#scale[1.2]{#bf{#it{ATLAS}} Internal}}{Gtt_0L_VR0L-B}";}
    else if(region == "Gtt0L_VR3_0L"){var_title = "#splitline{#scale[1.2]{#bf{#it{ATLAS}} Internal}}{Gtt_0L_VR0L-C}";}
    else if(region == "Gtt0L_VR4_0L"){var_title = "#splitline{#scale[1.2]{#bf{#it{ATLAS}} Internal}}{Gtt_0L_VR0L-D}";}

    else if(region == "Gtt0L_VR1_1L"){var_title = "#splitline{#scale[1.2]{#bf{#it{ATLAS}} Internal}}{Gtt_0L_VR1L-A}";}
    else if(region == "Gtt0L_VR2_1L"){var_title = "#splitline{#scale[1.2]{#bf{#it{ATLAS}} Internal}}{Gtt_0L_VR1L-B}";}
    else if(region == "Gtt0L_VR3_1L"){var_title = "#splitline{#scale[1.2]{#bf{#it{ATLAS}} Internal}}{Gtt_0L_VR1L-C}";}
    else if(region == "Gtt0L_VR4_1L"){var_title = "#splitline{#scale[1.2]{#bf{#it{ATLAS}} Internal}}{Gtt_0L_VR1L-D}";}

    for(unsigned int v = 0; v < m_variable_list.size(); v++){
      /*
std::vector<std::string> m_variable_list{ "RCTTMass_jets_n", "RCTTMass_jet0_nbjets", "RCTTMass_jet0_nconsts", "RCTTMass_jet0_pt"
    , "RCTTMass_jets_nbjets", "RCTTMass_jets_nconsts", "RCTTMass_jets_pt"
    , "bjets_insidetop_RCTTMass_truthflavour", "bjets_outsidetop_RCTTMass_truthflavour", "bjets_n_outsidetop_RCTTMass"
    , "bjets_min_dr", "jets_n", "bjets85_n", "bjets70_n" };
      */
      const std::string& variable = m_variable_list.at(v);
      if(variable == "RCTTMass_jets_n"){ var_label = "Number of top-tagged jets";}
      if(variable == "RCTTMass_jet0_nbjets"){ var_label = "Number of b-jets matched to leading top-tagged jet";}
      if(variable == "RCTTMass_jet0_nconsts"){ var_label = "Number of subjets in leading top-tagged jet";}
      if(variable == "RCTTMass_jet0_pt"){ var_label = "Leading top-tagged jet p_{T}";}

      if(variable == "RCTTMass_jets_nbjets"){ var_label = "Number of b-jets matched top-tagged jet";}
      if(variable == "RCTTMass_jets_nconsts"){ var_label = "Number of subjets in top-tagged jets";}
      if(variable == "RCTTMass_jets_pt"){ var_label = "Leading top-tagged jet p_{T}";}

      if(variable == "bjets_insidetop_RCTTMass_truthflavour"){ var_label = "Truth-flavour of b-jets inside top-tagged jets";}
      if(variable == "bjets_outsidetop_RCTTMass_truthflavour"){ var_label = "Truth-flavour of b-jets outside top-tagged jets";}
      if(variable == "bjets_n_outsidetop_RCTTMass"){ var_label = "Number of b-jets outside top-tagged jets";}

      if(variable == "bjets_min_dr"){ var_label = "Minimum #DeltaR between b-jets";}
      if(variable == "jets_n"){ var_label = "Number of jets";}
      if(variable == "bjets85_n"){ var_label = "Number of b-jets (85% w.p.)";}
      if(variable == "bjets70_n"){ var_label = "Number of b-jets (70% w.p.)";}

      var_name = region + "_" + variable;
      var_config << "NEW" <<std::endl;
      var_config << "NAME : "<<var_name<<std::endl;
      var_config << "DRAWSTACK : "<<var_drawstack<<std::endl;
      var_config << "DRAWRES : "<<var_drawres<<std::endl;
      var_config << "DOSCALE : "<<var_doscale<<std::endl;
      var_config << "LABEL : "<<var_label<<std::endl;
      var_config << "EXTRALABEL : "<<var_title<<std::endl; 
      //var_config << "BLINDING : "<<var_blinding<<std::endl;
      var_config << std::endl;
    } //var loop

  } // reg loop

  var_config << std::endl << "END" << std::endl;
  var_config.close();
  return; 
}

