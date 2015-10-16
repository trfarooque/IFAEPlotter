#!/bin/bash

sampleFile="cl_sample_config.txt"
varFile="cl_variable_config.txt"
varFileEFF="cl_variable_config_eff.txt"
varFilePROJ="cl_variable_config_proj.txt"
styleLib="cl_style_config.txt"
fList="cl_filelist.txt"
outFormat="png"
#yLabel="Events"
resLabel="\"Ratio~to~4-2-0\""
resMin="0.5"
resMax="1.5"
resDrawOpt="e1"
msg="DEBUG"
showYields="FALSE"
numPref="Dst_ak4EM_numeff"
denPref="Dst_ak4TR_deneff"
projOpt="HIST,MEAN,RMS"
# ${resLabel}
plot --SAMPLES=${sampleFile} --VARIABLES=${varFile} --STYLELIB=${styleLib} --FILELIST=${fList} --OUTFORMAT=${outFormat} --YLABEL=${yLabel} --RESLABEL="Ratio to 4-2-0" --OUTPUTFILE=${outFile} --MSGLEVEL=${msg} --SHOWYIELDS=${showYields}

#plot --SAMPLES=${sampleFile} --VARIABLES=${varFileEFF} --STYLELIB=${styleLib} --FILELIST=${fList} --OUTFORMAT=${outFormat} --YLABEL=${yLabel} --RESLABEL=${resLabel} --OUTPUTFILE=${outFile} --MSGLEVEL=${msg} --SHOWYIELDS=${showYields} --DO1DPLOTS=0 --DOEFF=1 --NUMPREFIX=${numPref} --DENPREFIX=${denPref}

#plot --SAMPLES=${sampleFile} --VARIABLES=${varFilePROJ} --STYLELIB=${styleLib} --FILELIST=${fList} --OUTFORMAT=${outFormat} --YLABEL=${yLabel} --RESLABEL=${resLabel} --OUTPUTFILE=${outFile} --MSGLEVEL=${msg} --SHOWYIELDS=${showYields} --DO1DPLOTS=0 --DOPROJ=1 --PROJOPT=${projOpt}
