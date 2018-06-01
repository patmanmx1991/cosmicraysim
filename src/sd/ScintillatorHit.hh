//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id: B5HodoscopeHit.hh 76474 2013-11-11 10:36:34Z gcosmo $
//
/// \file B5ScintillatorHit.hh
/// \brief Definition of the B5ScintillatorHit class

#ifndef ScintillatorHit_h
#define ScintillatorHit_h

#include <math.h>

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4LogicalVolume.hh"
#include "G4Transform3D.hh"
#include "G4RotationMatrix.hh"
#include "G4ParticleDefinition.hh"

class G4AttDef;
class G4AttValue;

// ----------------------------------------------------------------------
/// Hodoscope hit
///
/// It records:
/// - the strip ID
/// - the particle time
/// - the strip logical volume, its position and rotation
class ScintillatorHit : public G4VHit
{
public:

    // Constructors
    ScintillatorHit();
    ScintillatorHit(const ScintillatorHit &right);
    virtual ~ScintillatorHit();

    // Allocators
    const ScintillatorHit& operator=(const ScintillatorHit &right);
    int operator==(const ScintillatorHit &right) const;
    inline void *operator new(size_t);
    inline void operator delete(void*aHit);

    // Draw and print
    void Draw();
    void Print();

    // Set values
    inline void SetType(G4int val) {fType = val; }
    inline void SetTime(G4double val) { fTime = val; }
    inline void SetEdep(G4double val) {  fEdep = val; }
    inline void AddEdep(G4double val) { fEdep += val; }
    inline void SetKinE(G4double val) {  fKinE = val; }
    inline void SetPos(G4ThreeVector xyz) { fPos = xyz; }
    inline void SetVolPos(G4ThreeVector xyz) { fVolPos = xyz; }
    inline void SetRot(G4RotationMatrix rmat) { fRot = rmat; }
    inline void SetParticleType(G4int ptype) {particle_type = ptype;}
    void SetAngles(G4ThreeVector dir);

    // Get all the values
    inline G4int GetID() const { return fId; }
    inline G4int GetType() const { return fType; }
    inline G4double GetTime() const { return fTime; }
    inline G4double GetEdep() const { return fEdep; }
    inline G4double GetKinE() const { return fKinE; }
    inline G4double GetThetaXZ() const { return fThetaXZ; }
    inline G4double GetThetaYZ() const { return fThetaYZ; }
    inline G4ThreeVector GetPos() const { return fPos; }
    inline G4ThreeVector GetVolPos() const { return fVolPos; }
    inline G4RotationMatrix GetRot() const { return fRot; }
    inline G4int GetParticlePDGEncoding() {return particle_type;}

private:
    G4int fId;
    G4int fType;
    G4double fTime;
    G4double fEdep;
    G4double fKinE;

    G4double fThetaXZ;
    G4double fThetaYZ;

    G4int particle_type;

    G4ThreeVector fPos;
    G4ThreeVector fVolPos;
    G4RotationMatrix fRot;
};

// ----------------------------------------------------------------------
// Allocators
typedef G4THitsCollection<ScintillatorHit> ScintillatorHitsCollection;

extern G4ThreadLocal G4Allocator<ScintillatorHit>* ScintillatorHitAllocator;


inline void* ScintillatorHit::operator new(size_t)
{
    if (!ScintillatorHitAllocator)
        ScintillatorHitAllocator = new G4Allocator<ScintillatorHit>;
    return (void*) ScintillatorHitAllocator->MallocSingle();
}

inline void ScintillatorHit::operator delete(void*aHit)
{
    ScintillatorHitAllocator->FreeSingle((ScintillatorHit*) aHit);
}
// ----------------------------------------------------------------------
#endif
