// --------------------------------
// Define all constants
{
   name: "VARIABLE",
   index: "myconstants"
   rpc_x: "2.0*m",
   rpc_y: "2.0*m",
   rpc_efficiency: "1.0"
   rpc_resolution: "0.4*mm",
   rpc_thickness: "6*mm",
   rpc_spacing: "25*mm",
   rpc_offset: "0.45*m",
}

{
   name: "INFO"
   index: "detectors",
   rpc_0: "y",
   rpc_1: "x",
   rpc_2: "y",
   rpc_3: "x",
   rpc_4: "y",
   rpc_5: "x",
   
   chamber_0: "y",
   chamber_1: "y",
   chamber_2: "y",
   chamber_3: "x",
   chamber_4: "x",
   chamber_5: "x",
   chamber_6: "y",
   chamber_7: "y",
   chamber_8: "y",
   chamber_9: "x",
   chamber_10: "x",
   chamber_11: "x",
   chamber_12: "y",
   chamber_13: "y",
   chamber_14: "y",
   chamber_15: "x",
   chamber_16: "x",
   chamber_17: "x",
}



// --------------------------------
// Materials
{
  name: "MATERIAL",
  index: "AWE_GASMIX1",
  element_names: ["Ar","C","O","H"],
  element_counts: [0.925,0.75,0.100,0.100],
  density: 2.4,  
  density_units: "g/cm3"
}
{
  name: "MATERIAL",
  index: "AWE_SCINTMIX",
  element_names: ["C","H"]
  element_counts: [0.9,0.1],
  density: 2.4,
  density_units: "g/cm3"
}

{
  name:	"MATERIAL",
  index: "BRISTOL_GASMIX1",
  element_names: ["O","Na","Si","Ca"]
  element_counts: [0.45,0.10,0.35,0.10],
  density: 2.4,
  density_units: "g/cm3"
}

// --------------------------------
// Main Detector geometry for placement
// --> Place as true muon for acceptance
{
 name: "DETECTOR",
 index: "truemuon",
 type: "truemuon",
}
// --> Main Box, just air
{
  name: "GEO",
  index: "volume",
  type: "box",
  size: [2.0,2.0,1.0],
  size_units: "m"
  position: [0.0,0.0,0.0],
  material: "G4_AIR",
  sensitive: "truemuon",
}
// --------------------------------
// --> Make Bristol RPCs (turn this into a templated function)
{
  name: "GEO",
  index: "rpc_template",
  type: "box",
  size: ["rpc_x","rpc_y","rpc_thickness"]
  material: "BRISTOL_GASMIX1",
  mother: "volume", 
  sensitive: "bristol_rpc_rec"
  color: [0.0,0.5,0.0,0.5]
  drawstyle: "solid",
}

{
  name: "GEO",
  index: "rpc_positions",
  rpc_0: ["0.0","0.0","rpc_offset",              "0.0","0.0"," 0.0"],
  rpc_1: ["0.0","0.0","rpc_offset+rpc_spacing",  "0.0","0.0","90.0"],
  rpc_2: ["0.0","0.0","-rpc_offset",             "0.0","0.0"," 0.0"],
  rpc_3: ["0.0","0.0","-rpc_offset-rpc_spacing", "0.0","0.0","90.0"],
}

// --------------------------------
// Scintillator Panel
// --> Make simple scintillator
{
  name: "GEO",
  index: "scintillator_template",
  type: "box",
  mother: "volume",
  // 
  // Big ~2*2 slab 
  size: [1.9,1.9,0.05],
  size_units: "m"
  //
  // Made out of G4 plastic scintillator
  material: "AWE_SCINTMIX",
  //
  // Sensitive scintillator object
  sensitive: "scint_true_save"
  //
  // Drawing options
  color: [0.,0.,1.,0.2],
  drawstyle: "solid"
}

{
  name: "GEO",
  index: "scintillator_positions"
  scintillator_0: ["0.0","0.0"," 0.38*m","0.0","0.0","0.0"]
  scintillator_1: ["0.0","0.0","-0.38*m","0.0","0.0","0.0"]
}

// --------------------------------
// --> drift template
{
  name: "GEO",
  index: "drift_template",
  type: "box",
  mother: "volume"
  // Set placement units for later
  position_units: "m",
  material: "AWE_GASMIX1",
  size: ["1.8*m","0.6*m","5*cm"]
}

// --> drift placements
{
  name: "GEO",
  index: "drift_positions",
  //
  // pos and rot [x,y,z,thx,thy,thz] 
  // pos and rot [x,y,z,thx,thy,thz] in m
  chamber_0: [0.0,-0.62,-0.25, 0.0, 0.0, 0.0],
  chamber_1: [0.0,0.0,-0.25, 0.0, 0.0, 0.0],
  chamber_2: [0.0,0.62,-0.25, 0.0, 0.0, 0.0],
  chamber_3: [-0.62,0.0,-0.16, 0.0, 0.0, 90.0],
  chamber_4: [0.0,0.0,-0.16, 0.0, 0.0, 90.0],
  chamber_5: [0.62,0.0,-0.16, 0.0, 0.0, 90.0],
  chamber_6: [0.0,-0.62,-0.05, 180.0, 0.0, 0.0],
  chamber_7: [0.0,0.0,-0.05, 180.0, 0.0, 0.0],
  chamber_8: [0.0,0.62,-0.05, 180.0, 0.0, 0.0],
  chamber_9: [-0.62,0.0,0.05, 0.0, 180.0, 90.0],
  chamber_10: [0.0,0.0,0.05, 0.0, 180.0, 90.0],
  chamber_11: [0.62,0.0,0.05, 0.0, 180.0, 90.0],
  chamber_12: [0.0,-0.62,0.16, 0.0, 0.0, 0.0],
  chamber_13: [0.0,0.0,0.16, 0.0, 0.0, 0.0],
  chamber_14: [0.0,0.62,0.16, 0.0, 0.0, 0.0],
  chamber_15: [-0.62,0.0,0.25, 0.0, 0.0, 90.0],
  chamber_16: [0.0,0.0,0.25, 0.0, 0.0, 90.0],
  chamber_17: [0.62,0.0,0.25, 0.0, 0.0, 90.0],
}







