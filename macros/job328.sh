#!/usr/bin/env bash
OUTPUTDIR=../../data_new/
nEvents=1000
iJob=328
root -q -b generateMomData.C"($iJob,\"$OUTPUTDIR\")"
root -q -b generateTSMomData.C"($iJob,\"$OUTPUTDIR\")"
root -q -b generatePatData.C"($iJob,\"$OUTPUTDIR\")"
