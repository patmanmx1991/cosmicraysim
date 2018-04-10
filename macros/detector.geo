{
  name: "FLUX",
  index: "target_box_0",
  position: [-2.0, 0.0, -1.8]
  size: [0.78, 4.2, 4.2]
}
//{
//  name: "GEO",
//  index: "target_box_0",
//  position: [-2.0, 0.0, -1.8]
//  size: [0.78, 4.2, 4.2]
//  mother: "world",
//  type: "box",
//}


{
  name: "FLUX",
  index: "target_box_1",
  position: [2.0, 0.0, -1.8]
  size: [0.78, 4.2, 4.2]
}
//{
//  name: "GEO",
//  index: "target_box_1",
//  position: [2.0, 0.0, -1.8]
//  size: [0.78, 4.2, 4.2]
//  mother: "world",
//  type: "box",
//  material: "G4_AIR",
//}




// ---------------------------------------------------
// List of detector objects that could be created
{
	name: "DETECTOR",
	index: "calo_0",
	type:  "truemuon"
}
{ name: "DETECTOR", index: "calo_1", clone: "calo_0" }
{ name: "DETECTOR", index: "calo_2", clone: "calo_0" }
{ name: "DETECTOR", index: "calo_3", clone: "calo_0" }
{ name: "DETECTOR", index: "calo_4", clone: "calo_0" }
{ name: "DETECTOR", index: "calo_5", clone: "calo_0" }
{ name: "DETECTOR", index: "calo_6", clone: "calo_0" }
{ name: "DETECTOR", index: "calo_7", clone: "calo_0" }
{ name: "DETECTOR", index: "calo_8", clone: "calo_0" }
{ name: "DETECTOR", index: "calo_9", clone: "calo_0" }
{ name: "DETECTOR", index: "calo_10", clone: "calo_0" }



// ---------------------------------------------------
// Detector Geometry : 3 2x2 argon trackers, one above, one either side
{ 
  name: "GEO",
  index: "tracker_geom_R1"
  mother: "world",
  type: "box",
  position: [-2.0, 0.0, -1.8]
  rotation: [0.0, 0.0, 90.0]
  size: [4.0, 0.75, 4.0]
  material: "G4_AIR",
}
{ 
  name: "GEO",
  index: "tracker_geom_R1_0",
  mother: "tracker_geom_R1",
  type: "box"
  position: [0.0,0.0,0.0],
  rotation: [0.0,0.0,0.0],
  sensitive: "calo_0",
  size: [3.5,0.5,3.5],
  material: "G4_Ar",
  color: [0.2,0.2,1.0,0.5],
}
{ 
  name: "GEO",
  index: "tracker_geom_R2"
  clone: "tracker_geom_R1"
  position: [2.0, 0.0, -1.8]
  rotation: [0.0, 0.0, 90.0]
}
{ 
  name: "GEO",
  index: "tracker_geom_R2_0",
  clone: "tracker_geom_R1_0"
  mother: "tracker_geom_R2",
  sensitive: "calo_1",
}


