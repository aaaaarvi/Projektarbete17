# Pattern matching tests

This subproject provides a FairTask to count the occurring event and track patterns in the STT based on data from the ideal track finder.

## Requirements

### for compiling
This folder has to be linked in `pandaroot/PndTrackers` and then added as a subdirectory in the main CMakeLists.txt file of pandaroot.

### for running
An example for executing the task is given in `macros/patternCounter.C`. The following data branches are required:
- STTHit: hit data from the STT after digitization
- SttMvdGemIdealTrackCand: track candidates produced by the ideal track finder
