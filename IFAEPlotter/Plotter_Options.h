#ifndef PLOTTER_OPTIONS_H
#define PLOTTER_OPTIONS_H

#include <string>
#include "IFAETopFramework/OptionsBase.h"
#include "IFAETopFramework/AnalysisUtils.h"

class Plotter_Options : public OptionsBase {
    
public:
    
    Plotter_Options();
    Plotter_Options( const Plotter_Options & );
    ~Plotter_Options();
    virtual void PrintOptions();

    inline const std::string& ConfigDelim() const { return m_config_delim; }
    inline const std::string& ConfigSample() const { return m_config_sample; }
    inline const std::string& ConfigVariable() const { return m_config_variable; }
    inline const std::string& FileList() const { return m_file_list; }
    inline const std::string& SystematicsList() const { return m_systematics_list; }
    inline const std::string& StyleLib() const { return m_style_lib; }
    inline const std::string& OutputFormat() const { return m_output_format; }
    inline const std::string& YLabel() const { return m_ylabel; }
    inline const std::string& ResLabel() const { return m_reslabel; }
    inline const std::string& ResDrawOpt() const { return m_resdrawopt; }
    inline const std::string& PrefixList() const { return m_prefixlist; }
    inline const std::string& SuffixList() const { return m_suffixlist; }
    inline const std::string& NumSuffix() const { return m_num_suffix; }
    inline const std::string& DenSuffix() const { return m_den_suffix; }
    inline const std::string& NumPrefix() const { return m_num_prefix; }
    inline const std::string& DenPrefix() const { return m_den_prefix; }
    inline const std::string& ProjOpt() const { return m_projopt; }
    inline const std::string& Title() const { return m_title; }
    inline const std::string& LegOpt() const { return m_legopt; }
    inline const std::string& Blinding() const { return m_blinding; }
    inline const std::string& BlindSample() const { return m_blind_sample; }
    inline const std::string& BlindCriterion() const { return m_blind_criterion; }
    inline const std::string& PrintFormat() const { return m_print_format; } 
    inline const std::string& DistributionsFile()  const { return m_dist_file; }

    inline bool NewConfigFormat() const { return m_new_config_format; }
    inline bool NewSampleFormat() const { return m_new_sample_format; }
    inline bool NewVariableFormat() const { return m_new_variable_format; }
    inline bool NewStyleFormat() const { return m_new_style_format; }
    inline bool NewSystematicsFormat() const { return m_new_systematics_format; }
    inline bool NewFileListFormat() const { return m_new_filelist_format; }
    inline double ResMin() const { return m_resmin; }
    inline double ResMax() const { return m_resmax; }
    inline double YMin() const { return m_ymin; }
    inline double YMax() const { return m_ymax; }
    inline double YScale() const { return m_yscale; }
    inline double XMin() const { return m_xmin; }
    inline double XMax() const { return m_xmax; }
    inline bool ShowYields() const { return m_show_yields; }//backward compatibility
    inline const std::string PrintValue() const { return m_print_value; }
    inline bool WriteHistos() const { return m_write_histos; }
    inline bool Do1DPlots() const { return m_do1DPlots; }
    inline bool DoProjections() const { return m_doProjections; }
    inline bool DoEff() const { return m_doEff; }
    inline bool DoSystematics() const { return m_doSystematics; }
    inline double TitleXMin() const { return m_titlexmin; }
    inline double TitleYMin() const { return m_titleymin; }
    inline double TitleXMax() const { return m_titlexmax; }
    inline double TitleYMax() const { return m_titleymax; }
    inline double TitleTextSize() const { return m_title_textsize; }
    inline double LegendTextSize() const { return m_legend_textsize; }
    inline double LegendXMin() const { return m_legend_xmin; }
    inline double LegendXMax() const { return m_legend_xmax; }
    inline double LegendYMin() const { return m_legend_ymin; }
    inline double LegendYMax() const { return m_legend_ymax; }

    inline double XTitleOffset() const { return m_xtitle_offset; }
    inline double XTitleSize() const { return m_xtitle_size; }
    inline double YTitleOffset() const { return m_ytitle_offset; }
    inline double YTitleSize() const { return m_ytitle_size; }
    inline double ResTitleOffset() const { return m_restitle_offset; }
    inline double ResTitleSize() const { return m_restitle_size; }

    inline double XLabelOffset() const { return m_xlabel_offset; }
    inline double XLabelSize() const { return m_xlabel_size; }
    inline double YLabelOffset() const { return m_ylabel_offset; }
    inline double YLabelSize() const { return m_ylabel_size; }
    inline double ResLabelOffset() const { return m_reslabel_offset; }
    inline double ResLabelSize() const { return m_reslabel_size; }

    inline double BottomMargin() const { return m_bottom_margin; }
    inline double TopMargin() const { return m_top_margin; }
    inline double LeftMargin() const { return m_left_margin; }
    inline double RightMargin() const { return m_right_margin; }

    inline double GlobalScale() const { return m_global_scale; }
    inline double BlindThreshold() const { return m_blind_threshold; }
    inline bool AllFromFile() const { return m_all_from_file; }

protected:
    virtual bool IdentifyOption( const std::string &, const std::string & );
    
private:

    std::string m_config_delim;
    std::string m_config_sample;
    std::string m_config_variable;
    std::string m_file_list;
    std::string m_systematics_list;
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
    std::string m_blinding;
    std::string m_blind_sample;
    std::string m_blind_criterion;
    std::string m_print_format;
    std::string m_dist_file;
    std::string m_print_value;

    bool m_new_config_format;
    bool m_new_sample_format;
    bool m_new_variable_format;
    bool m_new_style_format;
    bool m_new_systematics_format;
    bool m_new_filelist_format;
    bool m_write_histos; 
    bool m_do1DPlots;
    bool m_doProjections;
    bool m_doEff;
    bool m_doSystematics;
    bool m_show_yields;
    bool m_all_from_file;

    double m_resmin;
    double m_resmax;
    double m_ymin;
    double m_ymax;
    double m_yscale;
    double m_xmin;
    double m_xmax;
    double m_titlexmin;
    double m_titleymin;
    double m_titlexmax;
    double m_titleymax;
    double m_title_textsize;
    double m_legend_textsize;

    double m_legend_xmin;
    double m_legend_xmax;
    double m_legend_ymin;
    double m_legend_ymax;

    double m_xtitle_offset;
    double m_xtitle_size;
    double m_ytitle_offset;
    double m_ytitle_size;
    double m_restitle_offset;
    double m_restitle_size;

    double m_xlabel_offset;
    double m_xlabel_size;
    double m_ylabel_offset;
    double m_ylabel_size;
    double m_reslabel_offset;
    double m_reslabel_size;

    double m_bottom_margin;
    double m_top_margin;
    double m_left_margin;
    double m_right_margin;

    double m_global_scale;
    double m_blind_threshold;

};

#endif //PLOTTER_OPTIONS_H
