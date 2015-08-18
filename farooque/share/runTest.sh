#!/bin/bash

sampleFile="IFAEPlotter/share/test_sample_config.txt"
varFile="IFAEPlotter/share/test_variable_config.txt"
styleLib="IFAEPlotter/share/test_style_config.txt"
fList="IFAEPlotter/share/test_filelist.txt"


plot --samples ${sampleFile} --variables ${varFile} --style ${styleLib} --files ${fList}
