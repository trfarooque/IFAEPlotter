#!/bin/bash

sampleFile="$ROOTCOREBIN/data/IFAEPlotter/test_sample_config.txt"
varFile="$ROOTCOREBIN/data/IFAEPlotter/test_variable_config.txt"
styleLib="$ROOTCOREBIN/data/IFAEPlotter/test_style_config.txt"
fList="$ROOTCOREBIN/data/IFAEPlotter/test_filelist.txt"
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
