#include "LargeSteelDrum.hh"

#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4Transform3D.hh"
#include "G4SDManager.hh"
#include "G4MultiFunctionalDetector.hh"
#include "G4VPrimitiveScorer.hh"
#include "G4PSEnergyDeposit.hh"
#include "G4PSDoseDeposit.hh"
#include "G4VisAttributes.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnionSolid.hh"
#include "G4Torus.hh"


#include "geo/GeoUtils.hh"
#include "geo/GeoObject.hh"
#include "sd/DetectorManager.hh"
#include "geo/simple/GeoBox.hh"
#include "geo/simple/GeoTubs.hh"

namespace COSMIC {

LargeSteelDrum::LargeSteelDrum(DBTable table) {
  Construct(table);
}

void LargeSteelDrum::Construct(DBTable table)
{
    SetID(table.GetIndexName());
	SetType("largesteeldrum");
	std::string fName = table.GetIndexName();
    std::string fMotherName = table.GetS("mother");
	fMotherLogical = GEO::GetMotherLogicalFromStore(table);

    // --------------------------------------------------------------------------
	// Get Parameters
	double shell_r_max = 0.5*57.4*cm;
	double shell_size_z = 88*cm;
	double shell_thickness = 2.5*cm;
	double cap_r_max   = 0.5*61.5*cm;
	double cap_size_z  = 4.5*cm;
	double base_r_max  = 0.5*58.8*cm;
	double base_size_z = 3.5*cm;
    double ring_radius = 1.5*cm;

    // --------------------------------------------------------------------------
	// Get Materials
  	G4Material* shell_mat = MaterialFactory::GetMaterial("COSMIC_STEEL");
  	G4Material* fill_mat  = MaterialFactory::GetMaterial("G4_CONCRETE");

    // --------------------------------------------------------------------------
	// Create a steel drum 
    
  	// - Uniform Shell
  	std::string shell_name = "unishell";
    G4Tubs* shell_sol = new G4Tubs( shell_name, 0.0, shell_r_max, shell_size_z/2.0, 0.0, CLHEP::twopi);

    // - End Cap
    std::string cap_name = "cap";
    G4Tubs* cap_sol = new G4Tubs( cap_name, 0.0, cap_r_max, cap_size_z/2.0, 0.0, CLHEP::twopi);
    G4ThreeVector cap_pos = G4ThreeVector(0.0, 0.0, 0.5*(shell_size_z+cap_size_z));

    // - Base Cap
    std::string base_name = "base";
    G4Tubs* base_sol = new G4Tubs( base_name, 0.0, base_r_max, base_size_z/2.0, 0.0, CLHEP::twopi);
    G4ThreeVector base_pos = G4ThreeVector(0.0, 0.0, -0.5*(shell_size_z+base_size_z));

    // Torus Ring
    std::string tors_name = "torus";
    G4Torus* tors_sol = new G4Torus( tors_name, 0.0, ring_radius, shell_r_max, 0.0, CLHEP::twopi);
    G4ThreeVector tors_pos = G4ThreeVector(0.0, 0.0, 0.0);
    G4ThreeVector tors_off = G4ThreeVector(0.0, 0.0, 1.0*m);


    // Combined Drum Volume
	G4VSolid* steel_sol        = new G4UnionSolid("shell", shell_sol, cap_sol,  0, cap_pos);
	steel_sol                  = new G4UnionSolid("shell", steel_sol,  base_sol, 0, base_pos);

    G4VSolid* rings_sol = new G4UnionSolid("rings", tors_sol, tors_sol, 0, -0.20*tors_off);
    rings_sol = new G4UnionSolid("rings", rings_sol, tors_sol, 0, -0.50*tors_off);
    rings_sol = new G4UnionSolid("rings", rings_sol, tors_sol, 0, -0.70*tors_off);

    G4VSolid* drum_sol                  = new G4UnionSolid("drum", steel_sol, rings_sol, 0, 0.35*tors_off);
    G4LogicalVolume* drum_log = new G4LogicalVolume(drum_sol, shell_mat, "drum");

    // --------------------------------------------------------------------------
    // Set Vis of the main drum
    G4VisAttributes *drum_vis = new G4VisAttributes();
    drum_vis->SetColour(G4Colour(1.0,1.0,1.0,0.3));
    drum_vis->SetForceSolid(true);
    drum_log->SetVisAttributes(drum_vis);

    // --------------------------------------------------------------------------
    // Assign the main volume into mother
    table.SetIndexName(fName + "_outer");
    fLogical  = drum_log;
    fPhysical = GeoSolid::ConstructPhysicalPlacement(table, fMotherLogical, fLogical);

    // --------------------------------------------------------------------------
    // Create the sub space inside the drum and place inside main logical
    std::string fill_name = fName;
    G4Tubs* fill_sol = new G4Tubs( fill_name, 0.0, (shell_r_max - shell_thickness), shell_size_z/2.0, 0.0, CLHEP::twopi);
    G4LogicalVolume* fill_log = new G4LogicalVolume(fill_sol, fill_mat, fill_name);
    new G4PVPlacement( new G4RotationMatrix(), G4ThreeVector(), fill_log, fill_name, drum_log, true, 0);

    // --------------------------------------------------------------------------
    // Set Vis of the filler drum
    G4VisAttributes *fill_vis = new G4VisAttributes();
    fill_vis->SetColour(G4Colour(1.0,0.7,0.7,0.2));
    fill_vis->SetForceSolid(true);
    fill_log->SetVisAttributes(fill_vis);


}

} // namespace COSMIC





