#include <iostream>
#include <string>

#include "IFAETopFramework/OptionsBase.h"
#include "IFAEPlotter/Plotter_Options.h"

//_____________________________________________________________
//
Plotter_Options::Plotter_Options():
  OptionsBase(),
  m_config_sample(""),
  m_config_variable(""),
  m_file_list(""),
  m_style_lib("IFAEPlotter/share/test_style_config.txt"),
  m_output_format("png"),
  m_ylabel("Events"),
  m_reslabel("Data/MC"),
  m_resmin(0.5),
  m_resmax(1.5),
  m_show_yields(false)
{}

//_____________________________________________________________
//
Plotter_Options::Plotter_Options( const Plotter_Options &q ):
OptionsBase(q)
{
  m_config_sample      = q.m_config_sample;
  m_config_variable    = q.m_config_variable;
  m_file_list          = q.m_file_list;
  m_style_lib          = q.m_style_lib;
  m_output_format      = q.m_output_format;
  m_ylabel             = q.m_ylabel;
  m_reslabel           = q.m_reslabel;
  m_resmin             = q.m_resmin;
  m_resmax             = q.m_resmax;
  m_show_yields        = q.m_show_yields;
}

//_____________________________________________________________
//
Plotter_Options::~Plotter_Options()
{}


//_____________________________________________________________
//
bool Plotter_Options::IdentifyOption ( const std::string &argument, const std::string &value ) {
    
  std::string temp_arg = argument;
  std::string temp_val = value;

  if(!OptionsBase::IdentifyOption(argument, value)){
    if( temp_arg.find("--SAMPLES") != std::string::npos ){
      m_config_sample = temp_val;
    } 
    else if( temp_arg.find("--VARIABLES") != std::string::npos ){
      m_config_variable = temp_val;
    } 
    else if( temp_arg.find("--FILELIST") != std::string::npos ){
      m_file_list = temp_val;
    } 
    else if( temp_arg.find("--STYLELIB") != std::string::npos ){
      m_style_lib = temp_val;
    } 
    else if( temp_arg.find("--OUTFORMAT") != std::string::npos ){
      std::transform(temp_val.begin(), temp_val.end(), temp_val.begin(), toupper);
      m_output_format = temp_val;
    } 
    else if( temp_arg.find("--YLABEL") != std::string::npos ){
      m_ylabel = temp_val;
    } 
    else if( temp_arg.find("--RESLABEL") != std::string::npos ){
      m_reslabel = temp_val;
    } 
    else if( temp_arg.find("--RESMIN") != std::string::npos ){
      m_resmin = atof(temp_val.c_str());
    } 
    else if( temp_arg.find("--RESMAX") != std::string::npos ){
      m_resmax = atof(temp_val.c_str());
    } 
    else if( temp_arg.find("--SHOWYIELDS") != std::string::npos ){
      m_show_yields = (atoi(temp_val.c_str()) > 0);
    } 
    else {
      return false;
    }
  }
  return true;
}


//_____________________________________________________________
//
void Plotter_Options::PrintOptions(){
    OptionsBase::PrintOptions();
    std::cout << "============== Plotter_Options ================="     << std::endl;
    std::cout << " m_config_sample          = " << m_config_sample      << std::endl;
    std::cout << " m_config_variable        = " << m_config_variable    << std::endl;
    std::cout << " m_file_list               = " << m_file_list         << std::endl;
    std::cout << " m_style_lib               = " << m_style_lib         << std::endl;
    std::cout << " m_output_format           = " << m_output_format     << std::endl;
    std::cout << " m_ylabel                  = " << m_ylabel            << std::endl;
    std::cout << " m_reslabel                = " << m_reslabel          << std::endl;
    std::cout << " m_resdrawopt              = " << m_resdrawopt        << std::endl;
    std::cout << " m_resmin                  = " << m_resmin            << std::endl;
    std::cout << " m_resmax                  = " << m_resmax            << std::endl;
    std::cout << " m_show_yields             = " << m_show_yields       << std::endl;
    std::cout << "============================================="        << std::endl;
    std::cout << "" << std::endl;
}








