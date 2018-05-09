# Making A Geometry Table

The geometry factory is setup to load in all of the different JSON "GEO" tables, and create the world as the program starts.
This way simple geometries can be created without having to recompile each time, but there is still freedom to have complex geometry definitions hardcoded to be generated from a JSON table input format and placed anywhere within the world.

It is recommended that all tables defining the geometry you care about are put into some ".geo" files inside your working directory, so they can be loaded at runtime using the command ```cosmicraysim -g mytable.geo ...```.


## Geometry Tables
At the bare minimum the geometry tables require the "name", "index", "type", and "mother"* fields.

```
{
  name: "GEO",
  index: "mybox",
  type: "box",
  mother: "world"
  ...
}
```

- **name** : Tells the framework its a GEO table.
- **index** : Index of this table. The index is ALSO used to define the name of the volume being created.
- **type** : Tells the framework what actual object to create. This is passed to the GeoObjectFactory::Construct() function, which uses it to initialise objects.
- **mother** : Tells the framework what mother volume this object should be placed inside. The exception is that the world volume does not require this, and should always be defined as a simple box placed at the origin.



The following table should be used as a template to define the base world volume. Its index should always be left as "world", and other volumes should refer to their mother as "world" if they want to be placed inside it.
```
{
  name: "GEO",     // required
  index: "world",  // required
  type: "box",     // required
  //
  size: ["30*m","30*m","30*m"], // user defined
  material: "G4_AIR",           // user defined
}
```


## Geomtry Types

There is limited support for some simple geometry objects to be loaded at runtime, alongside more complex objects to be placed inside a chosen mother volume. These objects all inherit from the GeoObject class.




### Solids

The GeoSolids class is used for making simple solids and setting their drawoptions and positions correctly. Each geosolid contains all of its solid, logical, and physical volumes in one class. The following types all inherit from the GeoSolids object

- box
- tubs


Each of these objects therefore have the following fields that can be changed in the JSON table.

- **material (required)** : Sets the material of the logical volume. Can use any NIST manager string, e.g. "G4_Pb" returns lead.
- **mother** : Set the index of the table defining the mother volume. This is required for all objects, except the world volume.
- **position [units]** : Position of the center of the solid, given in terms of its mother co-ordinate system. Default is [0.,0.,0.].
- **rotation** : Rotation of the center of the solid about each axis in degrees, given in terms of its mother co-ordinate system. Default is [0.,0.,0.].
- **drawstyle** : Set the visulation style. Options are "wireframe" or "solid". Default is "wireframe".
- **color** : Provide a real RGB vector for setting the draw color. Can be either [R,G,B], or [R,G,B,A].


Any value that is tagged with [units] requires the units to be defined inside the entry, otherwise it will refer to the GEANT4 defaults, which are usually awkward to use. Notes on defining units are given in the "Editing Tables" documentation.


#### Solid : "box"
A simple G4Box can be created by specifying its size and position inside the world. It has the following options.

- **size [units]  (required)** : Length in x, y, and z, in meters.

** Example : **
```
{
        name: "GEO",
        index: "base",
        material: "G4_C",
        mother: "world",
        type: "box",
        drawstyle: "wireframe",
        color: [0.2,0.2,0.2,0.8],
        size: [30., 30.0, 1.],
        size_units: "m",
        position: ["0.0*m", "0.0*m", "-4.5*m"],
}
```


#### Solid : "tubs"
A simple G4Tubs object can be created by giving its size in the z-axis and its radius. Tt has the following options

- **r_max [units]  (required)**: Max radius.
- **size_z [units]  (required)**: Length of the tube.
- **r_min [units]** : Min radius, for making hollow tubes. Default is 0.0.
- **phi_start** : Minimum Phi Angle in radians, for making semi cylinders. Default is 0.0.
- **phi_delta** : Maximum Phi Angle in radians, for making semi cylinders. Default is 2pi.

