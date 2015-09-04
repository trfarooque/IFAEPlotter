#!/bin/bash

sampleFile="IFAEPlotter/share/test_sample_config.txt"
varFile="IFAEPlotter/share/test_variable_config.txt"
styleLib="IFAEPlotter/share/test_style_config.txt"
fList="IFAEPlotter/share/test_filelist.txt"
outFormat="PNG,ROOT"
yLabel="Events"
resLabel="Data/MC"
outFile="testRootFile"
msg="DEBUG"
showYields=1
plot --SAMPLES=${sampleFile} --VARIABLES=${varFile} --STYLELIB=${styleLib} --FILELIST=${fList} --OUTFORMAT=${outFormat} --YLABEL=${yLabel} --RESLABEL=${resLabel} --OUTPUTFILE=${outFile} --MSGLEVEL=${msg} --SHOWYIELDS=${showYields}
