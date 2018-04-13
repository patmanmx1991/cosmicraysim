// ---------------------------------------------------
// World Geometry : 20 x 20 x 30 AIR
// Then air and carbon base

{
        name: "GEO",
        index: "world",
        material: "G4_AIR",
        size: [30., 30.0, 10.],
        type: "box",
}
{
        name: "GEO",
        index: "base",
        material: "G4_C",
        size: [30., 30.0, 1.],
        position: [0.0, 0.0, -4.5],
        mother: "world",
        type: "box",
	color: [0.3,0.3,0.3,0.6],
	drawstyle: "solid",
}

// ---------------------------------------------------
// Flux Generator Source Location : Default is Shukla
{
        name: "FLUX",
        index: "source_box",
        size: [30.0,30.0, 0.5],
        position: [0.0,0.0, 4.75],
}

{ 
  name: "GEO",
  index: "source_box",
  type: "box",
  material: "G4_AIR",
  size: [30.0,30.0, 0.5],
  position: [0.0,0.0, 4.75],
  mother: "world",
}

// ---------------------------------------------------
// Problem Geometry : i.e our nuclear drum
{
	name: "GEO",
	index: "cask",
	mother: "world",
	type: "DSC_VSC24",
	position: [0.0,0.0,-1.6],
	rotation: [0.0,0.0,0.0],
//	matrep_index: [9],
//	matrep_material: ["empty_rod"],
	storage_mask: [0,1,3,4,6,7,8,9,10,11,12,13,14,15,16,17,19,20,22,23]
}

// ---------------------------------------------------
// Detector Solution : Two True Muon Trackers
// ---------------------------------------------------
// List of detector objects that could be created
{
  name: "DETECTOR",
  index: "trkr0",
  type:  "truemuon"
}
{
  name: "GEO",	
  index: "tracker_0",
  position: [-2.0, 0.0, -1.8]
  size: [0.78, 4.2, 4.2]
  mother: "world",
  type: "box",
  material: "G4_Ar"
  sensitive: "trkr0"
  color: [1.0,0.0,0.0]
}
{
  name: "FLUX",
  index: "target_box_0",
  position: [-2.0, 0.0, -1.8]
  size: [1.0, 5.0, 5.0]
}
{
  name: "DETECTOR",
  index: "trkr1",
  type:  "truemuon"
}
{
  name: "GEO",
  index: "tracker_1",
  position: [2.0, 0.0, -1.8]
  size: [0.78, 4.2, 4.2]
  mother: "world",
  type: "box",
  material: "G4_Ar"
  sensitive: "trkr1"
  color: [1.0,0.0,0.0]
}
{
  name: "FLUX",
  index: "target_box_1",
  position: [2.0, 0.0, -1.8]
  size: [1.0, 5.0, 5.0]
}



// ---------------------------------------------------
// Triggers	
// Only save events when both detectors hit
{
  name: "TRIGGER",
  index: "coincidence",
  type: "simple",
  detectors: ["tracker_0_trkr0","tracker_1_trkr1"]
}
