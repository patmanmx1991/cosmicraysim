# Editing the DataBase tables
=====

CosmicRaySim uses JSON tables loaded at runtime to define the behaviour of the geometry, flux, and detectors.

As soon as the framework starts it will first look inside $COSMICRAYSIM/data/ and load all tables that end in ".db",
which are "default database" files defining some standard behaviour.

For example the "data/global.db" JSON table contains the default choice of physics and flux generators.




Any JSON table 



When using the framework you will have to modify a 