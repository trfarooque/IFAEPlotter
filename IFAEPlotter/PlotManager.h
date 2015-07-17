#ifndef PLOTMANAGER_H
#define PLOTMANAGER_H

#include <string>
#include <map>
#include <vector>
#include "IFAEPlotter/SampleAttributes.h"
#include "IFAEPlotter/VariableAttributes.h"
class HistManager;
class PlotUtils;
class StyleDictionary;

class PlotManager{

 public:
  PlotManager(const std::string& config_sample, const std::string& config_filelist, const std::string& config_variable, const std::string& config_style );
  ~PlotManager(){}

  void Initialize();
  void Execute();
  void Terminate();

 private:

  HistManager* m_hstMngr;
  PlotUtils* m_plotUtils;
  SampleAttributesMap m_attr_map;
  VariableAttributesMap m_var_map;
  StyleDictionary* m_styleDict;
  std::map<std::string, std::vector<std::string> > m_filename_map;

  int ParseConfigFile(const std::string config_file, std::vector<std::map<std::string, std::string> >& ret_map, const std::string& delim=" : ");
  int ParseSampleConfig(const std::string& config_sample, const std::string& delim=" : ");
  int ParseVariableConfig(const std::string& config_variable, const std::string& delim=" : ");
  int ParseStyleConfig(const std::string& config_style, const std::string& delim=" : ");
  int ParseFileList(const std::string& filelist, const std::string& delim=" : ");

  void FillHistManager();

};

#endif
