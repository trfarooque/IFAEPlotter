#!/bin/bash

declare -a fList=("SIGPLOT_Dibosons_TRF_closure.txt" "SIGPLOT_Singletop_TRF_closure.txt" "SIGPLOT_SM4tops_TRF_closure.txt" "SIGPLOT_topEW_TRF_closure.txt" "SIGPLOT_ttbb_TRF_closure.txt" "SIGPLOT_ttcc_TRF_closure.txt" "SIGPLOT_ttlight_TRF_closure.txt" "SIGPLOT_uH_TRF_closure.txt" "SIGPLOT_cH_TRF_closure.txt" "SIGPLOT_Wjets22beauty_TRF_closure.txt" "SIGPLOT_Wjets22charm_TRF_closure.txt" "SIGPLOT_Wjets22light_TRF_closure.txt" "SIGPLOT_Zjets22beauty_TRF_closure.txt" "SIGPLOT_Zjets22charm_TRF_closure.txt" "SIGPLOT_Zjets22light_TRF_closure.txt")

styleLib="${PWD}/IFP_shape_style_lib_lines.txt"

newConfig="TRUE"
newFileList="FALSE"
showYields="TRUE"
outFormat="PNG"
lumi=1.00
ylabel="Norm. to 1 pb"
ttl_xmin=0.145
ttl_ymin=0.8
ttl_xmax=0.38
ttl_ymax=0.95 
y_title_off=1. 
maxxleg=0.95

declare -a OutFList=("SIGPLOT_Dibosons_TRF_closure" "SIGPLOT_Singletop_TRF_closure" "SIGPLOT_SM4tops_TRF_closure" "SIGPLOT_topEW_TRF_closure" "SIGPLOT_ttbb_TRF_closure" "SIGPLOT_ttcc_TRF_closure" "SIGPLOT_ttlight_TRF_closure" "SIGPLOT_uH_TRF_closure" "SIGPLOT_cH_TRF_closure" "SIGPLOT_Wjets22beauty_TRF_closure" "SIGPLOT_Wjets22charm_TRF_closure" "SIGPLOT_Wjets22light_TRF_closure" "SIGPLOT_Zjets22beauty_TRF_closure" "SIGPLOT_Zjets22charm_TRF_closure" "SIGPLOT_Zjets22light_TRF_closure")

declare -a ConfList=("${PWD}/Shape_TRF_closure_sample_config.txt" "${PWD}/Shape_TRF_closure_sample_config.txt" "${PWD}/Shape_TRF_closure_sample_config.txt" "${PWD}/Shape_TRF_closure_sample_config.txt" "${PWD}/Shape_TRF_closure_sample_config.txt" "${PWD}/Shape_TRF_closure_sample_config.txt" "${PWD}/Shape_TRF_closure_sample_config.txt" "${PWD}/Shape_TRF_closure_sample_config.txt" "${PWD}/Shape_TRF_closure_sample_config.txt" "${PWD}/Shape_TRF_closure_sample_config.txt" "${PWD}/Shape_TRF_closure_sample_config.txt" "${PWD}/Shape_TRF_closure_sample_config.txt" "${PWD}/Shape_TRF_closure_sample_config.txt" "${PWD}/Shape_TRF_closure_sample_config.txt" "${PWD}/Shape_TRF_closure_sample_config.txt")

variableConfig="${PWD}/Variable_config_hbsm_2017-04-28_trf_rat.txt"

arraylength=${#OutFList[@]}

# use for loop to read all values and indexes
for (( i=1; i<${arraylength}+1; i++ ));
do
    echo $i " / " ${arraylength} " : Plots in " ${OutFList[$i-1]}
    
    plot --SAMPLES=${ConfList[$i-1]} --VARIABLES=${variableConfig} --FILELIST=${fList[$i-1]} --STYLELIB=${styleLib} \
        --NEWCONFIG=${newConfig} --NEWFILELIST=${newFileList} --OUTPUTFOLDER=${OutFList[$i-1]} \
        --SHOWYIELDS=${showYields} --OUTFORMAT=${outFormat} --GLOBALSCALE=${lumi} --YLABEL="Norm. to 1 pb" --XLABEL="LH discriminant" \
        --TITLEXMIN=${ttl_xmin} --TITLEYMIN=${ttl_ymin} --TITLEXMAX=${ttl_xmax} --TITLEYMAX=${ttl_ymax} \
        --RESMIN="0.2" --RESMAX="1.8" --RESLABEL="DT/TRF" --YTITLEOFFSET=${y_title_off} --LEGENDXMAX=${maxxleg}  --LEGENDXMIN=0.7  \

done


