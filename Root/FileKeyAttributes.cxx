#include "IFAEPlotter/FileKeyAttributes.h"
#include "IFAEPlotter/SampleAttributes.h"
#include <iostream>

FileKeyAttributes::FileKeyAttributes(const std::string& key, std::vector<std::string>* file_list, bool multi_sample
				     , bool multi_name, SampleAttributes* single_sample, SampleAttributesVector* sample_list) : 
  m_key(key),
  m_multi_sample(multi_sample),
  m_multi_name(multi_name),
  m_file_list(file_list),
  m_single_sample(single_sample),
  m_sample_list(sample_list),
  m_single_sample_scales(NULL),
  m_sample_scales(NULL)
{}

FileKeyAttributes::FileKeyAttributes(): FileKeyAttributes("",NULL){ }

FileKeyAttributes::FileKeyAttributes( FileKeyAttributes& q){
  m_key                  = q.m_key;
  m_multi_sample         = q.m_multi_sample;
  m_multi_name           = q.m_multi_name;
  m_file_list            = q.m_file_list;
  m_single_sample        = q.m_single_sample;
  m_sample_list          = q.m_sample_list;
  m_single_sample_scales = q.m_single_sample_scales;
  m_sample_scales        = q.m_sample_scales;
}

FileKeyAttributes::~FileKeyAttributes(){

  delete m_file_list;
  delete m_sample_list;
  delete m_single_sample_scales;
  delete m_sample_scales;

}
