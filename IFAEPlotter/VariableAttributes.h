#ifndef VARIABLEATTRIBUTES_H
#define VARIABLEATTRIBUTES_H

#include <string>
#include <map>
class VariableAttributes{
 public:
  VariableAttributes(std::string name, std::string label, bool draw_stack=false, std::string draw_res="", bool isLog=false,  int rebin=-1);
  ~VariableAttributes(){ }

  void SetName(std::string name){ m_name = name; }
  void SetLabel(std::string label){ m_label = label; }
  void SetIsLog(bool isLog){ m_is_log = isLog; }
  void SetRebin(int rebin){ m_rebin = rebin; }
  void SetDrawStack(bool draw_stack){ m_draw_stack = draw_stack; }
  void SetDrawRes(const std::string& draw_res){ m_draw_res = draw_res; }

  const std::string& Name(){return m_name;}
  const std::string& Label(){return m_label;}
  bool DrawStack(){return m_draw_stack;}
  const std::string& DrawRes(){return m_draw_res;}
  bool IsLog(){return m_is_log;}
  int Rebin(){return m_rebin;}
 private:  
  std::string m_name;
  std::string m_label;
  int m_draw_stack;
  std::string m_draw_res;
  bool m_is_log;
  int m_rebin;
};

typedef std::map<std::string, VariableAttributes*> VariableAttributesMap;
 

#endif
