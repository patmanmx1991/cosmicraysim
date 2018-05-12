//----------------
// Bar definitions
// Estimate events for 1 day exposure
// Depth (m)        Events      CPU Time?
//   25             253k
//   50             128k
//   75             83k
{
  name: "VARIABLE",
  index: "simconstants",
  world_box_width: "10.0*m",

  det_system_offset: "-0.5*m",
  det_system_spacing: "10*cm",
  det_system_xsep: "20*cm",
  det_system_lowbar: "0.0*cm",
  det_tunnel_depth: "75*m",
  world_box_length: "2.0*det_tunnel_depth*2.144506",// 2*depth*tan(60+delta) (where delta = 5)


  target_tunnel_dist: "7.0*m",
  target_tunnel_size: "0.5*m",
  target_tunnel_length: "0.5*m",

  sampling_target_box_size: "2*m",
  sampling_target_box_scale: "1"
}
// ---------------------------------------------------
// World Geometry : 20 x 20 x 30 AIR
// Then air and carbon base
{
  name: "GEO",
  index: "world",
  material: "Standard_Rock",
  size: ["world_box_length", "world_box_width", "2*det_tunnel_depth"],
  type: "box"
  color: [0.7,0.3,0.3,0.5],
  drawstyle: "solid"
}

// Target tunnel @ 15 degrees
{
  name: "GEO",
  index: "target_tunnel_15",
  material: "G4_AIR",
  mother: "world",
  size: ["target_tunnel_size", "target_tunnel_length", "target_tunnel_size"],
  position: ["0.0","0.0","target_tunnel_dist"],
  rotation_mother: [0.0,15.0,0.0],
  color: [0.9,0.3,0.9,0.9],
  drawstyle: "solid"
  type: "box"
}

// Target tunnel @ 15 degrees
{
  name: "GEO",
  index: "target_tunnel_15w",
  material: "G4_WATER",
  mother: "world",
  size: ["target_tunnel_size", "target_tunnel_length", "target_tunnel_size"],
  position: ["0.0","0.0","target_tunnel_dist"],
  rotation_mother: [0.0,-15.0,0.0],
  color: [0.3,0.9,0.9,0.9],
  drawstyle: "solid"
  type: "box"
}

// Target tunnel @ 30 degrees
{
  name: "GEO",
  index: "target_tunnel_30",
  material: "G4_AIR",
  mother: "world",
  size: ["target_tunnel_size", "target_tunnel_length", "target_tunnel_size"],
  position: ["0.0","0.0","target_tunnel_dist"],
  rotation_mother: [0.0,30.0,0.0],
  color: [0.9,0.3,0.9,0.9],
  drawstyle: "solid"
  type: "box"
}

// Target tunnel @ 30 degrees
{
  name: "GEO",
  index: "target_tunnel_30w",
  material: "G4_WATER",
  mother: "world",
  size: ["target_tunnel_size", "target_tunnel_length", "target_tunnel_size"],
  position: ["0.0","0.0","target_tunnel_dist"],
  rotation_mother: [0.0,-30.0,0.0],
  color: [0.3,0.9,0.9,0.9],
  drawstyle: "solid"
  type: "box"
}

// Target tunnel @ 45degrees
{
  name: "GEO",
  index: "target_tunnel_45",
  material: "G4_AIR",
  mother: "world",
  size: ["target_tunnel_size", "target_tunnel_length", "target_tunnel_size"],
  position: ["0.0","0.0","target_tunnel_dist"],
  rotation_mother: [0.0,45.0,0.0],
  color: [0.9,0.3,0.9,0.9],
  drawstyle: "solid"
  type: "box"
}

// Target tunnel @ 45degrees
{
  name: "GEO",
  index: "target_tunnel_45w",
  material: "G4_WATER",
  mother: "world",
  size: ["target_tunnel_size", "target_tunnel_length", "target_tunnel_size"],
  position: ["0.0","0.0","target_tunnel_dist"],
  rotation_mother: [0.0,-45.0,0.0],
  color: [0.3,0.9,0.9,0.9],
  drawstyle: "solid"
  type: "box"
}

// Target tunnel @ 60 degrees
{
  name: "GEO",
  index: "target_tunnel_60",
  material: "G4_AIR",
  mother: "world",
  size: ["target_tunnel_size", "target_tunnel_length", "target_tunnel_size"],
  position: ["0.0","0.0","target_tunnel_dist"],
  rotation_mother: [0.0,60.0,0.0],
  color: [0.9,0.3,0.9,0.9],
  drawstyle: "solid"
  type: "box"
}

// Target tunnel @ 60 degrees
{
  name: "GEO",
  index: "target_tunnel_60w",
  material: "G4_WATER",
  mother: "world",
  size: ["target_tunnel_size", "target_tunnel_length", "target_tunnel_size"],
  position: ["0.0","0.0","target_tunnel_dist"],
  rotation_mother: [0.0,-60.0,0.0],
  color: [0.3,0.9,0.9,0.9],
  drawstyle: "solid"
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
// Main mother tunnel and detector volume (for easy placement)
//
{
  name: "GEO",
  index: "tunnel",
  type: "eliptunnel",
  mother: "world",
  material: "G4_AIR",
  size: ["11.630*m", "7.740*m", "world_box_width"],
  position: ["0.0","0.0","0.0"],
  rotation: [90.0,0.0,90.0],
  color: [1.0,1.0,1.0,0.8],
  drawstyle: "wireframe"
}

{
  name: "GEO",
  index: "detector",
  type: "box",
  mother: "tunnel",
  material: "G4_AIR",
  size: ["sampling_target_box_size", "sampling_target_box_size", "sampling_target_box_size"],
  position: ["0.0","0.5*sampling_target_box_size","0.0"],
  rotation_mother: [0.0,-90.0,90.0],
  color: [0.3,0.9,0.1,0.9],
  invisible: 1,
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
  color: [0.3,0.9,0.1,0.9],
  drawstyle: "solid"
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
  size: ["world_box_length", "world_box_width", "1.*mm"],
  position: ["0.0","0.0", "det_tunnel_depth - 2*mm"],
  require_n: 2
}

// Target for the main detector stack
{
  name: "FLUX",
  index: "target_box_0",
  size: ["sampling_target_box_scale*4*cm","sampling_target_box_scale*80*cm","sampling_target_box_scale*6.0*10.0*cm"],
  position: ["0.0","0.0","0.0"]
}

// Target for one lower detector
{
  name: "FLUX",
  index: "target_box_1",
  size: ["sampling_target_box_scale*4*cm","sampling_target_box_scale*80*cm","sampling_target_box_scale*10*cm"],
  position: ["det_system_xsep","0.0","det_system_offset+det_system_lowbar"],
}

// Target for the other lower detector
{
  name: "FLUX",
  index: "target_box_2",
  size: ["sampling_target_box_scale*4*cm","sampling_target_box_scale*80*cm","sampling_target_box_scale*10*cm"],
  position: ["-det_system_xsep","0.0","det_system_offset+det_system_lowbar"],
}
