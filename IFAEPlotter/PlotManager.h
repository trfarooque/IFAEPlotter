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


 private:

  Plotter_Options* m_opt; //not owned
  HistManager* m_hstMngr; //owned
  PlotUtils* m_plotUtils; //owned
  std::map<std::string, SampleAttributes*> m_attr_map; //owned
  std::map<std::string, VariableAttributes*> m_var_map; //owned
  std::map<std::string, SystematicsAttributes*> m_sys_map; //owned
  StyleDictionary* m_styleDict; //owned
  std::map<std::string, FileKeyAttributes*> m_filekey_map; //owned

  int CopyVariableListFromFile(const std::string& dist_file);

  //------------------------ INPUTMAKER ------------------
  int ReadHistogramsFromFile(int dim);
  void makeEfficiencyHistograms();
  void ProjectByBin();
  void FillHistManager();
  void WriteHistogramsToFile();
  int ReadSystematicsFromFiles(); 
  int ReadAllSystematics(FileKeyAttributes* fk_att); 

  //----------------- SYST CALCULATION CLASS
  int PrintSystematics();
  void QuadraticHistSum(const std::string& h_orig_name, const std::string& h_add_name);
  void QuadraticHistSum(TH1D* h_orig, TH1D* h_add);

  //const double* ParseRebinEdges( int nbin, const std::string& bindef);
  //----------------------- HISTMANAGER ---------------------------
  void ParseRebinEdges( int nbin, const std::string& bindef, double* xbins_new_ptr);
  TH1D* VariableRebinning(const std::string& histname, int nbin, const double* binedges);
  TH1D* VariableRebinning(const std::string& histname, TH1D* hist, int nbin, const double* binedges);

};

#endif
