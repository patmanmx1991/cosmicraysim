# Getting Started
======

When the Cosmic Ray package is built it creates an application in the build area called "cosmicraysim".

This application is used to load geometry databases and produce outputs. The structure of the framework means that any sensitive detectors loaded should automatically add their event information to the output TTree, so that is easy to load in multiple detectors at once and optimise setups.

The options for the application are:
```
cosmicraysim -n NEvents -g tablefile [-h] [-i] [-n nevents] [-j ntriggs] [-t exposur] [-c chunksz] [-o outtag] [-g tablefile2] 
             [--run runnumber] [--subrun subrunnumber]

 -n nevents : Events to generate
 -j ntriggs : Triggers to generate
 -t exposur : Exposure to generate
 -c chunksz : Event chunk size for exposure/trigger mode
 -s seed    : Seed. Default is -1, meaning get from time+pid
 -o outtag  : Output File Tag. Will create file : outtag.run.subrun.root 
 -i         : Flag. Run in interactive mode.
 -g geofile : Load a geometry JSON file. Can use multiple times.
 --run    r : Set Run ID Manually
 --subrun r : Set Sub Run ID Manually


```



