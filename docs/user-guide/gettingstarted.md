# Getting Started
======

When the Cosmic Ray package is built it creates an application in the build area called "cosmicraysim".

This application is used to load geometry databases and produce outputs. The structure of the framework means that any sensitive detectors loaded should automatically add their event information to the output TTree, so that is easy to load in multiple detectors at once and optimise setups.

The options for the application are:
```
cosmicraysim -n NEvents -o outputfile [-h] [-i] [-m] [-g tablefile] [-g tablefile2] ...

     -n NEvents     : Specify how many events to generate in batch mode
     -o outputfile  : Specifiy name of the output file
   [ -h ]           : Print this help message
   [ -i ]           : Open the Geant4 Visualisation Manager
   [ -m ]           : Run a Geant4 Macro
   [ -g tablefile ] : Load a JSON table file, can be used multiple times

```



