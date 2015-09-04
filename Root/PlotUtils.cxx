#include "IFAEPlotter/PlotUtils.h"
#include "IFAEPlotter/StyleDictionary.h"
#include "IFAEPlotter/Plotter_Options.h"
#include "IFAETopFramework/HistManager.h"

#include "THStack.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TLegend.h"
#include "TLegendEntry.h"
//#include "TIter.h"
#include "TLatex.h"
#include "TLine.h"
#include <algorithm>

PlotUtils::PlotUtils(Plotter_Options* opt, HistManager* hstMngr, SampleAttributesMap& attrbt_map, VariableAttributesMap& var_map, StyleDictionary* style_dict
		     , std::vector<std::string>* samples_noshape ) : 
  m_opt(opt),
  m_hstMngr(hstMngr),
  m_attrbt_map(attrbt_map),
  m_var_map(var_map),
  m_style_dict(style_dict),
  m_outfile(NULL)
{ 

  if(m_opt->MsgLevel() == Debug::DEBUG) std::cout<<"PlotUtils::PlotUtils start"<<std::endl; 

  m_drawSum = (m_attrbt_map.find("SUM") != m_attrbt_map.end());
  m_samples_noshape = samples_noshape;
  if(m_opt->OutputFormat().find("ROOT") != std::string::npos){
    m_outfile = TFile::Open(Form("%s%s.root", m_opt->OutputFolder().c_str(), m_opt->OutputFile().c_str()), "RECREATE");
  }

  if(m_opt->MsgLevel() == Debug::DEBUG) std::cout<<"PlotUtils::PlotUtils end"<<std::endl; 

}

PlotUtils::~PlotUtils(){
  delete m_outfile;
}

void PlotUtils::Terminate(){
  m_outfile->Close();
}

