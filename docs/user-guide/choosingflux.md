# Choosing a Cosmic Flux Generator

Cosmic Ray Sim is setup to easily include multiple flux generators. The type of generator is loaded at runtime by specifying its name in the "GLOBAL"  configuration table.
```
{
  name: "GLOBAL",
  index: "config",
  flux: "shukla",
  physics: "default",
  batchcommands: "",
}
```


The flux string in this table is read in the ```PrimaryGeneratorFactory::LoadGenerator()``` function, and a generator is returned. 

## Shukla Generator : "shukla"
The Shukla generator can be loaded by specifying "shukla" in the global configuration file, and is based on the Shukla cosmic ray predictions tuning paper.
It only generators positive and negative muons.

If loaded the generator itself will look for some further tables in the DB. These being the "SHUKLA/config", and "FLUX/source_box" tables.

### SHUKLA : config (required)
```
// Shukla default values
{
  name: "SHUKLA",
  index: "config",
  parameters: "nottingham",
  min_energy: 0.1,
  max_energy: 5000.,
  I0: 88.5,
  n: 3.0,
  E0: 4.28,
  epsilon: 854.,
  radius: 6371.,
  distance: 36.61,
}
```

The commented out fields show the defaults which are loaded inside the generator. This table is usually kept in the database itself so the user doesn't have to provide it. If you want to edit any of these uncomment the line and change the value. Their descriptions are below. Each field assumes a unit to match the shukla paper, so its best not to use your own units in the entries.

- **parameters**: Use a hard coded parameter set, each of the ones shown in the energy fit table in the Shukla paper are shown. 
Options are : "nottingham", "princealbert", "hamburg", "proton", "helium". 
It is also possible to load the parameter set, then change one parameter by including one of the other fields below. The default is the "nottingham" parameter set.
- **min_energy**: Minimum energy to generate muons at in GeV. Default is 5000. GeV. 
- **max_energy**: Maximum energy to generate muons at in GeV. Default is 0.1 GeV. 
- **I0**: Integrated Vertical Flux. Default is 88.5.
- **n**: Energy spectrum power. Default is 3.0.
- **E0**: Energy loss parameter in GeV. Default is 4.28 GeV.
- **epsilon**: High energy power modification in GeV. Default is 854.0 GeV.
- **radius**: Radius of the earth + altitude of source box in km. Default is 6371.0 km.
- **distance**: Vertical distance of air showers in km. Default is 36.61 km (to give R/d = 174 used in paper).


### FLUX : source_box (required)

So that CosmicRaySim knows where to generate the particles in your custom geometry the user needs to specify a source box. Particles will then be generated on by randomly throwing in X and Y in this box. An example box and the options required are below

```
{
  name: "FLUX",
  index: "source_box",
  size: [30.0,30.0, 0.5],
  size_units: "m",
  position: [0.0,0.0, 4.75],
  position_units: "m",
}
```

This table will be searched for at runtime by the Shukla generator and used to define where particles are produced. Its position and size are defined by a Geant4 box placed in the mother volume given the users specifications.

- **size**: 3D size in meters of the source box. User should give x, y, and z lengths.
- **position**: Position of the center of the source box in the WORLD volume co-ordinate system.


### FLUX : target_box (optional)

The Shukla generator also lets you skip over trajectories that are pointing away from some defined set of targets to save CPU time. This assumes the generated trajectory is a good approximation for where it will end up, which is only really true for propogation through low density geometries such as air. When a "target_box" table is loaded this feature will automatically be turned on. When using it make sure to use conservative large boxes that fully surround your geometry area of interest.

```
{
  name: "FLUX",
  index: "target_box_0",
  position: [-2.0, 0.0, -1.8]
  size: [0.78, 4.2, 4.2]
  units: "m",
}
```

Again the user just has to specify the target box position and size

- **size**: 3D size in meters of the source box. User should give x, y, and z lengths.
- **position**: Position of the center of the source box in the WORLD volume co-ordinate system.


