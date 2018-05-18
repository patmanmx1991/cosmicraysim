#ifndef __POCA_UTILS_HH__
#define __POCA_UTILS_HH__

#include "G4ThreeVector.hh"

namespace COSMIC{
namespace PoCAUtils {

	// Given two muon tracks and positions get the poca. 
	void GetPoCAFromTracks(G4ThreeVector& poca, G4double& angle, G4double& distance,
                           G4ThreeVector posA, G4ThreeVector momA,
                           G4ThreeVector posB, G4ThreeVector momB) ;


}
} // - namespace COSMIC
#endif
