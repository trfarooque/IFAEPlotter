#ifndef SYSTEMATICSATTRIBUTES_H
#define SYSTEMATICSATTRIBUTES_H

#include <string>
#include <map>
#include <vector>

class Plotter_Options;

class SystematicsAttributes{

 public:
  SystematicsAttributes();
  SystematicsAttributes(const std::string& name, const std::string& name_up="", const std::string & name_down=""
			, bool one_sided=false, bool newfile=false, bool newdir=false, const std::string& symmetrisation=""
			, const std::string& suffix="");
  ~SystematicsAttributes(){}
  SystematicsAttributes(SystematicsAttributes &q);

 private:
  std::string m_name;
  std::string m_name_up;
  std::string m_name_down;
  std::string m_symmetrisation;
  std::string m_suffix;
  bool m_one_sided;
  bool m_newfile;
  bool m_newdir;

 public:
  void SetName(const std::string& name){ m_name = name; }
  void SetNameUp(const std::string& name_up){ m_name_up = name_up; }
  void SetNameDown(const std::string& name_down){ m_name_down = name_down; }
  void SetSymmetrisation(const std::string& symmetrisation){ m_symmetrisation = symmetrisation; }
  void SetSuffix(const std::string& suffix){ m_suffix = suffix; }
  void SetOneSided(bool one_sided){ m_one_sided = one_sided; }
  void SetNewFile(bool newfile){ m_newfile = newfile; }
  void SetNewDir(bool newdir){ m_newdir = newdir; }

  const std::string& Name() const{ return m_name; }
  const std::string& NameUp() const{ return m_name_up; }
  const std::string& NameDown() const{ return m_name_down; }
  const std::string& Symmetrisation() const{ return m_symmetrisation; }
  const std::string& Suffix() const{ return m_suffix; }
  bool OneSided() const{ return m_one_sided; }
  bool NewFile() const{ return m_newfile; }
  bool NewDir() const{ return m_newdir; }
  static std::map<std::string, SystematicsAttributes*> ParseSystematicsConfig( Plotter_Options* opt );

};

typedef std::map<std::string, SystematicsAttributes*> SystematicsAttributesMap;
typedef std::vector< SystematicsAttributes* > SystematicsAttributesVector;

#endif
