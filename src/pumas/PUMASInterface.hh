#ifndef __PUMAS_INTERFACE_HH__
#define __PUMAS_INTERFACE_HH__

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>

#include "G4Navigator.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4Material.hh"
#include "G4ThreeVector.hh"
#include "G4TransportationManager.hh"
#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "globals.hh"
#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4Box.hh"
#include "G4SystemOfUnits.hh"
#include "G4RunManager.hh"
#include "G4ThreeVector.hh"
#include "G4DataVector.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleGun.hh"
#include "Randomize.hh"
#include "globals.hh"
#include "G4Types.hh"

#include "pumas/pumas.h"

namespace PUMASInterface {

bool debugpumas = true;
std::vector<std::vector<std::vector<int> > > material_grid;

double xstep = 0.5;
double xmin = -30.0;
double xmax = 30.0;
double ystep = 0.5;
double ymin = -30.0;
double ymax = 30.0;
double zstep = 0.5;
double zmin = -30.0;
double zmax = 30.0;

static struct pumas_context * context = NULL;


static double gGlobalDensity = 2.65E+03;
// Default local for no variation in density. In future may have to add user
// defined locals?
static double locals_uniform(struct pumas_medium * medium,
    struct pumas_state * state, struct pumas_locals * locals){

	// Set the density treating as uniform rock
	locals->density = gGlobalDensity;
}

static struct pumas_medium media[1] = { { 0, &locals_uniform } };



static double uniform01(struct pumas_context * context)
{
        return rand() / (double)RAND_MAX;
}

double spectrum_gaisser(double cos_theta, double kinetic)
{
        const double r_pi = 1.;
        const double E_pi = 115.;
        const double r_K = 0.054;
        const double E_K = 850.;
        const double E = kinetic + 0.10566;
        const double E_star = E * cos_theta;
        return 1.4 * pow(E, -2.7) * (r_pi / (1. + 1.1 * E_star / E_pi) +
                                        r_K / (1. + 1.1 * E_star / E_K));
}


static double medium2(struct pumas_context * context,
    struct pumas_state * state, struct pumas_medium ** medium_ptr)
{

	if (debugpumas) std::cout << "Calling Medium" << std::endl;
	// Get position of the state
	double x = state->position[0];
	double y = state->position[1];
	double z = state->position[2];

	// Check range
	if (debugpumas) std::cout << "Checking Medium " << x << " " << y << " " << z << std::endl;
	if (debugpumas) std::cout << "Checking Medium Min " << xmin << " " << ymin << " " << zmin << std::endl;
	if (debugpumas) std::cout << "Checking Medium Max " << xmax << " " << ymax << " " << zmax << std::endl;
	if (debugpumas) std::cout << "Checking Bool Max " << (x > xmax) << " " << (y > ymax) << " " << (z > zmax) << std::endl;
	if (debugpumas) std::cout << "Checking Bool Min " << (x < xmin) << " " << (y < ymin) << " " << (z < zmin) << std::endl;

	if (x < xmin || x > xmax) return -1;
	if (y < ymin || y > ymax) return -1;
	if (z < zmin || z > zmax) return -1;


	// Get the corresponding indices in our grid for XYZ
	uint xbin = (x - xmin)/xstep;
	uint ybin = (y - ymin)/ystep;
	uint zbin = (z - zmin)/zstep;
	if (debugpumas) std::cout << "Getting material index " << xbin << " " << ybin << " " << zbin << std::endl;
	int material_index = material_grid[xbin][ybin][zbin]-1;
	if (debugpumas) std::cout << "Index : " << material_index << std::endl;

	if (material_index < 0) return -1;

	if (debugpumas) std::cout << "Setting medium ptr : " << medium_ptr << std::endl;
	// Read from Geant4
	if (medium_ptr != NULL) *medium_ptr = *medium_ptr = media + material_index;

	// Check inside the world volume
	if (debugpumas) std::cout << "Checking Step" << std::endl;

	// return step size depending on distance to next boundary...
	double step = 1;
	if (debugpumas) std::cout << "Returning " << step << std::endl;
	return step;
}

pumas_context* BuildPUMAS(){

	
	std::cout << "Building context" << std::endl;

	pumas_initialise(PUMAS_PARTICLE_MUON, "materials/mdf/standard.xml",
	                 "../dedx/muon", NULL);

	std::cout << "Mapping materials" << std::endl;

	/* Map the PUMAS material indices */
	pumas_material_index("StandardRock", &media[0].material);
	G4Navigator* nav =  G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();

	// Create our grid
	int nbinsx = (xmax - xmin)/xstep;
	int nbinsy = (ymax - ymin)/ystep;
	int nbinsz = (zmax - zmin)/zstep;
	for (int i = 0; i < nbinsx; i++){
		material_grid.push_back(std::vector<std::vector<int> >(0));
		for (int j = 0; j < nbinsy; j++){
			material_grid[i].push_back(std::vector<int>(0));
			for (int k = 0; k < nbinsz; k++){

				double x = xmin + (i+0.5)*xstep;
				double y = ymin + (j+0.5)*ystep;
				double z = zmin + (k+0.5)*zstep;

				G4VPhysicalVolume* phys = nav->LocateGlobalPointAndSetup(G4ThreeVector(x*m,y*m,z*m));
				if (!phys) {
					material_grid[i][j].push_back(0);
					continue;
				}
				G4LogicalVolume* log = phys->GetLogicalVolume();
				G4Material* mat = log->GetMaterial();
				std::cout << "Setting material to : " << mat->GetDensity() / (g/cm3) << std::endl;

				material_grid[i][j].push_back(1);
			}
		}
	}


	std::cout << "Creating context" << std::endl;

	/* Create a new PUMAS simulation context */
	pumas_context_create(0, &context);

	std::cout <<"Adding settings" << std::endl;
	/* Configure the context for a backward transport */
	context->forward = 0;

	std::cout << "Setting Medium" << std::endl;
	/* Set the medium callback */
	context->medium = &medium2;

	std::cout << " Setting random" << std::endl;
	/* Provide a PRNG for the Monte-Carlo simulation */
	context->random = &uniform01;

	std::cout << "Retrun" << std::endl;
	return context;
}





}
#endif
