Installation
------------

```
rcSetup Base,2.3.50
git clone ssh://git@gitlab.cern.ch:7999/htx/IFAEPlotter.git
git clone ssh://git@gitlab.cern.ch:7999/htx/IFAETopFramework.git
# if you want to get a tagged version
# cd IFAETopFramework; git checkout IFAETopFramework-XX-YY-ZZ; cd -
rc checkout IFAETopFramework/share/packages.txt
rc find_packages
rc clean
rc compile
```

Currently recommended versions
----------------------------
See the most recent tag in the tab 'tag' above.

Help and support
--------------------
```diff
trisha.farooque@SPAMNOTcern.ch
```

Overview
--------------

IFAEPlotter is designed to read in configuration files and make overlay plots from ROOT histograms. 
It currently supports the following features:
1. Overlaying TH1 for different samples onto a TCanvas.
2. Making efficiency histograms from provided numerator and denominator histogram name patterns
3. Projecting 2D histograms into 1D slices and making overlay plots out of them for various samples.

***Only the first option is currently documented***

Example of tutorials 
--------------

Three tutorials are given under 

    https://gitlab.cern.ch/htx/IFAEPlotter/tree/tutorials_1/tutorials

***data_mc_plots***: perform a set of data MC plots based on input files available either from EOS or at3. After setting up the package run as 
    
    source  makePlots_DataMC_june2017_vfcnc.sh

***mc_to_mc_plots***: perform a set of MC plots comparing shapes of discriminating variables between signal and background; based on input files available either from EOS or at3. After setting up the package run as 
    
    source  makePlots_new_MCMC.sh

***trf_closure***: perform a set of MC plots showing TRF closure checks for several samples; based on input files available either from EOS or at3. After setting up the package run as 
    
    source  makePlots_shape_trf.sh

Those tutorials include example of how to rebin a plot, check for example 
    
    tutorials/trf_closure/Variable_config_hbsm_2017-04-28_trf_rat.txt   

and compare with description of options REBIN and REBINVAR below. Example of output plots were discussed here https://indico.cern.ch/event/732674/contributions/3021400/attachments/1660374/2660159/fcnc_low_mass_hplus_intro.pdf slide 6


Configuration files
----------------------
IFAEPlotter will take in the following configuration files as input:
1. A sample configuration file - This list all the samples that are to be overlaid on each plot 
(e.g. data, signal, various backgrounds). The attributes of each sample are controlled by the 
parameters given herein.

2. A variable configuration file - This lists the names of each histogram to be plotted and the 
associated plotting options. 

3. A file list : This lists the files associated to each sample and, optionally, a normalisation scaling factor 
associated to each file. For example, if one uses dijet pT slices (JZ0, JZ1, ...) to represent a multijet background, 
then each pT slice may result in a different histogram file, and each file would be associated to the cross section 
weight of that slice. If all of these files are associated to a single sample ID, then they will be added together 
(properly weighted) and drawn as a single histogram in the overlay plot.

4. Style library : This is a common file that can be reused by several programs. Each line in the style library associates 
a style set (line colour, marker style, etc.) with a key. The sample configuration should associate this key with a given sample.


Input format
-----------------------
In the current implementation, histograms for different samples should reside in different histograms - although, 
as noted above, one can have multiple input files per sample. The histograms in each file for a given variable 
must have identical names.


Configuration parameters
--------------------------

### 1. Global options:

In general, global options are applied to all samples and variables. If the same option exists in a config 
file, the config file option will override the global option for a given sample/variable.

----

--SAMPLES : The sample configuration file

--VARIABLES : The variable configuration file

--FILELIST : The file list for samples

--STYLELIB : Style library to use

--OUTFORMAT(DEFAULT="PNG") : Comma-separated list of all formats in which the plots should be printed. 
Currently supports eps ("EPS"), png ("PNG"), pdf ("PDF"), .C ("CPP") and .root ("ROOT") files.

--YLABEL(DEFAULT="Events") : Title of Y-axis. Overriden by variable configuration.

--RESLABEL(DEFAULT="Data/MC") : Title of Y-axis in bottom panel. Overriden by variable configuration.

--RESDRAWOPT(DEFAULT=NONE): Drawing option for residual histogram in bottom panel. 
Overriden by variable configuration, which is in turn overriden by sample configuration. 
If RESDRAWOPT is not provided anywhere, use the drawing option for each sample as given in the sample configuration.

