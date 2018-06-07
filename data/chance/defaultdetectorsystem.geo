// ---------------------------------------------------
// World Geometry : 20 x 20 x 30 AIR
// Then air and carbon base
{
  name: "GEO",
  index: "world",
  material: "G4_AIR",
  size: ["3.*m", "3.0*m", "7.*m"],
  type: "box",
}

// --------------------------------------------------
// AWE Detector
{
  name: "GEO",
  index: "system",
  type: "hybrid_muontom",
  mother: "world",
  position: ["0.0","0.0","-1.0*m"],
}

{
  name: "GEO",
  index: "system_above",
  type: "hybrid_muontom",
  mother: "world",
  position: ["0.0","0.0","1.0*m"],
}



// ---------------------------------------------------
// Flux Generator Source Location : Default is Shukla
{
  name: "FLUX",
  index: "source_box",
  size: ["1.0*m", "1.*m", "0.05*m"],
  position: ["0.0","0.0", "3.475*m"],
}

{
  name: "FLUXNULL",
  index: "target_box_0",
  size: ["1.80*m","0.60*m","0.07*m"],
  position: ["1.0","0.0","0.0*m"],
  rotation: [90.0,0.0,90.0],
  region: "precision"
}

