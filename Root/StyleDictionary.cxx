#include "IFAEPlotter/StyleDictionary.h"
#include "IFAEPlotter/Plotter_Options.h"
#include "IFAETopFramework/AnalysisUtils.h"
#include "TCanvas.h"
#include <iostream>

StyleDictionary::StyleDictionary(const std::string& name) : m_name(name){

  m_style_map.clear();
}

int StyleDictionary::ParseStyleConfig(Plotter_Options* opt){

  const std::string& config_style = opt->StyleLib(); 
  const std::string& delim = opt->ConfigDelim();
  bool block_format = false;
  if( opt->OptStr().find("--NEWSTYLECONFIG") != std::string::npos ){ block_format = opt->NewStyleFormat(); }
  else if( opt->OptStr().find("--NEWCONFIG") != std::string::npos ){ block_format = opt->NewConfigFormat(); }

  int sc = 0; 
  std::vector<std::map<std::string, std::string> > parsed_map;
  int nline = AnalysisUtils::ParseConfigFile(config_style, parsed_map, delim, block_format); 
  if(nline < 0){ std::cout<<"Style configuration file could not be opened. Exiting"<<std::endl; return nline; }

  std::string key = "";
  for(int l = 0; l < nline; l++){

    key = "";
    std::map<std::string, std::string> keymap = parsed_map.at(l);

    if( keymap.find("KEY") != keymap.end() ){ key = keymap["KEY"];}
    else{std::cout<<"Error : No key found for style on line "<<l<<". Ignoring."<<std::endl; continue;}
    AddStyle(key);

    if( ( keymap.find("LINECOLOUR") == keymap.end() ) && ( keymap.find("FILLCOLOUR") == keymap.end() )  && ( keymap.find("MARKERCOLOUR") == keymap.end() ) 
	&& ( keymap.find("COLOUR") == keymap.end() ) ){
      std::cout<<"Error: No colours found for style on line "<<l<<" with key "<<key<<". Ignoring"<<std::endl;

    }

    if( keymap.find("LINECOLOUR") != keymap.end() ){ SetLineColour(key, keymap["LINECOLOUR"]); }
    else if( keymap.find("LINECOLOUR") != keymap.end() ){ SetLineColour(key, keymap["COLOUR"]); } 
    else if( keymap.find("LINECOLOR") != keymap.end() ){ std::cout<<"Field LINECOLOR not recognised. Did you mean LINECOLOUR? #GODSAVETHEQUEEN"<<std::endl; }

    if( keymap.find("LINESTYLE") != keymap.end() ){ SetLineStyle( key, atoi(keymap["LINESTYLE"].c_str()) );}

    if( keymap.find("LINEWIDTH") != keymap.end() ){ SetLineWidth( key, atoi(keymap["LINEWIDTH"].c_str()) );}

    if( keymap.find("FILLCOLOUR") != keymap.end() ){ SetFillColour(key, keymap["FILLCOLOUR"]); }
    else if( keymap.find("FILLCOLOUR") != keymap.end() ){ SetFillColour(key, keymap["COLOUR"]); } 
    else if( keymap.find("FILLCOLOR") != keymap.end() ){ std::cout<<"Field FILLCOLOR not recognised. Did you mean FILLCOLOUR? #GODSAVETHEQUEEN"<<std::endl; }

    if( keymap.find("FILLSTYLE") != keymap.end() ){ SetFillStyle(key, atoi(keymap["FILLSTYLE"].c_str()) );}

    if( keymap.find("MARKERCOLOUR") != keymap.end() ){ SetMarkerColour(key, keymap["MARKERCOLOUR"]); }
    else if( keymap.find("MARKERCOLOUR") != keymap.end() ){ SetMarkerColour(key, keymap["COLOUR"]); } 
    else if( keymap.find("MARKERCOLOR") != keymap.end() ){ std::cout<<"Field MARKERCOLOR not recognised. Did you mean MARKERCOLOUR? #GODSAVETHEQUEEN"<<std::endl; }

    if( keymap.find("MARKERSTYLE") != keymap.end() ){ SetMarkerStyle(key, atoi(keymap["MARKERSTYLE"].c_str()) );}

    if( keymap.find("MARKERSIZE") != keymap.end() ){ SetMarkerSize(key, atoi(keymap["MARKERSIZE"].c_str()) );}


    if( keymap.find("COLOR") != keymap.end() ){ std::cout<<"Field COLOR not recognised. Did you mean COLOUR? #GODSAVETHEQUEEEN"<<std::endl; }
    keymap.clear();
  }

  if(opt->MsgLevel() == Debug::DEBUG) std::cout<<"StyleDictionary::ParseStyleConfig : nline = "<<nline<<" NStyles() = "<<NStyles()<<std::endl; 

  parsed_map.clear();
  return sc;


}

