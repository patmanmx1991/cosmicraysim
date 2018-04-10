# Making A Geometry Table

The geometry factory is setup to load in all of the different JSON "GEO" tables, and create the world as the program starts.
This way simple geometries can be created without having to recompile each time, but there is still freedom to have complex geometry definitions be hardcoded to be generated from a JSON table input format and placed anywhere within the world.

It is recommended that all tables defining the geometry you care about are put into some ".geo" files inside your working directory, so they can be loaded at runtime using the command ```cosmicraysim -g mytable.geo```.


## Geometry Tables
At the bare minimum the geometry tables require the "name", "index", and "type" fields.

```
{
  name: "GEO",
  index: "world",
  type: "box",
  ...
}
```

- **name** : Tells the framework its a GEO table.
- **index** : Index of this table. The index is ALSO used to define the name of the volume being created.
- **type** : Tells the framework what actual object to create. This is passed to the GeoObjectFactory::Construct() function, which uses it to initialise objects.

## Geomtry Types

There is limited support for some simple geometry objects to be loaded at runtime, alongside more complex objects to be placed inside a chosen mother volume. These objects all inherit from the GeoObject class.




### Solids

The GeoSolids class is used for making simple solids and setting their drawoptions and positions correctly. Each geosolid contains all of its solid, logical, and physical volumes in one class. The following types all inherit from the GeoSolids object

- box
- tubs


Each of these objects therefore have the following fields that can be changed in the JSON table.

- **material (required)** : Sets the material of the logical volume. Can use any NIST manager string, e.g. "G4_Pb" returns lead.
- **mother** : Set the index of the table defining the mother volume. This is required for all objects, except the world volume.
- **position** : Position of the center of the solid, given in terms of its mother co-ordinate system. Default is [0.,0.,0.].
- **rotation** : Rotation of the center of the solid about each axis in degrees, given in terms of its mother co-ordinate system. Default is [0.,0.,0.].
- **drawstyle** : Set the visulation style. Options are "wireframe" or "solid". Default is "wireframe".
- **color** : Provide a real RGB vector for setting the draw color. Can be either [R,G,B], or [R,G,B,A].



#### Solid : "box"
A simple G4Box can be created by specifying its size and position inside the world. It has the following options.

- **size (required)** : Length in x, y, and z, in meteres.

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
        position: [0.0, 0.0, -4.5],
}
```


#### Solid : "tubs"
A simple G4Tubs object can be created by giving its size in the z-axis and its radius. it has the following options

- **r_max (required)**: Max radius in meters.
- **size_z (required)**: Length of the tube.
- **r_min** : Min radius in meters, for making hollow tubes.
- **phi_start** : Minimum Phi Angle, for making semi cylinders.
- **phi_max** : Maximum Phi Angle, for making semi cylinders.

** Example : **
```
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
position: [0.12,0.12,0.0],
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
position: [0.12,0.12,0.0],
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
position: [ -0.60, 0.12, 0.0 ],
rotation: [0.0,0.0,0.0],
material: "G4_U",
drawstyle: "solid",
color: [0.2,1.0,0.2,0.3],
}
```

