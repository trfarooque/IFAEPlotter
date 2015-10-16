#ifndef PLOTMANAGER_H
#define PLOTMANAGER_H

#include <string>
#include <map>
#include <vector>
#include "IFAEPlotter/SampleAttributes.h"
#include "IFAEPlotter/VariableAttributes.h"

class Plotter_Options;
class HistManager;
class PlotUtils;
class StyleDictionary;

class PlotManager{

 public:
  //PlotManager(const std::string& config_sample, const std::string& config_filelist, const std::string& config_variable, const std::string& config_style );
  PlotManager(Plotter_Options* opt);
  ~PlotManager();

  void Initialize();
  void Execute();
  void Terminate();

 private:

  Plotter_Options* m_opt; //not owned
  HistManager* m_hstMngr; //owned
  PlotUtils* m_plotUtils; //owned
  SampleAttributesMap m_attr_map; //owned
  VariableAttributesMap m_var_map; //owned
  StyleDictionary* m_styleDict; //owned
  std::map<std::string, std::vector<std::string> > m_filename_map;
  std::map<std::string, bool > m_filekey_multi_map;
  std::map<std::string, std::vector<std::string> > m_filekey_samples;
  std::map<std::string, std::vector<double> > m_filescale_map;
  std::vector<std::string> m_multi_extra;

  bool m_new_sample_format;
  bool m_new_variable_format;
  bool m_new_style_format;
  bool m_new_filelist_format;

  void TrimString(std::string& str, const std::string& whitespace=" \t");
  std::string::size_type ParseString(std::string& base, std::string& piece, const std::string& delim); 


  int ParseConfigFile_New(const std::string& config_file, std::vector<std::map<std::string, std::string> >& ret_map, const std::string& delim=" : ");
  int ParseConfigFile_Old(const std::string& config_file, std::vector<std::map<std::string, std::string> >& ret_map, const std::string& delim=" : ");

  int ParseConfigFile(const std::string& config_file, std::vector<std::map<std::string, std::string> >& ret_map, const std::string& delim=" : ", bool newformat=false);

  int ParseSampleConfig(const std::string& config_sample, const std::string& delim=" : ");
  int ParseVariableConfig(const std::string& config_variable, const std::string& delim=" : ");
  int ParseStyleConfig(const std::string& config_style, const std::string& delim=" : ");
  int ParseFileList(const std::string& filelist, const std::string& delim=" : ");

  int ReadHistogramsFromFile(int dim);
  void makeEfficiencyHistograms();
  void ProjectByBin();
  void FillHistManager();

};

#endif
