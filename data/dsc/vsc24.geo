{
	name: "GEO",
	index: "volume",
	type: "tubs",
	r_max: 1.36,
	size_z: 2.4,
	position: [0.0,0.0,-1.6],
	rotation: [0.0,0.0,0.0],
	material: "G4_AIR",
}
{
        name: "GEO",
        index: "concrete_cask",
        mother: "volume",
        type: "tubs",
        r_max: 1.34,
	r_min: 0.80825,
        size_z: 2.40,
        position: [0.0,0.0,0.0],
        rotation: [0.0,0.0,0.0],
        material: "G4_C",
        drawstyle: "solid",
        color: [0.3,0.3,0.3,0.4],
}
{
        name: "GEO",
        index: "concrete_bottom",
        mother: "volume",
        type: "tubs",
        r_max: 0.80825,
        size_z: 0.05,
        position: [0.0,0.0,-2.35],
        rotation: [0.0,0.0,0.0],
        material: "G4_C",
        drawstyle: "solid",
        color: [0.3,0.3,0.3,0.4],
}
{
        name: "GEO",
        index: "steel_shell",
        mother: "volume",
        type: "tubs",
        r_max: 0.80825,
	r_min: 0.78375,
        size_z: 2.30,
        position: [0.0,0.0,0.0],
        rotation: [0.0,0.0,0.0],
        material: "G4_Fe",
        drawstyle: "solid",
        color: [0.3,0.3,0.3,0.8],
}

{
        name: "GEO",
        index: "steel_bottom",
        mother: "volume",
        type: "tubs",
        r_max: 0.80825,
        size_z: 0.0254,
        position: [0.0,0.0,-2.3127],
        rotation: [0.0,0.0,0.0],
        material: "G4_Fe",
        drawstyle: "solid",
        color: [0.3,0.3,0.3,0.8],
}

{
        name: "GEO",
        index: "steel_top",
        mother: "volume",
        type: "tubs",
        r_max: 0.80825,
        size_z: 0.0254,
        position: [0.0,0.0,2.3127],
        rotation: [0.0,0.0,0.0],
        material: "G4_Fe",
        drawstyle: "solid",
        color: [0.3,0.3,0.3,0.8],
}

{
	name: "GEO",
	index: "rod_positions",
	rod_0:  [  0.60, -0.12, 0.0],
	rod_1:  [  0.60,  0.12, 0.0],
	rod_2:  [  0.36, -0.36, 0.0],
	rod_3:  [  0.36, -0.12, 0.0],
	rod_4:  [  0.36,  0.12, 0.0],
	rod_5:  [  0.36,  0.36, 0.0],
	rod_6:  [  0.12, -0.60, 0.0],
	rod_7:  [  0.12, -0.36, 0.0],
	rod_8:  [  0.12, -0.12, 0.0],
	rod_9:  [  0.12,  0.12, 0.0],
	rod_10: [  0.12,  0.36, 0.0],
	rod_11: [  0.12,  0.60, 0.0],
        rod_12: [ -0.12, -0.60, 0.0],
        rod_13: [ -0.12, -0.36, 0.0],
        rod_14: [ -0.12, -0.12, 0.0],
        rod_15: [ -0.12,  0.12, 0.0],
        rod_16: [ -0.12,  0.36, 0.0],
        rod_17: [ -0.12,  0.60, 0.0],
	rod_18: [ -0.36, -0.36, 0.0],
        rod_19: [ -0.36, -0.12, 0.0],
        rod_20: [ -0.36,  0.12, 0.0],
        rod_21: [ -0.36,  0.36, 0.0],
        rod_22: [ -0.60, -0.12, 0.0],
        rod_23: [ -0.60,  0.12, 0.0],
}

{
        name: "GEO",
        index: "uranium_rod",
        mother: "volume",
        type: "box",
        size: [0.21,0.21,4.0]
        rotation: [0.0,0.0,0.0],
        material: "G4_U",
        drawstyle: "solid",
        color: [0.1,1.0,0.1,0.4],
}


{
        name: "GEO",
        index: "tungsten_rod",
        mother: "volume",
        type: "box",
        size: [0.21,0.21,4.0]
        rotation: [0.0,0.0,0.0],
        material: "G4_W",
        drawstyle: "solid",
        color: [0.6,0.4,0.1,0.4],
}

{
        name: "GEO",
        index: "lead_rod",
        mother: "volume",
        type: "box",
        size: [0.21,0.21,4.0]
        rotation: [0.0,0.0,0.0],
        material: "G4_Pb",
        drawstyle: "solid",
        color: [0.1,0.1,0.6,0.4],
}

{
        name: "GEO",
        index: "empty_rod",
        mother: "volume",
        type: "box",
        size: [0.21,0.21,4.0]
        rotation: [0.0,0.0,0.0],
        material: "G4_AIR",
        color: [0.1,0.1,0.1,0.4],
}
