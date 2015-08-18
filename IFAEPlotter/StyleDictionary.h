#ifndef STYLEDICTIONARY_H
#define STYLEDICTIONARY_H

#include <map>
#include <string>
//#include "TColor.h"
//#include "TStyle.h"
//#include "TWidth.h"
#include "Rtypes.h"


struct StyleStruct{
  std::string key;
  Color_t lineColour;
  Style_t lineStyle;
  Width_t lineWidth;
  Color_t fillColour;
  Style_t fillStyle;
};

class StyleDictionary{

 public:
  StyleDictionary(const std::string& name);
  ~StyleDictionary(){ }

  void AddStyle(const std::string& key); 
  void AddStyle(const std::string& key, const std::string& val_lnCol, int val_lnStyle, int val_lnWdt, const std::string& val_fillCol, int val_fillStyle); 

  void SetLineColour(const std::string &key, const std::string &value);
  void SetLineColour(const std::string &key, Color_t colour);

  void SetFillColour(const std::string &key, const std::string &value);
  void SetFillColour(const std::string &key, Color_t colour);

  void SetLineStyle(const std::string &key, int value);
  void SetLineStyle(const std::string &key, Style_t style);

  void SetFillStyle(const std::string &key, int value);
  void SetFillStyle(const std::string &key, Style_t style);

  void SetLineWidth(const std::string &key, int value);
  void SetLineWidth(const std::string &key, Width_t width);

  std::string Name(){return m_name;}
  Color_t LineColour(const std::string& key);
  Width_t LineWidth(const std::string& key);
  Style_t LineStyle(const std::string& key);
  Color_t FillColour(const std::string& key);
  Style_t FillStyle(const std::string& key);

  Color_t ParseColourString(const std::string& value);


 private:

  std::string m_name;
  std::map<std::string, StyleStruct> m_style_map;

};

#endif
