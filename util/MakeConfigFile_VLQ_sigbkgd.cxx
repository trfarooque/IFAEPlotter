#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

#include "TFile.h"
#include "TH1D.h"


//  usage : MakeConfigFile_VLQ_sigbkg [super_merge] [bkgd_comp]



int main(int argc, char** argv){


  std::map<std::string, std::vector<std::string> > reg_normal_merge{
    {"0Tex0Hex", {"2bexHighMtbmin",
	  "2bexLowMtbmin",
	  "2bex",
	  "3bexHighMtbmin",
	  "3bexLowMtbmin",
	  "3bex",
	  "4binHighMtbmin",
	  "4binLowMtbmin",
	  "4bin"}},

      {"0Tex1Hex", {"2bexHighMtbmin",
	    "2bexLowMtbmin",
	    "2bex",
	    "3bexHighMtbmin",
	    "3bexLowMtbmin",
	    "3bex",
	    "4binHighMtbmin",
	    "4binLowMtbmin",
	    "4bin"}},

	{"0Tin2Hin", {"2bexHighMtbmin",
	      "2bexLowMtbmin",
	      "2bex",
	      "3bexHighMtbmin",
	      "3bexLowMtbmin",
	      "3bex"}},

	  {"1Tex0Hex", {"2bexHighMtbmin",
		"2bexLowMtbmin",
		"2bex",
		"3bexHighMtbmin",
		"3bexLowMtbmin",
		"3bex",
		"4binHighMtbmin",
		"4binLowMtbmin",
		"4bin"}},

	    {"1Tex1Hex", {"2bexHighMtbmin",
		  "2bexLowMtbmin",
		  "2bex",
		  "3bexHighMtbmin",
		  "3bexLowMtbmin",
		  "3bex"}},

	      {"2THin", {"4binHighMtbmin",
		    "4binLowMtbmin",
		    "4bin"}},

		{"2Tin0_1Hwin", {"2bexHighMtbmin",
		      "2bexLowMtbmin",
		      "2bex",
		      "3bexHighMtbmin",
		      "3bexLowMtbmin",
		      "3bex"}}

  };

  std::map<std::string, std::vector<std::string> > reg_super_merge{
    {"0THex", {"2bexHighMtbmin",
	  "2bexLowMtbmin",
	  "2bex",
	  "3bexHighMtbmin",
	  "3bexLowMtbmin",
	  "3bex",
	  "4binHighMtbmin",
	  "4binLowMtbmin",
	  "4bin"}},

      {"1THex", {"2bexHighMtbmin",
	    "2bexLowMtbmin",
	    "2bex",
	    "3bexHighMtbmin",
	    "3bexLowMtbmin",
	    "3bex",
	    "4binHighMtbmin",
	    "4binLowMtbmin",
	    "4bin"}},

	{"2THex", {"2bexHighMtbmin",
	      "2bexLowMtbmin",
	      "2bex",
	      "3bexHighMtbmin",
	      "3bexLowMtbmin",
	      "3bex",
	      "4binHighMtbmin",
	      "4binLowMtbmin",
	      "4bin"}},

	  {"3THin", {"2bexHighMtbmin",
		"2bexLowMtbmin",
		"2bex",
		"3bexHighMtbmin",
		"3bexLowMtbmin",
		"3bex",
		"4binHighMtbmin",
		"4binLowMtbmin",
		"4bin"}}
  };




  std::map<std::string, std::vector<std::string> >* regions = NULL;
  if( atoi(argv[1]) == 1 ) { regions = &reg_super_merge; }
  else {  regions = &reg_normal_merge; }

  std::string var_config_base = (atoi(argv[2]) == 1) ? "VLQ_bkgd_meff_variables_config" : "VLQ_sigbkgd_meff_variables_config";
  for(std::pair<std::string, std::vector<std::string> > reg_pair : *regions){

    std::ofstream f_config;
    f_config.open((var_config_base + "_" + reg_pair.first + ".txt").c_str());
    f_config << "BEGIN" << std::endl;

    for(const std::string& jet : {"6jex", "7jin"}){

      for(const std::string& reg : reg_pair.second){

	std::string var_name = "c0lep" + reg_pair.first + jet + reg + "_meff";
	f_config << "NEW" << std::endl;
	f_config << "NAME : " << var_name << std::endl;
	if( atoi(argv[2]) == 1 ){
	  f_config << "DRAWSTACK : TRUE" << std::endl;
	  f_config << "DRAWRES : NONE" << std::endl;
	  f_config << "DOSCALE : NORM" << std::endl;
	}
	else{
	  f_config << "DRAWSTACK : FALSE" << std::endl;
	  f_config << "DRAWRES : RESIDUAL" << std::endl;
	  f_config << "DOSCALE : NORM" << std::endl;
	  f_config << "RESREFLINE : 0.05"<<std::endl;
	  f_config << "RESMIN : 1.e-5" << std::endl;
	  f_config << "RESMAX : 100." << std::endl;
	  f_config << "ISLOGRES : TRUE" << std::endl;
	}
	f_config << "REBIN : 2" << std::endl;
	f_config << 
	  "EXTRALABEL : #splitline{#splitline{#scale[1.2]{#bf{#it{ATLAS}} Internal}}{#sqrt{s}=13 TeV, 36.5 fb^{-1}}}{" 
		 << reg << "}" << std::endl;
	f_config << std::endl;

      }//bjet-boost combinations

    }//jet multiplicities

    f_config << "END" <<std::endl;
    f_config.close();

  }//boost categories

  return 0;

}

