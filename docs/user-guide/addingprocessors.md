# Adding Detectors

The Analysis Manager keeps track of all Detectors, Triggers, and Processors. Therefore additional processors can be setup to read information in from lower level processing/detection and calculate extra quantities. Each processor has unique inputs dependent on what it is doing. The standard detector processors are loaded automatically by the sensitive objects, but extra processors can be defined.

For example, the POCA processor takes two other tracker objects and calculates a point of closest approach before saving it into the TTree.

```
{
  name: "PROCESSOR",
  index: "mypoca",
  trackerA: "detectortop",
  trackerB: "detectorbottom"
}
```