void PlotUtils::OverlayHists(){

  if(m_opt->MsgLevel() == Debug::DEBUG) std::cout<<"PlotUtils::OverlayHists start"<<std::endl; 

  int nsample = m_attrbt_map.size();
  int ndist = m_var_map.size();

  bool var_draw_stack = 0;
  bool var_isLog = false;
  bool var_isShape = false;
  bool var_do_width = false;

  bool ds_draw_stack = false;
  int ds_res_opt = -1;

  //One preliminary loop to find the baseline sample
  std::string s_base_name = ""; 
  std::string s_base_suffix = "";
  for(SampleAttributesMap::iterator at_it = m_attrbt_map.begin(); at_it != m_attrbt_map.end(); ++at_it){
    if(at_it->second->ResOpt() == 1){
      if(s_base_name != ""){ std::cout<<"Error: More than one baseline specified for residual calculation"<<std::endl; }
      else{ 
	s_base_name = at_it->first;
	s_base_suffix = at_it->second->Suffix();
      }
    }
  } 

  for(VariableAttributesMap::iterator va_it = m_var_map.begin(); va_it != m_var_map.end(); ++va_it){

    const std::string& var_name     = va_it->second->Name();
    const std::string& var_label    = va_it->second->Label();
    const std::string& var_draw_res = va_it->second->DrawRes();
    const std::string& var_ylabel = (va_it->second->YLabel() != "") ? va_it->second->YLabel() : m_opt->YLabel();
    const std::string& var_reslabel = (va_it->second->ResLabel() != "") ? va_it->second->ResLabel() : m_opt->ResLabel();

    var_isShape                = (va_it->second->DoScale() == "SHAPE"); 
    var_do_width               = va_it->second->DoWidth();
    var_draw_stack = va_it->second->DrawStack();
    var_isLog = va_it->second->IsLog();


    std::string hname_sum = m_drawSum ? var_name + "_" + m_attrbt_map["SUM"]->Suffix() : "";

    bool drawRes = ( (var_draw_res != "") && (var_draw_res != "NONE") );
    if(drawRes && (s_base_name == "") ){std::cout<<"No reference sample specified for residual calculation"<<std::endl;}
    //NOTE:
    //Eventually, TCanvas and TLegend properties will be defined in the StyleDictionary
    //Make a TCanvas and set its attributes
    std::string canv_name = "canv_" + var_name;
    std::string hs_stack_name = "hs_stack_" + var_name;
    std::string hs_nostack_name = "hs_nostack_" + var_name;
    std::string hs_res_name = "hs_res_" + var_name;

    TCanvas* canv_a = new TCanvas(canv_name.c_str(), "", 800, 800);
    SetStyleCanvas( *canv_a, drawRes );

    THStack* hs_res_a = drawRes ? new THStack(hs_res_name.c_str(), "") : NULL;
    std::string hbasename = var_name + "_" + s_base_suffix;
    TH1D* h_base = drawRes ? m_hstMngr->GetTH1D(hbasename) : NULL;

    THStack* hs_nostack_a = new THStack(hs_nostack_name.c_str(), "");
    THStack* hs_stack_a = var_draw_stack ? new THStack(hs_stack_name.c_str(), "") : NULL;

    TLegend* leg_a = new TLegend();
    TLegend* leg_yield = (m_opt->ShowYields() && (!var_isShape || var_draw_stack) ) ? new TLegend() : NULL;
    double textsize=0.03;
    if(drawRes){textsize=0.03;}
    SetStyleLegend(*leg_a, textsize);
    leg_a->Clear();
    if(leg_yield){
      SetStyleLegend(*leg_yield, textsize, 42, 0.05);
      leg_yield->Clear();
    }
    //Decide on the rules for drawing stack and for drawing residuals or ratios
    //This means decide how many pads the canvas will have
    //Make a new THStack
    //Make a TLegend and set its style

    for(SampleAttributesMap::iterator at_it = m_attrbt_map.begin(); at_it != m_attrbt_map.end(); ++at_it){

      if( var_isShape && !var_draw_stack && at_it->first == "SUM" ){continue;}
      if( var_isShape && (std::find(m_samples_noshape->begin(), m_samples_noshape->end(), at_it->first) != m_samples_noshape->end()) ){continue;}

      const std::string& ds_name = at_it->second->Name();
      const std::string& ds_suffix = at_it->second->Suffix();
      const std::string& ds_leglabel = at_it->second->LegLabel();
      const std::string& ds_stylekey = at_it->second->StyleKey();
      const std::string& ds_drawopt = at_it->second->DrawOpt();
      bool ds_isShape = (at_it->second->DrawScale() == "SHAPE"); 
      ds_draw_stack = at_it->second->DrawStack();
      ds_res_opt = at_it->second->ResOpt();

      std::string hist_name = var_name + "_" + ds_suffix;
      SetStyleHist(hist_name, ds_stylekey);
      TH1D* hist_a = m_hstMngr->GetTH1D(hist_name);
      leg_a->AddEntry(hist_a, ds_leglabel.c_str(), "lpf");
      //leg_a->AddEntry(hist_a, Form("%.1f",hist_a->Integral()), "lpf");
      if(leg_yield){ 
	if(!ds_isShape){leg_yield->AddEntry(hist_a, Form("%.4g",hist_a->Integral()), ""); }
	else{ leg_yield->AddEntry(hist_a, " ", ""); }
      }

      if(var_do_width){
	int nbin = hist_a->GetNbinsX();
	for(int nb = 0; nb <= nbin; nb++){
	  double bc = hist_a->GetBinContent(nb)/hist_a->GetXaxis()->GetBinWidth(nb);
	  double be = hist_a->GetBinError(nb)/hist_a->GetXaxis()->GetBinWidth(nb);
	  hist_a->SetBinContent(nb, bc);
	  hist_a->SetBinError(nb, be);
	}
      }

      if(var_draw_stack && ds_draw_stack){ hs_stack_a->Add(hist_a, "hist"); }
      else{ 
	hs_nostack_a->Add(hist_a, ds_drawopt.c_str()); 

	if(drawRes && (ds_res_opt == 0) && (ds_name != "SUM") ){
	  std::string resname_a = var_name + "_" + ds_suffix + "_res_" + s_base_suffix;
	  TH1D* hist_res_a = makeResidual(resname_a, hist_name, hbasename, var_draw_res);
	  hs_res_a->Add(hist_res_a);
	  resname_a.clear();
	} 

      }//if this sample is not be be stacked

      //Clear strings
      hist_name.clear();
    }//sample loop

    if( drawRes && var_draw_stack ){
      std::string resname_sum_a = var_name + "_sum_" + "_res_" + s_base_suffix;
      TH1D* hsum_res_a = makeResidual(resname_sum_a, hname_sum, hbasename, var_draw_res);
      hs_res_a->Add(hsum_res_a);
      resname_sum_a.clear();
    }
    //int size_res = hs_res_a ? hs_res_a->GetNhists() : 0;
    //std::cout<<" var_draw_stack = "<<var_draw_stack<<" drawRes = "<<drawRes<<" hs_res_a->size() = "<<size_res<<std::endl; 

    //Draw the THStack in the top panel, set the x and y axis labels
    double c_min = 1.e-10;
    double c_max = 1.; 
    double stretch_max = var_isLog ? 1.E3 : 1.3;
    double stretch_min = var_isLog ? 1.E-2 : 0.5;

    TPad* curpad = (TPad*)(canv_a->cd(1));
    if(var_isLog){curpad->SetLogy();}

    if(var_draw_stack){
      hs_stack_a->Draw();
      c_max = hs_stack_a->GetHistogram()->GetMaximum();
      c_min = hs_stack_a->GetHistogram()->GetMinimum();

      if(hs_nostack_a->GetNhists() > 0){
	hs_nostack_a->Draw("samenostack");
	c_max = max(c_max, hs_nostack_a->GetHistogram()->GetMaximum());
	c_min = max(c_min, hs_nostack_a->GetHistogram()->GetMinimum());
      }

      if(var_label != ""){ hs_stack_a->GetHistogram()->GetXaxis()->SetTitle(var_label.c_str()); }
      else{ hs_stack_a->GetHistogram()->GetXaxis()->SetTitle( ((TH1D*)(hs_stack_a->GetStack()->First()))->GetXaxis()->GetTitle() ) ; }

      if(var_ylabel != ""){ hs_stack_a->GetHistogram()->GetYaxis()->SetTitle(var_ylabel.c_str()); }
      else{ hs_stack_a->GetHistogram()->GetYaxis()->SetTitle( ((TH1D*)(hs_stack_a->GetStack()->First()))->GetYaxis()->GetTitle() ) ; }

      c_max *= stretch_max;
      c_min *= stretch_min;

      hs_stack_a->SetMinimum(c_min);
      hs_stack_a->SetMaximum(c_max);

    }
    else{
      hs_nostack_a->Draw("nostack"); 
      c_max = hs_nostack_a->GetHistogram()->GetMaximum();
      c_min = hs_nostack_a->GetHistogram()->GetMinimum();
      if(var_label != ""){ hs_nostack_a->GetHistogram()->GetXaxis()->SetTitle(var_label.c_str()); }
      else{ hs_nostack_a->GetHistogram()->GetXaxis()->SetTitle( ((TH1D*)(hs_nostack_a->GetStack()->First()))->GetXaxis()->GetTitle() ) ; }

      if(var_ylabel != ""){ hs_nostack_a->GetHistogram()->GetYaxis()->SetTitle(var_ylabel.c_str()); }
      else{ hs_nostack_a->GetHistogram()->GetYaxis()->SetTitle( ((TH1D*)(hs_nostack_a->GetStack()->First()))->GetYaxis()->GetTitle() ) ; }

      c_max *= stretch_max;
      c_min *= stretch_min;

      hs_nostack_a->SetMinimum(c_min);
      hs_nostack_a->SetMaximum(c_max);
    }

    if(var_isLog && c_min <= 1.e-10){ c_min = 1.e-10; }
    if(leg_yield){
      ResizeLegend(*leg_yield, 0.89, 0.89);
      ResizeLegend(*leg_a, leg_yield->GetX1NDC(), leg_yield->GetY2NDC() );
    }
    else{ ResizeLegend(*leg_a, 0.89, 0.89 );}
    leg_a->DrawClone();
    if(leg_yield){ leg_yield->DrawClone(); }

    curpad->RedrawAxis();
    curpad->Update();
    curpad->Modified();

    if(drawRes){
      canv_a->cd(2);

      //double r_min = 0.9*hs_res_a->GetHistogram()->GetMinimum();
      //double r_max = 1.1*hs_res_a->GetHistogram()->GetMaximum();

      double r_min = 0.; double r_max = 0.;
      if(var_draw_res == "RESIDUAL"){ r_min = -1.; r_max = 1.;}
      else if(var_draw_res == "FRACRES"){ r_min = -1.; r_max = 1.;}
      else if(var_draw_res == "RATIO"){ r_min = 0.1; r_max = 1.9;}
      else if(var_draw_res == "INVRATIO"){ r_min = 0.1; r_max = 1.9;}
 
      hs_res_a->Draw("nostack");

      double nbinx = hs_res_a->GetHistogram()->GetNbinsX();
      double rxmin = hs_res_a->GetHistogram()->GetXaxis()->GetBinLowEdge(1);
      double rxmax = hs_res_a->GetHistogram()->GetXaxis()->GetBinUpEdge(nbinx);
      double ry = ( (var_draw_res == "RESIDUAL") || (var_draw_res == "FRACRES") ) ? 0. : 1.;
      TLine* lnref = new TLine(rxmin, ry, rxmax, ry); // a reference line

      if(var_label != ""){ hs_res_a->GetHistogram()->GetXaxis()->SetTitle(var_label.c_str()); }
      else{ hs_res_a->GetHistogram()->GetXaxis()->SetTitle( ((TH1D*)(hs_res_a->GetStack()->First()))->GetXaxis()->GetTitle() ) ; }


      if(var_reslabel != ""){ hs_res_a->GetHistogram()->GetYaxis()->SetTitle(var_reslabel.c_str()); }
      hs_res_a->SetMinimum(r_min);
      hs_res_a->SetMaximum(r_max);
      lnref->DrawClone("same");

      hs_res_a->GetHistogram()->GetXaxis()->SetTitleOffset(0.9);
      hs_res_a->GetHistogram()->GetYaxis()->SetTitleOffset(0.5);
      
      hs_res_a->GetHistogram()->GetXaxis()->SetTitleSize(0.09);
      hs_res_a->GetHistogram()->GetYaxis()->SetTitleSize(0.07);
 
      hs_res_a->GetHistogram()->GetXaxis()->SetLabelSize(0.09);
      hs_res_a->GetHistogram()->GetYaxis()->SetLabelSize(0.07);
      
      hs_res_a->GetHistogram()->GetYaxis()->CenterTitle();

      canv_a->cd(2)->Update();
      canv_a->cd(2)->Modified();
      delete lnref;
    }

    //Write to output file/ print to a png
    if(m_opt->OutputFormat().find("PNG") != std::string::npos){ canv_a->SaveAs(Form("%s%s.png" ,m_opt->OutputFolder().c_str() ,canv_name.c_str())); }
    if(m_opt->OutputFormat().find("EPS") != std::string::npos){ canv_a->SaveAs(Form("%s%s.eps" ,m_opt->OutputFolder().c_str() ,canv_name.c_str())); }
    if(m_opt->OutputFormat().find("PDF") != std::string::npos){ canv_a->SaveAs(Form("%s%s.pdf" ,m_opt->OutputFolder().c_str() ,canv_name.c_str())); }

    if(m_opt->OutputFormat().find("ROOT") != std::string::npos){ m_outfile->cd(); canv_a->Write(); }


    //Delete objects
    delete canv_a;
    delete hs_stack_a;
    delete hs_nostack_a;
    delete hs_res_a;
    //delete leg_a;
    //delete leg_yield;
    //Clear strings
    //var_name.clear();
    //var_label.clear();

    canv_name.clear();
    hs_stack_name.clear();
    hs_nostack_name.clear();
    hs_res_name.clear();
    hname_sum.clear();
    hbasename.clear();
  }//variables loop

  if(m_opt->MsgLevel() == Debug::DEBUG) std::cout<<"PlotUtils::OverlayHists end"<<std::endl; 

  return;

}