The generator looks for any FLUX tables which have "target_box_" in them. So multiple target boxes can also be specified like in the example below, and only tracks which pass through either one (it doesn't care about both), will be accepted.


```
{
  name: "FLUX",
  index: "target_box_0",
  position: [-2.0, 0.0, -1.8]
  size: [0.78, 4.2, 4.2]
  units: "m",
}
{
  name: "FLUX",
  index: "target_box_1",
  position: [2.0, 0.0, -1.8]
  size: [0.78, 4.2, 4.2]
  units: "m",
}
```



## CRY Generator : "cry"

The CRY generator uses the CRY LLNL CRY libraries to produce cosmic rays within a user specified source box (actually a thin plane), and can be selected at runtime using the string "cry" in the global config table. 


When the CRYGenerator loads up, it will look for the "CRY/config" and "FLUX/source_box" DB tables. The inputs to each of these tables are described below.

### CRY : config (required)

The CRY config table just provides the standard settings that the CRY libraries support, but is included in the standard JSON format. This table is included in the data folder so the user doesn't need to worry about it if they just want a standard cosmic ray flux. To change any of the default settings uncomment the fields and edit as required.

```
// CRY default values
{
  name: "CRY",
  index: "config",
  min_particles: -1,
  max_particles: -1,
  gen_neutrons: 1,
  gen_protons: 1,
  gen_gammas: 1,
  gen_electrons: 1,
  gen_muons: 1,
  gen_pions: 1,
  latitude: 0.0,
  altitude: 0.0,
  date: "1-1-2007",
}
```

- **min_particles** : Skip cosmic ray events that contain below a min. threshold of particles. Default is -1, which means no threshold.
- **max_particles** : Skip cosmic ray events that contain above a max. threshold of particles. Default is -1, which means no threshold.
- **gen_neutrons** : Allow neutrons to be generated as primaries. Default is 1 (True).
- **gen_protons** : Allow protons to be generated as primaries. Default is 1 (True).
- **gen_gammas** : Allow gammas to be generated as primaries. Default is 1 (True).
- **gen_electrons** : Allow neutrons to be generated as primaries. Default is 1 (True).
- **gen_muons** : Allow muons to be generated as primaries. Default is 1 (True).
- **latitude** : Latitude of source box. Angle between +-90 degrees. Default is 0 (Equator).
- **altitude** : Altitude of source box in meters. CRY onyl supports 3 options: 0 m, 2100 m, 11300 m. Default is 0 m.
- **date** : Specify date in string format to account for solar minima/maxima. Given as "m-d-yyyy" format. Default is "1-1-2007".



### FLUX : source_box (required)

So that CosmicRaySim knows where to generate the particles in your custom geometry the user needs to specify a source box. Particles will then be generated on the surface of this box. An example box and the options required are below

```
{
  name: "FLUX",
  index: "source_box",
  size: [30.0,30.0, 0.5],
  position: [0.0,0.0, 4.75],
  units: "m",
}
```

This table will be searched for at runtime by the Shukla generator and used to define where particles are produced. Its position and size are defined by a Geant4 box placed in the mother volume given the users specifications.

- **size**: 3D size in meters of the source box. User should give x, y, and z lengths.
- **position**: Position of the center of the source box in the WORLD volume co-ordinate system.


### FLUX : target_box (optional)

The CRY generator also lets you skip over trajectories that are pointing away from some defined set of targets to save CPU time. This assumes the generated trajectory is a good approximation for where it will end up, which is only really true for propogation through low density geometries such as air. When a "target_box" table is loaded this feature will automatically be turned on. When using it make sure to use conservative large boxes that fully surround your geometry area of interest.

Since the CRY generator can produce multiple primaries (unlike the Shukla generator), if at least one primary has the correct trajectory that lines up with a target box, it will select the entire event.


```
{
  name: "FLUX",
  index: "target_box_0",
  position: [-2.0, 0.0, -1.8]
  size: [0.78, 4.2, 4.2]
  units: "m",
}
```

Again the user just has to specify the target box position and size

- **size**: 3D size in meters of the source box. User should give x, y, and z lengths.
- **position**: Position of the center of the source box in the WORLD volume co-ordinate system.


The generator looks for any FLUX tables which have "target_box_" in them. So multiple target boxes can also be specified like in the example below, and only tracks which pass through either one (it doesn't care about both), will be accepted.


```
{
  name: "FLUX",
  index: "target_box_0",
  position: [-2.0, 0.0, -1.8]
  size: [0.78, 4.2, 4.2]
  units: "m",
}
{
  name: "FLUX",
  index: "target_box_1",
  position: [2.0, 0.0, -1.8]
  size: [0.78, 4.2, 4.2]
  units: "m",
}
```
