#!/bin/bash

fList="${PWD}/SIGPLOT_file_list.txt"

styleLib="${PWD}/IFP_shape_style_lib_lines.txt"

newConfig="TRUE"
newFileList="FALSE"
showYields="TRUE"
showSeparation="FALSE"

outFormat="PNG,EPS"
lumi=36500.
ylabel="Arbitrary Units"
ttl_xmin=0.180
ttl_ymin=0.78
ttl_xmax=0.38
ttl_ymax=0.9125 
y_title_off=1.5 

legend_size=0.04
maxxleg=0.96
minxleg=0.75
minyleg=0.7

y_label_size=0.3


OutFList="shape_discriminant"
ConfList="${PWD}/Shape_fcnc_sample_config.txt "

variableConfig="${PWD}/FCNC_SHAPE_variable_config.txt"

plot --SAMPLES=${ConfList} --VARIABLES=${variableConfig} --FILELIST=${fList} --STYLELIB=${styleLib}  \
    --NEWCONFIG=${newConfig} --NEWFILELIST=${newFileList} --OUTPUTFOLDER=${OutFList} \
    --SHOWYIELDS=${showYields} --OUTFORMAT=${outFormat} --GLOBALSCALE=${lumi} --YLABEL="Norm. to unity" \
    --TITLEXMIN=${ttl_xmin} --TITLEYMIN=${ttl_ymin} --TITLEXMAX=${ttl_xmax} --TITLEYMAX=${ttl_ymax}  \
    --RESMIN="0.2" --RESMAX="1.8" --RESDRAWOPT="hist" --RESLABEL="Data/MC" --YTITLEOFFSET=${y_title_off} --LEGENDTEXTSIZE=${legend_size} --LEGENDXMAX=${maxxleg} --LEGENDYMAX=0.925 --LEGENDXMIN=0.5  --YMIN=0.0 

    

