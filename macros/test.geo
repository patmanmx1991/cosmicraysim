{
name: "GEO",
index: "world",
material: "G4_AIR",
size: [20., 30.0, 20.],
type: "box",
}


{
  name: "FLUX",
  index: "cosmic_generator",
  type: "shukla",
}
{
  name: "SHUKLA",
  index: "global",
  source_box: "world"
}

{
name: "SHUKLA",
index: "source_box",
size: [15.0,0.01,15.0],
position: [0.0,14.9,0.0],
}

{
name: "GEO",
index: "shukla_source",
size: [15.0,0.01,15.0],
position: [0.0,14.9,0.0],
material: "G4_AIR",
mother: "world",
type: "box"
}


{
	name: "GEO",
	index: "detbox",
	size: [20.,2.,20.],
	position: [0.0,13.0,0.0],
	material: "G4_C",
	mother: "world",
	sensitive: "muons",
	type: "box"
}
{
	name: "DETECTOR",	
	index: "muons",
	type: "edep",
}

{
        name: "GEO",
        index: "detbox2",
        size: [20.,2.,20.],
        position: [0.0,10.0,0.0],
        material: "G4_C",
        mother: "world",
        sensitive: "muons2",
        type: "box"
}
{
        name: "DETECTOR",
        index: "muons2",
        type: "edep",
}

