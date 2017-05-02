#include "IFAEPlotter/SampleAttributes.h"
#include "IFAEPlotter/Plotter_Options.h"
#include "IFAETopFramework/AnalysisUtils.h"

#include <iostream>

SampleAttributes::SampleAttributes(const std::string& name, const std::string& suffix, const std::string& leglabel, const std::string& stylekey
				   , const std::string& drawopt, const std::string& legopt
				   , const std::string& drawscale, const std::string& scale_to_ref
				   , bool draw_stack, bool do_sum, int res_opt, const std::string& resdrawopt
				   , const std::string& blinding, const std::string& print_format
				   , bool write, const std::string& outfile_name
				   , const std::string& in_pattern, const std::string& in_suffix, const std::string& in_prefix
				   , bool no_shape, const std::string& print_value) : 
  m_name(name),
  m_suffix(suffix),
  m_leglabel(leglabel),
  m_stylekey(stylekey),
  m_drawopt(drawopt),
  m_res_drawopt(resdrawopt),
  m_drawscale(drawscale),
  m_scale_to_ref(scale_to_ref),
  m_legopt(legopt),
  m_blinding(blinding),
  m_print_format(print_format),
  m_outfile_name(outfile_name),
  m_in_pattern(in_pattern),
  m_in_suffix(in_suffix),
  m_in_prefix(in_prefix),
  m_print_value(print_value),
  m_draw_stack(draw_stack),
  m_do_sum(do_sum),
  m_res_opt(res_opt),
  m_write(write),
  m_no_shape(no_shape)
{}

SampleAttributes::SampleAttributes() : SampleAttributes("", "", "", ""){}

SampleAttributes::SampleAttributes(SampleAttributes& q){

  m_name             = q.m_name;
  m_suffix           = q.m_suffix;
  m_leglabel         = q.m_leglabel;
  m_stylekey         = q.m_stylekey;
  m_drawopt          = q.m_drawopt;
  m_res_drawopt      = q.m_res_drawopt;
  m_drawscale        = q.m_drawscale;
  m_scale_to_ref     = q.m_scale_to_ref;
  m_legopt           = q.m_legopt;
  m_blinding         = q.m_blinding;
  m_print_format     = q.m_print_format;
  m_outfile_name     = q.m_outfile_name;
  m_in_pattern       = q.m_in_pattern;
  m_in_suffix        = q.m_in_suffix;
  m_in_prefix        = q.m_in_prefix;
  m_print_value      = q.m_print_value;
  m_draw_stack       = q.m_draw_stack;
  m_do_sum           = q.m_do_sum;
  m_res_opt          = q.m_res_opt;
  m_write            = q.m_write;
  m_no_shape         = q.m_no_shape;

}

SampleAttributes::~SampleAttributes(){}

