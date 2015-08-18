#include "IFAEPlotter/PlotManager.h"

#include <stdlib.h>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <TFile.h>
#include <TChain.h>

int main(int argc, char** argv){

  //run as:
  //plot --samples sample_config.txt --variables variable_config.txt --style stylelib.txt --files filelist.txt 
  //global flags --outputfile --outputformat 

  int optind = 1;

  std::stringstream ss; std::string sw = "";

  std::string samplefile = "";
  std::string varfile = "";
  std::string stylefile = "style_default.txt";
  std::string filelist = "";
  std::string outfile = "";
  std::string outformat = ".png";
  double globalscale = 1.;

  while ((optind < argc) && (argv[optind][0]=='-')) {
    ss << argv[optind]; sw = ss.str();
    std::cout<<"Parsing '"<<argv[optind]<<" : "<<sw<<std::endl;
    if     (sw == "--samples"){optind++; ss.str(""); ss << argv[optind]; ss >> samplefile; std::cout<<"samplefile = "<<samplefile<<std::endl;}
    else if(sw == "--variables"){optind++; ss.str(""); ss << argv[optind]; ss >> varfile; std::cout<<"varfile = "<<varfile<<std::endl;}
    else if(sw == "--style"){optind++; ss.str(""); ss << argv[optind]; ss >> stylefile; std::cout<<"stylefile = "<<stylefile<<std::endl;}
    else if(sw == "--files"){optind++; ss.str(""); ss << argv[optind]; ss >> filelist; std::cout<<"filelist = "<<filelist<<std::endl;}
    else if(sw == "--outfile"){optind++; ss.str(""); ss << argv[optind]; ss >> outfile; std::cout<<"outfile = "<<outfile<<std::endl;}
    else if(sw == "--outformat"){optind++; ss.str(""); ss << argv[optind]; ss >> outformat; std::cout<<"outformat = "<<outformat<<std::endl;}
    else if(sw == "--globalscale"){optind++; ss.str(""); ss << argv[optind]; ss >> globalscale; std::cout<<"globalscale = "<<globalscale<<std::endl;}

    else std::cout<<"Unknown switch "<<argv[optind]<<" sw = "<<sw<<std::endl;
    optind++; ss.str(""); ss.clear(); sw.clear();  
  }
  
  PlotManager* plotMngr = new PlotManager(samplefile, filelist, varfile, stylefile); 
  plotMngr->Execute();
  plotMngr->Terminate();
  return 0;
  
}
