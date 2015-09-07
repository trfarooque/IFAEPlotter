#ifndef SAMPLEATTRIBUTES_H
#define SAMPLEATTRIBUTES_H

#include <string>
#include <map>

class SampleAttributes{

 public:
  SampleAttributes();
  SampleAttributes(const std::string& name, const std::string& suffix, const std::string& leglabel, const std::string& stylekey
		   , const std::string& drawopt="hist", const std::string& drawscale="NORM", bool draw_stack=false, int res_opt=-1);
  ~SampleAttributes(){}

 private:
  std::string m_name;
  std::string m_suffix;
  std::string m_leglabel;
  std::string m_stylekey;
  std::string m_drawopt;
  std::string m_drawscale;
  std::string m_legopt; 

  bool m_draw_stack;
  int m_res_opt;

 public:
  void SetName(const std::string& name){ m_name = name; }
  void SetSuffix(const std::string& suffix){ m_suffix = suffix; }
  void SetStyleKey(const std::string& stylekey){ m_stylekey = stylekey; }
  void SetDrawOpt(const std::string& drawopt){ m_drawopt = drawopt; }
  void SetDrawScale(const std::string& drawscale){ m_drawscale = drawscale; }
  void SetLegLabel(const std::string& leglabel){ m_leglabel = leglabel; }
  void SetLegOpt(const std::string& legopt){ m_legopt = legopt; }
  void SetDrawStack(bool draw_stack){ m_draw_stack = draw_stack; }
  void SetResOpt(int res_opt){ m_res_opt = res_opt; }

  const std::string& Name(){ return m_name; }
  const std::string& Suffix(){ return m_suffix; }
  const std::string& LegLabel(){ return m_leglabel; }
  const std::string& LegOpt(){ return m_legopt; }
  const std::string& StyleKey(){ return m_stylekey; }
  const std::string& DrawOpt(){ return m_drawopt; }
  const std::string& DrawScale(){ return m_drawscale; }
  bool DrawStack(){ return m_draw_stack; }
  int ResOpt(){ return m_res_opt; }


};

typedef std::map<std::string, SampleAttributes*> SampleAttributesMap;

#endif