SampleAttributesMap SampleAttributes::ParseSampleConfig( Plotter_Options* opt ){

  const std::string& config_sample = opt->ConfigSample(); 
  const std::string& delim = opt->ConfigDelim();
  bool block_format = false;
  if( opt->OptStr().find("--NEWSAMPLECONFIG") != std::string::npos ){ block_format = opt->NewSampleFormat(); }
  else if( opt->OptStr().find("--NEWCONFIG") != std::string::npos ){ block_format = opt->NewConfigFormat(); }

  SampleAttributesMap samp_map; samp_map.clear();  
  std::vector<std::map<std::string, std::string> > parsed_map;
  int nline = AnalysisUtils::ParseConfigFile(config_sample, parsed_map, delim, block_format); 
  if(nline < 0){ std::cout<<"Sample configuration file could not be opened. Exiting"<<std::endl; return samp_map; }
  std::string name = "";

  for(int l = 0; l < nline; l++){
    name = "";

    std::map<std::string, std::string>& keymap = parsed_map.at(l);

    if( keymap.find("NAME") == keymap.end() ){ 
      std::cout<<"Error : No name found for sample in block "<<l<<". Ignoring."<<std::endl; 
      continue;
    }
    SampleAttributes* sampleObj = new SampleAttributes();
    name = keymap["NAME"];
    sampleObj->SetName( name ); 


    if( keymap.find("INPATTERN") != keymap.end() ){ sampleObj->SetInPattern(keymap["INPATTERN"]); }
    if( keymap.find("INSUFFIX") != keymap.end() ){ sampleObj->SetInSuffix(keymap["INSUFFIX"]); }
    if( keymap.find("INPREFIX") != keymap.end() ){ sampleObj->SetInPrefix(keymap["INPREFIX"]); }

    if( keymap.find("SUFFIX") != keymap.end() ){ sampleObj->SetSuffix(keymap["SUFFIX"]); }
    else{ sampleObj->SetSuffix( name ); }

    if( keymap.find("LEGLABEL") != keymap.end() ){ sampleObj->SetLegLabel(keymap["LEGLABEL"]); }

    if( keymap.find("STYLEKEY") != keymap.end() ){ sampleObj->SetStyleKey(keymap["STYLEKEY"]); }
    if( keymap.find("DRAWOPT") != keymap.end() ){ sampleObj->SetDrawOpt(keymap["DRAWOPT"]); }
    if( keymap.find("RESDRAWOPT") != keymap.end() ){ sampleObj->SetResDrawOpt(keymap["RESDRAWOPT"]); }
    if( keymap.find("LEGOPT") != keymap.end() ){ sampleObj->SetLegOpt(keymap["LEGOPT"]); }
    if( keymap.find("DRAWSCALE") != keymap.end() ){ sampleObj->SetDrawScale(keymap["DRAWSCALE"]); }
    if( keymap.find("SCALETOREF") != keymap.end() ){ sampleObj->SetScaleToRef(keymap["SCALETOREF"]); }
    if( keymap.find("DRAWSTACK") != keymap.end() ){ sampleObj->SetDrawStack(AnalysisUtils::BoolValue(keymap["DRAWSTACK"], "DRAWSTACK")); }
    if( keymap.find("DOSUM") != keymap.end() && (keymap["DOSUM"] != "") ){ sampleObj->SetDoSum(AnalysisUtils::BoolValue(keymap["DOSUM"], "DOSUM")); }
    else{ sampleObj->SetDoSum( sampleObj->DrawStack() ); }

    if( keymap.find("RESOPT") != keymap.end() ){ 
      if(keymap["RESOPT"] == "SKIP"){sampleObj->SetResOpt(-1);}
      else if(keymap["RESOPT"] == "INC"){sampleObj->SetResOpt(0);}
      else if(keymap["RESOPT"] == "REF"){sampleObj->SetResOpt(1);}
      else{ std::cout<<"UNknown value "<<keymap["RESOPT"]<<" for RESOPT option"<<std::endl; }
    }
    if( keymap.find("BLINDING") != keymap.end() ){ sampleObj->SetBlinding(keymap["BLINDING"]); }
    if( keymap.find("YIELDFORMAT") != keymap.end() ){ sampleObj->SetPrintFormat(keymap["YIELDFORMAT"]); }
    if( keymap.find("PRINTFORMAT") != keymap.end() ){ sampleObj->SetPrintFormat(keymap["PRINTFORMAT"]); }
    if( keymap.find("PRINTVALUE") != keymap.end() ){ sampleObj->SetPrintValue(keymap["PRINTVALUE"]); }
    if( keymap.find("WRITE") != keymap.end() ){ sampleObj->SetWrite(AnalysisUtils::BoolValue(keymap["WRITE"], "WRITE")); }
    if( keymap.find("OUTFILENAME") != keymap.end() ){ sampleObj->SetOutFileName(keymap["OUTFILENAME"]); }
    if( keymap.find("NOSHAPE") != keymap.end() ){ sampleObj->SetNoShape(AnalysisUtils::BoolValue(keymap["NOSHAPE"], "NOSHAPE")); }


    //---------- read all parameters------
    if(opt->MsgLevel() == Debug::DEBUG) std::cout<<" Adding sample "<<name<<std::endl;
    samp_map[name] = sampleObj;
    keymap.clear();
  }

  if(opt->MsgLevel() == Debug::DEBUG) std::cout<<"SampleAttributes::ParseSampleConfig : nline = "<<nline<<" samp_map.size() = "<<samp_map.size()<<std::endl; 

  parsed_map.clear();
  return samp_map;

}
