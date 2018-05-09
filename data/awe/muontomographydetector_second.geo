// --------------------------------
// Materials
{
  name: "MATERIAL",
  index: "AWE_GASMIX1",
  element_names: ["Ar","C","O","H"],
  element_counts: [0.925,0.75,0.100,0.100],
  density: 8.05,  
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
// Scintillator Panel
// --> Make simple scintillator
{
  name: "DETECTOR",
  index: "scintillator",
  type:  "scintillator"
  effiency: 1.0,
  time_resolution: "30.0*ns",
  processor: 0,
  save: 0,
}
// --> Place plastic scintillator as one big panel
{
  name: "GEO",
  index: "scintillator_top",
  type: "box",
  mother: "volume",
  // 
  // Big ~2*2 slab 
  size: [1.9,1.9,0.05],
  size_units: "m"
  //
  // Place above drifts
  position: [0.0,0.0,0.38],
  position_units: "m",
  //
  // Made out of G4 plastic scintillator
  material: "G4_C",
  //
  // Sensitive scintillator object
  sensitive: "scintillator"
  //
  // Drawing options
  color: [0.,0.,1.,0.2],
  drawstyle: "solid"
}

{
  name: "GEO",
  index: "scintillator_bottom",
  type: "box",
  mother: "volume",
  //
  // Big ~2*2 slab
  size: [1.9,1.9,0.05],
  size_units: "m"
  //
  // Place above drifts
  position: [0.0,0.0,-0.38],
  position_units: "m",
  //
  // Made out of G4 plastic scintillator
  material: "G4_C",
  //
  // Sensitive scintillator object
  sensitive: "scintillator"
  //
  // Drawing options
  color: [0.,0.,1.,0.2],
  drawstyle: "solid"
}

// -------------------------------------
// Drift Chambers
// --> Drift Detector : Only Y pos info
{
  name: "DETECTOR",
  index: "longdrift",
  type:  "awedrift"
  //
  // Position resolution width
  resolution: "2*mm",
  //
  // Wire offsets in XYZ
  wire_position: "3.0*cm",
  //
  // 99% Efficieny
  efficiency: 1.00,
  //
  // Whether to save debug info
  processor: 0,
  save: 0,
}

// --> drift template
{
  name: "GEO",
  index: "drift_chamber",
  type: "box",
  mother: "volume"
  //
  // Dimensions
  size: [1.80,0.60,0.07],
  size_units: "m"
  //
  // Set placement units for later
  position_units: "m",
  //
  // Fill with gas max above
  material: "AWE_GASMIX1",
  //
  // Make Drift Sensitive
  sensitive: "longdrift"
  //
  // Draw options
  color: [1.,0.,0.,0.3],
  drawstyle: "wireframe"

}

// --> drift placements
{
  name: "GEO",
  index: "chamber_positions",
  //
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







