#include <TFile.h>
#include <TH1.h>
#include <string>
#include <sstream>
//#include <map>
#include <TKey.h>
#include <IFAETopFramework/AnalysisUtils.h>

int main(int argc, char** argv){

  std::stringstream ss; std::string sw = "";

  std::string input_filename     = "";
  std::string output_filename    = "";
  std::string input_name        = "";
  std::string output_name       = "";
  //std::string replace_mode       = "";

  int optind = 1;

  std::string _switch = ""; 

  while ((optind < argc) && (argv[optind][0]=='-')) {
    ss << argv[optind]; _switch = ss.str();  
    optind++;  ss << argv[optind];
    if       (_switch == "--input_filename")   { ss >> input_filename; }
    else if  (_switch == "--output_filename")  { ss >> output_filename; }
    else if  (_switch == "--input_name")      { ss >> input_name; }
    else if  (_switch == "--output_name")     { ss >> output_name; }
    //else if  (_switch == "--replace_mode")     { ss >> replace_mode; }
    else std::cout<<"Unknown switch "<<_switch<<std::endl;

    optind++; ss.clear(); _switch.clear();
  }

  TFile* infile   = TFile::Open(input_filename.c_str(), "READ");
  TFile* outfile  = TFile::Open(output_filename.c_str(), "UPDATE");

  TIter next(infile->GetListOfKeys());
  TKey *key=NULL;

  std::string className = "";
  std::string oldname = "";
  std::string newname = "";

  while( (key = (TKey*)next()) ) {

    className = (std::string)(key->GetClassName());
    if( className.find("TH1") == std::string::npos ){ className.clear(); continue; }
    className.clear();

    oldname = (std::string)(key->GetName());
    newname  = AnalysisUtils::ReplaceString(oldname, input_name, output_name);
    TH1D* outhist = (TH1D*)(((key->ReadObj()))->Clone(newname.c_str()));
    outhist->SetDirectory(0);
    outfile->cd();
    outhist->Write();
    delete outhist;

    oldname.clear(); newname.clear();

  }

  outfile->Close();
  delete outfile;

  infile->Close();
  delete infile;





}