void StyleDictionary::AddStyle(const std::string& key){

  StyleStruct style_col;
  style_col.key = key;
  style_col.lineColour = kBlack;
  style_col.lineStyle = 1;
  style_col.lineWidth = 1;
  style_col.fillColour = 0;
  style_col.fillStyle = 1;
  style_col.markerColour = kBlack;
  style_col.markerStyle = 1;
  style_col.markerSize = 1;
  m_style_map[key] = style_col;

}

void StyleDictionary::AddStyle(const std::string& key, const std::string& val_lnCol, int val_lnStyle, int val_lnWdt
			       , const std::string& val_markCol, int val_markStyle, int val_markSize
			       , const std::string& val_fillCol, int val_fillStyle){

  StyleStruct style_col;
  style_col.key = key;
  style_col.lineColour = ParseColourString(val_lnCol);
  style_col.lineStyle = val_lnStyle;
  style_col.lineWidth = val_lnWdt;
  style_col.markerColour = ParseColourString(val_markCol);
  style_col.markerStyle = val_markStyle;
  style_col.markerSize = val_markSize;
  style_col.fillColour = ParseColourString(val_fillCol);
  style_col.fillStyle = val_fillStyle;

  m_style_map[key] = style_col;

  return;
}

void StyleDictionary::SetLineColour(const std::string &key, const std::string &value){
  SetLineColour(key, ParseColourString(value) );
  return;
}

void StyleDictionary::SetLineColour(const std::string &key, Color_t colour){
  if(m_style_map.find(key) == m_style_map.end() ){ AddStyle(key); }
  m_style_map[key].lineColour = colour;
  return;
}

void StyleDictionary::SetMarkerColour(const std::string &key, const std::string &value){
  SetMarkerColour(key, ParseColourString(value) );
  return;
}

void StyleDictionary::SetMarkerColour(const std::string &key, Color_t colour){
  if(m_style_map.find(key) == m_style_map.end() ){ AddStyle(key); }
  m_style_map[key].markerColour = colour;
  return;
}

void StyleDictionary::SetFillColour(const std::string &key, const std::string &value){
  SetFillColour(key, ParseColourString(value) );
  return;
}

void StyleDictionary::SetFillColour(const std::string &key, Color_t colour){
  if(m_style_map.find(key) == m_style_map.end() ){ AddStyle(key); }
  m_style_map[key].fillColour = colour;
  return;
}

void StyleDictionary::SetLineStyle(const std::string &key, int value){
  if(m_style_map.find(key) == m_style_map.end() ){ AddStyle(key); }
  m_style_map[key].lineStyle = value;
  return;
}

void StyleDictionary::SetLineStyle(const std::string &key, Style_t style){
  if(m_style_map.find(key) == m_style_map.end() ){ AddStyle(key); }
  m_style_map[key].lineStyle = style;
  return;
}

void StyleDictionary::SetMarkerStyle(const std::string &key, int value){
  if(m_style_map.find(key) == m_style_map.end() ){ AddStyle(key); }
  m_style_map[key].markerStyle = value;
  return;
}

void StyleDictionary::SetMarkerStyle(const std::string &key, Style_t style){
  if(m_style_map.find(key) == m_style_map.end() ){ AddStyle(key); }
  m_style_map[key].markerStyle = style;
  return;
}

void StyleDictionary::SetFillStyle(const std::string &key, int value){
  if(m_style_map.find(key) == m_style_map.end() ){ AddStyle(key); }
  m_style_map[key].fillStyle = value;
  return;
}

void StyleDictionary::SetFillStyle(const std::string &key, Style_t style){
  if(m_style_map.find(key) == m_style_map.end() ){ AddStyle(key); }
  m_style_map[key].fillStyle = style;
  return;
}

void StyleDictionary::SetMarkerSize(const std::string &key, int value){
  if(m_style_map.find(key) == m_style_map.end() ){ AddStyle(key); }
  m_style_map[key].markerSize = value;
  return;
}

void StyleDictionary::SetMarkerSize(const std::string &key, Size_t size){
  if(m_style_map.find(key) == m_style_map.end() ){ AddStyle(key); }
  m_style_map[key].markerSize = size;
  return;
}

