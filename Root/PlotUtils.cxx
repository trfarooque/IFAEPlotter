#include "IFAEPlotter/PlotUtils.h"
#include "THStack.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TLegend.h"
#include "TLegendEntry.h"
//#include "TIter.h"
#include "TLatex.h"
#include "IFAETopFramework/HistManager.h"
#include "IFAEPlotter/StyleDictionary.h"

PlotUtils::PlotUtils(HistManager* hstMngr, SampleAttributesMap& attrbt_map, VariableAttributesMap& var_map, StyleDictionary* style_dict ) : 
  m_attrbt_map(attrbt_map),
  m_var_map(var_map),
  m_style_dict(style_dict){ 

  m_drawSum = (m_attrbt_map.find("SUM") != m_attrbt_map.end());
  //m_hstMngr = new HistManager(*hstMngr);
  m_hstMngr = hstMngr;
}


void PlotUtils::OverlayHists(){

  int nsample = m_attrbt_map.size();
  int ndist = m_var_map.size();

  bool var_draw_stack = 0;
  bool var_isLog = false;
  
  bool ds_draw_stack = false;
  bool ds_is_baseline = false;

  //One preliminary loop to find the baseline sample
  std::string s_base_name = ""; 
  std::string s_base_suffix = "";
  for(SampleAttributesMap::iterator at_it = m_attrbt_map.begin(); at_it != m_attrbt_map.end(); ++at_it){
    if(at_it->second->IsBaseline()){
      if(s_base_name != ""){ std::cout<<"Error: More than one baseline specified for residual calculation"<<std::endl; }
      else{ 
	s_base_name = at_it->first;
	s_base_suffix = at_it->second->Suffix();
      }
    }
  } 

  for(VariableAttributesMap::iterator va_it = m_var_map.begin(); va_it != m_var_map.end(); ++va_it){

    const std::string& var_name = va_it->second->Name();
    const std::string& var_label = va_it->second->Label();
    const std::string& var_draw_res = va_it->second->DrawRes();
    bool var_isShape = (va_it->second->DoScale() == "SHAPE"); 
    var_draw_stack = va_it->second->DrawStack();
    var_isLog = va_it->second->IsLog();

    std::string hname_sum = ( !var_isShape && m_drawSum ) ? var_name + "_" + m_attrbt_map["SUM"]->Suffix() : "";

    bool drawRes = (var_draw_res != "");
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
    TLegend* leg_yield = var_isShape ? NULL : new TLegend();
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

      if(var_isShape && at_it->first == "SUM"){continue;}

      const std::string& ds_name = at_it->second->Name();
      const std::string& ds_suffix = at_it->second->Suffix();
      const std::string& ds_leglabel = at_it->second->LegLabel();
      const std::string& ds_stylekey = at_it->second->StyleKey();
      const std::string& ds_drawopt = at_it->second->DrawOpt();
      bool ds_isShape = (at_it->second->DrawScale() == "SHAPE"); 
      ds_draw_stack = at_it->second->DrawStack();
      ds_is_baseline = at_it->second->IsBaseline();

      std::string hist_name = var_name + "_" + ds_suffix;
      SetStyleHist(hist_name, ds_stylekey);
      TH1D* hist_a = m_hstMngr->GetTH1D(hist_name);
      std::cout<<" ds_name = "<<ds_name<<" s_base_name = "<<s_base_name<<" isBaseline = "<<ds_is_baseline
	       <<" BaselineComp = "<<(ds_name == s_base_name)<<" h_base_name = "<<h_base->GetName()<<" h_base = "<<h_base
	       <<" hist_name = "<<hist_name<<" hist_intgl = "<<hist_a->Integral()<<std::endl;

      if(var_draw_stack && ds_draw_stack){ hs_stack_a->Add(hist_a, "hist"); }
      else{ 
	hs_nostack_a->Add(hist_a, ds_drawopt.c_str()); 

	if(drawRes && (ds_name != s_base_name) && (ds_name != "SUM") ){
	  std::string resname_a = var_name + "_" + ds_suffix + "_res_" + s_base_suffix;
	  std::cout<<"makeResidual = "<<hist_name<<" versus "<<hbasename<<std::endl;
	  TH1D* hist_res_a = makeResidual(resname_a, hist_name, hbasename, var_draw_res);
	  hs_res_a->Add(hist_res_a);
	  resname_a.clear();
	} 

      }
      leg_a->AddEntry(hist_a, ds_leglabel.c_str(), "lpf");
      //leg_a->AddEntry(hist_a, Form("%.1f",hist_a->Integral()), "lpf");
      if(leg_yield){ 
	if(!ds_isShape){leg_yield->AddEntry(hist_a, Form("%.4g",hist_a->Integral()), ""); }
	else{ leg_yield->AddEntry(hist_a, " ", ""); }
      }
      //Clear strings
      hist_name.clear();
    }//sample loop

    if( drawRes && var_draw_stack && !var_isShape ){
      std::string resname_sum_a = var_name + "_sum_" + "_res_" + s_base_suffix;
      std::cout<<"makeResidual = "<<hname_sum<<" versus "<<hbasename<<std::endl;
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
      else{ hs_stack_a->GetXaxis()->SetTitle( ((TH1D*)(hs_stack_a->GetStack()->First()))->GetXaxis()->GetTitle() ) ; }
      hs_stack_a->GetHistogram()->GetYaxis()->SetTitle("Arbitrary Units");
      c_max *= stretch_max;
      c_min *= stretch_min;
      hs_stack_a->SetMinimum(c_min);
      hs_stack_a->SetMaximum(c_max);

    }
    else{
      hs_nostack_a->Draw("nostack"); 
      c_max = max(c_max, hs_nostack_a->GetHistogram()->GetMaximum());
      c_min = max(c_min, hs_nostack_a->GetHistogram()->GetMinimum());
      hs_nostack_a->GetHistogram()->GetXaxis()->SetTitle(var_label.c_str());
      hs_nostack_a->GetHistogram()->GetYaxis()->SetTitle("Arbitrary Units");
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
    leg_a->Draw();
    if(leg_yield){ leg_yield->Draw(); }
    curpad->Update();
    curpad->Modified();

    if(drawRes){
      canv_a->cd(2);
      //if(var_isLog){canv_a->cd(2)->SetLogy();}
      hs_res_a->Draw("nostack");

      double r_min = 0.9*hs_res_a->GetHistogram()->GetMinimum();
      double r_max = 1.1*hs_res_a->GetHistogram()->GetMaximum();

      hs_res_a->GetXaxis()->SetTitle(var_label.c_str());
      hs_res_a->GetYaxis()->SetTitle("Ratio");
      hs_res_a->SetMinimum(r_min);
      hs_res_a->SetMinimum(r_max);
      //lnref->Draw("same");

      hs_res_a->GetHistogram()->GetXaxis()->SetTitleOffset(0.9);
      hs_res_a->GetHistogram()->GetYaxis()->SetTitleOffset(0.5);
      
      hs_res_a->GetHistogram()->GetXaxis()->SetTitleSize(0.09);
      hs_res_a->GetHistogram()->GetYaxis()->SetTitleSize(0.07);

      hs_res_a->GetHistogram()->GetXaxis()->SetLabelSize(0.09);
      hs_res_a->GetHistogram()->GetYaxis()->SetLabelSize(0.07);
      
      hs_res_a->GetHistogram()->GetYaxis()->CenterTitle();

      canv_a->cd(2)->Update();
      canv_a->cd(2)->Modified();

    }

    //Write to output file/ print to a png
    canv_a->SaveAs(Form("%s.png",canv_a->GetName()));

    //Delete objects
    delete canv_a;
    delete hs_stack_a;
    delete hs_nostack_a;
    delete hs_res_a;
    delete leg_a;
    delete leg_yield;
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
  if(opt == "RATIO"){ hist_res->Divide(href); }
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

//void PlotUtils::SetStyleHist(std::string hname, int i_linecolour, int i_fillcolour, int i_linestyle, int i_fillstyle){return;}
//EDIT THIS
/*
void OverlayHists(string cname, vector<string> vh1name, vector<string> leglabels, vector<string> drawopts,
                  int draw_stack, int draw_res, string resylabels, string extralabel){
  //draw_res=0 => do not draw residual panel
  //draw_res=1 => draw ratio of each histogram to the first (baseline) one
  //draw_res=2 => draw difference of each histogram from the first (baseline) one

  //draw_stack=0 => draw THStack with "nostack"
  //draw_stack=1 => draw all histograms in a stack
  //draw_stack=2 => draw all but first histogram in a stack and then overlay the first histogram on top

  //if draw_stack>0 then draw_res should have no effect; do not draw residual panel in this case
  if(vh1name.size()==1){draw_stack=0; draw_res=0;}
  if(draw_stack>0){draw_res=0;}
  TCanvas* canv=new TCanvas(cname.c_str(), cname.c_str() ,800,800);

  if(vh1name.size()<=0){cout<<"NO HISTS"<<endl; return;}
  if(vh1name.size()!=leglabels.size()){cout<<"NO. OF LEGEND LABELS="<<leglabels.size()<<" MUST MATCH NO. OF HISTS="<<vh1name.size()<<endl;
    return;
  }
  if(vh1name.size()!=drawopts.size()){cout<<"NO. OF DRAWING OPTIONS="<<drawopts.size()<<" MUST MATCH NO. OF HISTS="<<vh1name.size()<<endl;
    return;
  }

  string ttl_draw="";
  if(extralabel == ""){ttl_draw = h1d[vh1name.at(0)]->GetTitle();}
  else{ttl_draw = extralabel.c_str();}

  const char* xttl=h1d[vh1name.at(0)]->GetXaxis()->GetTitle();
  const char* yttl=h1d[vh1name.at(0)]->GetYaxis()->GetTitle();

  THStack hs(Form("hs_%s",cname.c_str()), "");
  THStack hs_res(Form("hs_res_%s",cname.c_str()), "");

  TLegend leg(0.2,0.7,0.4,0.9);
  double textsize=0.03;
  if(draw_res>0){textsize=0.05;}
  SetStyleLegend(leg, textsize);
  leg.Clear();

  TPaveText pv_ttl(0.65,0.75,0.9,0.95, "NDC");
  pv_ttl.SetLineColor(kWhite);
  pv_ttl.SetFillColor(kWhite);
  pv_ttl.SetShadowColor(0);
  pv_ttl.SetTextSize(textsize);

  pv_ttl.AddText(ttl_draw.c_str());

  if(draw_res>0){canv->Divide(1,2);
    canv->cd(1)->SetPad(0.,0.35,0.95,0.95);
    canv->cd(2)->SetPad(0.,0.,0.95,0.35);
    canv->cd(1)->SetBottomMargin(0.0001);
    canv->cd(2)->SetTopMargin(0.0001);
    canv->cd(2)->SetBottomMargin(0.2);
    canv->cd(1)->SetBorderMode(0);
    canv->cd(2)->SetBorderMode(0);
  }
  double nbinx=  h1d[vh1name.at(0)]->GetNbinsX();
  double rxmin=h1d[vh1name.at(0)]->GetXaxis()->GetBinLowEdge(1);
  double rxmax=h1d[vh1name.at(0)]->GetXaxis()->GetBinUpEdge(nbinx);
  double ry=0.;
  if(draw_res==1){ry=1.;}
  if(draw_res==2){ry=0.;}

  TLine* lnref=new TLine(rxmin,ry,rxmax,ry);

  int k=0;
  for(vector<string>::iterator vsit=vh1name.begin(); vsit!=vh1name.end(); ++vsit){
    //leg.AddEntry(h1d[*vsit],Form("#splitline{%s: Mean=%.3g#pm%.3g}{Nentries=%.0f}",leglabels.at(k).c_str()
    //                   ,h1d[*vsit]->GetMean(),h1d[*vsit]->GetMeanError(),h1d[*vsit]->GetEntries() ) );
    //leg.AddEntry(h1d[*vsit],Form("%s: Mean=%.3g#pm%.3g",leglabels.at(k).c_str()
    //                           ,h1d[*vsit]->GetMean(),h1d[*vsit]->GetMeanError()) );
    leg.AddEntry(h1d[*vsit],leglabels.at(k).c_str());

    h1d[*vsit]->SetStats(0);
    if(draw_stack>1 && k==0){k++; cout<<"skipping "<<*vsit<<endl; continue;}
    hs.Add(h1d[*vsit],drawopts.at(k).c_str());
    if(k>0 && draw_res>0){
      string sres_h1=Form("res_%s",vsit->c_str());
      h1d[sres_h1]=(TH1D*)(h1d[*vsit]->Clone(sres_h1.c_str()));
      h1d[sres_h1]->SetStats(0);
      h1d[sres_h1]->SetName(sres_h1.c_str());
      h1d[sres_h1]->SetTitle("");

      if(draw_res==1){h1d[sres_h1]->Divide(h1d[vh1name.at(0)]);}
      else if(draw_res==2){h1d[sres_h1]->Add(h1d[vh1name.at(0)],-1);}
      else{cout<<"UNKNOWN VALUE FOR draw_res="<<draw_res<<endl; break;}
      hs_res.Add(h1d[sres_h1],"sameshist");
    }

    if(draw_stack>2 || draw_stack<0){cout<<"UNKNOWN VALUE FOR draw_stack="<<draw_stack<<endl; break;}
    k++;
  }

  canv->cd(1);
  if(draw_stack==0){
    hs.Draw("nostack");
  }
  else{
    hs.Draw();
  }
  //if(cname.find("_eff_")!=string::npos || cname.find("_pur_")!=string::npos){hs.GetHistogram()->SetMinimum(0.); hs.GetHistogram()->SetMaximum(1.);}
  //else{hs.GetHistogram()->SetMinimum(1.e-10);  hs.GetHistogram()->SetMaximum(hs.GetHistogram()->GetMaximum()*1.5);}
  //if(cname.find("_EFF_")!=string::npos || cname.find("_PUR_")!=string::npos){hs.SetMinimum(0.); hs.SetMaximum(1.1);}
  //else{
  hs.SetMinimum(1.e-10);  hs.SetMaximum(hs.GetHistogram()->GetMaximum()*1.5);
    //}
  hs.GetHistogram()->GetXaxis()->SetTitle(xttl);
  hs.GetHistogram()->GetYaxis()->SetTitle(yttl);
  hs.GetHistogram()->SetTitle(hs.GetTitle());
  if(draw_stack>1){cout<<"Overlay "<<vh1name.at(0)<<endl; h1d[vh1name.at(0)]->Draw(Form("sames%s",drawopts.at(0).c_str()));}
  canv->cd(1)->Update();
  canv->cd(1)->Modified();
  leg.Draw();
  pv_ttl.Draw();
  if(s_label!=""){
    ATLASLabel(0.6,0.7,s_label.c_str());
  }
  if(draw_res>0){
    canv->cd(2);
    hs_res.Draw("nostack");
    hs_res.GetXaxis()->SetTitle(xttl);
    hs_res.GetYaxis()->SetTitle(resylabels.c_str());
    hs_res.GetYaxis()->SetRangeUser(1.e-10, hs_res.GetHistogram()->GetMaximum()*1.5);
    lnref->Draw("same");

    hs_res.GetXaxis()->SetTitleOffset(0.9);
    hs_res.GetYaxis()->SetTitleOffset(0.5);

    hs_res.GetXaxis()->SetTitleSize(0.09);
    hs_res.GetXaxis()->SetTitleSize(0.07);
  }

  //canv->SaveAs(Form("%s.png",canv->GetName()));
  outfile->cd(); canv->Write(); delete canv;
  delete lnref;
  ttl_draw.clear();
  return;
}

*/
