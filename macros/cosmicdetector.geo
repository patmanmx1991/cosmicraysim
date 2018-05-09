// ---------------------------------------------------
// World Geometry : 20 x 20 x 30 AIR
// Then air and carbon base
{
  name: "GEO",
  index: "world",
  material: "G4_AIR",
  size: ["3.*m", "3.0*m", "2.*m"],
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
  xsep: "80*cm",
  lowbar: "5.0*cm"
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
{ name: "GEO", index: "bar1", clone: "bar0", position: ["0.0","0.0","1*spacing+offset"] }
{ name: "GEO", index: "bar2", clone: "bar0", position: ["0.0","0.0","2*spacing+offset"] }
{ name: "GEO", index: "bar3", clone: "bar0", position: ["0.0","0.0","3*spacing+offset"] }
{ name: "GEO", index: "bar4", clone: "bar0", position: ["0.0","0.0","4*spacing+offset"] }

{ name: "GEO", index: "bar5", clone: "bar0", position: ["xsep","0.0","offset+lowbar"] }
{ name: "GEO", index: "bar6", clone: "bar0", position: ["-1.0*xsep","0.0","offset+lowbar"] }









// ---------------------------------------------------
// Flux Generator Source Location : Default is Shukla
{
  name: "FLUX",
  index: "source_box",
  size: ["3.0*m", "3.0*m", "0.5*m"],
  position: ["0.0","0.0", "9.75*m"],
}