void StyleDictionary::SetLineWidth(const std::string &key, int value){
  if(m_style_map.find(key) == m_style_map.end() ){ AddStyle(key); }
  m_style_map[key].lineWidth = value;
  return;
}

void StyleDictionary::SetLineWidth(const std::string &key, Width_t width){
  if(m_style_map.find(key) == m_style_map.end() ){ AddStyle(key); }
  m_style_map[key].lineWidth = width;
  return;
}

Color_t StyleDictionary::LineColour(const std::string& key) const{
  if(m_style_map.find(key) == m_style_map.end() ){
    std::cout<<"Key "<<key<<" not found in dictionary"<<std::endl; 
    return kBlack;
  }
  return m_style_map.at(key).lineColour;
}

Width_t StyleDictionary::LineWidth(const std::string& key) const{
  if(m_style_map.find(key) == m_style_map.end() ){
    std::cout<<"Key "<<key<<" not found in dictionary"<<std::endl; 
    return 0;
  }
  return m_style_map.at(key).lineWidth;
}

Style_t StyleDictionary::LineStyle(const std::string& key) const{
  if(m_style_map.find(key) == m_style_map.end() ){
    std::cout<<"Key "<<key<<" not found in dictionary"<<std::endl; 
    return 0;
  }
  return m_style_map.at(key).lineStyle;
}

Color_t StyleDictionary::MarkerColour(const std::string& key) const{
  if(m_style_map.find(key) == m_style_map.end() ){
    std::cout<<"Key "<<key<<" not found in dictionary"<<std::endl; 
    return kBlack;
  }
  return m_style_map.at(key).markerColour;
}

Size_t StyleDictionary::MarkerSize(const std::string& key) const{
  if(m_style_map.find(key) == m_style_map.end() ){
    std::cout<<"Key "<<key<<" not found in dictionary"<<std::endl; 
    return 0;
  }
  return m_style_map.at(key).markerSize;
}

Style_t StyleDictionary::MarkerStyle(const std::string& key) const{
  if(m_style_map.find(key) == m_style_map.end() ){
    std::cout<<"Key "<<key<<" not found in dictionary"<<std::endl; 
    return 0;
  }
  return m_style_map.at(key).markerStyle;
}



Color_t StyleDictionary::FillColour(const std::string& key) const{
  if(m_style_map.find(key) == m_style_map.end() ){
    std::cout<<"Key "<<key<<" not found in dictionary"<<std::endl; 
    return kBlack;
  }
  return m_style_map.at(key).fillColour;
}

Style_t StyleDictionary::FillStyle(const std::string& key) const{
  if(m_style_map.find(key) == m_style_map.end() ){
    std::cout<<"Key "<<key<<" not found in dictionary"<<std::endl; 
    return 0;
  }
  return m_style_map.at(key).fillStyle;
}



Color_t StyleDictionary::ParseColourString(const std::string& value){

  Color_t base_colour = kBlack;
  Color_t colour = kBlack;

  std::string base = ""; 
  int displ = 0;  
  bool add = true;
  if(value.find("+") <= value.size()){
    base = value.substr( 0 ,value.find("+") );
    displ = atoi( value.substr(value.find("+"), value.size()).c_str() ); 
  } 
  else if(value.find("-") <= value.size()){
    base = value.substr( 0 ,value.find("-") );
    displ = atoi( value.substr(value.find("-"), value.size()).c_str() ); 
  }
  else{ base = value; } 

  if(base == "kBlack"){ base_colour = kBlack; }
  else if(base == "kWhite"){ base_colour = kWhite; }
  else if(base == "kGray"){ base_colour = kGray; }
  else if(base == "kRed"){ base_colour = kRed; }
  else if(base == "kPink"){ base_colour = kPink; }
  else if(base == "kRed"){ base_colour = kRed; }
  else if(base == "kMagenta"){ base_colour = kMagenta; }
  else if(base == "kViolet"){ base_colour = kViolet; }
  else if(base == "kBlue"){ base_colour = kBlue; }
  else if(base == "kAzure"){ base_colour = kAzure; }
  else if(base == "kCyan"){ base_colour = kCyan; }
  else if(base == "kTeal"){ base_colour = kTeal; }
  else if(base == "kGreen"){ base_colour = kGreen; }
  else if(base == "kSpring"){ base_colour = kSpring; }
  else if(base == "kYellow"){ base_colour = kYellow; }
  else if(base == "kOrange"){ base_colour = kOrange; }

  if(add){colour = base_colour + displ;}
  else {colour = base_colour - displ;}

  return colour;

}
