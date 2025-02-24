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
  void MakeTableFromHists(const bool opt_bin);

  //StyleSetter
  int SetStyleCanvas(TCanvas& canv, bool divide, double bottommargin=0.2, double topmargin=0.05, double leftmargin=0.15, double rightmargin=0.05);
  int SetStyleHist(std::string hname, std::string key_style);
  int SetStyleLegend(TLegend& leg, double textsize=0.03, int textfont=42, double margin=0.3);
  int ResizeLegend(TLegend& leg, double xpt=-1., double ypt=0.89, const std::string& justify="r");

  //MomentsGetter
  TH1D* makeResidual(const std::string& resname, TH1D* hist, TH1D* href, const std::string& opt="RATIO", const std::string& erropt="");
  TH1D* makeResidual(const std::string& resname, const std::string& s_hnum, const std::string& s_href
		     , const std::string& opt="RATIO", const std::string& erropt="");

  //TextFormatter
  std::vector<std::string> ParseMomentsTableHeader(const std::string& header_line, const std::string& delim="," ); 
  std::string MakeMomentText(TH1D* hist, const std::string& moment="YIELD", const std::string& print_format="%4g");
  std::string MakeResidualMomentText(TH1D* hist, TH1D* href, const std::string& moment="YIELD", const std::string& print_format="%4g");
  std::string MakeMomentsTableRow(TH1D* hist, const std::vector<std::string>& moment_list, const std::string& print_format="%4g", const bool use_width=false);
  std::string MakeResidualMomentsTableRow(TH1D* hist, TH1D* href, const std::vector<std::string>& moment_list, const std::string& print_format="%4g", const bool /*use_width*/=false);
  std::string MakeHistTableRow(TH1D* hist, const std::string& print_format="%4g", const bool print_error=true, const std::map<int, std::string>* bin_labels = NULL);
  //std::string MakeLegendPrintText(const std::string& print_value);

  //HistTreater
  TH1D* GetHistByBinWidth(TH1D* hist);
  TH1D* GetHistTimesBinWidth(TH1D* hist);


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
