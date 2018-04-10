// ---------------------------------------------------
// World Geometry : 20 x 20 x 30 AIR
// Then air and concrete base

{
	name: "GEO",
	index: "world",
	material: "G4_AIR",
	size: [30., 30.0, 10.],
	type: "box",
	color: [1.0,0.0,0.0,0.3],
}
{
        name: "GEO",
        index: "base",
        material: "G4_C",
        size: [30., 30.0, 1.],
        position: [0.0, 0.0, -4.5],
        mother: "world",
        type: "box",
        drawstyle: "wireframe",
        color: [0.2,0.2,0.2,0.8],
}




// ---------------------------------------------------
// Flux Generator Source Location : Default is Shukla
{
	name: "FLUX",
	index: "source_box",
       	size: [30.0,30.0,0.5],
        position: [0.0,0.0,4.75],
}
// Make a box so we can see Shukla location
//{
//	name: "GEO",
//	index: "shukla_source",
//	size: [30.0,30.0,0.5],
//	position: [0.0,0.0,4.75],
//	mother: "world",
//	material: "G4_AIR",
//	type: "box"
//}
