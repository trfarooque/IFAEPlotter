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

    inline const std::string& ConfigSample() const { return m_config_sample; }
    inline const std::string& ConfigVariable() const { return m_config_variable; }
    inline const std::string& FileList() const { return m_file_list; }
    inline const std::string& StyleLib() const { return m_style_lib; }
    inline const std::string& OutputFormat() const { return m_output_format; }
    inline const std::string& YLabel(){ return m_ylabel; }
    inline const std::string& ResLabel(){ return m_reslabel; }
    inline const std::string& ResDrawOpt(){ return m_resdrawopt; }
    inline double ResMin(){ return m_resmin; }
    inline double ResMax(){ return m_resmax; }
    inline bool ShowYields(){ return m_show_yields; }

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
    double m_resmin;
    double m_resmax;
    bool m_show_yields;

};

#endif //PLOTTER_OPTIONS_H