----

--NEWCONFIG(DEFAULT=FALSE) : Switch to use new config file format. 

--NEWSAMPLECONFIG(DEFAULT=FALSE) : Switch to use new file format for sample configuration. 

--NEWVARIABLECONFIG(DEFAULT=FALSE) : Switch to use new file format for variable configuration. 

--NEWSTYLECONFIG(DEFAULT=FALSE) : Switch to use new file format for style library. 

--NEWFILELIST(DEFAULT=FALSE) : Switch to use new format for file list. 

----

--WRITEHISTOS(DEFAULT=FALSE) : Write histograms for specified samples in output ROOT file.

--DO1DPLOTS(DEFAULT=TRUE) : Create overlay plots for TH1Ds by sample

--DOPROJ(DEFAULT=FALSE) : Create overlays of bin projections of TH2Ds by sample and make plots of MEAN/RMS/FRACRMS

--DOEFF(DEFAULT=FALSE) : Create overlays of efficiency plots by samples from specified numerator and denominator TH1Ds.

----

--RESMIN(DEFAULT=NONE) : Minimum of y-axis range for bottom panel

--RESMAX(DEFAULT=NONE) : Maximum of y-axis range for bottom panel

--YMIN(DEFAULT=NONE) : Minimum of y-axis range. Overriden by variable configuration.

--YMAX(DEFAULT=NONE) : Maximum of y-axis range. Overriden by variable configuration. Has precedence over YSCALE.

--YSCALE(DEFAULT=NONE) : Scale by which the maximum of the overlaid histograms is stretched. Overriden by variable configuration.

--XMIN(DEFAULT=NONE) : Minimum of x-axis range. Overriden by variable configuration.

--XMAX(DEFAULT=NONE) : Maximum of x-axis range. Overriden by variable configurati

----

--SHOWYIELDS(DEFAULT=FALSE) : Show yields in the legend. TRUE or FALSE

--LEGOPT(DEFAULT="") : Option to pass to TLegend for each entry. Overriden by sample configuration. If neither specified, the draw option
of each sample is used to determine the style of the legend entry

--TITLEXMIN(DEFAULT=NONE) : NDC min-x. coordinate for title box. Overriden by variable configuration.

--TITLEYMIN(DEFAULT=NONE) : NDC min-y. coordinate for title box. Overriden by variable configuration.

--TITLEXMAX(DEFAULT=NONE) : NDC max-x. coordinate for title box. Overriden by variable configuration.

--TITLEYMAX(DEFAULT=NONE) : NDC max-y. coordinate for title box. Overriden by variable configuration.

--TITLETEXTSIZE(DEFAULT=0.035(0.045) for single (double) panel plots) : Text size for title box. Overriden by variable configuration.

--LEGENDTEXTSIZE(DEFAULT=0.035(0.045) for single (double) panel plots) : Text size for legend box. Overriden by variable configuration.

--LEGENDXMAX(DEFAULT=NONE) : NDC max-x coordinate for legend. Overriden by variable configuration. 

--LEGENDYMAX(DEFAULT=NONE) : NDC max-y coordinate for legend. Overriden by variable configuration. 

--XTITLEOFFSET(DEFAULT=1.) : Offset of x-axis title. Overriden by variable configuration. 

--XTITLESIZE(DEFAULT=0.04) : Text size of x-axis title. Overriden by variable configuration. 

--YTITLEOFFSET(DEFAULT depends on y-axis range) : Offset of y-axis title in top panel. Overriden by variable configuration. 

--YTITLESIZE(DEFAULT=0.04 (no bottom panel) / 0.05 (otherwise)) : Text size of y-axis title in top panel. Overriden by variable configuration. 

----

--RESTITLEOFFSET(DEFAULT=1.) : Offset of y-axis title in bottom panel. Overriden by variable configuration. 

--RESTITLESIZE(DEFAULT=0.04) : Text size of y-axis title in bottom panel. Overriden by variable configuration. 

--XLABELOFFSET(DEFAULT=0.005) : Offset of x-axis label. Overriden by variable configuration. 

--XLABELSIZE(DEFAULT=0.04) : Text size of x-axis label. Overriden by variable configuration. 

--YLABELOFFSET(0.005) : Offset of y-axis label in top panel.Overriden by variable configuration. 

