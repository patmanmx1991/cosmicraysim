# Choosing a Physics List

Cosmic Ray Sim is setup to easily swap the physics list. The type of generator is loaded at runtime by specifying its name in the "data/global.db" configuration table.
```
{
  name: "GLOBAL",
  index: "config",
  flux: "shukla",
  physics: "default",
  batchcommands: "",
}
```


The flux string in this table is read in the ```PhysicsFactory::LoadPhysicsList()``` function, and a generator is returned. 

In the future the plan is to have multiple physics lists accessible as strings, and give geometries the option to choose whether they want different physics to be applied inside them.


## Available Physics Lists

Available options for the global physics list at this time are

- **default** : A simple list from the Geant4 examples. Only really has EM physics.
- **shielding** : Shielding physics list contains many more processors than the default. Recommended one to use for full runs, but is slower to use during debugging, etc, so is not yet the default.
