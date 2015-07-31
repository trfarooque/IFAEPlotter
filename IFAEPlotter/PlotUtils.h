#ifndef PLOTUTILS_H
#define PLOTUTILS_H

#include <vector>
#include <string>
#include "IFAEPlotter/SampleAttributes.h"
#include "IFAEPlotter/VariableAttributes.h"

class HistManager;
class StyleDictionary;
class TLegend;
class TCanvas;
//class TPad;
class TH1D;

class PlotUtils{

 public:
  PlotUtils(HistManager* hstMngr, SampleAttributesMap& attrbt_map, VariableAttributesMap& var_map, StyleDictionary* style_dict );
  ~PlotUtils(){}

  void OverlayHists();
  int SetStyleCanvas(TCanvas& canv, bool divide);
  int SetStyleHist(std::string hname, std::string key_style);
  int SetStyleLegend(TLegend& leg, double textsize=0.03, int textfont=42, double margin=0.3);
  int ResizeLegend(TLegend& leg, double xpt=-1., double ypt=0.89, const std::string& justify="r");
  TH1D* makeResidual(const std::string& resname, TH1D* hist, TH1D* href, const std::string& opt="RATIO");
  TH1D* makeResidual(const std::string& resname, const std::string& s_hnum, const std::string& s_href, const std::string& opt="RATIO");

  //void SetStyleHist(std::string hname, int i_linecolour, int i_fillcolour, int i_linestyle, int i_fillstyle);

 private:
  bool m_drawSum;
  HistManager* m_hstMngr;
  SampleAttributesMap m_attrbt_map;
  VariableAttributesMap m_var_map; 
  StyleDictionary* m_style_dict;

  //std::vector<std::string> m_dist_list;
};


#endif
