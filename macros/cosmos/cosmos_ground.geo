//----------------
// Bar definitions
{
  name: "VARIABLE",
  index: "simconstants",
  world_box_width: "150.0*m",
  world_box_length: "150.0*m",
  world_box_depth: "200.0*m",
  surface_soil_depth: "0.5*m",
  subsurface_soil_depth: "2.5*m",
  bedrock_depth: "150*m",
  detector_height: "0.5*m",
  sampling_target_box_size: "2*m"
}

// ---------------------------------------------------
// World Geometry
// Then air and carbon base
{
  name: "GEO",
  index: "world",
  material: "G4_AIR",
  size: ["world_box_length", "world_box_width", "world_box_depth"],
  type: "box",
  color: [0.9,0.9,0.99,0.9],
  drawstyle: "solid"
  force_auxedge: 1
  sensitive: "neutron_det"
}

// --------------------------------------------------
// Detector
{
  name: "DETECTOR",
  index: "neutron_det",
  type: "neutron"
}
{
  name: "GEO",
  index: "surface_soil",
  type: "box",
  mother: "world",
  material: "Soil_0p1",
  size: ["world_box_length", "world_box_width", "surface_soil_depth"],
  position: ["0.0","0.0","-0.5*world_box_depth + bedrock_depth + 0.5*surface_soil_depth + subsurface_soil_depth"],
  color: [0.2,0.9,0.2,0.8],
  drawstyle: "solid",
  force_auxedge: 1,
  sensitive: "neutron_det"
}

{
  name: "GEO",
  index: "subsurface_soil",
  type: "box",
  mother: "world",
  material: "Soil_1p0",
  size: ["world_box_length", "world_box_width", "subsurface_soil_depth"],
  position: ["0.0","0.0","-0.5*world_box_depth + bedrock_depth + 0.5*subsurface_soil_depth"],
  color: [0.2,0.5,0.2,0.8],
  drawstyle: "solid",
  force_auxedge: 1,
  sensitive: "neutron_det"
}

{
  name: "GEO",
  index: "bedrock",
  type: "box",
  mother: "world",
  material: "Standard_Rock",
  color: [0.8,0.6,0.6,0.5],
  drawstyle: "solid",
  force_auxedge: 1
  size: ["world_box_length", "world_box_width", "bedrock_depth"],
  position: ["0.0","0.0","-0.5*world_box_depth + 0.5*bedrock_depth"],
  sensitive: "neutron_det"
}


//--------------------------------------------
// Main mother tunnel and detector volume (for easy placement)
//
{
  name: "GEO",
  index: "detector_moderator",
  type: "box",
  mother: "world",
  material: "G4_PLASTIC_SC_VINYLTOLUENE",
  size: ["25*cm","25*cm","120*cm"],
  position: ["0.0","0.0","-0.5*world_box_depth + bedrock_depth + 0.5*surface_soil_depth + subsurface_soil_depth + 50*cm"],
  color: [0.8,0.8,0.8,0.8],
  drawstyle: "solid",
  force_auxedge: 1
}



{
  name: "GEO",
  index: "detector",
  type: "box",
  mother: "detector_moderator",
  material: "G4_PLASTIC_SC_VINYLTOLUENE",
  size: ["5*cm","5*cm","100*cm"],
  position: ["0.0","0.0","0.0"],
  color: [1.0,1.0,1.0,1.0],
  drawstyle: "solid",
  force_auxedge: 1,
  sensitive: "neutron_det"
}

// ---------------------------------------------------
// Flux Generator Source Location : Default is Shukla
// Shukla default values
// CRY default values
{
  name: "GLOBAL",
  index: "config",
  flux: "cry",
  physics: "shielding",
  batchcommands: "",
}

{
  name: "CRY",
  index: "config",
  min_particles: 1.,
  max_particles: -1.,
  gen_neutrons: 1.,
  gen_protons: 0,
  gen_gammas: 0.,
  gen_electrons: 0.,
  gen_muons: 0,
  gen_pions: 0.,
  latitude: 0.0,
  altitude: 0.0,
  date: "1-1-2007",
}

{
  name: "FLUX",
  index: "source_box",
  size: ["world_box_length", "world_box_width", "1.*mm"],
  position: ["0.0","0.0", "0.5*world_box_depth - 2*mm"],
}

// Target for the main detector stack
{
  name: "FLUX",
  index: "target_box_0",
  size: ["20*m","20*m","1*mm"],
  position: ["0.0","0.0","-0.5*world_box_depth + bedrock_depth + 0.5*surface_soil_depth + subsurface_soil_depth"],

}
