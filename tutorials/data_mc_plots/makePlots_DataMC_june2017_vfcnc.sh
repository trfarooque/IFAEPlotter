#!/bin/bash

fList="${PWD}/DATA_MC_file_list_2017-04-20_hbsm_june2017_vfcnc.txt"

styleLib="${PWD}/IFP_style_lib_stacks.txt"

newConfig="TRUE"
newFileList="FALSE"
showYields="TRUE"
outFormat="EPS,PNG"
lumi=36100.
ylabel="Arbitrary Units"
ttl_xmin=0.18
ttl_ymin=0.76
ttl_xmax=0.38
ttl_ymax=0.87 
y_title_off=1.1 
maxxleg=0.85

declare -a OutFList=("data_mc_test")
declare -a ConfList=("${PWD}/Shape_DataMC_sample_config_june2017_vfcnc.txt")

variableConfig="${PWD}/Variable_config_hbsm_2017-06-28_datamc_fcnc.txt"

arraylength=${#OutFList[@]}

# use for loop to read all values and indexes
for (( i=1; i<${arraylength}+1; i++ ));
do
    echo $i " / " ${arraylength} " : Plots in " ${OutFList[$i-1]}
    
    plot --SAMPLES=${ConfList[$i-1]} --VARIABLES=${variableConfig} --FILELIST=${fList} --STYLELIB=${styleLib} \
	--NEWCONFIG=${newConfig} --NEWFILELIST=${newFileList} --OUTPUTFOLDER=${OutFList[$i-1]} \
	--SHOWYIELDS=${showYields} --OUTFORMAT=${outFormat} --GLOBALSCALE=${lumi} --YLABEL="Events" \
	--TITLEXMIN=${ttl_xmin} --TITLEYMIN=${ttl_ymin} --TITLEXMAX=${ttl_xmax} --TITLEYMAX=${ttl_ymax} \
	--RESMIN="0.2" --RESMAX="1.8" ---RESLABEL="Data/MC" --YTITLEOFFSET=${y_title_off} --LEGENDXMAX=${maxxleg}     
  
done