--YLABELSIZE(DEFAULT=0.04 (no bottom panel) / 0.05 (otherwise)) : Text size of y-axis label in top panel. Overriden by variable configuration. 

--RESLABELOFFSET(DEFAULT=0.005) : Offset of y-axis label in bottom panel. Overriden by variable configuration. 

--RESLABELSIZE(DEFAULT=0.04) : Text size of y-axis label in bottom panel. Overriden by variable configuration. 

--BOTTOMMARGIN : Bottom margin of canvas. Overriden by variable configuration. 

--TOPMARGIN : Top margin of canvas. Overriden by variable configuration. 

--LEFTMARGIN : Left margin of canvas. Overriden by variable configuration. 

--RIGHTMARGIN : Right margin of canvas. Overriden by variable configuration. 

--TITLE : Title string to be drawn (e.g. ATLAS internal). Accepts #splitline.

--BLINDING : Type of blinding to apply to blind sample. Supported options: 

 * "THRESH" : Only blind bins/histograms where the blinding variable is above threshold. 

 * "BIN" : Blinding is applied to individual bins. 

 * "YIELD" :  Do not print the yield of the blind sample in the legend. 

Options can be concatenated. Overriden by variable configuration.  

----

***Blinding options***

--BLINDSAMPLE : Sample to blind (e.g. Data)

--BLINDTHRESHOLD : Threshold above which to blind

--BLINDCRITERION(DEFAULT=SBYB) : Variable on which blinding threshold is calculated. Supported options: 

 * SBYB = SIGNAL/SUM as specified in the sample configuration file.

 * NSIG = Number of events in SIGNAL sample. 

----

--GLOBALSCALE : A global constant by which all histograms are scaled. Multiplied to the individual sample scales.

--YIELDFORMAT : Printf style formatting string used to print yield in legends. Overriden by sample configuration.

--DISTRIBUTIONFILE : Single file from which all distributions can be read.

--ALLFROMFILE(DEFAULT=FALSE) : TRUE or FALSE. All distributions from the input files will be plotted when TRUE.

***Other global options exist***

### 2. Sample attributes: 

NAME : Name of sample. Histograms are currently stacked in alphabetical order using this name.

LEGLABEL : Label used in legend entry

STYLEKEY : Style key from style library to associate to this sample

DRAWOPT(DEFAULT=HIST) : Histogram drawing option

RESDRAWOPT : Drawing option in residual panel. 

LEGOPT : Option to pass to TLegend for this sample's entry

DRAWSCALE(DEFAULT=NORM) : "NORM" if this should be drawn normalised to given sample weight. "SHAPE" if unit-normalisation wanted. 
"NONE" if no weight is to be applied

SCALETOREF : The key to another sample, to whose integral this sample will be normalised

DRAWSTACK (DEFAULT=FALSE) : TRUE or FALSE. Sample added to THStack if TRUE. If FALSE, the sample will be drawn with a "nostack" option (e.g. data).

DOSUM : TRUE or FALSE. Sample added to "SUM" if TRUE. Option reverts to same value as DRAWSTACK if not provided.

RESOPT(DEFAULT=SKIP) : SKIP or INC or REF. This option is used to determine how this sample should be used in the calculation of residuals
for the bottom panel. 

 * SKIP : Sample is not drawn in the residual panel. 

 * REF : Reference sample with respect to which the residuals are to be calculated. 

 * INC : Residuals are calculated and drawn for all samples indicating "INC"

----

BLINDING : SIGNAL or NONE. Contamination is calculated as the maximum value over all SIGNAL samples. 
SBYB values are always calulated assuming SUM to be the total background.

YIELDFORMAT : Printf style formatting string used to print yield in legend. IF "NONE" then yield is not printed

WRITE : TRUE or FALSE. All histograms for sample written to file with name OUTFILENAME if WRITEHISTOS is TRUE in global options.

OUTFILENAME : Name of file to which histograms for sample can be written.

NOSHAPE (DEFAULT=FALSE) : TRUE or FALSE. When TRUE, this sample will not be drawn when drawing non-stacked shape plots.

---- 

**NOTE** The keyword SUM can be used to draw a sum of all samples with DOSUM=TRUE with a statistical error band. Add it as just another sample in the sample configuration file. 

---- 

INSUFFIX : Suffix to be appended to name of histogram (NAME in Variable attribute) when reading from file

