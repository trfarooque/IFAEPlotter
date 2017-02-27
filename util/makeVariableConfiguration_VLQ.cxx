#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <TFile.h>
#include <TH1.h>
#include <TChain.h>

//std::vector<std::string> m_variable_list{ "meff" };
std::vector<std::string> m_variable_list{ "meff", "hthad", "mtbmin", "mbb_mindR", "met", "met_phi", "met_sig",
    "jets_n", "bjets_n", "el_n", "mu_n", "lep_n", "RCTTMass_jets_n", "RCMTop_jets_n", "RCMHiggs_jets_n", "RCjets_n", 
    "jets_pt", "jets_eta", "jets_m", "jets_btagw", "jet0_pt", "jet0_eta", "jet0_m", "jet0_btagw",
    "bjets_pt", "bjets_eta", "bjets_btagw", "bjet0_pt", "bjet0_eta", "bjet0_btagw",
    "RCjets_pt", "RCjets_eta", "RCjets_m", "RCjets_nconsts", 
    "RCjet0_pt", "RCjet0_eta", "RCjet0_m", "RCjet0_nconsts", 
    "dR_jetjet", "dR_bjetbjet", 
    "dPhi_jetmet", "dPhi_lepmet"
    };

std::vector<std::string> m_variable_list_1L{"ptw", "mtw", "dPhi_lepmet", "lep0_pt_zoom", "lep0_eta", "lep0_phi" };
std::vector<std::string> m_variable_list_1L_e{"dR_ejet", "dR_ebjet" };
std::vector<std::string> m_variable_list_1L_mu{"dR_mujet", "dR_mubjet" };
std::vector<std::string> m_variable_list_RC{
  "RCTTMass_jets_pt", "RCTTMass_jets_eta", "RCTTMass_jets_m", "RCTTMass_jets_consts_n",
    "RCMTop_jets_pt", "RCMTop_jets_eta", "RCMTop_jets_m", 
    "RCMHiggs_jets_pt", "RCMHiggs_jets_eta", "RCMHiggs_jets_m", 
    };
std::vector<std::string> m_channels{"c0lep", "c1lep"};// "c0l"};//c1l
std::vector<std::string> m_njnb_1L{"5jin2bin"}; //, "5j3b", "5j4b", "6j2b", "6j3b", "6j3bLowMbb", "6j3bHighMbb", "6j4b", "6j4bLowMbb", "6j4bHighMbb"};
std::vector<std::string> m_njnb_0L{"6jin2bin"}; //, "6j3b", "6j4b", "7j2b", "7j3b", "7j3bLowMtbmin", "7j3bHighMtbmin", "7j4b", "7j4bLowMtbmin", "7j4bHighMtbmin"};

std::vector<std::string> m_nRC{ }; 
/*
  "0RCTTMass", "1RCTTMass", "2RCTTMass", 
    "0Tex0Hex", "0Tex1Hex", "0Tex2Hin", 
    "1Tex0Hex", "1Tex1Hex", "1Tex2Hin", 
    "2Tin0Hex", "2Tin1Hex", "2Tin2Hin"
    };
*/
std::ofstream m_var_config; 

void AddVariablesByChannel(const std::string& channel);

//int main(int argc, char** argv){
int main(){

  m_var_config.open("VLQ_variable_config.txt");
  m_var_config << std::endl << "BEGIN" <<std::endl; 

  for(std::string channel : m_channels){

    AddVariablesByChannel(channel);
  }

  m_var_config << std::endl << "END" << std::endl;
  m_var_config.close();

  return 0;
}

