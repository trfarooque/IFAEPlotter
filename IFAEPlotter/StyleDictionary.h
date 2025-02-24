#ifndef STYLEDICTIONARY_H
#define STYLEDICTIONARY_H

#include <map>
#include <string>
#include "Rtypes.h"

class Plotter_Options;

struct StyleStruct{
  std::string key;
  Color_t lineColour;
  Style_t lineStyle;
  Width_t lineWidth;
  Color_t fillColour;
  Style_t fillStyle;
  Color_t markerColour;
  Style_t markerStyle;
  Size_t markerSize;
};

class StyleDictionary{

 public:
  StyleDictionary(const std::string& name);
  ~StyleDictionary(){ }

  int ParseStyleConfig(Plotter_Options* opt);
  void AddStyle(const std::string& key); 
  void AddStyle(const std::string& key, const std::string& val_lnCol, int val_lnStyle, int val_lnWdt
		, const std::string& val_markCol, int val_markStyle, int val_markWdt
		, const std::string& val_fillCol, int val_fillStyle); 
  void RemoveStyle(const std::string& key); 

  void SetLineColour(const std::string &key, const std::string &value);
  void SetLineColour(const std::string &key, Color_t colour);

  void SetFillColour(const std::string &key, const std::string &value);
  void SetFillColour(const std::string &key, Color_t colour);

  void SetMarkerColour(const std::string &key, const std::string &value);
  void SetMarkerColour(const std::string &key, Color_t colour);

  void SetLineStyle(const std::string &key, int value);
  void SetLineStyle(const std::string &key, Style_t style);

  void SetFillStyle(const std::string &key, int value);
  void SetFillStyle(const std::string &key, Style_t style);

  void SetMarkerStyle(const std::string &key, int value);
  void SetMarkerStyle(const std::string &key, Style_t style);

  void SetLineWidth(const std::string &key, int value);
  void SetLineWidth(const std::string &key, Width_t width);

  void SetMarkerSize(const std::string &key, int value);
  void SetMarkerSize(const std::string &key, Size_t size);

  std::string Name() const{return m_name;}
  Color_t LineColour(const std::string& key) const;
  Width_t LineWidth(const std::string& key) const;
  Style_t LineStyle(const std::string& key) const;
  Color_t FillColour(const std::string& key) const;
  Style_t FillStyle(const std::string& key) const;
  Color_t MarkerColour(const std::string& key) const;
  Style_t MarkerStyle(const std::string& key) const;
  Size_t MarkerSize(const std::string& key) const;
  unsigned int NStyles() const{ return m_style_map.size(); }
  static Color_t ParseColourString(const std::string& value);


 private:

  std::string m_name;
  std::map<std::string, StyleStruct> m_style_map;

};

#endif
