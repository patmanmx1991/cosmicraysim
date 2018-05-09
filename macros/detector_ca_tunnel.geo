//----------------
// Bar definitions
{
  name: "VARIABLE",
  index: "simconstants",
  det_system_offset: "-0.5*m",
  det_system_spacing: "10*cm",
  det_system_xsep: "20*cm",
  det_system_lowbar: "0.0*cm",
  det_tunnel_depth: "25*m",
  target_tunnel_dist: "10.0*m",
  target_tunnel_size: "1*m",
  sampling_target_box_size: "2*m"
}
// ---------------------------------------------------
// World Geometry : 20 x 20 x 30 AIR
// Then air and carbon base
{
  name: "GEO",
  index: "world",
  material: "Standard_Rock",
  size: ["400.*m", "20.0*m", "2*det_tunnel_depth"],
  type: "box"
}



// Target tunnel @ 15 degrees
{
  name: "GEO",
  index: "target_tunnel_15",
  material: "G4_AIR",
  mother: "world",
  size: ["target_tunnel_size", "20.0*m", "target_tunnel_size"],
  position: ["target_tunnel_dist*0.96592582628","0.0","target_tunnel_dist*0.2588190451"],
  type: "box"
}

// Target tunnel @ 30 degrees
{
  name: "GEO",
  index: "target_tunnel_30",
  material: "G4_AIR",
  mother: "world",
  size: ["target_tunnel_size", "20.0*m", "target_tunnel_size"],
  position: ["target_tunnel_dist*0.5","0.0","target_tunnel_dist*0.866"],
  type: "box"
}

// Target tunnel @ 45 degrees
{
  name: "GEO",
  index: "target_tunnel_45",
  material: "G4_AIR",
  mother: "world",
  size: ["target_tunnel_size", "20.0*m", "target_tunnel_size"],
  position: ["target_tunnel_dist*0.70710678118","0.0","target_tunnel_dist*0.70710678118"],
  type: "box"
}

// Target tunnel @ 60 degrees
{
  name: "GEO",
  index: "target_tunnel_60",
  material: "G4_AIR",
  mother: "world",
  size: ["target_tunnel_size", "20.0*m", "target_tunnel_size"],
  position: ["target_tunnel_dist*0.866","0.0","target_tunnel_dist*0.5"],
  type: "box"
}

// Target tunnel @ 75 degrees
{
  name: "GEO",
  index: "target_tunnel_75",
  material: "G4_AIR",
  mother: "world",
  size: ["target_tunnel_size", "20.0*m", "target_tunnel_size"],
  position: ["target_tunnel_dist*0.2588190451","0.0","target_tunnel_dist*0.96592582628"],
  type: "box"
}

// Target tunnel @ 90 degrees
{
  name: "GEO",
  index: "target_tunnel_90",
  material: "G4_AIR",
  mother: "world",
  size: ["target_tunnel_size", "20.0*m", "target_tunnel_size"],
  position: ["0.0","0.0","target_tunnel_dist"],
  type: "box"
}


// Target tunnel @ 15 degrees
{
  name: "GEO",
  index: "target_tunnel_15w",
  material: "G4_WATER",
  mother: "world",
  size: ["target_tunnel_size", "20.0*m", "target_tunnel_size"],
  position: ["-target_tunnel_dist*0.96592582628","0.0","target_tunnel_dist*0.2588190451"],
  type: "box"
}

// Target tunnel @ 30 degrees
{
  name: "GEO",
  index: "target_tunnel_30w",
  material: "G4_WATER",
  mother: "world",
  size: ["target_tunnel_size", "20.0*m", "target_tunnel_size"],
  position: ["-target_tunnel_dist*0.5","0.0","target_tunnel_dist*0.866"],
  type: "box"
}

// Target tunnel @ 45 degrees
{
  name: "GEO",
  index: "target_tunnel_45w",
  material: "G4_WATER",
  mother: "world",
  size: ["target_tunnel_size", "20.0*m", "target_tunnel_size"],
  position: ["-target_tunnel_dist*0.70710678118","0.0","target_tunnel_dist*0.70710678118"],
  type: "box"
}

