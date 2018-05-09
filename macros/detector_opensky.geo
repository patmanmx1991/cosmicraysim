// ---------------------------------------------------
// World Geometry : 20 x 20 x 30 AIR
// Then air and carbon base
{
  name: "GEO",
  index: "world",
  material: "G4_AIR",
  size: ["100.*m", "20.0*m", "20.*m"],
  type: "box",
}

// --------------------------------------------------
// Detector
{
  name: "DETECTOR",
  index: "scint",
  type: "scintillator",
}

{
  name: "DETECTOR",
  index: "truemuon",
  type: "truemuon",
}

{
  name: "GEO",
  index: "detector",
  type: "box",
  mother: "world",
  material: "G4_AIR",
  size: ["2.0*m","2.0*m","2.0*m"],
  position: ["0.0*m","0.0","0.0"],
  sensitive: "truemuon",
}


//--------------------------------------------
// Main mother detector volume (for easy placement)
{
  name: "GEO",
  index: "bar0",
  type: "box",
  mother: "detector",
  material: "G4_PLASTIC_SC_VINYLTOLUENE",
  size: ["4*cm","80*cm","10*cm"],
  position: ["0.0","0.0","0.0"],
  sensitive: "scint",
}

//----------------
// Bar definitions
{
  name: "VARIABLE",
  index: "detspacing",
  offset: "-0.5*m",
  spacing: "10*cm"
  xsep: "20*cm",
  lowbar: "0.0*cm"
}
{
  name: "GEO",
  index: "bar0",
  type: "box",
  mother: "detector",
  material: "G4_PLASTIC_SC_VINYLTOLUENE",
  size: ["4*cm","80*cm","10*cm"],
  position: ["0.0","0.0","0.0*spacing+offset"],
  sensitive: "scint",
  color: [0.0,0.0,1.0],
}

// - The main stack of scintillator detectors
{ name: "GEO", index: "bar1", clone: "bar0", position: ["0.0","0.0","1*spacing+offset"] }
{ name: "GEO", index: "bar2", clone: "bar0", position: ["0.0","0.0","2*spacing+offset"] }
{ name: "GEO", index: "bar3", clone: "bar0", position: ["0.0","0.0","3*spacing+offset"] }
{ name: "GEO", index: "bar4", clone: "bar0", position: ["0.0","0.0","4*spacing+offset"] }
{ name: "GEO", index: "bar5", clone: "bar0", position: ["0.0","0.0","4*spacing+offset"] }

// - The lower two scintillator detectors
{ name: "GEO", index: "bar6", clone: "bar0", position: ["xsep","0.0","offset+lowbar"] }
{ name: "GEO", index: "bar7", clone: "bar0", position: ["-1.0*xsep","0.0","offset+lowbar"] }


// ---------------------------------------------------
// Flux Generator Source Location : Default is Shukla
{
  name: "FLUX",
  index: "source_box",
  size: ["100.*m", "20.0*m", "20.*m"],
  position: ["0.0","0.0", "9.75*m"],
}

// Target for the main detector stack
{
  name: "FLUX",
  index: "target_box_0",
  size: ["4*cm","80*cm","6.0*10.0*cm"],
  position: ["0.0","0.0","0.0"],
}

// Target for one lower detector
{
  name: "FLUX",
  index: "target_box_1",
  size: ["4*cm","80*cm","10*cm"],
  position: ["xsep","0.0","offset+lowbar"],
}

// Target for the other lower detector
{
  name: "FLUX",
  index: "target_box_2",
  size: ["4*cm","80*cm","10*cm"],
  position: ["-xsep","0.0","offset+lowbar"],
}
