//----------------
// Bar definitions
{
  name: "VARIABLE",
  index: "simconstants",

  world_box_width: "20.0*m",
  target_tunnel_dist: "15.0*m",
  target_tunnel_size: "1.0*m",
  target_tunnel_length: "20.0*m",

  det_tunnel_depth: "75*m",// also sets the world box depth to be 2*det_tunnel_depth

  // Should be set to 2*depth*tan(60+delta) (where delta = 5)
  world_box_length: "2.0*det_tunnel_depth*2.144506",

  det_sx: "4.0*cm",
  det_sy: "80.0*cm",
  det_sz: "15.0*cm",
  det_system_offset: "0.5*15*cm",// can't seem to set this as "0.5*det_sz"???
  det_system_spacing: "15*cm",// can't seem to set this as "det_sz"???

  det_system_xsep: "20*cm",
  det_system_lowbar: "0.0*cm",
  sampling_target_box_size: "0.9*m"

}
// ---------------------------------------------------
// World Geometry : 20 x 20 x 30 AIR
// Then air and carbon base
{
  name: "GEO",
  index: "world",
  material: "G4_AIR",
  size: ["world_box_length", "world_box_width", "2*det_tunnel_depth"],
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
  sensitive: "truemuon"
}

{
  name: "GEO",
  index: "bar0",
  type: "box",
  mother: "detector",
  material: "G4_PLASTIC_SC_VINYLTOLUENE",
  size: ["det_sx","det_sy","det_sz"],
  //size: ["4*cm","80*cm","15*cm"],
  position: ["0.0","0.0","0.0*det_system_spacing+det_system_offset -0.5*sampling_target_box_size"],
  sensitive: "scint",
  color: [0.0,0.0,1.0],
}


// - The main stack of scintillator detectors
{ name: "GEO", index: "bar1", clone: "bar0", position: ["0.0","0.0","1*det_system_spacing+det_system_offset-0.5*sampling_target_box_size"] }
{ name: "GEO", index: "bar3", clone: "bar0", position: ["0.0","0.0","3*det_system_spacing+det_system_offset-0.5*sampling_target_box_size"] }
{ name: "GEO", index: "bar2", clone: "bar0", position: ["0.0","0.0","2*det_system_spacing+det_system_offset-0.5*sampling_target_box_size"] }
{ name: "GEO", index: "bar4", clone: "bar0", position: ["0.0","0.0","4*det_system_spacing+det_system_offset-0.5*sampling_target_box_size"] }
{ name: "GEO", index: "bar5", clone: "bar0", position: ["0.0","0.0","5*det_system_spacing+det_system_offset-0.5*sampling_target_box_size"] }

// - The lower two scintillator detectors
{ name: "GEO", index: "bar6", clone: "bar0", position: ["det_system_xsep","0.0","det_system_offset+det_system_lowbar-0.5*sampling_target_box_size"] }
{ name: "GEO", index: "bar7", clone: "bar0", position: ["-1.0*det_system_xsep","0.0","det_system_offset+det_system_lowbar-0.5*sampling_target_box_size"] }

// ---------------------------------------------------
// Flux Generator Source Location : Default is Shukla
{
  name: "FLUX",
  index: "source_box",
  size: ["world_box_length", "world_box_width", "1.*mm"],
  position: ["0.0","0.0", "det_tunnel_depth - 2*mm"],

}

// Target for the main detector stack
{
  name: "FLUX",
  index: "target_box_0",
  size: ["sampling_target_box_size","sampling_target_box_size","sampling_target_box_size"],
  position: ["0.0","0.0","0.5*sampling_target_box_size"],
}

// The coincidence trigger - set up to mimic the DAQ system
// These are all OR'd together
{
  name:  "TRIGGER",
  index: "trigger56",
  type: "coincidence",
  energy_threshold: "0",
  require_n: "2",
  processors: ["bar5_scint","bar6_scint"]
}
{  name:  "TRIGGER",  index: "trigger46",  clone: "trigger56",  processors: ["bar4_scint","bar6_scint"] }
{  name:  "TRIGGER",  index: "trigger36",  clone: "trigger56",  processors: ["bar3_scint","bar6_scint"] }
{  name:  "TRIGGER",  index: "trigger26",  clone: "trigger56",  processors: ["bar2_scint","bar6_scint"] }
{  name:  "TRIGGER",  index: "trigger16",  clone: "trigger56",  processors: ["bar1_scint","bar6_scint"] }
{  name:  "TRIGGER",  index: "trigger06",  clone: "trigger56",  processors: ["bar0_scint","bar6_scint"] }

{  name:  "TRIGGER",  index: "trigger57",  clone: "trigger56",  processors: ["bar5_scint","bar7_scint"] }
{  name:  "TRIGGER",  index: "trigger47",  clone: "trigger56",  processors: ["bar4_scint","bar7_scint"] }
{  name:  "TRIGGER",  index: "trigger37",  clone: "trigger56",  processors: ["bar3_scint","bar7_scint"] }
{  name:  "TRIGGER",  index: "trigger27",  clone: "trigger56",  processors: ["bar2_scint","bar7_scint"] }
{  name:  "TRIGGER",  index: "trigger17",  clone: "trigger56",  processors: ["bar1_scint","bar7_scint"] }
{  name:  "TRIGGER",  index: "trigger07",  clone: "trigger56",  processors: ["bar0_scint","bar7_scint"] }
