#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

#include "TFile.h"
#include "TH1D.h"
#include "IFAETopFramework/AnalysisUtils.h"

int main(int argc, char** argv){

  double blind_thresh = 0.05;
  double blind_thresh_max = 0.075;

  std::map<std::string, std::vector<std::string> > regions; 

  if( atoi(argv[1]) == 1){ 
    regions =  {
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
  }

  else{ 
    regions = {
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

  }

  TFile* f_bkgd;
  std::map<std::string, TFile*> f_sig;  f_sig.clear();

  std::string variable="meff";
  double lumi=36500.;


  f_bkgd = TFile::Open("/eos/atlas/user/f/farooque/VLQFitInputsTest/2017_02_21_VLQAnalysis-00-03-06-00TEST/Backgrounds.root", "READ");
  f_sig["0_TTS"]   = TFile::Open("/eos/atlas/user/f/farooque/VLQFitInputsTest/2017_02_21_VLQAnalysis-00-03-06-00TEST/VLQ_TT_1000_TSinglet.root", "READ");
  f_sig["1_TTD"]   = TFile::Open("/eos/atlas/user/f/farooque/VLQFitInputsTest/2017_02_21_VLQAnalysis-00-03-06-00TEST/VLQ_TT_1000_TDoublet.root", "READ");
  f_sig["2_ZtZt"]   = TFile::Open("/eos/atlas/user/f/farooque/VLQFitInputsTest/2017_02_21_VLQAnalysis-00-03-06-00TEST/VLQ_TT_1000_ZtZt.root", "READ");
  
  std::ofstream f_sbyb;
  if( atoi(argv[1]) == 1){ f_sbyb.open("SigBkgdYields_0L_NewRegions_NormalMerge.txt"); }
  else{ f_sbyb.open("SigBkgdYields_0L_NewRegions_SuperMerge.txt"); } 

  f_sbyb << "\\documentclass[landscape]{article}" << std::endl;
  f_sbyb << "\\usepackage[margin=0.5in]{geometry}" << std::endl;
  f_sbyb << "\\usepackage{multirow}" << std::endl;
  f_sbyb << "\\begin{document}" << std::endl;

  std::vector<std::string> njets{"6jex", "7jin"};
  for( const std::string& jet : njets ){

    f_sbyb << " N\\_jets = " << jet  << "\\\\" << std::endl;
    f_sbyb << "\\vspace{5 mm}"<<std::endl;

    for(std::pair<std::string, std::vector<std::string> > reg_pair : regions){

      f_sbyb << "Boosted object category : " << AnalysisUtils::ReplaceString(reg_pair.first,"_","\\_")  << "\\\\" << std::endl; 
      f_sbyb << "\\vspace{5 mm}"<<std::endl;

      std::string column_header = "\\begin{tabular}{c|c|";
      int ncol = (reg_pair.second).size();
      for(int i = 0; i<ncol; i++){ column_header += "c"; }
      column_header += "}"; 
      f_sbyb << column_header << std::endl;
      f_sbyb << "\\vspace{5 mm}"<<std::endl;

      std::string s_yield_bkgd = "Background &  ";
      std::map<std::string, std::string> s_yield_sig; s_yield_sig.clear();
      std::map<std::string, std::string> s_SbyB_sig; s_yield_sig.clear();
      std::map<std::string, std::string> s_SbySqB_sig; s_yield_sig.clear();
      std::string s_blinding     = "($S/B<$0.05) &  ";
      std::string s_blinding_max = "($S/B<$0.075) &  ";

      for(std::pair<std::string, TFile*> sig_pair : f_sig){

	s_yield_sig[sig_pair.first]   = "\\multirow{3}{*}{"+sig_pair.first+"} & Yield ";
	s_SbyB_sig[sig_pair.first]    = " & $S/B$";
	s_SbySqB_sig[sig_pair.first]  = " & $S/\\sqrt{B}$";

      }

      for(const std::string& region : reg_pair.second){
	f_sbyb << " & " << AnalysisUtils::ReplaceString(region, "_", "\\_");
	std::string histname = "c0lep" + reg_pair.first + jet + region + "_" + variable;

	TH1D* h_bkgd = (TH1D*)(f_bkgd->Get(histname.c_str()));
	double err_yield_bkgd;
	double yield_bkgd = h_bkgd->IntegralAndError(0,-1,err_yield_bkgd)*lumi;
	err_yield_bkgd *= lumi;
	s_yield_bkgd  += Form(" & %.3g $\\pm$ %.2g", yield_bkgd, err_yield_bkgd);

	std::cout<<" histname = "<<histname << "bkgd_yield = " << yield_bkgd <<std::endl;

	double blind_cut = 10000.;
	double blind_cut_max = 10000.;
	for(std::pair<std::string, TFile*> sig_pair : f_sig){

	  TH1D* h_sig = (TH1D*)( (sig_pair.second)->Get(histname.c_str()) );

	  double err_yield_sig;
	  double yield_sig  = h_sig->IntegralAndError(0,-1,err_yield_sig)*lumi;
	  err_yield_sig *= lumi;

	  double SbyB_sig    = (yield_bkgd > 0.) ? yield_sig/yield_bkgd : 0.;
	  double SbySqB_sig  = (yield_bkgd > 0.) ? yield_sig/sqrt(yield_bkgd) : 0.;

	  TH1D* h_ratio = (TH1D*)(h_sig->Clone(Form("%s_clone",h_sig->GetName())));
	  h_ratio->Divide(h_bkgd);

	  for(int b = 1; b <= h_ratio->GetNbinsX(); b++){

	    double edge = h_ratio->GetXaxis()->GetBinLowEdge(b);
	    //if(edge >= blind_cut_max) break;

	    if( (blind_cut >= edge) && h_ratio->GetBinContent(b) >= blind_thresh ){ blind_cut = edge; }
	    if( (blind_cut_max >= edge) && h_ratio->GetBinContent(b) >= blind_thresh_max ){ blind_cut_max = edge; break; }

	  }//bin loop


	  delete h_ratio;

	  s_yield_sig[sig_pair.first]   += Form(" & %.3g $\\pm$ %.2g", yield_sig, err_yield_sig);
	  s_SbyB_sig[sig_pair.first]    += Form(" & %.3g", SbyB_sig);
	  s_SbySqB_sig[sig_pair.first]  += Form(" & %.3g", SbySqB_sig);

	}//signal loop

	s_blinding  += Form(" & %.0f", blind_cut);
	s_blinding_max  += Form(" & %.0f", blind_cut_max);

	histname.clear();

      }//region loop inside table

      f_sbyb << " \\\\"<<std::endl;
      f_sbyb << " \\hline" <<std::endl; 

      f_sbyb << s_yield_bkgd << "\\\\" << std::endl;
      f_sbyb << " \\hline" <<std::endl; 
      
      for(std::pair<std::string, TFile*> sig_pair : f_sig){
	f_sbyb << s_yield_sig[sig_pair.first] << "\\\\" << std::endl;
	f_sbyb << s_SbyB_sig[sig_pair.first] << "\\\\" << std::endl;
	f_sbyb << s_SbySqB_sig[sig_pair.first] << "\\\\" << std::endl;
	f_sbyb << " \\hline" <<std::endl; 
      }// signal loop
      
      f_sbyb << s_blinding << "\\\\" << std::endl;
      f_sbyb << s_blinding_max << "\\\\" << std::endl;
      f_sbyb << s_blinding << "\\\\" << std::endl;
      f_sbyb << " \\hline" <<std::endl; 
      f_sbyb << "\\vspace{5 mm}"<<std::endl;
      f_sbyb << "\\end{tabular}\\\\"<<std::endl;
      
      f_sbyb << "\\vspace{10 mm}"<<std::endl;
      
    }// boosted object catgories
    
    f_sbyb << "\\vspace{15 mm}"<<std::endl;
    
  }//nRC
  
  f_sbyb << "\\end{document}" <<std::endl;
  f_sbyb.close();

  for(std::pair<std::string, TFile*> sig_pair : f_sig){ (sig_pair.second)->Close(); }
  f_bkgd->Close();

  return 0;

}