** Example : **
```
{
  name: "GEO",
  index: "cask",
  mother: "world",
  type: "tubs",
  r_max: "1.35.*m",
  r_min: "0.35*m",
  size_z: "2.4.*m",
  phi_start: 0.0,
  phi_delta: 1.0,
  position: [0.0,0.0,-1.6],
  position_units: "m",
  rotation: [0.0,0.0,90.0],
  material: "G4_AIR",
  drawstyle: "solid",
  color: [0.2,0.2,0.2,0.8],
}
```


### Solid : "cone",
A simple G4Cons object can be created by givin the cone inner/outer dimensions at each end. It has the following options.

- **r_max1 [units]  (required)**: Max radius at base.
- **size_z [units]  (required)**: Length of the cone.
- **r_min1 [units]** : Min radius at base, for making hollow cones. Default is 0.0.
- **r_max2 [units]** : Max radius at tip, for making truncated cones. Default is 0.0.
- **r_min2 [units]** : Min radius at tip, for making hollow truncated cones. Default is 0.0.
- **phi_start** : Minimum Phi Angle in radians, for making semi cones. Default is 0.0.
- **phi_delta** : Delta phi Angle in radians, for making semi cones. Default is 2pi.


** Example : **
```
{
  name: "GEO",
  index: "cask",
  mother: "world",
  type: "cons",
  r_max1: "1.35.*m",
  r_max2: "0.35.*m",
  r_min1: "0.15.*m",
  r_min2: "0.25.*m",
  size_z: "2.4.*m",
  phi_start: 0.0,
  phi_delta: 1.0,
  position: [0.0,0.0,-1.6],
  position_units: "m",
  rotation: [0.0,0.0,90.0],
  material: "G4_AIR",
  drawstyle: "solid",
  color: [0.2,0.2,0.2,0.8],
}
```


#### Solid : "eliptube"
A simple G4EllipticalTube can be created by specifying its size and position inside the world. It has the following options.

- **size [units]  (required)** : Length in x, y, and z, in meters.

** Example : **
```
{
        name: "GEO",
        index: "base",
        material: "G4_C",
        mother: "world",
        type: "eliptube",
        drawstyle: "wireframe",
        color: [0.2,0.2,0.2,0.8],
        size: [30., 30.0, 1.],
        size_units: "m",
        position: ["0.0*m", "0.0*m", "-4.5*m"],
}
```




## Loading Multiple Tables

If you want to load multiple GEO objects it is as simple as including each of their tables one after another inside your geometry definition.
The "mother" field can also be used to place objects inside other already built geometries, but you **MUST** make sure that the "mother" volume index has already been defined in a table earlier in the file for this to work correctly.


## Cloning Geomtries
We don't yet have features to use the Geant4 replica options, but you can make use of the table cloning feature of the database when making repeated geometries 

For example, the following tables
```
{
name: "GEO",
index: "uranium_rod_0",
mother: "inner",
type: "box",
size: [0.21,0.21,4.0]
size_units: "m",
position: [0.12,0.12,0.0],
position_units: "m",
rotation: [0.0,0.0,0.0],
material: "G4_U",
drawstyle: "solid",
color: [0.2,1.0,0.2,0.3],
}
{ name: "GEO", clone: "uranium_rod_0", index: "uranium_rod_1", position: [ -0.60, 0.12, 0.0 ] }
```

are identical at runtime to
```
{
name: "GEO",
index: "uranium_rod_0",
mother: "inner",
type: "box",
size: [0.21,0.21,4.0]
size_units: "m",
position: [0.12,0.12,0.0],
position_units: "m",
rotation: [0.0,0.0,0.0],
material: "G4_U",
drawstyle: "solid",
color: [0.2,1.0,0.2,0.3],
}
{
name: "GEO",
index: "uranium_rod_1",
mother: "inner",
type: "box",
size: [0.21,0.21,4.0]
size_units: "m",
position: [ -0.60, 0.12, 0.0 ],
position_units: "m",
rotation: [0.0,0.0,0.0],
material: "G4_U",
drawstyle: "solid",
color: [0.2,1.0,0.2,0.3],
}
```

