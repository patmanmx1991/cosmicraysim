# Adding Detectors


The detector objects are setup so that any GEO definition can be assigned a sensitive object which converts it into a G4VSensitiveDetector. Alongside this, by default, any sensitive detectors are automatically registered with the analysis manager and their outputs are appended to the ROOT output tree, allowing the information saved to easily change according to the users geometry definition.


To use a detector first you need to make a DETECTOR table that says what type it is, and includes any extra information available to it. The index is used when referencing it, so it is possible in the future to load multiple detectors, each with many different options (such as efficiencies).

The example detector below sets up a SimpleScintillatorSD detector, and calls it just "sct".
```
{
   name: "DETECTOR",
   index: "sct",
   type: "scintillator",
}
```


When the actual geometry is being defined, this detector can then be assigned to a simple geometry object by adding a "sensitive" field to the geometry table.
The example below, takes our "sct" detector information, and uses it to setup three different detectors.
```
{
  name: "GEO",
  index: "box0",
  type: "box",
  material: "G4_Ar",
  size: ["0.5*m","0.5*m","0.5*m"],
  sensitive: "sct",
}
{
  name: "GEO",
  index: "box1",
  type: "box",
  material: "G4_Ar",
  size: ["0.5*m","0.5*m","0.5*m"],
  position: ["1.0*m","0.0","0.0"],
  sensitive: "sct",
}
```

This example demonstrates that a single detector table can be used to setup multiple sensitive detectors if required. The name of the sensitive detector is automatically formed from the GEO box index, and the DETECTOR index. So in this case two new detectors available, called 
```
box0_sct
box1_sct
```


Unless explicitly stated in the detector tables, these detectors will both automatically add any information they have to the output TTree. This is done by each detector registering a "Processor" with the analysis manager. In the example above, the output TTree will then have the following variables.
```
box0_sct_E
box0_sct_t
box0_sct_x
box0_sct_y
box0_sct_z
box0_sct_thXZ
box0_sct_thYZ
box1_sct_E
box1_sct_t
box1_sct_x
box1_sct_y
box1_sct_z
box1_sct_thXZ
box1_sct_thYZ
```


Currently, three different detectors are available. These are below. More information on how to use each one will be added in the future.
```
truemuon
scintillator
longdrift
```