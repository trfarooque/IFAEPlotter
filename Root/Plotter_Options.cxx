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
  m_resdrawopt(""),
  m_prefixlist(""),
  m_suffixlist(""),
  m_num_suffix(""),
  m_den_suffix(""),
  m_num_prefix(""),
  m_den_prefix(""),
  m_projopt("MEAN"),
  m_legopt(""),
  m_title(""),
  m_blinding(""),
  m_blind_sample(""),
  m_blind_criterion("CONTAMINATION"),
  m_yield_format("%4g"),
  m_new_config_format(false),
  m_new_sample_format(false),
  m_new_variable_format(false),
  m_new_style_format(false),
  m_new_filelist_format(false),
  m_do1DPlots(true),
  m_doProjections(false),
  m_doEff(false),
  m_show_yields(false),
  m_resmin(0.),
  m_resmax(0.),
  m_ymin(0.),
  m_ymax(0.),
  m_xmin(0.),
  m_xmax(0.),
  m_titlexmin(0.),
  m_titleymin(0.),
  m_titlexmax(0.),
  m_titleymax(0.),
  m_global_scale(1.),
  m_blind_threshold(0.)
{}

//_____________________________________________________________
//
Plotter_Options::Plotter_Options( const Plotter_Options &q ):
OptionsBase(q)
{
  m_config_sample        = q.m_config_sample;
  m_config_variable      = q.m_config_variable;
  m_file_list            = q.m_file_list;
  m_style_lib            = q.m_style_lib;
  m_output_format        = q.m_output_format;
  m_ylabel               = q.m_ylabel;
  m_reslabel             = q.m_reslabel;
  m_resdrawopt           = q.m_resdrawopt;
  m_prefixlist           = q.m_prefixlist;
  m_suffixlist           = q.m_suffixlist;
  m_num_suffix           = q.m_num_suffix;
  m_den_suffix           = q.m_den_suffix;
  m_num_prefix           = q.m_num_prefix;
  m_den_prefix           = q.m_den_prefix;
  m_projopt              = q.m_projopt;
  m_new_config_format    = q.m_new_config_format;
  m_new_sample_format    = q.m_new_sample_format;
  m_new_variable_format  = q.m_new_variable_format;
  m_new_style_format     = q.m_new_style_format;
  m_new_filelist_format  = q.m_new_filelist_format;
  m_do1DPlots            = q.m_do1DPlots;
  m_doProjections        = q.m_doProjections;
  m_doEff                = q.m_doEff;
  m_resmin               = q.m_resmin;
  m_resmax               = q.m_resmax;
  m_ymin                 = q.m_ymin;
  m_ymax                 = q.m_ymax;
  m_xmin                 = q.m_xmin;
  m_xmax                 = q.m_xmax;
  m_show_yields          = q.m_show_yields;
  m_legopt               = q.m_legopt;
  m_title                = q.m_title;
  m_blinding             = q.m_blinding;
  m_blind_sample         = q.m_blind_sample;
  m_blind_criterion      = q.m_blind_criterion;
  m_yield_format         = q.m_yield_format;
  m_titlexmin            = q.m_titlexmin;
  m_titleymin            = q.m_titleymin;
  m_titlexmax            = q.m_titlexmax;
  m_titleymax            = q.m_titleymax;
  m_global_scale         = q.m_global_scale;
  m_blind_threshold      = q.m_blind_threshold;
}

//_____________________________________________________________
//
Plotter_Options::~Plotter_Options()
{}


