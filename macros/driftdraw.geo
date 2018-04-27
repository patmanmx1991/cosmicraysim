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
  name: "GEO",
  index: "detector0",
  type: "awe_muontom",
  mother: "world",
}

{
  name: "DETECTORNULL",
  index: "longdrift",
  type:  "longdrift"
  restrict_x: 1,
  restrict_y: 0,
  restrict_z: 1,
  resolution_x: 0.30,
  resolution_y: 0.01,
  resolution_z: 0.07,
  processor: 0
  save: 0
}

{
        name: "GEONULL",
        index: "drift_chamber",
        type: "box",
        size: [1.80,0.60,0.07],
        size_units: "m"
        position_units: "m",
        material: "G4_Ar",
        color: [1.,0.,0.,0.3],
        drawstyle: "wireframe"
        mother: "world"
        sensitive: "longdrift"
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
  size: ["1.80*m","0.60*m","0.07*m"],
  position: ["1.0","0.0","0.0*m"],
  rotation: [90.0,0.0,90.0],
  region: "precision"
}

