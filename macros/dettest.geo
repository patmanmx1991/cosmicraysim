// ---------------------------------------------------
// World Geometry : 20 x 20 x 30 AIR
// Then air and carbon base
{
  name: "GEO",
  index: "world",
  material: "G4_AIR",
  size: ["10.*m", "10.0*m", "10.*m"],
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
  index: "detector0",
  type: "box",
  mother: "world",
  material: "G4_Ar",
  size: ["2.0*m","2.0*m","2.0*m"],
  position: ["1.0*m","0.0","0.0"],
  sensitive: "truemuon",	   
}

{
  name:  "TRIGGER",
  index: "trigger0",
  type: "simple",
  processors: ["detector0_truemuon"]
}


// ---------------------------------------------------
// Flux Generator Source Location : Default is Shukla
{
  name: "FLUX",
  index: "source_box",
  size: ["10.0*m", "10.0*m", "0.5*m"],
  position: ["0.0","0.0", "4.75*m"],
}

{
  name: "FLUX",
  index: "target_box_0",
  size: ["12*m","32.0*m","8.0*m"]
  position: ["0.0","0.0","-10.0*m"],
  rotation: [90.0,0.0,90.0],
  region: "precision"
}

