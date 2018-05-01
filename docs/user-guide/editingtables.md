# Editing the DataBase tables

CosmicRaySim uses JSON tables loaded at runtime to define the behaviour of the geometry, flux, and detectors.

Each table is enclosed by a set of parentheses, and must contain the fields "name", and "index", alongside any other relevant information.
The framework is setup so that flux/geometry objects are loaded from a given table name/index combination, but other elements of the code can read any generic information contained in tables by simply requesting their name and index.
```
{
  name: "TABLE_NAME",
  index: "INDEX_NAME",
  info1: "optional_info1"
  info2: "optional_info2"
}
```


## Default Tables

As soon as the framework starts it will first look inside $COSMICRAYSIM/data/ and load all tables that end in ".db",
which are "default database" files defining some standard behaviour.

For example the "data/global.db" JSON table contains the default choice of physics and flux generators.

**data/global.db**
```
{
  name: "GLOBAL",
  index: "config",
  flux: "shukla",
  physics: "default",
  batchcommands: "",
}
```


## Loading Table Files

Any extra JSON tables can be loaded at runtime, either by adding it to the "data" directory if it ends in ".db", or by loading it with the command line flag "-g" in the cosmicraysim application (the recommended way). 

**newtable.geo**
```
{
  table: "GEO",
  index: "newgeom"
  type: "box",
  ...
}
```

Load command
```
cosmicraysim -g newtable.geo -g newtable2.geo ...
```


## Table Type Formats
The JSON reader can load many different data types, and tries to cast between them where possible. Sometimes this doesn't always work, if you see an exception caused when accessing the database but the entry is definitely in the table of interest then you should check the type is correct and the line is well written.


#### Integer
```
{
  ...
  integer_value_1: 1,
  integer_value_2: -1,
  ...
}
```

#### Real
```
{
  ...
  real_value_1:  5.5,
  real_value_2: -1.0,
  real_value_3:  2.,
  ...
}
```

#### String
```
{
  ...
  string_value: "my json table string",
  ...
}
```

#### Vector
```
{
  ...
  vector_int: [1, 2, 3],
  vector_real: [1.6, 2.8, 9.2, 4.5],
  vector_string: ["first_string", "second_string"],
  ...
}
```

## Duplicate Tables
When the run database is loaded at program start, each of the tables found in the JSON files are loaded into a vector. When this happens, for each new table, the vector is checked to see if the name/index combination already exists. If it does, then the existing table is replaced. This means any table given in the default database can simply be overriden by duplicating it in the users geometry file and editing the values in it.

E.g. if you want to use the CRY generator instead of the default Shukla generator, then it can simply be chosen by making a new global config table in the users custom geometry file.
```
{
  name: "GLOBAL",
  index: "config",
  flux: "cry",
  physics: "shielding",
}
```

## DBEvaluator
The JSON database uses a copy of the G4GDMLEvaluator to expand string inputs and convert them to G4Double's. Many entries in the code will require a double with associated units, and its a bit awkward covering all user specified cases of what those units may need to be. If an integer or double value is requested by the code, but it ends up finding a string in the database, then it will automatically evaluate that string to get a double/int.

#### Evaluations
For example, the following tables are equivalent
```
{
  ...
  myvalue: 1.0
  ...
}
{
  ...
  myvalue: "1.0"
  ...
}
{
  ...
  myvalue: "0.5*2.0"
  ...
}
```


#### Entry Units
In addition, standard GEANT4 units can be given inside these strings, allowing them to be used in geometry definitions.
```
{
  ...
  mydistance: "2.0*cm",
  myposition: ["5.0*m","2.0*cm","1.0*mm"],
  ...
}
```

In some cases it is awkward to explicitly write out the units in a vector, so extra uniform units can be given by specifying "value_units".
The following tables are equivalent
```
{
  ...
  mydistance: "2.0*m",
  myposition: ["5.0*m","2.0*m","1.0*m"],
  ...
}
{
  ...
  mydistance: 2.0,
  mydistance_units: "m",
  myposition: [5.0, 2.0, 1.0],
  myposition_units: "m",
  ...
}
{
  ...
  mydistance: 1.0,
  mydistance_units: "2*m",
  myposition: [2.5, 1.0, 0.5],
  myposition_units: "2*m",
  ...
}
```


#### Default Units
If a table contains values of only a single type, you can cheat and set the units for all entries evaluated in that table by adding a field called "units". For example, say I have a table containing 12 positions, but I don't want to have to write out what the units are for each one because I know they should all be in meters.
```
{
  ...
  pos1: [0.0, 0.5, 0.5],
  pos2: [0.0, 0.6, 0.5],
  ...
  pos24: [0.5, 0.2, 0.1],
  units: "m",
  ...
}
```

***CAUTION: If your table contains more than one type of unit then this is risky, as the scaling is applied to all. E.g. If you use this in a TUBS geometry object, then it will apply the units "m" to the length, but also to any angular values that may be read. Only use for tables you definitely know are in common units.***

### Variables
In addition to the standard units, a user can specify any set of custom variables that can be easily changed in the JSON tables. This is done by defining a "VARIABLE" table with all possible constants in it. These variable tables are loaded whenever the code evaluates a string (which is quite slow so shouldn't be done on an event-by-event basis).

The example below defines some variables, and then places GEO objects using the constants so it can be quickly adjusted.
```
{
  name: "VARIABLE",
  index: "myvariables1",
  geoposx1: "2.0*m",
  geoposyz1: "5.0*m",
  geolength1: "0.1*m",
}
{
  name: "VARIABLE",
  index: "myothervariables2",
  geoposx2: "2.0*m",
  geoposy2: "5.0*m",
  geoposz2: "5.0*m",
  geolength2: "0.1*m",
}
{
  name: "GEO",
  index: "box1"
  type: "box",
  material: "G4_Ar",
  mother: "world",
  position: ["geoposx1","geoposyz1","geoposyz1"],
  size: ["geolength1","geolength1","geolength1"],
  ...
}
{
  name: "GEO",
  index: "box2"
  type: "box",
  material: "G4_Ar",
  mother: "world",
  position: ["geoposx2","geoposy2","geoposz2"],
  size: ["geolength2*0.5","geolength2*0.5","geolength2*0.5"],
  ...
}
```


***CAUTION: All VARIABLE tables are loaded at once, regardless of whether they are kept in seperate tables. So bear that in mind when writing out a geometry file.***

## Comments
Comments can be added anywhere in a table by including non-JSON format '//' characters.

```
// Geometry Definition
{

  // This is the JSON TABLE for my DRUM
  name: "GEO",
  index: "bigbox",

  // Make it a simple geo box
  type: "box",

  // Define size as 5m on all edges
  size: [5., 5., 5.],
}
```


## Cloning Tables

To make loading geometries easier it is possible to clone an already existing table and edit some of its properties. If another table you want to clone already exists you can simply make a new table with a "clone" string field pointing to the other table.

You must specify the fields name, clone, and index in the new table, where clone is the index of another table in with the same table name. Note, this only works across tables with the same TABLE_NAME, so you can't copy a "GEO" table to make a "FLUX" table. Then the DB reader will clone the other table, and use any entries to override the originals. 



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

























