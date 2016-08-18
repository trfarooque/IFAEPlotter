#ifndef VARIABLEATTRIBUTES_H
#define VARIABLEATTRIBUTES_H

#include <string>
#include <map>
#include <vector>

class Plotter_Options; 
class VariableAttributes{
 public:
  VariableAttributes();
  VariableAttributes(const std::string& name, const std::string& label, const std::string& do_scale
		     , bool do_width=false, bool draw_stack=false, const std::string& draw_res="", const std::string& draw_res_err=""
		     , bool isLogY=false, bool isLogX=false
		     , const std::string& ylabel="", const std::string& reslabel=""
		     , bool has_resmin=false, bool has_resmax=false, double resmin=0.5, double resmax=1.5
		     , bool has_ymin=false, bool has_ymax=false, bool has_yscale=false
		     , double ymin=0., double ymax=0., double yscale=0. 
		     , bool has_xmin=false, bool has_xmax=false, double xmin=0., double xmax=0.

		     , bool has_ttl_xmin=false, bool has_ttl_xmax=false, double ttl_xmin=0., double ttl_xmax=0.
		     , bool has_ttl_ymin=false, bool has_ttl_ymax=false, double ttl_ymin=0., double ttl_ymax=0.   
		     , bool has_ttl_textsize=false, double ttl_textsize=0. 
		     , bool has_leg_textsize=false, double leg_textsize=0. 

		     , bool has_leg_ymin=false, bool has_leg_ymax=false
		     , double leg_ymin=0., double leg_ymax=0. 
		     , bool has_leg_xmin=false, bool has_leg_xmax=false
		     , double leg_xmin=0., double leg_xmax=0. 

		     , bool has_xtitle_size=false, bool has_xtitle_offset=false
		     , double xtitle_size=0., double xtitle_offset=0.
		     , bool has_ytitle_size=false, bool has_ytitle_offset=false
		     , double ytitle_size=0., double ytitle_offset=0.
		     , bool has_restitle_size=false, bool has_restitle_offset=false
		     , double restitle_size=0., double restitle_offset=0.

		     , bool has_xlabel_size=false, bool has_xlabel_offset=false
		     , double xlabel_size=0., double xlabel_offset=0.
		     , bool has_ylabel_size=false, bool has_ylabel_offset=false
		     , double ylabel_size=0., double ylabel_offset=0.
		     , bool has_reslabel_size=false, bool has_reslabel_offset=false
		     , double reslabel_size=0., double reslabel_offset=0.

		     , bool has_xaxis_ndiv=false, int xaxis_ndiv=0
		     , bool has_yaxis_ndiv=false, int yaxis_ndiv=0
		     , bool has_resaxis_ndiv=false, int resaxis_ndiv=0

		     , bool has_bottom_margin=false, bool has_top_margin=false
		     , bool has_left_margin=false, bool has_right_margin=false
		     , double bottom_margin=0., double top_margin=0.
		     , double left_margin=0., double right_margin=0.

		     , bool is_count=false 
		     , const std::string& resdrawopt="", const std::string& extralabel=""
		     , int rebin=0, const std::string& rebinedges="", const std::string& output_folder="", const std::string& blinding="");
  VariableAttributes(VariableAttributes& q);

  ~VariableAttributes(){ }

  static std::map<std::string, VariableAttributes*> ParseVariableConfig( Plotter_Options* opt );

