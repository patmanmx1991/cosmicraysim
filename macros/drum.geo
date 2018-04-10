
// ---------------------------------------------------
// Problem Geometry : i.e our nuclear drum
{
	name: "GEO",
	index: "cask",
	mother: "world",
	type: "tubs",
	r_max: 1.35.,
	size_z: 2.4.,
	position: [0.0,0.0,-1.6],
	rotation: [0.0,0.0,0.0],
	material: "G4_AIR",
	drawstyle: "solid",
	color: [0.2,0.2,0.2,0.8],
}

{
        name: "GEO",
        index: "inner",
        mother: "cask",
        type: "tubs",
        r_max: 1.10.,
        size_z: 2.15.,
        position: [0.0,0.0,0.0],
        rotation: [0.0,0.0,0.0],
        material: "G4_AIR",
        drawstyle: "solid",
        color: [0.3,0.3,0.3,0.8],
}

{
        name: "GEO",
        index: "uranium_rod_0",
        mother: "inner",
        type: "box",
	    size: [0.21,0.21,4.0]
        position: [0.12,0.12,0.0],
        rotation: [0.0,0.0,0.0],
        material: "G4_U",
        drawstyle: "solid",
	color: [0.2,1.0,0.2,0.3],
}




//{ name: "GEO", clone: "uranium_rod_0", index: "uranium_rod_1", position: [ -0.60, 0.12, 0.0 ] }
//{ name: "GEO", clone: "uranium_rod_0", index: "uranium_rod_2", position: [ -0.36, 0.12, 0.0 ] }
{ name: "GEO", clone: "uranium_rod_0", index: "uranium_rod_3", position: [ -0.12, 0.12, 0.0 ] }
//{ name: "GEO", clone: "uranium_rod_0", index: "uranium_rod_4", position: [  0.12, 0.12, 0.0 ] }
//{ name: "GEO", clone: "uranium_rod_0", index: "uranium_rod_5", position: [  0.36, 0.12, 0.0 ] }
//{ name: "GEO", clone: "uranium_rod_0", index: "uranium_rod_6", position: [  0.60, 0.12, 0.0 ] }

//{ name: "GEO", clone: "uranium_rod_0", index: "uranium_rod_7", position: [ -0.36, 0.36, 0.0 ] }
{ name: "GEO", clone: "uranium_rod_0", index: "uranium_rod_8", position: [ -0.12, 0.36, 0.0 ] }
{ name: "GEO", clone: "uranium_rod_0", index: "uranium_rod_9", position: [  0.12, 0.36, 0.0 ] }
//{ name: "GEO", clone: "uranium_rod_0", index: "uranium_rod_10", position: [  0.36, 0.36, 0.0 ] }

//{ name: "GEO", clone: "uranium_rod_0", index: "uranium_rod_11", position: [ -0.12, 0.60, 0.0 ] }
//{ name: "GEO", clone: "uranium_rod_0", index: "uranium_rod_12", position: [  0.12, 0.60, 0.0 ] }


//{ name: "GEO", clone: "uranium_rod_0", index: "uranium_rod_13", position: [ -0.60, -0.12, 0.0 ] }
{ name: "GEO", clone: "uranium_rod_0", index: "uranium_rod_14", position: [ -0.36, -0.12, 0.0 ] }
//{ name: "GEO", clone: "uranium_rod_0", index: "uranium_rod_15", position: [ -0.12, -0.12, 0.0 ] }
//{ name: "GEO", clone: "uranium_rod_0", index: "uranium_rod_16", position: [  0.12, -0.12, 0.0 ] }
{ name: "GEO", clone: "uranium_rod_0", index: "uranium_rod_17", position: [  0.36, -0.12, 0.0 ] }
//{ name: "GEO", clone: "uranium_rod_0", index: "uranium_rod_18", position: [  0.60, -0.12, 0.0 ] }

{ name: "GEO", clone: "uranium_rod_0", index: "uranium_rod_19", position: [ -0.36, -0.36, 0.0 ] }
//{ name: "GEO", clone: "uranium_rod_0", index: "uranium_rod_20", position: [ -0.12, -0.36, 0.0 ] }
//{ name: "GEO", clone: "uranium_rod_0", index: "uranium_rod_21", position: [  0.12, -0.36, 0.0 ] }
{ name: "GEO", clone: "uranium_rod_0", index: "uranium_rod_22", position: [  0.36, -0.36, 0.0 ] }

{ name: "GEO", clone: "uranium_rod_0", index: "uranium_rod_23", position: [ -0.12, -0.60, 0.0 ] }
{ name: "GEO", clone: "uranium_rod_0", index: "uranium_rod_24", position: [  0.12, -0.60, 0.0 ] }





