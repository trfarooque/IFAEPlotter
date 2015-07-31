#ifndef SAMPLEATTRIBUTES_H
#define SAMPLEATTRIBUTES_H

#include <string>
#include <map>

class SampleAttributes{

 public:
  SampleAttributes();
  SampleAttributes(std::string name, std::string suffix, std::string leglabel, std::string stylekey, std::string drawopt="hist"
		   , int do_scale=0, bool draw_stack=false, bool is_baseline=false
		   , int linecolour=-1, int fillcolour=-1, int linestyle=-1, int fillstyle=-1);
  ~SampleAttributes(){}

 private:
  std::string m_name;
  std::string m_suffix;
  std::string m_leglabel;
  std::string m_stylekey;
  std::string m_drawopt;
  int m_do_scale;
  bool m_draw_stack;
  bool m_is_baseline;
  int m_linecolour;
  int m_fillcolour;
  int m_linestyle;
  int m_fillstyle;

 public:
  void SetName(std::string name){ m_name = name; }
  void SetSuffix(std::string suffix){ m_suffix = suffix; }
  void SetStyleKey(std::string stylekey){ m_stylekey = stylekey; }
  void SetLineStyle(int linestyle){ m_linestyle = linestyle; }
  void SetFillStyle(int fillstyle){ m_fillstyle = fillstyle; }
  void SetLineColour(int linecolour){ m_linecolour = linecolour; }
  void SetFillColour(int fillcolour){ m_fillcolour = fillcolour; }
  void SetLegLabel(std::string leglabel){ m_leglabel = leglabel; }
  void SetDoScale(int do_scale){ m_do_scale = do_scale; }
  void SetDrawStack(bool draw_stack){ m_draw_stack = draw_stack; }
  void SetIsBaseline(bool is_baseline){ m_is_baseline = is_baseline; }

  const std::string& Name(){ return m_name; }
  const std::string& Suffix(){ return m_suffix; }
  const std::string& LegLabel(){ return m_leglabel; }
  const std::string& StyleKey(){ return m_stylekey; }
  const std::string& DrawOpt(){ return m_drawopt; }
  bool DrawStack(){ return m_draw_stack; }
  bool IsBaseline(){ return m_is_baseline; }
  int DoScale(){ return m_do_scale; }
  int LineColour(){ return m_linecolour; }
  int FillColour(){ return m_fillcolour; }
  int LineStyle(){ return m_linestyle; }
  int FillStyle(){ return m_fillstyle; }


};

typedef std::map<std::string, SampleAttributes*> SampleAttributesMap;

#endif