  void SetName(const std::string& name){ m_name = name; }
  void SetLabel(const std::string& label){ m_label = label; }
  void SetYLabel(const std::string& ylabel){ m_ylabel = ylabel; }
  void SetResLabel(const std::string& reslabel){ m_reslabel = reslabel; }
  void SetIsLogY(bool isLogY){ m_is_logY = isLogY; }
  void SetIsLogX(bool isLogX){ m_is_logX = isLogX; }
  void SetRebin(int rebin){ m_rebin = rebin; }
  void SetRebinEdges(const std::string& rebinedges){ m_rebinedges = rebinedges; }
  void SetDoScale(const std::string& do_scale){ m_do_scale = do_scale; }
  void SetDrawStack(bool draw_stack){ m_draw_stack = draw_stack; }
  void SetDrawRes(const std::string& draw_res){ m_draw_res = draw_res; }
  void SetDrawResErr(const std::string& draw_res_err){ m_draw_res_err = draw_res_err; }
  void SetDoWidth(bool do_width){ m_do_width = do_width; }
  void SetResDrawOpt(const std::string& resdrawopt){ m_resdrawopt = resdrawopt; }
  void SetBlinding(const std::string& blinding){ m_blinding = blinding; }
  void SetResMin(double resmin){ m_resmin = resmin; }
  void SetResMax(double resmax){ m_resmax = resmax; }
  void SetHasResMin(bool has_resmin){ m_has_resmin = has_resmin; }
  void SetHasResMax(bool has_resmax){ m_has_resmax = has_resmax; }
  void SetYMin(double ymin){ m_ymin = ymin; }
  void SetYMax(double ymax){ m_ymax = ymax; }
  void SetYScale(double yscale){ m_yscale = yscale; }
  void SetHasYMin(bool has_ymin){ m_has_ymin = has_ymin; }
  void SetHasYMax(bool has_ymax){ m_has_ymax = has_ymax; }
  void SetHasYScale(bool has_yscale){ m_has_yscale = has_yscale; }
  void SetXMin(double xmin){ m_xmin = xmin; }
  void SetXMax(double xmax){ m_xmax = xmax; }
  void SetHasXMin(bool has_xmin){ m_has_xmin = has_xmin; }
  void SetHasXMax(bool has_xmax){ m_has_xmax = has_xmax; }
  void SetTitleXMin(double ttl_xmin){ m_ttl_xmin = ttl_xmin; }
  void SetTitleYMin(double ttl_ymin){ m_ttl_ymin = ttl_ymin; }
  void SetTitleXMax(double ttl_xmax){ m_ttl_xmax = ttl_xmax; }
  void SetTitleYMax(double ttl_ymax){ m_ttl_ymax = ttl_ymax; }
  void SetTitleTextSize(double ttl_textsize){ m_ttl_textsize = ttl_textsize; }
  void SetLegendTextSize(double leg_textsize){ m_leg_textsize = leg_textsize; }
  void SetHasTitleXMin(bool has_ttl_xmin){ m_has_ttl_xmin = has_ttl_xmin; }
  void SetHasTitleYMin(bool has_ttl_ymin){ m_has_ttl_ymin = has_ttl_ymin; }
  void SetHasTitleXMax(bool has_ttl_xmax){ m_has_ttl_xmax = has_ttl_xmax; }
  void SetHasTitleYMax(bool has_ttl_ymax){ m_has_ttl_ymax = has_ttl_ymax; }
  void SetHasTitleTextSize(bool has_ttl_textsize){ m_has_ttl_textsize = has_ttl_textsize; }
  void SetHasLegendTextSize(bool has_leg_textsize){ m_has_leg_textsize = has_leg_textsize; }

  void SetHasLegendXMin(bool has_leg_xmin){ m_has_leg_xmin = has_leg_xmin; }
  void SetHasLegendXMax(bool has_leg_xmax){ m_has_leg_xmax = has_leg_xmax; }
  void SetHasLegendYMin(bool has_leg_ymin){ m_has_leg_ymin = has_leg_ymin; }
  void SetHasLegendYMax(bool has_leg_ymax){ m_has_leg_ymax = has_leg_ymax; }

  void SetLegendXMin(double leg_xmin){ m_leg_xmin = leg_xmin; }
  void SetLegendXMax(double leg_xmax){ m_leg_xmax = leg_xmax; }
  void SetLegendYMin(double leg_ymin){ m_leg_ymin = leg_ymin; }
  void SetLegendYMax(double leg_ymax){ m_leg_ymax = leg_ymax; }

  void SetHasXTitleOffset(bool has_xtitle_offset){ m_has_xtitle_offset = has_xtitle_offset; }
  void SetHasXTitleSize(bool has_xtitle_size){ m_has_xtitle_size = has_xtitle_size; }
  void SetHasYTitleOffset(bool has_ytitle_offset){ m_has_ytitle_offset = has_ytitle_offset; }
  void SetHasYTitleSize(bool has_ytitle_size){ m_has_ytitle_size = has_ytitle_size; }
  void SetHasResTitleOffset(bool has_restitle_offset){ m_has_restitle_offset = has_restitle_offset; }
  void SetHasResTitleSize(bool has_restitle_size){ m_has_restitle_size = has_restitle_size; }

