# Editing the DataBase tables
=====

CosmicRaySim uses JSON tables loaded at runtime to define the behaviour of the geometry, flux, and detectors.

Each table is enclosed by a set of parentheses, and must contain the fields "name", and "index", alongside any other relevant information.
The framework is setup so that flux/geometry objects are loaded from a given table name/index combination, but can read any generic information contained in that table.
```
  name: "TABLE_NAME",
  index: "INDEX_NAME",
  info1: "optional_info1"
  info2: "optional_info2"
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

Run command
```
cosmicraysim -g newtable.geo -g newtable2.geo ...
```


## Table Type Formats
The JSON reader can load many different data types, but gets a bit upset when the wrong datatype is loaded. If you see an exception caused when accessing the database but the entry is definitely in the table of interest then you should check the type is correct.


#### Integer
```
  integer_value_1: 1,
  integer_value_2: -1,
```
- Make sure it has no trailing decimel.

#### Real
```
  real_value_1:  5.5,
  real_value_2: -1.0,
  real_value_3:  2.,
```
- Make sure it has a trailing decimal

#### String
```
  string_value: "my json table string",
```

#### Vector
```
  vector_int: [1, 2, 3],
  vector_real: [1.6, 2.8, 9.2, 4.5],
  vector_string: ["first_string", "second_string"],
```


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

























