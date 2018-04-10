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

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4LogicalVolume.hh"
#include "G4Transform3D.hh"
#include "G4RotationMatrix.hh"
#include "G4ParticleDefinition.hh"

#include <math.h>

class G4AttDef;
class G4AttValue;

/// Hodoscope hit
///
/// It records:
/// - the strip ID
/// - the particle time
/// - the strip logical volume, its position and rotation

class ScintillatorHit : public G4VHit
{
public:

    ScintillatorHit();
    ScintillatorHit(const ScintillatorHit &right);
    virtual ~ScintillatorHit();

    const ScintillatorHit& operator=(const ScintillatorHit &right);
    int operator==(const ScintillatorHit &right) const;

    inline void *operator new(size_t);
    inline void operator delete(void*aHit);

   // void Draw();
   // virtual const std::map<G4String,G4AttDef>* GetAttDefs() const;
   // virtual std::vector<G4AttValue>* CreateAttValues() const;
    void Print();



    G4int GetID() const { return fId; }
    G4int GetType() const { return fType; }// type=0, muon ; type=1 neutron; type=2 gamma
    void SetType(G4int val){fType=val; }// type=0, muon ; type=1 neutron; type=2 gamma

    void SetTime(G4double val) { fTime = val; }
    G4double GetTime() const { return fTime; }

    void SetEdep(G4double val) {  fEdep=val; }
    void AddEdep(G4double val) { fEdep += val; }
    G4double GetEdep() const { return fEdep; }

    void SetKinE(G4double val) {  fKinE=val; }
    G4double GetKinE() const { return fKinE; }

    void SetAngles(G4ThreeVector dir) {
        fThetaXZ = std::atan2(dir.getZ(),dir.getX());
        fThetaYZ = std::atan2(dir.getZ(),dir.getY());
    }

    G4double GetThetaXZ() const { return fThetaXZ; }
    G4double GetThetaYZ() const { return fThetaYZ; }

    void SetPos(G4ThreeVector xyz) { fPos = xyz; }
    G4ThreeVector GetPos() const { return fPos; }

    void SetRot(G4RotationMatrix rmat) { fRot = rmat; }
    G4RotationMatrix GetRot() const { return fRot; }

    //void SetLogV(G4LogicalVolume* val) { fPLogV = val; }
    //const G4LogicalVolume* GetLogV() const { return fPLogV; }

    void SetParticleType(G4int ptype){particle_type = ptype;}
    G4int GetParticlePDGEncoding(){return particle_type;}

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
    G4RotationMatrix fRot;
    //const G4LogicalVolume* fPLogV;
};

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

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