  void SetXTitleOffset(double xtitle_offset){ m_xtitle_offset = xtitle_offset; }
  void SetXTitleSize(double xtitle_size){ m_xtitle_size = xtitle_size; }
  void SetYTitleOffset(double ytitle_offset){ m_ytitle_offset = ytitle_offset; }
  void SetYTitleSize(double ytitle_size){ m_ytitle_size = ytitle_size; }
  void SetResTitleOffset(double restitle_offset){ m_restitle_offset = restitle_offset; }
  void SetResTitleSize(double restitle_size){ m_restitle_size = restitle_size; }

  void SetHasXLabelOffset(bool has_xlabel_offset){ m_has_xlabel_offset = has_xlabel_offset; }
  void SetHasXLabelSize(bool has_xlabel_size){ m_has_xlabel_size = has_xlabel_size; }
  void SetHasYLabelOffset(bool has_ylabel_offset){ m_has_ylabel_offset = has_ylabel_offset; }
  void SetHasYLabelSize(bool has_ylabel_size){ m_has_ylabel_size = has_ylabel_size; }
  void SetHasResLabelOffset(bool has_reslabel_offset){ m_has_reslabel_offset = has_reslabel_offset; }
  void SetHasResLabelSize(bool has_reslabel_size){ m_has_reslabel_size = has_reslabel_size; }

  void SetXLabelOffset(double xlabel_offset){ m_xlabel_offset = xlabel_offset; }
  void SetXLabelSize(double xlabel_size){ m_xlabel_size = xlabel_size; }
  void SetYLabelOffset(double ylabel_offset){ m_ylabel_offset = ylabel_offset; }
  void SetYLabelSize(double ylabel_size){ m_ylabel_size = ylabel_size; }
  void SetResLabelOffset(double reslabel_offset){ m_reslabel_offset = reslabel_offset; }
  void SetResLabelSize(double reslabel_size){ m_reslabel_size = reslabel_size; }

  void SetHasXAxisNdiv(bool has_xaxis_ndiv){ m_has_xaxis_ndiv = has_xaxis_ndiv; }
  void SetHasYAxisNdiv(bool has_yaxis_ndiv){ m_has_yaxis_ndiv = has_yaxis_ndiv; }
  void SetHasResAxisNdiv(bool has_resaxis_ndiv){ m_has_resaxis_ndiv = has_resaxis_ndiv; }

  void SetXAxisNdiv(int xaxis_ndiv){ m_xaxis_ndiv = xaxis_ndiv; }
  void SetYAxisNdiv(int yaxis_ndiv){ m_yaxis_ndiv = yaxis_ndiv; }
  void SetResAxisNdiv(int resaxis_ndiv){ m_resaxis_ndiv = resaxis_ndiv; }

  void SetHasBottomMargin(bool has_bottom_margin){ m_has_bottom_margin = has_bottom_margin; }
  void SetHasTopMargin(bool has_top_margin){ m_has_top_margin = has_top_margin; }
  void SetHasLeftMargin(bool has_left_margin){ m_has_left_margin = has_left_margin; }
  void SetHasRightMargin(bool has_right_margin){ m_has_right_margin = has_right_margin; }

  void SetBottomMargin(double bottom_margin){ m_bottom_margin = bottom_margin; }
  void SetTopMargin(double top_margin){ m_top_margin = top_margin; }
  void SetLeftMargin(double left_margin){ m_left_margin = left_margin; }
  void SetRightMargin(double right_margin){ m_right_margin = right_margin; }

  void SetIsCount(bool is_count){ m_is_count = is_count; }
  void SetNProjBin(int nprojbin){ m_nprojbin = nprojbin; }
  void SetExtraLabel(const std::string& extralabel){ m_extralabel = extralabel; }
  void SetOutputFolder(const std::string& output_folder){ m_output_folder = output_folder; }

