#ifndef VARIABLEATTRIBUTES_H
#define VARIABLEATTRIBUTES_H

#include <string>
#include <map>
class VariableAttributes{
 public:
  VariableAttributes(const std::string& name, const std::string& label, const std::string& do_scale
		     , bool do_width=false, bool draw_stack=false, std::string draw_res="", bool isLog=false
		     , const std::string& ylabel="", const std::string& reslabel=""
		     , bool has_resmin=false, bool has_resmax=false, double resmin=0.5, double resmax=1.5
		     , const std::string& resdrawopt="", int rebin=-1);
  ~VariableAttributes(){ }

  void SetName(const std::string& name){ m_name = name; }
  void SetLabel(const std::string& label){ m_label = label; }
  void SetYLabel(const std::string& ylabel){ m_ylabel = ylabel; }
  void SetResLabel(const std::string& reslabel){ m_reslabel = reslabel; }
  void SetIsLog(bool isLog){ m_is_log = isLog; }
  void SetRebin(int rebin){ m_rebin = rebin; }
  void SetDoScale(const std::string& do_scale){ m_do_scale = do_scale; }
  void SetDrawStack(bool draw_stack){ m_draw_stack = draw_stack; }
  void SetDrawRes(const std::string& draw_res){ m_draw_res = draw_res; }
  void SetDoWidth(bool do_width){ m_do_width = do_width; }
  void SetResMin(double resmin){ m_resmin = resmin; }
  void SetResMax(double resmax){ m_resmax = resmax; }
  void SetResDrawOpt(const std::string& resdrawopt){ m_resdrawopt = resdrawopt; }
  void SetHasResMin(bool has_resmin){ m_has_resmin = has_resmin; }
  void SetHasResMax(bool has_resmax){ m_has_resmax = has_resmax; }

  const std::string& Name(){ return m_name; }
  const std::string& Label(){ return m_label; }
  const std::string& YLabel(){ return m_ylabel; }
  const std::string& ResLabel(){ return m_reslabel; }
  const std::string& DoScale(){ return m_do_scale; }
  bool DrawStack(){ return m_draw_stack; }
  const std::string& DrawRes(){ return m_draw_res; }
  bool IsLog(){ return m_is_log; }
  int Rebin(){ return m_rebin; }
  bool DoWidth(){ return m_do_width; }
  double ResMin(){ return m_resmin; }
  double ResMax(){ return m_resmax; }
  const std::string& ResDrawOpt(){ return m_resdrawopt; }
  bool HasResMin(){ return m_has_resmin; }
  bool HasResMax(){ return m_has_resmax; }

 private:  

  std::string m_name;
  std::string m_label;
  std::string m_ylabel;
  std::string m_reslabel;
  int m_draw_stack;
  std::string m_do_scale;
  std::string m_draw_res;
  bool m_is_log;
  int m_rebin;
  bool m_do_width;
  double m_resmin;
  double m_resmax; 
  std::string m_resdrawopt;
  bool m_has_resmin;
  bool m_has_resmax;

};

typedef std::map<std::string, VariableAttributes*> VariableAttributesMap;
 

#endif
