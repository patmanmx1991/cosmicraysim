// ---------------------------------------------------
// World Geometry : 20 x 20 x 30 AIR
// Then air and carbon base
{
  name: "GEO",
  index: "world",
  material: "G4_AIR",
  size: ["30.*m", "30.0*m", "20.*m"],
  type: "box",
}


// --------------------------------------------------
// Detector
{
  name: "DETECTOR",
  index: "truemuon",
  type: "longdrift",
}

{
  name: "GEO",
  index: "cask",
  type: "DSC_VSC24"
  mother: "world",
  position: [0.0,0.0,0.0],
  rotation: [0.0,0.0,0.0],
}

{
  name: "GEO",
  index: "detector0",
  type: "box",
  mother: "world",
  material: "G4_PLASTIC_SC_VINYLTOLUENE",
  size: ["2.0*m","2.0*m","2.0*m"],
  position: ["1.0*m","0.0","0.0"],
  sensitive: "truemuon",	   
}

{
  name: "GEO",
  index: "detector1",
  type: "box",
  mother: "world",
  material: "G4_PLASTIC_SC_VINYLTOLUENE",
  size: ["2.0*m","2.0*m","2.0*m"],
  position: ["1.0*m","0.0","-10.0*m"],
  sensitive: "truemuon",
}

{
  name: "GEO",
  index: "detector2",
  type: "box",
  mother: "world",
  material: "G4_PLASTIC_SC_VINYLTOLUENE",
  size: ["2.0*m","2.0*m","2.0*m"],
  position: ["1.0*m","0.0","10.0*m"],
  sensitive: "truemuon",
}

{
  name:  "TRIGGER",
  index: "trigger0",
  type: "simple",
  processors: ["detector0_truemuon"]
}

{
  name:  "TRIGGER",
  index: "trigger1",
  type:	 "simple",
  processors: ["detector1_truemuon"]
}

{
  name:  "TRIGGER",
  index: "trigger2",
  type:	 "simple",
  processors: ["detector2_truemuon"]
}


// ---------------------------------------------------
// Flux Generator Source Location : Default is Shukla
{
  name: "FLUX",
  index: "source_box",
  size: ["30.0*m", "30.0*m", "0.5*m"],
  position: ["0.0","0.0", "9.75*m"],
}

{
  name: "FLUX",
  index: "target_box_0",
  size: ["12*m","32.0*m","8.0*m"]
  position: ["0.0","0.0","-10.0*m"],
  rotation: [90.0,0.0,90.0],
  region: "precision"
}

