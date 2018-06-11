// ---------------------------------------------------
// World Geometry : 20 x 20 x 30 AIR
// Then air and carbon base
{
  name: "GLOBAL",
  index: "config",
  flux: "cry",
  physics: "QGSP_BERT_HP",
  batchcommands: "",
  //gdml: "detector.gdml",
}
{
  name: "GEO",
  index: "world",
  material: "G4_AIR",
  size: ["3.*m", "3.0*m", "3.*m"],
  type: "box",
}

// --------------------------------------------------
// AWE Detector
{
  name: "GEO",
  index: "system",
  type: "hybrid_muontom",
  mother: "world",
  position: ["0.0","0.0","-0.374*m"],
  input_file: "onlyrpcdetector.geo"
}

{
  name: "GEO",
  index: "system_above",
  type: "hybrid_muontom",
  mother: "world",
  position: ["0.0","0.0","0.382*m"],
  input_file: "onlyrpcdetector.geo"
}

{
  name: "TRIGGER",
  index: "trig",
  type: "simple"
  processors: ["system_above","system"]
}

// ---------------------------------------------------
// Flux Generator Source Location : Default is Shukla
{
  name: "FLUX",
  index: "source_box",
  size: ["1.0*m", "1.0*m", "0.001*m"],
  position: ["0.0","0.0", "1.42*m"],
}

{
  name: "FLUXNULL",
  index: "target_box_0",
  size: ["2.0*m","2.0*m","2.0*m"],
  position: [0.0,0.0,0.0]
}

{
name: "DETECTOR"
index: "muoninouttracker"
type:  "muoninouttracker"
}

{
name:     "GEO",
index:    "targetregion"
type:     "box",
material: "G4_AIR"
size:     ["0.6*m","0.6*m","0.3*m"]
position: [0.0,0.0,0.0]
mother:   "world",
sensitive: "muoninouttracker"
}

{
name: "GEO",
index: "sSteelOuter"
type: "tubs"
r_max: "14.5*cm"
r_min: "13.0*cm"
size_z: "40*cm"
drawstyle: "solid"
mother: "targetregion"
}

{
name: "GEO",
index: "sSteelOuter"
type: "tubs"
mother: "targetregion"
material: "G4_Fe"
r_max: "14.5*cm"
r_min: "13.0*cm"
size_z: "40*cm"
position: ["0.0","0.0","0.0"]
rotation: [90.0,0.0,0.0]
}

{
name: "GEO", index: "sSteelCap", clone: "sSteelOuter",
r_max: "15*cm", r_min: "0*cm" size_z: "3.5*cm"
position: ["0*cm","-21.75*cm","0.0"]
}

{
name: "GEO", index: "sSteelBase", clone: "sSteelOuter", 
r_max: "14.5*cm", r_min: "0*cm", size_z: "2*cm",
position: ["0*cm","21.*cm","0.0"]
}

{
name: "GEO", index: "sConcreteTube", mother: "targetregion"
type: "tubs", material: "G4_CONCRETE"
r_max: "13*cm", size_z: "40*cm", 
position: ["0.0","0.0","0.0"], rotation: [90.0,0.0,0.0],
drawstyle: "solid", color: [0.5,0.5,0.5,0.5]
}


// ---------------------------
// Make Sheet
{
name: "GEO", index: "sUSheet" mother: "sConcreteTube"
type: "box" material: "G4_U"
size: ["10*cm","10*cm","10*cm"]
position: ["0.0*cm","0.0*cm","0.0*cm"]
drawstyle: "solid", color: [0.0,1.0,0.0,0.5]
}