TH1D* PlotUtils::makeResidual(const std::string& resname, const std::string& s_hnum, const std::string& s_href, const std::string& opt){

  return makeResidual(resname, m_hstMngr->GetTH1D(s_hnum), m_hstMngr->GetTH1D(s_href), opt);

}

TH1D* PlotUtils::makeResidual(const std::string& resname, TH1D* hist, TH1D* href, const std::string& opt){

  TH1D* hist_res = (TH1D*)(hist->Clone(resname.c_str()) );

  hist_res->SetStats(0);
  hist_res->SetName(resname.c_str());
  hist_res->SetTitle("");
  if(opt == "RATIO"){ hist_res->Divide(href, hist_res); }
  else if(opt == "INVRATIO"){ hist_res->Divide(hist_res, href); }
  else if(opt == "RESIDUAL"){ hist_res->Add(href, -1);}
  else{ std::cout<<"Unknown option for residual calculation : "<<opt<<std::endl;}
  m_hstMngr->SetTH1D(resname, hist_res);
  return hist_res;

}

int PlotUtils::SetStyleCanvas(TCanvas& canv, bool divide){

  if(divide){canv.Divide(1,2);
    canv.cd(1)->SetPad(0.,0.35,0.95,0.95);
    canv.cd(2)->SetPad(0.,0.,0.95,0.35);
    canv.cd(1)->SetBottomMargin(0.0001);
    canv.cd(2)->SetTopMargin(0.0001);
    canv.cd(2)->SetBottomMargin(0.2);
    canv.cd(1)->SetBorderMode(0);
    canv.cd(2)->SetBorderMode(0);
  }

  return 0;
}