bool Plotter_Options::BoolValue(std::string &arg_val, bool& bin_val){

  std::transform(arg_val.begin(), arg_val.end(), arg_val.begin(), ::toupper);
  if( arg_val.find("TRUE") != std::string::npos ){ bin_val = true; return true; }
  else if( arg_val.find("FALSE") != std::string::npos ){ bin_val = false; return true; }
  else{std::cout<<"Error : Unknown value "<<arg_val<<" for binary option "<<std::endl; return false; }
}
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
    else if( temp_arg.find("--RESDRAWOPT") != std::string::npos ){
      m_resdrawopt = temp_val;
    }
    else if( temp_arg.find("--PREFIXLIST") != std::string::npos ){
      m_prefixlist = temp_val;
    } 
    else if( temp_arg.find("--SUFFIXLIST") != std::string::npos ){
      m_suffixlist = temp_val;
    } 
    else if( temp_arg.find("--NUMSUFFIX") != std::string::npos ){
      m_num_suffix = temp_val;
    } 
    else if( temp_arg.find("--DENSUFFIX") != std::string::npos ){
      m_den_suffix = temp_val;
    } 
    else if( temp_arg.find("--NUMPREFIX") != std::string::npos ){
      m_num_prefix = temp_val;
    } 
    else if( temp_arg.find("--DENPREFIX") != std::string::npos ){
      m_den_prefix = temp_val;
    } 
    else if( temp_arg.find("--PROJOPT") != std::string::npos ){
      m_projopt = temp_val;
    } 
    else if( temp_arg.find("--NEWCONFIG") != std::string::npos ){
      //m_new_config_format = atoi(temp_val.c_str()) > 0);
      BoolValue(temp_val, m_new_config_format);
    } 

    else if( temp_arg.find("--NEWSAMPLECONFIG") != std::string::npos ){
      //m_new_sample_format = (atoi(temp_val.c_str()) > 0);
      BoolValue(temp_val, m_new_sample_format);
    } 
    else if( temp_arg.find("--NEWVARIABLECONFIG") != std::string::npos ){
      //m_new_variable_format = (atoi(temp_val.c_str()) > 0);
      BoolValue(temp_val, m_new_variable_format);
    } 
    else if( temp_arg.find("--NEWSTYLECONFIG") != std::string::npos ){
      //m_new_style_format = (atoi(temp_val.c_str()) > 0);
      BoolValue(temp_val, m_new_style_format);
    } 
    else if( temp_arg.find("--NEWFILELIST") != std::string::npos ){
      //m_new_filelist_format = (atoi(temp_val.c_str()) > 0);
      BoolValue(temp_val, m_new_filelist_format);
    } 


    else if( temp_arg.find("--DO1DPLOTS") != std::string::npos ){
      //m_do1DPlots = (atoi(temp_val.c_str()) > 0);
      BoolValue(temp_val, m_do1DPlots);
    } 
    else if( temp_arg.find("--DOPROJ") != std::string::npos ){
      //m_doProjections = (atoi(temp_val.c_str()) > 0);
      BoolValue(temp_val, m_doProjections);
    } 
    else if( temp_arg.find("--DOEFF") != std::string::npos ){
      //m_doEff = (atoi(temp_val.c_str()) > 0);
      BoolValue(temp_val, m_doEff);
    } 
    else if( temp_arg.find("--RESMIN") != std::string::npos ){
      m_resmin = atof(temp_val.c_str());
    } 
    else if( temp_arg.find("--RESMAX") != std::string::npos ){
      m_resmax = atof(temp_val.c_str());
    } 
    else if( temp_arg.find("--YMIN") != std::string::npos ){
      m_ymin = atof(temp_val.c_str());
    } 
    else if( temp_arg.find("--YMAX") != std::string::npos ){
      m_ymax = atof(temp_val.c_str());
    } 
    else if( temp_arg.find("--XMIN") != std::string::npos ){
      m_xmin = atof(temp_val.c_str());
    } 
    else if( temp_arg.find("--XMAX") != std::string::npos ){
      m_xmax = atof(temp_val.c_str());
    } 
    else if( temp_arg.find("--SHOWYIELDS") != std::string::npos ){
      //m_show_yields = (atoi(temp_val.c_str()) > 0);
      BoolValue(temp_val, m_show_yields);
    } 
    else if( temp_arg.find("--LEGOPT") != std::string::npos ){
      m_legopt = temp_val;
    } 
    else if( temp_arg.find("--TITLEXMIN") != std::string::npos ){
      m_titlexmin = atof(temp_val.c_str());
    } 
    else if( temp_arg.find("--TITLEYMIN") != std::string::npos ){
      m_titleymin = atof(temp_val.c_str());
    } 
    else if( temp_arg.find("--TITLEXMAX") != std::string::npos ){
      m_titlexmax = atof(temp_val.c_str());
    } 
    else if( temp_arg.find("--TITLEYMAX") != std::string::npos ){
      m_titleymax = atof(temp_val.c_str());
    } 
    else if( temp_arg.find("--TITLE") != std::string::npos ){
      m_title = temp_val;
    } 
    else if( temp_arg.find("--BLINDING") != std::string::npos ){
      m_blinding = temp_val;
    } 
    else if( temp_arg.find("--BLINDSAMPLE") != std::string::npos ){
      m_blind_sample = temp_val;
    } 
    else if( temp_arg.find("--GLOBALSCALE") != std::string::npos ){
      m_global_scale = atof(temp_val.c_str());
    } 
    else if( temp_arg.find("--BLINDTHRESHOLD") != std::string::npos ){
      m_blind_threshold = atof(temp_val.c_str());
    } 
    else if( temp_arg.find("--BLINDCRITERION") != std::string::npos ){
      m_blind_criterion = temp_val;
    } 
    else if( temp_arg.find("--YIELDFORMAT") != std::string::npos ){
      m_yield_format = temp_val;
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
    std::cout << "============== Plotter_Options ================="       << std::endl;
    std::cout << " m_config_sample          = " << m_config_sample        << std::endl;
    std::cout << " m_config_variable        = " << m_config_variable      << std::endl;
    std::cout << " m_file_list              = " << m_file_list            << std::endl;
    std::cout << " m_style_lib              = " << m_style_lib            << std::endl;
    std::cout << " m_output_format          = " << m_output_format        << std::endl;
    std::cout << " m_ylabel                 = " << m_ylabel               << std::endl;
    std::cout << " m_reslabel               = " << m_reslabel             << std::endl;
    std::cout << " m_resdrawopt             = " << m_resdrawopt           << std::endl;
    std::cout << " m_prefixlist             = " << m_prefixlist           << std::endl;
    std::cout << " m_suffixlist             = " << m_suffixlist           << std::endl;
    std::cout << " m_num_suffix             = " << m_num_suffix           << std::endl;
    std::cout << " m_den_suffix             = " << m_den_suffix           << std::endl;
    std::cout << " m_num_prefix             = " << m_num_prefix           << std::endl;
    std::cout << " m_den_prefix             = " << m_den_prefix           << std::endl;
    std::cout << " m_legopt                 = " << m_legopt               << std::endl;
    std::cout << " m_title                  = " << m_title                << std::endl;
    std::cout << " m_blinding               = " << m_blinding             << std::endl;
    std::cout << " m_blind_sample           = " << m_blind_sample         << std::endl;
    std::cout << " m_new_config_format      = " << m_new_config_format    << std::endl;
    std::cout << " m_new_sample_format      = " << m_new_sample_format    << std::endl;
    std::cout << " m_new_variable_format    = " << m_new_variable_format  << std::endl;
    std::cout << " m_new_style_format       = " << m_new_style_format     << std::endl;
    std::cout << " m_new_filelist_format    = " << m_new_filelist_format  << std::endl;
    std::cout << " m_do1DPlots              = " << m_do1DPlots            << std::endl;
    std::cout << " m_doProjections          = " << m_doProjections        << std::endl;
    std::cout << " m_doEff                  = " << m_doEff                << std::endl;
    std::cout << " m_show_yields            = " << m_show_yields          << std::endl;
    std::cout << " m_resmin                 = " << m_resmin               << std::endl;
    std::cout << " m_resmax                 = " << m_resmax               << std::endl;
    std::cout << " m_ymin                   = " << m_ymin                 << std::endl;
    std::cout << " m_ymax                   = " << m_ymax                 << std::endl;
    std::cout << " m_xmin                   = " << m_xmin                 << std::endl;
    std::cout << " m_xmax                   = " << m_xmax                 << std::endl;
    std::cout << " m_titlexmin              = " << m_titlexmin            << std::endl;
    std::cout << " m_titleymin              = " << m_titleymin            << std::endl;
    std::cout << " m_titlexmax              = " << m_titlexmax            << std::endl;
    std::cout << " m_titleymax              = " << m_titleymax            << std::endl;
    std::cout << " m_global_scale           = " << m_global_scale         << std::endl;
    std::cout << " m_blind_threshold        = " << m_blind_threshold      << std::endl;
    std::cout << " m_blind_criterion        = " << m_blind_criterion      << std::endl;
    std::cout << " m_yield_format           = " << m_yield_format         << std::endl;

    std::cout << "============================================="          << std::endl;
    std::cout << "" << std::endl;

    return;
}