void AddVariablesByChannel(const std::string& channel){

  std::string var_name = ""; //will change
  std::string var_drawstack = "TRUE"; //won't change 
  std::string var_drawres = "RATIO"; //won't change
  std::string var_doscale = "NORM"; //won't change
  std::string var_label = ""; //will change
  std::string var_title = ""; //will change for each region
  //std::string var_rebin = "0.,700.,800.,900.,1000.,1200.,1400.,1600.,2000.,2500.,3000."; //will change

  std::vector<std::string> m_prefix_list; m_prefix_list.clear();

  if(channel == "c0lep"){
    m_prefix_list.push_back(channel+"6jin2bin");
  }
  if(channel == "c1lep"){
    m_prefix_list.push_back(channel+"5jin2bin");
    //m_prefix_list.push_back(channel+"5jin2bin_e");
    //m_prefix_list.push_back(channel+"5jin2bin_mu");
  }

  const std::vector<std::string>& v_njnb = (channel == "c1lep") ? m_njnb_1L : m_njnb_0L;
  for(std::string njnb : v_njnb){
    for(const std::string nRC : m_nRC){
      if( (nRC == "2RCTTMass") && (njnb.find("High")!=std::string::npos || njnb.find("Low")!=std::string::npos) ) continue;
      m_prefix_list.push_back(channel+nRC+njnb);
      if(channel == "c1lep"){
	//m_prefix_list.push_back(channel+nRC+njnb+"_e");
	//m_prefix_list.push_back(channel+nRC+njnb+"_mu");
      }
    }//nRC
  }//njnb


  for(unsigned int r = 0; r < m_prefix_list.size(); r++){
    const std::string& region = m_prefix_list.at(r);
    std::cout << " region = " << region << std::endl;
    var_title = "#splitline{#splitline{#scale[1.2]{#bf{#it{ATLAS}} Internal}}{#sqrt{s}=13 TeV, 36.5 fb^{-1}}}{"; //+ region + "}"; 
    if(region == "c1lep5jin2bin"){ var_title += "#geq5j,#geq2b,1l}";}
    else if(region == "c1lep5jin2bin_e"){ var_title += "#geq5j,#geq2b,1e}";}
    else if(region == "c1lep5jin2bin_mu"){ var_title += "#geq5j,#geq2b,1#mu}";}
    else if(region == "c0lep6jin2bin"){ var_title += "#geq6j,#geq2b,0l}";}

    std::vector<std::string> v_variable = m_variable_list;

    if(channel == "c1lep"){
      v_variable.insert(v_variable.end(), m_variable_list_1L.begin(), m_variable_list_1L.end());
      if(region.find("_e") != std::string::npos){
	v_variable.insert(v_variable.end(), m_variable_list_1L_e.begin(), m_variable_list_1L_e.end());
      }
      else if(region.find("_mu") != std::string::npos){
	v_variable.insert(v_variable.end(), m_variable_list_1L_mu.begin(), m_variable_list_1L_mu.end());
      }
    }
    v_variable.insert(v_variable.end(), m_variable_list_RC.begin(), m_variable_list_RC.end());


    for(unsigned int v = 0; v < v_variable.size(); v++){
      const std::string& variable = v_variable.at(v);
      std::cout<< variable << ", ";
    }

    for(unsigned int v = 0; v < v_variable.size(); v++){

      const std::string& variable = v_variable.at(v);
      std::cout<<" variable = " << variable << std::endl;

      var_name = region + "_" + variable;
      std::cout<< " var_name = "<<var_name<<std::endl;
      m_var_config << "NEW" <<std::endl;
      m_var_config << "NAME : "<<var_name<<std::endl;
      m_var_config << "DRAWSTACK : "<<var_drawstack<<std::endl;
      m_var_config << "DRAWRES : "<<var_drawres<<std::endl;
      m_var_config << "DOSCALE : "<<var_doscale<<std::endl;
      m_var_config << "RESMIN : 0.2"<<std::endl;
      m_var_config << "RESMAX : 1.8"<<std::endl;
      //m_var_config << "REBIN : 2"<<std::endl; //ALERTRISHA
      //m_var_config << "REBINVAR : "<<var_rebin<<std::endl; //ALERTRISHA
      m_var_config << "EXTRALABEL : "<<var_title<<std::endl; 
      m_var_config << "OUTPUTFOLDER : "<<region<<std::endl;
      if(variable=="bjets_n"){ m_var_config << "LABEL : Number of b-jets (77% WP)" << std::endl;}
      /*
      if( (variable=="jets_n") || (variable=="bjets_n") || (variable=="el_n") || (variable=="mu_n")  
	  || (variable=="baseline_el_n") || (variable=="baseline_mu_n") 
	  || (variable=="lep_n") || (variable=="baseline_lep_n") 
	  || (variable=="rcjets_n") || (variable=="RCTTMass_jets_n") ){
	m_var_config << "ISCOUNT : TRUE" << std::endl; 
	//m_var_config << "ISLOGY : TRUE" << std::endl;
      }
      */
      if( (variable=="mJsum") || (variable=="mJsum_4j") ){ m_var_config << "XMAX : 800." << std::endl; }
      if(variable=="met"){ m_var_config << "XMAX : 700." << std::endl; }
      if( (variable=="mtbmin") || (variable=="mtw") ){ m_var_config << "XMAX : 500." << std::endl; }
      /*
      if( (variable=="meff") || (variable=="jets_pt") || (variable=="bjets_pt") 
	  || (variable=="rcjets_pt") || (variable=="RCTTMass_jets_pt") 
	  || (variable=="rcjets_m") || (variable=="RCTTMass_jets_m") ){ m_var_config << "REBIN : 2" << std::endl; }
      */
      m_var_config << std::endl;
    } //var loop

    v_variable.clear();

  } // reg loop


  m_prefix_list.clear();
 
  return; 
}