  const std::string& Name() const { return m_name; }
  const std::string& Label() const { return m_label; }
  const std::string& YLabel() const { return m_ylabel; }
  const std::string& ResLabel() const { return m_reslabel; }
  const std::string& DoScale() const { return m_do_scale; }
  bool DrawStack() const { return m_draw_stack; }
  const std::string& DrawRes() const { return m_draw_res; }
  const std::string& DrawResErr() const { return m_draw_res_err; }
  bool IsLogY() const { return m_is_logY; }
  bool IsLogX() const { return m_is_logX; }
  int Rebin() const { return m_rebin; }
  const std::string& RebinEdges() const { return m_rebinedges; }
  bool DoWidth() const { return m_do_width; }
  double ResMin() const { return m_resmin; }
  double ResMax() const { return m_resmax; }
  bool HasResMin() const { return m_has_resmin; }
  bool HasResMax() const { return m_has_resmax; }
  double YMin() const { return m_ymin; }
  double YMax() const { return m_ymax; }
  double YScale() const { return m_yscale; }
  bool HasYMin() const { return m_has_ymin; }
  bool HasYMax() const { return m_has_ymax; }
  bool HasYScale() const { return m_has_yscale; }
  double XMin() const { return m_xmin; }
  double XMax() const { return m_xmax; }
  bool HasXMin() const { return m_has_xmin; }
  bool HasXMax() const { return m_has_xmax; }
  double TitleXMin() const { return m_ttl_xmin; }
  double TitleYMin() const { return m_ttl_ymin; }
  double TitleXMax() const { return m_ttl_xmax; }
  double TitleYMax() const { return m_ttl_ymax; }
  double TitleTextSize() const { return m_ttl_textsize; }
  double LegendTextSize() const { return m_leg_textsize; }
  bool HasTitleXMin() const { return m_has_ttl_xmin; }
  bool HasTitleYMin() const { return m_has_ttl_ymin; }
  bool HasTitleXMax() const { return m_has_ttl_xmax; }
  bool HasTitleYMax() const { return m_has_ttl_ymax; }
  bool HasTitleTextSize() const { return m_has_ttl_textsize; }
  bool HasLegendTextSize() const { return m_has_leg_textsize; }

  bool HasLegendXMin() const { return m_has_leg_xmin; }
  bool HasLegendXMax() const { return m_has_leg_xmax; }
  bool HasLegendYMin() const { return m_has_leg_ymin; }
  bool HasLegendYMax() const { return m_has_leg_ymax; }

  double LegendXMin() const { return m_leg_xmin; }
  double LegendXMax() const { return m_leg_xmax; }
  double LegendYMin() const { return m_leg_ymin; }
  double LegendYMax() const { return m_leg_ymax; }

  bool HasXTitleSize() const { return m_has_xtitle_size; }
  bool HasXTitleOffset() const { return m_has_xtitle_offset; }
  bool HasYTitleSize() const { return m_has_ytitle_size; }
  bool HasYTitleOffset() const { return m_has_ytitle_offset; }
  bool HasResTitleSize() const { return m_has_restitle_size; }
  bool HasResTitleOffset() const { return m_has_restitle_offset; }

  double XTitleSize() const { return m_xtitle_size; }
  double XTitleOffset() const { return m_xtitle_offset; }
  double YTitleSize() const { return m_ytitle_size; }
  double YTitleOffset() const { return m_ytitle_offset; }
  double ResTitleSize() const { return m_restitle_size; }
  double ResTitleOffset() const { return m_restitle_offset; }

  bool HasXLabelSize() const { return m_has_xlabel_size; }
  bool HasXLabelOffset() const { return m_has_xlabel_offset; }
  bool HasYLabelSize() const { return m_has_ylabel_size; }
  bool HasYLabelOffset() const { return m_has_ylabel_offset; }
  bool HasResLabelSize() const { return m_has_reslabel_size; }
  bool HasResLabelOffset() const { return m_has_reslabel_offset; }

  double XLabelSize() const { return m_xlabel_size; }
  double XLabelOffset() const { return m_xlabel_offset; }
  double YLabelSize() const { return m_ylabel_size; }
  double YLabelOffset() const { return m_ylabel_offset; }
  double ResLabelSize() const { return m_reslabel_size; }
  double ResLabelOffset() const { return m_reslabel_offset; }

  bool HasXAxisNdiv() const { return m_has_xaxis_ndiv; }
  bool HasYAxisNdiv() const { return m_has_yaxis_ndiv; }
  bool HasResAxisNdiv() const { return m_has_resaxis_ndiv; }

