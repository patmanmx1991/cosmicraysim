{
	name: "DETECTOR",
 index: "truemuon",
 type: "truemuon",
}
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
{
  name: "DETECTOR",
  index: "scintillator",
  type:  "scintillator"
  processor: 0,
  save: 0,
}
{
	name: "GEO",
	index: "scintillator_panel",
	type: "box",
	size: [1.9,1.9,0.05],
	size_units: "m"
	position: [0.0,0.0,0.10],
	position_units: "m",
	material: "G4_C",
	color: [0.,0.,1.,0.2],
	mother: "volume",
	drawstyle: "solid"
	sensitive: "scintillator"
}

{
  name: "DETECTOR",
  index: "longdrift",
  type:  "longdrift"
  restrict_x: 1,
  restrict_y: 0,
  restrict_z: 1,
  resolution_y: 0.01,
  resolution_x: 0.01,
  resolution_y: 0.01,
//  wire_y: "3.0*cm",
  processor: 0,
  save: 0,
}

{
	name: "GEO",
	index: "drift_chamber",
	type: "box",
	size: [1.80,0.60,0.07],
	size_units: "m"
	position_units: "m",
	material: "G4_Ar",
	color: [1.,0.,0.,0.3],
	drawstyle: "wireframe"
	mother: "volume"
	sensitive: "longdrift"
}

{
	name: "GEO",
	index: "chamber_positions",
	// pos and rot [x,y,z,thx,thy,thz] in m
	chamber_0: [0.0,-0.62,-0.48, 0.0, 0.0, 0.0],
	chamber_1: [0.0,0.0,-0.48, 0.0, 0.0, 0.0],
	chamber_2: [0.0,0.62,-0.48, 0.0, 0.0, 0.0],
	chamber_3: [-0.62,0.0,-0.39, 0.0, 0.0, 90.0],
	chamber_4: [0.0,0.0,-0.39, 0.0, 0.0, 90.0],
	chamber_5: [0.62,0.0,-0.39, 0.0, 0.0, 90.0],
	chamber_6: [-0.62,0.0,-0.28, 0.0, 0.0, 90.0],
	chamber_7: [0.0,0.0,-0.28, 0.0, 0.0, 90.0],
	chamber_8: [0.62,0.0,-0.28, 0.0, 0.0, 90.0],
	chamber_9: [0.0,-0.62,-0.19, 0.0, 0.0, 0.0],
	chamber_10: [0.0,0.0,-0.19, 0.0, 0.0, 0.0],
	chamber_11: [0.0,0.62,-0.19, 0.0, 0.0, 0.0],
	chamber_12: [0.0,-0.62,-0.07, 0.0, 0.0, 0.0],
	chamber_13: [0.0,0.0,-0.07, 0.0, 0.0, 0.0],
	chamber_14: [0.0,0.62,-0.07, 0.0, 0.0, 0.0],
	chamber_15: [-0.62,0.0,0.02, 0.0, 0.0, 90.0],
	chamber_16: [0.0,0.0,0.02, 0.0, 0.0, 90.0],
	chamber_17: [0.62,0.0,0.02, 0.0, 0.0, 90.0],
}






