#ifndef SAMPLEATTRIBUTES_H
#define SAMPLEATTRIBUTES_H

#include <string>
#include <map>
#include <vector>

class SampleAttributes{

 public:
  SampleAttributes();
  SampleAttributes(const std::string& name, const std::string& suffix, const std::string& leglabel, const std::string& stylekey
		   , const std::string& drawopt="hist", const std::string& legopt = ""
		   , const std::string& drawscale="NORM", bool draw_stack=false, bool do_sum=false
		   , int res_opt=-1, const std::string& resdrawopt="" 
		   , const std::string& blinding="NONE", const std::string& yield_format=""
		   , bool write=false, const std::string& outfile_name = ""
		   , const std::string& in_suffix = "", const std::string& in_prefix = "", bool no_shape=false);
  SampleAttributes(SampleAttributes& q);
  ~SampleAttributes();

 private:
  std::string m_name;
  std::string m_suffix;
  std::string m_leglabel;
  std::string m_stylekey;
  std::string m_drawopt;
  std::string m_res_drawopt;
  std::string m_drawscale;
  std::string m_legopt; 
  std::string m_blinding;
  std::string m_yield_format;
  std::string m_outfile_name;
  std::string m_in_suffix;
  std::string m_in_prefix;

  bool m_draw_stack;
  bool m_do_sum;
  int m_res_opt;
  bool m_write;
  bool m_no_shape;

 public:
  void SetName(const std::string& name){ m_name = name; }
  void SetSuffix(const std::string& suffix){ m_suffix = suffix; }
  void SetStyleKey(const std::string& stylekey){ m_stylekey = stylekey; }
  void SetDrawOpt(const std::string& drawopt){ m_drawopt = drawopt; }
  void SetResDrawOpt(const std::string& resdrawopt){ m_res_drawopt = resdrawopt; }
  void SetDrawScale(const std::string& drawscale){ m_drawscale = drawscale; }
  void SetLegLabel(const std::string& leglabel){ m_leglabel = leglabel; }
  void SetLegOpt(const std::string& legopt){ m_legopt = legopt; }
  void SetBlinding(const std::string& blinding){ m_blinding = blinding; }
  void SetYieldFormat(const std::string& yield_format){ m_yield_format = yield_format; }
  void SetDrawStack(bool draw_stack){ m_draw_stack = draw_stack; }
  void SetDoSum(bool do_sum){ m_do_sum = do_sum; }
  void SetResOpt(int res_opt){ m_res_opt = res_opt; }
  void SetWrite(bool write){ m_write = write; }
  void SetOutFileName(const std::string& outfile_name){ m_outfile_name = outfile_name; }
  void SetInSuffix(const std::string& in_suffix){ m_in_suffix = in_suffix; }
  void SetInPrefix(const std::string& in_prefix){ m_in_prefix = in_prefix; }
  void SetNoShape(bool no_shape){ m_no_shape = no_shape; }

  const std::string& Name(){ return m_name; }
  const std::string& Suffix(){ return m_suffix; }
  const std::string& LegLabel(){ return m_leglabel; }
  const std::string& LegOpt(){ return m_legopt; }
  const std::string& StyleKey(){ return m_stylekey; }
  const std::string& DrawOpt(){ return m_drawopt; }
  const std::string& ResDrawOpt(){ return m_res_drawopt; }
  const std::string& DrawScale(){ return m_drawscale; }
  const std::string& Blinding(){ return m_blinding; }
  const std::string& YieldFormat(){ return m_yield_format; }
  const std::string& OutFileName(){ return m_outfile_name; }
  const std::string& InSuffix(){ return m_in_suffix; }
  const std::string& InPrefix(){ return m_in_prefix; }
  bool DrawStack(){ return m_draw_stack; }
  bool DoSum(){ return m_do_sum; }
  int ResOpt(){ return m_res_opt; }
  bool Write(){ return m_write; }
  bool NoShape(){ return m_no_shape; }

};

typedef std::map<std::string, SampleAttributes*> SampleAttributesMap;
typedef std::vector<SampleAttributes*> SampleAttributesVector;

#endif
