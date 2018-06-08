#include "ISOSpacingTest.hh"

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

#include "geo/GeoUtils.hh"
#include "geo/GeoObject.hh"
#include "sd/DetectorManager.hh"
#include "geo/simple/GeoBox.hh"
#include "geo/simple/GeoTubs.hh"

namespace COSMIC {

ISOSpacingTest::ISOSpacingTest(DBTable table) {
  Construct(table);
}

void ISOSpacingTest::Construct(DBTable table)
{
    SetID(table.GetIndexName());
	SetType("isospacingtest");
	std::string fName = table.GetIndexName();
    std::string fMotherName = table.GetS("mother");
	fMotherLogical = GEO::GetMotherLogicalFromStore(table);

	// ----------------------------------------------------------
	// Get the information from the user
	int number_objects = table.GetI("number_objects");

	std::vector<G4double> pos  = table.GetVecG4D("position");

	std::vector<G4double> size_in  = table.GetVecG4D("size");
	G4ThreeVector box_size = G4ThreeVector(size_in[0], size_in[1], size_in[2]);

	std::vector<G4double> spacingin = table.GetVecG4D("offset");
	G4ThreeVector spacing = G4ThreeVector(spacingin[0], spacingin[1], spacingin[2]);

  	G4Material* single_mat = MaterialFactory::GetMaterial(table.GetS("material"));

	double scaling = 1.0;
	if (table.Has("scaling")) scaling = table.GetD("scaling");

	std::string scaletype = "xyz";
	if (table.Has("scaletype")) scaletype = table.GetS("scaletype");

	// ----------------------------------------------------------
	// Loop over number of boxes and create
	std::vector<G4LogicalVolume*> boxes;
	for (int i = 0; i < number_objects; i++){

		// Create new box
		G4Box* single_sol = new G4Box( "single_sol", 0.5 * box_size[0], 0.5 * box_size[1], 0.5 * box_size[2]);
    	G4LogicalVolume* single_log = new G4LogicalVolume(single_sol, single_mat, "single_log");

    	// Create the visualisation
    	single_log->SetVisAttributes(MaterialFactory::GetVisForMaterial(table));

    	// Add to list
    	boxes.push_back(single_log);

    	// Log ISO details for user
		std::cout << " --> " << i+1 << " Width X:Y:Z = " << box_size[0]/mm << " : " << box_size[1]/mm << " : " << box_size[2]/mm << std::endl;

		// Update feature size depending on type
		if (scaletype.find("x") != std::string::npos) box_size[0] = box_size[0]*scaling;
		if (scaletype.find("y") != std::string::npos) box_size[1] = box_size[1]*scaling;
		if (scaletype.find("z") != std::string::npos) box_size[2] = box_size[2]*scaling;

	}

	// ----------------------------------------------------------
	// Loop over boxes and place
	double curscale = 1.0;
	G4ThreeVector curspacing = G4ThreeVector(pos[0],pos[1],pos[2]);
	for (int i = 0; i < number_objects; i++){

		// Get Logical
		G4LogicalVolume* log = boxes[i];

		// Place in mother
		new G4PVPlacement( new G4RotationMatrix(), curspacing, log, "isotest", fMotherLogical, true, 0);

		// Update spacing according to step and scale
		curspacing = curspacing + 1.5*spacing;
		if (scaletype.find("s") != std::string::npos) spacing = spacing * scaling;
		curspacing = curspacing + 0.5*spacing;
	}
}

} // namespace COSMIC





