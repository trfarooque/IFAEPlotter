#ifndef FILEKEYATTRIBUTES_H
#define FILEKEYATTRIBUTES_H

#include <string>
#include <map>
#include <vector>

class SampleAttributes;

class FileKeyAttributes{

 public:
  FileKeyAttributes();
  FileKeyAttributes(const std::string& key, std::vector<std::string>* file_list, bool multi_sample=false
		 , bool multi_name=false, SampleAttributes* single_sample=NULL, std::vector<SampleAttributes*> *sample_list=NULL );
  ~FileKeyAttributes();
  FileKeyAttributes(FileKeyAttributes &q);

 private:
  std::string m_key;
  bool m_multi_sample;
  bool m_multi_name;
  std::vector<std::string>* m_file_list;
  SampleAttributes* m_single_sample;
  std::vector<SampleAttributes*> *m_sample_list;
  std::vector<double>* m_single_sample_scales;
  std::vector< std::vector<double> >* m_sample_scales;

 public:
  void SetKey(const std::string& key){ m_key = key; }
  void SetMultiSample( bool multi_sample ){ m_multi_sample = multi_sample; }
  void SetMultiName( bool multi_name ){ m_multi_name = multi_name; }
  void SetFileList( std::vector<std::string>* file_list){ m_file_list = file_list; }
  void SetSingleSample( SampleAttributes* single_sample ){ m_single_sample = single_sample; }
  void SetSampleList( std::vector<SampleAttributes*> *sample_list){ m_sample_list = sample_list; }
  void SetSingleSampleScales( std::vector<double>* single_sample_scales){ m_single_sample_scales = single_sample_scales; }
  void SetSampleScales( std::vector<std::vector<double> >* sample_scales){ m_sample_scales = sample_scales; }


  const std::string& Key(){ return m_key; }
  bool MultiSample(){ return m_multi_sample; }
  bool MultiName(){ return m_multi_name; }
  std::vector<std::string>* FileList(){ return m_file_list; }
  SampleAttributes* SingleSample(){ return m_single_sample; }
  std::vector< SampleAttributes* >* SampleList(){ return m_sample_list; }
  std::vector<double>* SingleSampleScales(){ return m_single_sample_scales; }
  std::vector<std::vector<double> >* SampleScales(){ return m_sample_scales; }

};

typedef std::map<std::string, FileKeyAttributes*> FileKeyAttributesMap;
typedef std::vector< FileKeyAttributes* > FileKeyAttributesVector;

#endif