  int XAxisNdiv() const { return m_xaxis_ndiv; }
  int YAxisNdiv() const { return m_yaxis_ndiv; }
  int ResAxisNdiv() const { return m_resaxis_ndiv; }


  bool HasBottomMargin() const { return m_has_bottom_margin; }
  bool HasTopMargin() const { return m_has_top_margin; }
  bool HasLeftMargin() const { return m_has_left_margin; }
  bool HasRightMargin() const { return m_has_right_margin; }

  double BottomMargin() const { return m_bottom_margin; }
  double TopMargin() const { return m_top_margin; }
  double LeftMargin() const { return m_left_margin; }
  double RightMargin() const { return m_right_margin; }

  bool IsCount() const { return m_is_count; }

  int NProjBin() const { return m_nprojbin; }
  const std::string& ResDrawOpt() const { return m_resdrawopt; }
  const std::string& Blinding() const { return m_blinding; }
  const std::string& ExtraLabel() const { return m_extralabel; }
  const std::string& OutputFolder() const { return m_output_folder; }

 private:  

  std::string m_name;
  std::string m_label;
  std::string m_ylabel;
  std::string m_reslabel;
  int m_draw_stack;
  std::string m_do_scale;
  std::string m_draw_res;
  std::string m_draw_res_err;
  bool m_is_logY;
  bool m_is_logX;
  int m_rebin;
  std::string m_rebinedges;
  bool m_do_width;
  double m_resmin;
  double m_resmax; 
  bool m_has_resmin;
  bool m_has_resmax;
  double m_ymin;
  double m_ymax; 
  double m_yscale; 
  bool m_has_ymin;
  bool m_has_ymax;
  bool m_has_yscale;
  double m_xmin;
  double m_xmax; 
  bool m_has_xmin;
  bool m_has_xmax;
  double m_ttl_xmin;
  double m_ttl_ymin;
  double m_ttl_xmax; 
  double m_ttl_ymax; 
  double m_ttl_textsize; 
  double m_leg_textsize; 
  bool m_has_ttl_xmin;
  bool m_has_ttl_ymin;
  bool m_has_ttl_xmax;
  bool m_has_ttl_ymax;
  bool m_has_ttl_textsize;
  bool m_has_leg_textsize;

  bool m_has_leg_xmin;
  bool m_has_leg_xmax;
  bool m_has_leg_ymin;
  bool m_has_leg_ymax;

  double m_leg_xmin;
  double m_leg_xmax;
  double m_leg_ymin;
  double m_leg_ymax;

  bool m_has_xtitle_size;
  bool m_has_xtitle_offset;
  bool m_has_ytitle_size;
  bool m_has_ytitle_offset;
  bool m_has_restitle_size;
  bool m_has_restitle_offset;

  double m_xtitle_size;
  double m_xtitle_offset;
  double m_ytitle_size;
  double m_ytitle_offset;
  double m_restitle_size;
  double m_restitle_offset;

  bool m_has_xlabel_size;
  bool m_has_xlabel_offset;
  bool m_has_ylabel_size;
  bool m_has_ylabel_offset;
  bool m_has_reslabel_size;
  bool m_has_reslabel_offset;

  double m_xlabel_size;
  double m_xlabel_offset;
  double m_ylabel_size;
  double m_ylabel_offset;
  double m_reslabel_size;
  double m_reslabel_offset;

  bool m_has_xaxis_ndiv;
  bool m_has_yaxis_ndiv;
  bool m_has_resaxis_ndiv;

  int m_xaxis_ndiv;
  int m_yaxis_ndiv;
  int m_resaxis_ndiv;

  bool m_has_bottom_margin;
  bool m_has_top_margin;
  bool m_has_left_margin;
  bool m_has_right_margin;

  double m_bottom_margin;
  double m_top_margin;
  double m_left_margin;
  double m_right_margin;

  bool m_is_count;

  int m_nprojbin;
  std::string m_resdrawopt;
  std::string m_blinding;
  std::string m_extralabel;
  std::string m_output_folder;
};

typedef std::map<std::string, VariableAttributes*> VariableAttributesMap;
typedef std::vector<VariableAttributes*> VariableAttributesVector;
 

#endif
