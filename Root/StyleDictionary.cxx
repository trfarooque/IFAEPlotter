#include "IFAEPlotter/StyleDictionary.h"
#include "TCanvas.h"
#include <iostream>

StyleDictionary::StyleDictionary(const std::string& name) : m_name(name){

  m_style_map.clear();
}

void StyleDictionary::AddStyle(const std::string& key){

  StyleStruct style_col;
  style_col.key = key;
  style_col.lineColour = kBlack;
  style_col.lineStyle = 1;
  style_col.lineWidth = 1;
  style_col.fillColour = 0;
  style_col.fillStyle = 1;

  m_style_map[key] = style_col;

}

void StyleDictionary::AddStyle(const std::string& key, const std::string& val_lnCol, int val_lnStyle, int val_lnWdt, const std::string& val_fillCol, int val_fillStyle){

  StyleStruct style_col;
  style_col.key = key;
  style_col.lineColour = ParseColourString(val_lnCol);
  style_col.lineStyle = val_lnStyle;
  style_col.lineWidth = val_lnWdt;
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

Color_t StyleDictionary::LineColour(const std::string& key){
  if(m_style_map.find(key) == m_style_map.end() ){
    std::cout<<"Key not found in dictionary"<<std::endl; 
    return kBlack;
  }
  return m_style_map[key].lineColour;
}

Width_t StyleDictionary::LineWidth(const std::string& key){
  if(m_style_map.find(key) == m_style_map.end() ){
    std::cout<<"Key not found in dictionary"<<std::endl; 
    return 0;
  }
  return m_style_map[key].lineWidth;
}

Style_t StyleDictionary::LineStyle(const std::string& key){
  if(m_style_map.find(key) == m_style_map.end() ){
    std::cout<<"Key not found in dictionary"<<std::endl; 
    return 0;
  }
  return m_style_map[key].lineStyle;
}

Color_t StyleDictionary::FillColour(const std::string& key){
  if(m_style_map.find(key) == m_style_map.end() ){
    std::cout<<"Key not found in dictionary"<<std::endl; 
    return kBlack;
  }
  return m_style_map[key].fillColour;
}

Style_t StyleDictionary::FillStyle(const std::string& key){
  if(m_style_map.find(key) == m_style_map.end() ){
    std::cout<<"Key not found in dictionary"<<std::endl; 
    return 0;
  }
  return m_style_map[key].fillStyle;
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