// Target tunnel @ 60 degrees
{
  name: "GEO",
  index: "target_tunnel_60w",
  material: "G4_WATER",
  mother: "world",
  size: ["target_tunnel_size", "20.0*m", "target_tunnel_size"],
  position: ["-target_tunnel_dist*0.866","0.0","target_tunnel_dist*0.5"],
  type: "box"
}

// Target tunnel @ 75 degrees
{
  name: "GEO",
  index: "target_tunnel_75w",
  material: "G4_WATER",
  mother: "world",
  size: ["target_tunnel_size", "20.0*m", "target_tunnel_size"],
  position: ["-target_tunnel_dist*0.2588190451","0.0","target_tunnel_dist*0.96592582628"],
  type: "box"
}




















// --------------------------------------------------
// Detector
{
  name: "DETECTOR",
  index: "scint",
  type: "scintillator",
}

//--------------------------------------------
// Main mother detector volume (for easy placement)
//{
//  name: "GEO",
//  index: "detector",
//  type: "eliptube",
//  mother: "world",
//  material: "G4_AIR",
//  size: ["11.630*m", "7.740*m", "30.0*m"],
//  position: ["0.0*m","0.0","0.0"],
//  rotation: [90.0,0.0,90.0],
//  // sensitive: "truemuon",
//}



{
  name: "GEO",
  index: "detector",
  type: "box",
  mother: "world",
  material: "G4_AIR",
  size: ["5.0*m","20.0*m","5.0*m"],
  position: ["0.0*m","0.0","0.0"],
  sensitive: "truemuon"
}

{
  name: "GEO",
  index: "bar0",
  type: "box",
  mother: "detector",
  material: "G4_PLASTIC_SC_VINYLTOLUENE",
  size: ["4*cm","80*cm","10*cm"],
  position: ["0.0","0.0","0.0"],
  sensitive: "scint",
}


{
  name: "GEO",
  index: "bar0",
  type: "box",
  mother: "detector",
  material: "G4_PLASTIC_SC_VINYLTOLUENE",
  size: ["4*cm","80*cm","10*cm"],
  position: ["0.0","0.0","0.0*det_system_spacing+det_system_offset"],
  sensitive: "scint",
  color: [0.0,0.0,1.0],
}


// - The main stack of scintillator detectors
{ name: "GEO", index: "bar1", clone: "bar0", position: ["0.0","0.0","1*det_system_spacing+det_system_offset"] }
{ name: "GEO", index: "bar2", clone: "bar0", position: ["0.0","0.0","2*det_system_spacing+det_system_offset"] }
{ name: "GEO", index: "bar3", clone: "bar0", position: ["0.0","0.0","3*det_system_spacing+det_system_offset"] }
{ name: "GEO", index: "bar4", clone: "bar0", position: ["0.0","0.0","4*det_system_spacing+det_system_offset"] }
{ name: "GEO", index: "bar5", clone: "bar0", position: ["0.0","0.0","5*det_system_spacing+det_system_offset"] }

// - The lower two scintillator detectors
{ name: "GEO", index: "bar6", clone: "bar0", position: ["det_system_xsep","0.0","det_system_offset+det_system_lowbar"] }
{ name: "GEO", index: "bar7", clone: "bar0", position: ["-1.0*det_system_xsep","0.0","det_system_offset+det_system_lowbar"] }


// ---------------------------------------------------
// Flux Generator Source Location : Default is Shukla
{
  name: "FLUX",
  index: "source_box",
  size: ["400.*m", "20.0*m", "1.*mm"],
  position: ["0.0","0.0", "det_tunnel_depth - 2*mm"],
}

// Target for the main detector stack
{
  name: "FLUX",
  index: "target_box_0",
  size: ["sampling_target_box_size","sampling_target_box_size","sampling_target_box_size"],
  position: ["0.0*m","0.0","0.0"],
}
