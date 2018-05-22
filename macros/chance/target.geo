{
  name: "GLOBAL",
  index: "config",
  flux: "shukla",
  physics: "QGSP_BERT_HP",
  batchcommands: "",
  // gdml: "detector.gdml"
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
size: ["10*cm","10*cm","0.5*cm"]
position: ["0*cm","0.0*cm","17*cm"]
drawstyle: "solid", color: [0.0,1.0,0.0,0.5]
}

// Make Rods
{
name: "GEO", index: "sURod", mother: "sConcreteTube"
type: "tubs", material: "G4_U",
size_z: "10*cm", r_max: "1*cm",
position: ["0.0","0.0","5*cm"], rotation: [0.0,90.0,0.0]
drawstyle: "solid", color: [0.0,1.0,0.0,0.5]
}

{
name: "GEO", index: "sAlRod", clone: "sURod",
material: "G4_Al", position: ["0.0","0.0","13*cm"]
color: [1.0,1.0,1.0,0.5]
}

{
name: "GEO", index: "sFeRod", clone: "sURod",
material: "G4_Fe", position: ["0.0","0.0","9*cm"]
color: [1.0,0.5,0.0,0.5]
}

{
name: "GEO", index: "sPlasticRod", clone: "sURod",
material: "G4_POLYETHYLENE", position: ["0.0","0.0","1*cm"]
color: [0.5,0.5,1.0,0.5]
}

// Make Pennys
{
name: "GEO", index: "lvWPenny1"	mother: "sConcreteTube"
type: "tubs", material: "G4_W",
size_z: "1*cm", r_max: "1*cm",
position: ["0.0","0.0","-3*cm"], 
drawstyle: "solid", color: [1.0,0.0,0.0,0.5]
}

{
name: "GEO", index: "lvWPenny2"	clone: "lvWPenny1",
r_max: "2*cm", position: ["0.0","0.0","-7*cm"], 
}

{
name: "GEO", index: "lvWPenny3"	clone: "lvWPenny1",
r_max: "3*cm", position: ["0.0","0.0","-10*cm"], 
}

// Make Air Pocket
{
name: "GEO", index: "lvAirPocket"	mother: "sConcreteTube"
type: "tubs", material: "G4_AIR",
size_z: "5*cm", r_max: "10*cm",
position: ["0.0","0.0","-15*cm"], 
}




