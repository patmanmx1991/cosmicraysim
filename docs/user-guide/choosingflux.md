# Choosing a Cosmic Flux Generator

Cosmic Ray Sim is setup to easily include multiple flux generators. The type of generator is loaded at runtime by specifying its name in the "data/global.db" configuration table.
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

### Shukla Generator : "shukla"
The Shukla generator can be loaded by specifying "shukla" in the global configuration file, and is based on the Shukla cosmic ray predictions tuning paper.
It only generators positive and negative muons.

If loaded the generator itself will look for some further tables in the DB. These being the "SHUKLA/config", and "FLUX/source_box" tables.

#### SHUKLA : config (required)
```
// Shukla default values
{
  name: "SHUKLA",
  index: "config",
  // parameters: "nottingham",
  // min_energy: 0.1,
  // max_energy: 5000.,
  // I0: 88.5,
  // n: 3.0,
  // E0: 4.28,
  // epsilon: 854.,
  // radius: 6371.,
  // distance: 36.61,
}
```

The commented out fields show the defaults which are loaded inside the generator. This table is usually kept in the database itself so the user doesn't have to provide it. If you want to edit any of these uncomment the line and change the value. Their descriptions are below.

- **parameters**: Use a hard coded parameter set, each of the ones shown in the energy fit table in the Shukla paper are shown. Options are : "nottingham", "princealbert", "hamburg", "proton", "helium". It is also possible to load the parameter set, then change one parameter by including one of the other fields below. The default is the "nottingham" parameter set.
- **min_energy**: Minimum energy to generate muons at in GeV. Default is 5000. GeV. 
- **max_energy**: Maximum energy to generate muons at in GeV. Default is 0.1 GeV. 
- **I0**: Integrated Vertical Flux. Default is 88.5.
- **n**: Energy spectrum power. Default is 3.0.
- **E0**: Energy loss parameter in GeV. Default is 4.28 GeV.
- **epsilon**: High energy power modification in GeV. Default is 854.0 GeV.
- **radius**: Radius of the earth + altitude of source box in km. Default is 6371.0 km.
- **distance**: Vertical distance of air showers in km. Default is 36.61 km (to give R/d = 174 used in paper).

### CRY Generator : "cry"







