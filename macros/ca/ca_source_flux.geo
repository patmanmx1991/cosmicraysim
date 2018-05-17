// ---------------------------------------------------
// Flux Generator Source Location : Default is Shukla
// Shukla default values
{
  name: "SHUKLA",
  index: "config",
  min_energy: 0.1,
  max_energy: 50.0,
  I0: 88.5,
  n: 3.0,
  E0: 4.28,
  epsilon: 854.,
  radius: 6371.,
  distance: 36.61
}

{
  name: "FLUX",
  index: "source_box",
  size: ["world_box_length", "world_box_width", "1.*mm"],
  position: ["0.0","0.0", "det_tunnel_depth - 2*mm"]
}

// Target for the main detector stack
{
  name: "FLUX",
  index: "target_box_0",
  size: ["sampling_target_box_size","sampling_target_box_size","sampling_target_box_size"],
  position: ["0.0","0.0","0.5*sampling_target_box_size"],
}

// Target 2
{
  name: "FLUX",
  index: "target_box_1",
  size: ["det_sx","det_sy","det_sz"],
  position: ["-1.0*det_system_xsep","0.0","det_system_offset+det_system_lowbar-0.5*sampling_target_box_size"]

}

// Target 3
{
  name: "FLUX",
  index: "target_box_2",
  size: ["det_sx","det_sy","det_sz"],
  position: ["det_system_xsep","0.0","det_system_offset+det_system_lowbar-0.5*sampling_target_box_size"]

}
