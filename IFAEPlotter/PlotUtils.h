#ifndef PLOTUTILS_H
#define PLOTUTILS_H

#include <vector>
#include <map>
#include <string>
#include "IFAEPlotter/SampleAttributes.h"
#include "IFAEPlotter/VariableAttributes.h"

class Plotter_Options;
class HistManager;
class StyleDictionary;
class TLegend;
class TCanvas;
//class TPad;
class TH1D;
class TFile;

class PlotUtils{

 public:
  PlotUtils(Plotter_Options* opt, HistManager* hstMngr, SampleAttributesMap& attrbt_map, VariableAttributesMap& var_map
	    , StyleDictionary* style_dict );
  ~PlotUtils();

  void OverlayHists(const std::string& projopt);

  int SetStyleCanvas(TCanvas& canv, bool divide, double bottommargin=0.2, double topmargin=0.05, double leftmargin=0.15, double rightmargin=0.05);
  int SetStyleHist(std::string hname, std::string key_style);
  int SetStyleLegend(TLegend& leg, double textsize=0.03, int textfont=42, double margin=0.3);
  int ResizeLegend(TLegend& leg, double xpt=-1., double ypt=0.89, const std::string& justify="r");
  double GetSeparation(TH1D S1, TH1D B1);
  TH1D* makeResidual(const std::string& resname, TH1D* hist, TH1D* href, const std::string& opt="RATIO", const std::string& erropt="");
  TH1D* makeResidual(const std::string& resname, const std::string& s_hnum, const std::string& s_href
		     , const std::string& opt="RATIO", const std::string& erropt="");  
  void Terminate();

  //void SetStyleHist(std::string hname, int i_linecolour, int i_fillcolour, int i_linestyle, int i_fillstyle);

 private:
  bool m_drawSum;
  bool m_drawBlinder;
  Plotter_Options* m_opt; //not owned
  HistManager* m_hstMngr; //not owned
  SampleAttributesMap m_attrbt_map; //not owned
  VariableAttributesMap m_var_map; //not owned 
  VariableAttributesMap m_var_map_proj; //owned 
  StyleDictionary* m_style_dict; //not owned
  TFile* m_outfile; //only if ROOT is specified in output format
  std::string m_output_dir;
  //std::vector<std::string> m_dist_list;
};


#endif
