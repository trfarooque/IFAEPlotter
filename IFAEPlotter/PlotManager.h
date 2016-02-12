#ifndef PLOTMANAGER_H
#define PLOTMANAGER_H

#include <string>
#include <map>
#include <vector>

class TH1D;
class Plotter_Options;
class HistManager;
class PlotUtils;
class StyleDictionary;
class SampleAttributes;
class VariableAttributes;
class SystematicsAttributes;
class FileKeyAttributes;

class PlotManager{

 public:
  //PlotManager(const std::string& config_sample, const std::string& config_filelist, const std::string& config_variable, const std::string& config_style );
  PlotManager(Plotter_Options* opt);
  ~PlotManager();

  void Initialize();
  void Execute();
  void Terminate();

  static int ParseConfigFile_New(const std::string& config_file, std::vector<std::map<std::string, std::string> >& ret_map, const std::string& delim=" : ");
  static int ParseConfigFile_Old(const std::string& config_file, std::vector<std::map<std::string, std::string> >& ret_map, const std::string& delim=" : ");

  static int ParseConfigFile(const std::string& config_file, std::vector<std::map<std::string, std::string> >& ret_map, const std::string& delim=" : "
		      , bool newformat=false);

 private:

  Plotter_Options* m_opt; //not owned
  HistManager* m_hstMngr; //owned
  PlotUtils* m_plotUtils; //owned
  std::map<std::string, SampleAttributes*> m_attr_map; //owned
  std::map<std::string, VariableAttributes*> m_var_map; //owned
  std::map<std::string, SystematicsAttributes*> m_sys_map; //owned
  StyleDictionary* m_styleDict; //owned
  std::map<std::string, FileKeyAttributes*> m_filekey_map; //owned

  /*
  std::map<std::string, std::vector<std::string> > m_filename_map;
  std::map<std::string, bool > m_filekey_multi_map;
  std::map<std::string, bool > m_filekey_multiname_map;
  std::map<std::string, std::vector<std::string> > m_filekey_samples;
  */
  //std::map<std::string, std::vector<double> > m_filescale_map; -> In SampleAttributes now

  bool m_new_sample_format;
  bool m_new_variable_format;
  bool m_new_style_format;
  bool m_new_filelist_format;


  int ParseSampleConfig(const std::string& config_sample, const std::string& delim=" : ");
  int ParseVariableConfig(const std::string& config_variable, const std::string& delim=" : ");
  int ParseStyleConfig(const std::string& config_style, const std::string& delim=" : ");
  int ParseFileList(const std::string& filelist, const std::string& delim=" : ");

  int CopyVariableListFromFile(const std::string& dist_file);

  int ReadHistogramsFromFile(int dim);
  void makeEfficiencyHistograms();
  void ProjectByBin();
  void FillHistManager();
  void WriteHistogramsToFile();

  int ReadAllSystematics(FileKeyAttributes* fk_att); 
  void QuadraticHistSum(const std::string& h_orig_name, const std::string& h_add_name);
  void QuadraticHistSum(TH1D* h_orig, TH1D* h_add);

  //const double* ParseRebinEdges( int nbin, const std::string& bindef);
  void ParseRebinEdges( int nbin, const std::string& bindef, double* xbins_new_ptr);
  TH1D* VariableRebinning(const std::string& histname, int nbin, const double* binedges);
  TH1D* VariableRebinning(const std::string& histname, TH1D* hist, int nbin, const double* binedges);

};

#endif