INPREFIX : Prefix to be added to name of histogram (NAME in Variable attribute) when reading from file


### 3. Variable attributes:

NAME : Name of histogram in file

LABEL : Title of x-axis. Existing x-axis title from histogram used if this is not specified

YLABEL : Title of y-axis. Existing y-axis title from histogram used if this is not specified 

RESLABEL : Title of y-axis on bottom panel.

EXTRALABEL : Extra text to be added to the title box beyond what is already given in global --TITLE option

DRAWSTACK(DEFAULT=FALSE) : TRUE or FALSE. No histogram is stacked if this option is FALSE, even if the sample attribute indicates TRUE

DRAWRES : No bottom panel drawn if NONE. The following options are supported currently:

 * RATIO : sample /reference

 * INVRATIO : reference / sample

 * DIFF : sample - reference

 * RESIDUAL : (sample - reference)/sample

***Others to be added***

DRAWRESERR : Controls the manner in which errors are displayed in the residual panel. Errors are propagated and drawn on the calculated 
residual by default. "REFBAND" chnages this to produce a statistical error band on the reference line and a scaled error bar on each line 
in the residual panel.

ISLOGY(DEFAULT=FALSE) : TRUE or FALSE. canv->SetLogY() on top panel if TRUE

ISLOGX(DEFAULT=FALSE) : TRUE or FALSE. canv->SetLogY() on top panel if TRUE

----

REBIN(DEFAULT=0) : Nothing done if 0. Else, the given integer is used to perform TH1::Rebin().

REBINVAR : The bin edges required for rebinning into variable-length bins, given as a comma separated list. There must be REBIN+1 edges given in this list.

----

DOSCALE(DEFAULT=NONE) : SHAPE or NORM or NONE. 

DOWIDTH(DEFAULT=FALSE) : TRUE or FALSE. TRUE can be used to draw bin content / bin width for histograms with irregular bin sizes.

----

RESDRAWOPT : See global option

BLINDING : See global option

RESMIN(DEFAULT=NONE) : See global option 

RESMAX(DEFAULT=NONE) : See global option 

YMIN(DEFAULT=NONE) : See global option 

YMAX(DEFAULT=NONE) : See global option

YSCALE(DEFAULT=NONE) : See global option

XMIN(DEFAULT=NONE) : See global option 

XMAX(DEFAULT=NONE) : See global option 

TITLEXMIN(DEFAULT=NONE) : See global option 

TITLEYMIN(DEFAULT=NONE) : See global option

TITLEXMAX(DEFAULT=NONE) : See global option 

TITLEYMAX(DEFAULT=NONE) : See global option 

TITLETEXTSIZE(DEFAULT=0.035(0.045) for single(double) panel plots) : See global option	

ISCOUNT(DEFAUT=FALSE) : Set to TRUE to get count plots where only the integer divisions are drawn for the x-axis

XTITLEOFFSET(DEFAULT=1.) : See global option  

XTITLESIZE(DEFAULT=0.04) : See global option  

YTITLEOFFSET(DEFAULT depends on y-axis range) : See global option  

YTITLESIZE(DEFAULT=0.04 (no bottom panel) / 0.05 (otherwise)) : See global option  

RESTITLEOFFSET(DEFAULT=1.) : See global option  

RESTITLESIZE(DEFAULT=0.04) : See global option  

XLABELOFFSET(DEFAULT=0.005) : See global option  

XLABELSIZE(DEFAULT=0.04) : See global option  

YLABELOFFSET(0.005) : See global option  

YLABELSIZE(DEFAULT=0.04 (no bottom panel) / 0.05 (otherwise)) : See global option  

RESLABELOFFSET(DEFAULT=0.005) : See global option  

RESLABELSIZE(DEFAULT=0.04) : See global option  

BOTTOMMARGIN : See global option

TOPMARGIN : See global option

LEFTMARGIN : See global option

RIGHTMARGIN : See global option


********************

Some notes:

1. If DRAWSTACK = TRUE and DOSCALE=SHAPE for a variable, the summed histogram will be normalised to unit area. Each histogram 
in the stack will represent the fraction of the area. In this case, it is essential that the individual histograms are 
properly normalised through the options given in sample configuration

2. share folder contains a set of example configuration files and an example running script. Files are in Trisha's private area 
on lxplus. Please ask for access.

***More notes to be added***
