#ifndef PLOTTER_OPTIONS_H
#define PLOTTER_OPTIONS_H

#include <string>
#include "IFAETopFramework/OptionsBase.h"

class Plotter_Options : public OptionsBase {
    
public:
    
    Plotter_Options();
    Plotter_Options( const Plotter_Options & );
    ~Plotter_Options();
    virtual void PrintOptions();
    static bool BoolValue(std::string& arg_val, bool& bin_val);

    inline const std::string& ConfigSample() const { return m_config_sample; }
    inline const std::string& ConfigVariable() const { return m_config_variable; }
    inline const std::string& FileList() const { return m_file_list; }
    inline const std::string& StyleLib() const { return m_style_lib; }
    inline const std::string& OutputFormat() const { return m_output_format; }
    inline const std::string& YLabel(){ return m_ylabel; }
    inline const std::string& ResLabel(){ return m_reslabel; }
    inline const std::string& ResDrawOpt(){ return m_resdrawopt; }
    inline const std::string& PrefixList(){ return m_prefixlist; }
    inline const std::string& SuffixList(){ return m_prefixlist; }
    inline const std::string& NumSuffix(){ return m_num_suffix; }
    inline const std::string& DenSuffix(){ return m_den_suffix; }
    inline const std::string& NumPrefix(){ return m_num_prefix; }
    inline const std::string& DenPrefix(){ return m_den_prefix; }
    inline const std::string& ProjOpt(){ return m_projopt; }
    inline const std::string& Title(){ return m_title; }
    inline const std::string& LegOpt(){ return m_legopt; }

    inline bool NewConfigFormat(){ return m_new_config_format; }
    inline bool NewSampleFormat(){ return m_new_sample_format; }
    inline bool NewVariableFormat(){ return m_new_variable_format; }
    inline bool NewStyleFormat(){ return m_new_style_format; }
    inline bool NewFileListFormat(){ return m_new_filelist_format; }
    inline double ResMin(){ return m_resmin; }
    inline double ResMax(){ return m_resmax; }
    inline double YMin(){ return m_ymin; }
    inline double YMax(){ return m_ymax; }
    inline double XMin(){ return m_xmin; }
    inline double XMax(){ return m_xmax; }
    inline bool ShowYields(){ return m_show_yields; }
    inline bool Do1DPlots(){ return m_do1DPlots; }
    inline bool DoProjections(){ return m_doProjections; }
    inline bool DoEff(){ return m_doEff; }
    inline double TitleXMin(){ return m_titlexmin; }
    inline double TitleYMin(){ return m_titleymin; }
    inline double TitleXMax(){ return m_titlexmax; }
    inline double TitleYMax(){ return m_titleymax; }

protected:
    virtual bool IdentifyOption( const std::string &, const std::string & );
    
private:

    std::string m_config_sample;
    std::string m_config_variable;
    std::string m_file_list;
    std::string m_style_lib;
    std::string m_output_format;
    std::string m_ylabel;
    std::string m_reslabel;
    std::string m_resdrawopt;
    std::string m_prefixlist;
    std::string m_suffixlist;
    std::string m_num_suffix;
    std::string m_den_suffix;
    std::string m_num_prefix;
    std::string m_den_prefix;
    std::string m_projopt;
    std::string m_legopt;
    std::string m_title;

    bool m_new_config_format;
    bool m_new_sample_format;
    bool m_new_variable_format;
    bool m_new_style_format;
    bool m_new_filelist_format;
    bool m_do1DPlots;
    bool m_doProjections;
    bool m_doEff;
    bool m_show_yields;

    double m_resmin;
    double m_resmax;
    double m_ymin;
    double m_ymax;
    double m_xmin;
    double m_xmax;
    double m_titlexmin;
    double m_titleymin;
    double m_titlexmax;
    double m_titleymax;

};

#endif //PLOTTER_OPTIONS_H
