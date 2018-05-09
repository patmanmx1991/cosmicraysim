# Adding a trigger

By default, the analysis manager will save every event generated regardless of whether the detector was hit. To insert a trigger make a TRIGGER table and choose a type. The other options in the table will depend on the trigger type.

The AnalysisManager is setup so that all triggers will be processed, and if any are true, then the event will be saved.


#### Simple Trigger : "simple"

The simple trigger is just used to check at least one or many detectors has some information to be saved.

If we assume we have three detectors "box0_sc", "box1_sc", "box2_sc", then the following example will save events only if all 3 have information.
```
{
  name: "TRIGGER"
  index: "mytrigger",
  type: "simple",
  processors: ["box0_sc","box1_sc","box2_sc"]
}
``` 


Since only one actual TRIGGER object needs to be true for the event to saved, we can also save events provided at least one object has information
```
{
  name: "TRIGGER",
  index: "mytrigger0",
  type: "simple",
  processors: ["box0_sc"]
}
{
  name: "TRIGGER",
  index: "mytrigger1",
  type: "simple",
  processors: ["box1_sc"]
}
{
  name: "TRIGGER",
  index: "mytrigger2",
  type: "simple",
  processors: ["box2_sc"]
}
```