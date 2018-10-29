#!/bin/bash

#sampleConfig="${PWD}/VLQ_sample_config_SbyB.txt"
#variableConfig="${PWD}/VLQ_variable_config_SbyB.txt"

#fList="${PWD}/VLQ_file_list_SbyB.txt"
#styleLib="${PWD}/IFP_style_lib_sigbkgd.txt"

sampleConfig="${PWD}/VLQ_sample_config.txt"
variableConfig="${PWD}/VLQ_variable_config.v2.txt"

fList="${PWD}/VLQ_file_list.txt"
styleLib="${PWD}/IFP_style_lib_stacks.txt"

newConfig="TRUE"
newFileList="FALSE"
showYields="FALSE"
outFormat="PNG"
outFolder="VLQ_Presel_NOTRF.v2"
lumi=36470.16
#lumi=3209.
ylabel="Arbitrary Units"
ttl_xmin=0.18
ttl_ymin=0.76
ttl_xmax=0.38
ttl_ymax=0.87 
#blinding="THRESH"
#blindThresh=0.02

plot --SAMPLES=${sampleConfig} --VARIABLES=${variableConfig} --FILELIST=${fList} --STYLELIB=${styleLib} \
--NEWCONFIG=${newConfig} --NEWFILELIST=${newFileList} --OUTPUTFOLDER=${outFolder} \
--SHOWYIELDS=${showYields} --OUTFORMAT=${outFormat} --GLOBALSCALE=${lumi} --YLABEL="Events" \
--TITLEXMIN=${ttl_xmin} --TITLEYMIN=${ttl_ymin} --TITLEXMAX=${ttl_xmax} --TITLEYMAX=${ttl_ymax} #--MSGLEVEL=DEBUG
#--RESMIN="0.2" --RESMAX="1.8" #--PRINTVALUE="MEAN,YIELD,ENTRIES" --MAKEBINSTABLE="TRUE"  #--MAKEMOMENTSTABLE="TRUE"
#--WRITEHISTOS=TRUE #--RESLABEL="Merged/Inclusive"

 # --MSGLEVEL=DEBUG
#--BLINDING=${blinding} --BLINDTHRESHOLD=${blindThresh}
