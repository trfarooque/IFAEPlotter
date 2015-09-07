#!/bin/bash

sampleFile="IFAEPlotter/share/test_sample_config.txt"
varFile="IFAEPlotter/share/test_variable_config.txt"
styleLib="IFAEPlotter/share/test_style_config.txt"
fList="IFAEPlotter/share/test_filelist.txt"
outFormat="png,ROOT"
yLabel="Events"
resLabel="Data/MC"
resMin="0.2"
resMax="1.8"
resDrawOpt="e0"
outFile="testRootFile"
msg="DEBUG"
showYields=1
plot --SAMPLES=${sampleFile} --VARIABLES=${varFile} --STYLELIB=${styleLib} --FILELIST=${fList} --OUTFORMAT=${outFormat} --YLABEL=${yLabel} --RESLABEL=${resLabel} --OUTPUTFILE=${outFile} --MSGLEVEL=${msg} --SHOWYIELDS=${showYields}
