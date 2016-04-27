#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>

#include "TCanvas.h"
#include "TStyle.h"
#include "TColor.h"
#include "TPaveText.h"

#include "IFAETopFramework/HistManager.h"
#include "IFAETopFramework/AnalysisUtils.h"

std::string sample = "";
std::string variable = "";
const std::string s_core = "SR_h__nev_SR_wgt";

std::vector<std::string> SR_vec;
double mg_min = 950.;  double mg_max = 2050.; double  mg_bins = 100.;
double mx_min = 0.; double mx_max = 0.; double mx_bins = 0.;


HistManager* m_hstMngr = new HistManager();

const std::string atlas_text = "#bf{#it{ATLAS}} Internal";
const std::string lumi_text = "L^{int} = 3.2 fb^{-1}, #sqrt{s} = 13 TeV";
const std::string gtt_text = "#tilde{g}#tilde{g} production, #tilde{g} #rightarrow t#bar{t}+#tilde{#chi^{0}_{1}}, m(#tilde{q}) >> m(#tilde{g})";
const std::string gbb_text = "#tilde{g}#tilde{g} production, #tilde{g} #rightarrow b#bar{b}+#tilde{#chi^{0}_{1}}, m(#tilde{q}) >> m(#tilde{g})";

int main(int argc, char** argv){

  std::stringstream ss; std::string sw = "";
  while ((optind < argc) && (argv[optind][0]=='-')) {
    ss << argv[optind]; sw = ss.str();
    std::cout<<"Parsing '"<<argv[optind]<<" : "<<sw<<std::endl;
    if     (sw == "--sample"){optind++; ss.str(""); ss << argv[optind]; ss >> sample; std::cout<<"sample = "<<sample<<std::endl;}
    else if(sw == "--variable"){optind++; ss.str(""); ss << argv[optind]; ss >> variable; std::cout<<"variable = "<<variable<<std::endl;}
    else std::cout<<"Unknown switch "<<argv[optind]<<" sw = "<<sw<<std::endl;
    optind++; ss.str(""); ss.clear(); sw.clear();
  }

  gStyle->SetOptStat(0);
  //gStyle->SetPalette(kLightTemperature);
  //gStyle->SetPalette(kBird);
  gStyle->SetPaintTextFormat("2.1f");
  SR_vec.clear();
  std::string sys_input = "/afs/cern.ch/work/f/farooque/RunIIArea/DEVAREA/SUSYSignalSYSOutputs/" + variable + "_" + s_core + "_" + sample + ".txt";
  std::cout << " INPUT FILE :: "<<sys_input << std::endl;

  if(sample == "Gtt"){ 
    // Gtt
    mx_min = -25.;  mx_max = 1725.;  mx_bins = 50.;
    //if(variable == "Yields_Nominal"){
      SR_vec.push_back("SR-Gtt-0L-A");
      SR_vec.push_back("SR-Gtt-0L-B");
      SR_vec.push_back("SR-Gtt-0L-C");
      SR_vec.push_back("SR-Gtt-1L-A");
      SR_vec.push_back("SR-Gtt-1L-B");
      //}
      /*
    else{
      SR_vec.push_back("SR-Gtt-0L-A_UP");
      SR_vec.push_back("SR-Gtt-0L-A_DOWN");
      SR_vec.push_back("SR-Gtt-0L-B_UP");
      SR_vec.push_back("SR-Gtt-0L-B_DOWN");
      SR_vec.push_back("SR-Gtt-0L-C_UP");
      SR_vec.push_back("SR-Gtt-0L-C_DOWN");
      SR_vec.push_back("SR-Gtt-1L-A_UP");
      SR_vec.push_back("SR-Gtt-1L-A_DOWN");
      SR_vec.push_back("SR-Gtt-1L-B_UP");
      SR_vec.push_back("SR-Gtt-1L-B_DOWN");
    }
      */
  }
  else if(sample == "Gbb"){ 
    //if(variable == "Yields_Nominal"){
    // Gbb
    mx_min = -50.;  mx_max = 2050.;  mx_bins = 100.;

      SR_vec.push_back("SR-Gbb-A");
      SR_vec.push_back("SR-Gbb-B");
      SR_vec.push_back("SR-Gbb-C");
      //}
      /*
    else{
      SR_vec.push_back("SR-Gbb-A_UP");
      SR_vec.push_back("SR-Gbb-A_DOWN");
      SR_vec.push_back("SR-Gbb-B_UP");
      SR_vec.push_back("SR-Gbb-B_DOWN");
      SR_vec.push_back("SR-Gbb-C_UP");
      SR_vec.push_back("SR-Gbb-C_DOWN");
    }
      */
  }

  std::string _smg = ""; std::string _smx = "";
  for(std::string SR : SR_vec){
    //std::cout<<" Making histogram "<<"hist_"+SR <<std::endl; 
    m_hstMngr->BookTH2D("hist_"+SR, "", mg_bins, mg_min, mg_max, mx_bins, mx_min, mx_max, "", "m_{#tilde{g}} [GeV]", "m_{#tilde{#chi^{0}_{1}}} [GeV]");
  }

  std::vector< std::map<std::string, std::string> > sys_map; sys_map.clear();
  std::cout << " Now parsing input file" <<std::endl;
  int nline = AnalysisUtils::ParseConfigFile_Lines(sys_input, sys_map);
  std::cout<<" Number of grid points = "<<nline<<std::endl;
  TH2D* hcur = NULL;
  for( std::map<std::string, std::string> parsed_map : sys_map ){

    std::string sample = parsed_map["SAMPLE"];
    _smg = sample.substr(sample.find("_") + 1);

    _smx = _smg.substr(_smg.find("_")+1);
    _smg = _smg.substr(0, _smg.find("_"));

    for( std::pair<std::string, std::string> line_pair : parsed_map ){
      hcur = NULL;
      if(line_pair.first == "SAMPLE") continue;
      if(line_pair.second.find("nan") != std::string::npos) continue;
      //std::cout<<" Retrieving histogram "<<"hist_"+line_pair.first <<std::endl; 

      std::string SRtext = line_pair.first;
      if(SRtext.find("_UP") != std::string::npos){
	SRtext = SRtext.substr(0, SRtext.find("_UP"));
      }
      else if(SRtext.find("_DOWN") != std::string::npos){
	SRtext = SRtext.substr(0, SRtext.find("_DOWN"));
      }

      hcur = m_hstMngr->GetTH2D("hist_"+SRtext);
      int b_mx = hcur->GetYaxis()->FindBin(atof(_smx.c_str()));
      int b_mg = hcur->GetXaxis()->FindBin(atof(_smg.c_str()));
      double val = atof(line_pair.second.c_str()); 
      if( variable == "Syst" ){ val *= 100.; }
      if(val > hcur->GetBinContent(b_mg, b_mx))hcur->SetBinContent(b_mg, b_mx, val);
    }//Each SR

  }//Each sample

  hcur = NULL;
  //---------------- Histograms filled at this point ----------------------------
  //Make the TPaveTexts.
  TPaveText* ATLASPave = new TPaveText(0.2,0.78, 0.5, 0.83, "nbNDC");
  ATLASPave->AddText(atlas_text.c_str());
  ATLASPave->SetTextFont(42);
  ATLASPave->SetTextSize(0.035);
  ATLASPave->SetShadowColor(0);
  ATLASPave->SetFillColor(0);
  ATLASPave->SetFillStyle(0);
  ATLASPave->SetLineStyle(0);
  ATLASPave->SetLineColor(0);
  ATLASPave->SetLineWidth(0);

  TPaveText* ProcPave = new TPaveText(0.2, 0.86, 0.5, 0.88, "nbNDC");
  if(sample == "Gtt"){ProcPave->AddText(gtt_text.c_str());}
  else if(sample == "Gbb"){ProcPave->AddText(gbb_text.c_str());}
  ProcPave->SetTextFont(42);
  ProcPave->SetTextSize(0.02);
  ProcPave->SetShadowColor(0);
  ProcPave->SetFillColor(0);
  ProcPave->SetFillStyle(0);
  ProcPave->SetLineStyle(0);
  ProcPave->SetLineColor(0);
  ProcPave->SetLineWidth(0);

  TPaveText* LumiPave = new TPaveText(0.6, 0.86, 0.9, 0.88, "nbNDC");
  LumiPave->AddText(lumi_text.c_str());
  LumiPave->SetTextFont(42);
  LumiPave->SetTextSize(0.02);
  LumiPave->SetShadowColor(0);
  LumiPave->SetFillColor(0);
  LumiPave->SetFillStyle(0);
  LumiPave->SetLineStyle(0);
  LumiPave->SetLineColor(0);
  LumiPave->SetLineWidth(0);

  //Now loop over the histograms, draw them in a canvas and print them
  for(std::string SR : SR_vec){

    std::string SRtext = SR;
    if(SRtext.find("_UP") != std::string::npos){
      SRtext = SRtext.substr(0, SRtext.find("_UP"));
    }
    else if(SRtext.find("_DOWN") != std::string::npos){
      SRtext = SRtext.substr(0, SRtext.find("_DOWN"));
    }

    TPaveText* SRPave = new TPaveText(0.16,0.65, 0.45, 0.7, "nbNDC");
    SRPave->AddText(SRtext.c_str());
    SRPave->SetTextFont(42);
    SRPave->SetTextSize(0.035);
    SRPave->SetShadowColor(0);
    SRPave->SetFillColor(0);
    SRPave->SetFillStyle(0);
    SRPave->SetLineStyle(0);
    SRPave->SetLineColor(0);
    SRPave->SetLineWidth(0);

    hcur = m_hstMngr->GetTH2D("hist_"+SR);
    if(variable == "Yields_Nominal"){ hcur->SetZTitle("Number of Events"); } 
    else{ 
      hcur->SetZTitle("Total Uncertainty [%]"); 
    }
    TCanvas* canv = new TCanvas(Form("canv_%s", SR.c_str()), "", 700, 700);

    const UInt_t Number = 3;
    Double_t Red[Number]    = { 0.5, 1.0, 0.0};
    Double_t Green[Number]  = { 0.0,  1.0, 0.8};
    Double_t Blue[Number]   = { 0.0, 0.0, 1.0};
    Double_t Length[Number] = { 0.0, 0.0, 1.0 };
    Int_t nb=50;

    TColor::CreateGradientColorTable(Number,Length,Red,Green,Blue,nb);

    canv->SetLeftMargin(0.15);
    canv->SetRightMargin(0.15);
    canv->SetTopMargin(0.15);
    canv->SetBottomMargin(0.15);

    canv->cd();
    hcur->Draw("coltextz");
    hcur->GetYaxis()->SetTitleOffset(1.5);
    SRPave->DrawClone();
    ATLASPave->DrawClone();
    ProcPave->DrawClone();
    LumiPave->DrawClone();

    canv->SetTicks();
    canv->SaveAs( Form("canv_%s.png", SR.c_str()) );
    canv->SaveAs( Form("canv_%s.pdf", SR.c_str()) );
    delete canv;

    delete SRPave;
  }

  m_hstMngr->ClearAllTH2();

  delete ATLASPave;
  delete ProcPave;
  delete LumiPave;
  delete m_hstMngr;

  return 0;

}
