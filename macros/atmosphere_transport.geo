// https://en.wikipedia.org/wiki/International_Standard_Atmosphere
// Layer // Name 	// Geopotential Altitude above MSL  h (m) // Geometric Altitude above MSL z (m) // Lapse Rate ( °C/km) // Temperature T (°C) 	// Pressure p (Pa) 	// Density ρ (kg/m3)
// 0 	Troposphere 	-610 	-611 	−6.5 	+19.0 	108,900 (1.075 bar) 	1.2985
// 1 	Tropopause 	11,000 	11,019 	+0.0 	−56.5 	22,632 	0.3639
// 2 	Stratosphere 	20,000 	20,063 	+1.0 	−56.5 	5474.9 	0.0880
// 3 	Stratosphere 	32,000 	32,162 	+2.8 	−44.5 	868.02 	0.0132
// 4 	Stratopause 	47,000 	47,350 	+0.0 	−2.5 	110.91 	0.0020
// 5 	Mesosphere 	51,000 	51,413 	−2.8 	−2.5 	66.939
// 6 	Mesosphere 	71,000 	71,802 	−2.0 	−58.5 	3.9564
// 7 	Mesopause 	84,852 	86,000 	— 	−86.28 	0.3734

{
  name: "VARIABLE",
  index: "atmos_constants",

  world_thickness : "200.0*km",

  troposphere0_thickness : "11.0*km",

  troposphere1_thickness : "9.0*km",

  stratosphere0_thickness : "12*km",

  stratosphere1_thickness : "15.0*km",

  stratosphere2_thickness : "4.0*km",

  mesosphere0_thickness : "20.0*km",

  mesosphere1_thickness : "15*km",

}


// ---------------------------------------------------
// World Geometry
{
  name: "GEO",
  index: "world",
  material: "G4_Galactic",
  size: ["100.*km", "100.0*km", "world_thickness"],
  type: "box"
}

// --------------------------------------------------
// Detector
{
  name: "DETECTOR",
  index: "atmos_det",
  type: "neutron"
}

{
  name: "GEO",
  index: "troposphere0",
  material: "G4_AIR",
  density: "0.0012985",
  mother: "world",
  size: ["100*km", "100.0*km", "troposphere0_thickness"],
  position: ["0.0", "0.0", "-0.5*world_thickness+0.5*troposphere0_thickness"],
  type: "box",
  color: [0.1,0.7,1.0,0.9],
  drawstyle: "solid",
  sensitive:"atmos_det"
}

{
name: "GEO",
index: "troposphere1",
clone: "troposphere0",
density:"0.003639",
size: ["100*km", "100.0*km", "troposphere1_thickness"],
position: ["0.0", "0.0", "-0.5*world_thickness + troposphere0_thickness + 0.5*troposphere1_thickness"],
color: [0.1,0.1,1.0,0.9],
drawstyle: "solid",
sensitive:"atmos_det"
}

{
name: "GEO",
index: "stratosphere0",
clone: "troposphere0",
density:"	0.000880",
size: ["100*km", "100.0*km", "stratosphere0_thickness"],
position: ["0.0", "0.0", "-0.5*world_thickness + troposphere0_thickness + troposphere1_thickness + 0.5*stratosphere0_thickness"],
color: [0.3,0.3,1.0,0.8],
drawstyle: "solid",
sensitive:"atmos_det"
}

{
name: "GEO",
index: "stratosphere1",
clone: "troposphere0",
density:"0.000132",
size: ["100*km", "100.0*km", "stratosphere1_thickness"],
position: ["0.0", "0.0", "-0.5*world_thickness + troposphere0_thickness + troposphere1_thickness + stratosphere0_thickness + 0.5*stratosphere1_thickness"],
color: [0.4,0.4,1.0,0.4],
drawstyle: "solid",
sensitive:"atmos_det"
}

{
name: "GEO",
index: "stratosphere2",
clone: "troposphere0",
density:"0.0020",
size: ["100*km", "100.0*km", "stratosphere2_thickness"],
position: ["0.0", "0.0", "-0.5*world_thickness + troposphere0_thickness + troposphere1_thickness + stratosphere0_thickness + stratosphere1_thickness + 0.5*stratosphere2_thickness"],
color: [0.5,0.5,1.0,0.2],
drawstyle: "solid",
sensitive:"atmos_det"
}

{
name: "GEO",
index: "mesosphere0",
type: "box",
material: "G4_AIR"
density:"0.0004",
size: ["100*km", "100.0*km", "mesosphere0_thickness"],
position: ["0.0", "0.0", "-0.5*world_thickness + troposphere0_thickness + troposphere1_thickness + stratosphere0_thickness + stratosphere1_thickness + stratosphere2_thickness + 0.5*mesosphere0_thickness"],
color: [0.6,0.6,1.0,0.1],
drawstyle: "solid",
sensitive:"atmos_det"
}

{
name: "GEO",
index: "mesosphere1",
type: "box",
position: ["0.0", "0.0", "-0.5*world_thickness + troposphere0_thickness + troposphere1_thickness + stratosphere0_thickness + stratosphere1_thickness + stratosphere2_thickness + mesosphere0_thickness + 0.5*mesosphere1_thickness"],
material: "G4_AIR"
density:"0.00001",
size: ["100*km", "100.0*km", "mesosphere1_thickness"],
sensitive:"atmos_det",
color: [0.7,0.7,1.0,0.05],
drawstyle: "solid"
}

// ---------------------------------------------------
// Flux Generator Source Location : Default is Shukla
{
  name: "SHUKLA",
  index: "config",
  parameters: "proton"
  radius: 0.0,
  distance: 1000000
}

{
  name: "FLUX",
  index: "source_box",
  size: ["100.0*km", "100.0*km", "1.0*mm"],
  position: ["0.0","0.0","world_thickness*0.5"]
}

// Target for the main detector stack
{
  name: "FLUX",
  index: "target_box_0",
  size: ["1.0*km", "1.0*km", "1.0*mm"],
  position: ["0.0","0.0","-world_thickness*0.5"]
}
