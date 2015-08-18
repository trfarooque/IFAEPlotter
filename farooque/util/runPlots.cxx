#include "IFAEPlotter/PlotManager.h"

int main(int argc, char** argv){
  
  PlotManager* plotMngr = new PlotManager("test_sample_config.txt", "test_filelist.txt", "test_variable_config.txt", "test_style_config.txt"); 
  plotMngr->Execute();
  plotMngr->Terminate();
  return 0;
  
}