int PlotUtils::SetStyleHist(std::string hname, std::string style_key){

  TH1D* hist = m_hstMngr->GetTH1D(hname);
  if(hist == NULL){ std::cout<<"SetStyleHist : Histogram "<<hname<<" not found"<<std::endl; return -1;}
  hist->SetLineColor( m_style_dict->LineColour(style_key) );
  hist->SetLineStyle( m_style_dict->LineStyle(style_key) );
  hist->SetLineWidth( m_style_dict->LineWidth(style_key) );
  hist->SetFillColor( m_style_dict->FillColour(style_key) );
  hist->SetFillStyle( m_style_dict->FillStyle(style_key) );

  return 0;
}

int PlotUtils::SetStyleLegend(TLegend &leg, double textsize, int textfont, double margin){
  leg.SetFillColor(0);
  leg.SetLineColor(0);
  leg.SetMargin(margin);
  leg.SetTextSize(textsize);
  leg.SetTextFont(textfont);
  leg.SetShadowColor(0);

  return 0;
}

int PlotUtils::ResizeLegend(TLegend& leg, double xpt, double ypt, const std::string& justify){

  float textsize = leg.GetTextSize();
  float nrows = (float)leg.GetNRows();
  float margin = leg.GetMargin();
  TIter liter(leg.GetListOfPrimitives());
  float maxlsize = 0.;
  int nrow = 0;
  TCanvas* canv_test = new TCanvas("canv_test", "canv_test", 800, 800); canv_test->cd();

  while( TLegendEntry* lentry = (TLegendEntry*)( liter() ) ){
    TLatex entrytex(0., 0., lentry->GetLabel());
    entrytex.SetNDC(kTRUE);
    entrytex.SetTextSize(textsize);
    entrytex.Draw();
    if( entrytex.GetXsize() > maxlsize ){maxlsize = entrytex.GetXsize();}
    nrow++;
  }
  delete canv_test;

  float X1 = 0., X2 = 0., Y1 = 0., Y2 = 0.;
  float delX = maxlsize*(1.+margin+0.1);//*textsize;
  float delY = nrows * textsize;

  if(justify == "r"){
    X2 = (xpt > 0.) ? xpt : 0.89; 
    Y2 = (ypt > 0.) ? ypt : 0.89;
    X1 = X2 - delX;
    Y1 = Y2 - delY;
  }
  else if(justify == "l"){
    X1 = (xpt > 0.) ? xpt : 0.01; 
    Y1 = (ypt > 0.) ? ypt : 0.01;
    X2 = X1 + delX; 
    Y2 = Y1 + delY;
  }
  else{std::cout<<"Error : UNknown option for legend justification"<<std::endl;}

  leg.SetX1NDC(X1);
  leg.SetX2NDC(X2);
  leg.SetY1NDC(Y1);
  leg.SetY2NDC(Y2);

  return 0;
}
