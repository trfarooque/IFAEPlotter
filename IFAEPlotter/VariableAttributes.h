#ifndef VARIABLEATTRIBUTES_H
#define VARIABLEATTRIBUTES_H

#include <string>
#include <map>
class VariableAttributes{
 public:
  VariableAttributes(const std::string& name, const std::string& label, const std::string& do_scale
		     , bool do_width=false, bool draw_stack=false, const std::string& draw_res="", const std::string& draw_res_err=""
		     , bool isLogY=false, bool isLogX=false
		     , const std::string& ylabel="", const std::string& reslabel=""
		     , bool has_resmin=false, bool has_resmax=false, double resmin=0.5, double resmax=1.5
		     , bool has_ymin=false, bool has_ymax=false, double ymin=0., double ymax=0.
		     , bool has_xmin=false, bool has_xmax=false, double xmin=0., double xmax=0.
		     , bool has_ttl_xmin=false, bool has_ttl_xmax=false, double ttl_xmin=0., double ttl_xmax=0.
		     , bool has_ttl_ymin=false, bool has_ttl_ymax=false, double ttl_ymin=0., double ttl_ymax=0.   
		     , const std::string& resdrawopt="", const std::string& extralabel="", int rebin=-1, const std::string& blinding="");
  VariableAttributes(VariableAttributes& q);

  ~VariableAttributes(){ }

  void SetName(const std::string& name){ m_name = name; }
  void SetLabel(const std::string& label){ m_label = label; }
  void SetYLabel(const std::string& ylabel){ m_ylabel = ylabel; }
  void SetResLabel(const std::string& reslabel){ m_reslabel = reslabel; }
  void SetIsLogY(bool isLogY){ m_is_logY = isLogY; }
  void SetIsLogX(bool isLogX){ m_is_logX = isLogX; }
  void SetRebin(int rebin){ m_rebin = rebin; }
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
  void SetHasYMin(bool has_ymin){ m_has_ymin = has_ymin; }
  void SetHasYMax(bool has_ymax){ m_has_ymax = has_ymax; }
  void SetXMin(double xmin){ m_xmin = xmin; }
  void SetXMax(double xmax){ m_xmax = xmax; }
  void SetHasXMin(bool has_xmin){ m_has_xmin = has_xmin; }
  void SetHasXMax(bool has_xmax){ m_has_xmax = has_xmax; }
  void SetTitleXMin(double ttl_xmin){ m_ttl_xmin = ttl_xmin; }
  void SetTitleYMin(double ttl_ymin){ m_ttl_ymin = ttl_ymin; }
  void SetTitleXMax(double ttl_xmax){ m_ttl_xmax = ttl_xmax; }
  void SetTitleYMax(double ttl_ymax){ m_ttl_ymax = ttl_ymax; }
  void SetHasTitleXMin(bool has_ttl_xmin){ m_has_ttl_xmin = has_ttl_xmin; }
  void SetHasTitleYMin(bool has_ttl_ymin){ m_has_ttl_ymin = has_ttl_ymin; }
  void SetHasTitleXMax(bool has_ttl_xmax){ m_has_ttl_xmax = has_ttl_xmax; }
  void SetHasTitleYMax(bool has_ttl_ymax){ m_has_ttl_ymax = has_ttl_ymax; }


  void SetNProjBin(int nprojbin){ m_nprojbin = nprojbin; }
  void SetExtraLabel(const std::string& extralabel){ m_extralabel = extralabel; }

  const std::string& Name(){ return m_name; }
  const std::string& Label(){ return m_label; }
  const std::string& YLabel(){ return m_ylabel; }
  const std::string& ResLabel(){ return m_reslabel; }
  const std::string& DoScale(){ return m_do_scale; }
  bool DrawStack(){ return m_draw_stack; }
  const std::string& DrawRes(){ return m_draw_res; }
  const std::string& DrawResErr(){ return m_draw_res_err; }
  bool IsLogY(){ return m_is_logY; }
  bool IsLogX(){ return m_is_logX; }
  int Rebin(){ return m_rebin; }
  bool DoWidth(){ return m_do_width; }
  double ResMin(){ return m_resmin; }
  double ResMax(){ return m_resmax; }
  bool HasResMin(){ return m_has_resmin; }
  bool HasResMax(){ return m_has_resmax; }
  double YMin(){ return m_ymin; }
  double YMax(){ return m_ymax; }
  bool HasYMin(){ return m_has_ymin; }
  bool HasYMax(){ return m_has_ymax; }
  double XMin(){ return m_xmin; }
  double XMax(){ return m_xmax; }
  bool HasXMin(){ return m_has_xmin; }
  bool HasXMax(){ return m_has_xmax; }
  double TitleXMin(){ return m_ttl_xmin; }
  double TitleYMin(){ return m_ttl_ymin; }
  double TitleXMax(){ return m_ttl_xmax; }
  double TitleYMax(){ return m_ttl_ymax; }
  bool HasTitleXMin(){ return m_has_ttl_xmin; }
  bool HasTitleYMin(){ return m_has_ttl_ymin; }
  bool HasTitleXMax(){ return m_has_ttl_xmax; }
  bool HasTitleYMax(){ return m_has_ttl_ymax; }

  int NProjBin(){ return m_nprojbin; }
  const std::string& ResDrawOpt(){ return m_resdrawopt; }
  const std::string& Blinding(){ return m_blinding; }
  const std::string& ExtraLabel(){ return m_extralabel; }

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
  bool m_do_width;
  double m_resmin;
  double m_resmax; 
  bool m_has_resmin;
  bool m_has_resmax;
  double m_ymin;
  double m_ymax; 
  bool m_has_ymin;
  bool m_has_ymax;
  double m_xmin;
  double m_xmax; 
  bool m_has_xmin;
  bool m_has_xmax;
  double m_ttl_xmin;
  double m_ttl_ymin;
  double m_ttl_xmax; 
  double m_ttl_ymax; 
  bool m_has_ttl_xmin;
  bool m_has_ttl_ymin;
  bool m_has_ttl_xmax;
  bool m_has_ttl_ymax;

  int m_nprojbin;
  std::string m_resdrawopt;
  std::string m_blinding;
  std::string m_extralabel;
};

typedef std::map<std::string, VariableAttributes*> VariableAttributesMap;
 

#endif
