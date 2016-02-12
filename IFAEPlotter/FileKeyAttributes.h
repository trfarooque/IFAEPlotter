#ifndef FILEKEYATTRIBUTES_H
#define FILEKEYATTRIBUTES_H

#include <string>
#include <map>
#include <vector>
#include <iostream>

class SampleAttributes;

class FileKeyAttributes{

 public:
  FileKeyAttributes();
  FileKeyAttributes(const std::string& key, std::vector<std::string>* file_list=NULL, bool multi_sample=false
		    ,bool multi_scale=false, bool multi_name=false, bool has_syst=false, SampleAttributes* single_sample=NULL
		    , std::vector<SampleAttributes*> *sample_list=NULL, std::map<std::string, std::vector<std::string> >* syst_file_list=NULL);
  ~FileKeyAttributes();
  FileKeyAttributes(FileKeyAttributes &q);

 private:
  std::string m_key;
  bool m_multi_sample;
  bool m_multi_name;
  bool m_multi_scale;
  bool m_has_syst;
  std::vector<std::string>* m_file_list;
  std::map<std::string, std::vector<std::string> >* m_syst_file_list;
  SampleAttributes* m_single_sample;
  std::vector<SampleAttributes*> *m_sample_list;
  std::vector<double>* m_single_sample_scales;
  std::vector< std::vector<double> >* m_sample_scales;

  std::map<std::string, std::vector<double> >* m_syst_single_sample_scales;
  std::map<std::string, std::vector< std::vector<double> > >* m_syst_sample_scales;

 public:
  void SetKey(const std::string& key){ m_key = key; }
  void SetMultiSample( bool multi_sample ){ m_multi_sample = multi_sample; }
  void SetMultiName( bool multi_name ){ m_multi_name = multi_name; }
  void SetMultiScale( bool multi_scale ){ m_multi_scale = multi_scale; }
  void SetHasSyst( bool has_syst ){ m_has_syst = has_syst; }
  void SetFileList( std::vector<std::string>* file_list){ m_file_list = file_list; }
  void SetSystFileList( std::map<std::string, std::vector<std::string> >* syst_file_list){ m_syst_file_list = syst_file_list; }
  void SetSingleSample( SampleAttributes* single_sample ){ m_single_sample = single_sample; }
  void SetSampleList( std::vector<SampleAttributes*> *sample_list){ m_sample_list = sample_list; }
  void SetSingleSampleScales( std::vector<double>* single_sample_scales){ m_single_sample_scales = single_sample_scales; }
  void SetSampleScales( std::vector<std::vector<double> >* sample_scales){ m_sample_scales = sample_scales; }

  void SetSystSingleSampleScales( std::map<std::string, std::vector<double> >* syst_single_sample_scales){ m_syst_single_sample_scales = syst_single_sample_scales; }
  void SetSystSampleScales      ( std::map<std::string, std::vector<std::vector<double> > >* syst_sample_scales){ m_syst_sample_scales = syst_sample_scales; }


  const std::string& Key(){ return m_key; }
  bool MultiSample(){ return m_multi_sample; }
  bool MultiName(){ return m_multi_name; }
  bool MultiScale(){ return m_multi_scale; }
  bool HasSyst(){ return m_has_syst; }
  std::vector<std::string>* FileList(){ return m_file_list; }
  std::map<std::string, std::vector<std::string> >* SystFileList(){ return m_syst_file_list; }
  std::vector<std::string>& SystFileList(const std::string& syst){ 
    if(m_syst_file_list->find(syst) == m_syst_file_list->end()){ 
      std::cout<<"ERROR File list for systematic "<<syst<<"not found"<<std::endl; 
    }
    return m_syst_file_list -> at(syst); 
  }
  SampleAttributes* SingleSample(){ return m_single_sample; }
  std::vector< SampleAttributes* >* SampleList(){ return m_sample_list; }
  std::vector<double>* SingleSampleScales(){ return m_single_sample_scales; }
  std::vector<std::vector<double> >* SampleScales(){ return m_sample_scales; }

  std::map<std::string, std::vector<double> >* SystSingleSampleScales(){ return m_syst_single_sample_scales; }
  std::map<std::string, std::vector<std::vector<double> > >* SystSampleScales(){ return m_syst_sample_scales; }

  std::vector<double>& SystSingleSampleScales(const std::string& syst){ 
    if(m_syst_single_sample_scales->find(syst) == m_syst_single_sample_scales->end()){ 
      std::cout<<"ERROR : Single sample scales for systematic "<<syst<<"not found"<<std::endl; 
    }
    return m_syst_single_sample_scales -> at(syst); 
  }

  std::vector<std::vector<double> >& SystSampleScales(const std::string& syst){ 
    if(m_syst_sample_scales->find(syst) == m_syst_sample_scales->end()){ 
      std::cout<<"ERROR : Sample scales for systematic "<<syst<<"not found"<<std::endl; 
    }
    return m_syst_sample_scales -> at(syst); 
  }



};

typedef std::map<std::string, FileKeyAttributes*> FileKeyAttributesMap;
typedef std::vector< FileKeyAttributes* > FileKeyAttributesVector;

#endif
