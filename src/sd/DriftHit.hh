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
// $Id: DriftChamberHit.hh 76474 2013-11-11 10:36:34Z gcosmo $
//
/// \file DriftChamberHit.hh
/// \brief Definition of the DriftChamberHit class

#ifndef DriftChamberHit_h
#define DriftChamberHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4LogicalVolume.hh"
#include "G4Transform3D.hh"
#include "G4RotationMatrix.hh"

class G4AttDef;
class G4AttValue;

// ----------------------------------------------------------------------
// Drift Hit Object
class DriftChamberHit : public G4VHit
{
public:

    // COnstructors
    DriftChamberHit();
    DriftChamberHit(G4int z);
    DriftChamberHit(const DriftChamberHit &right);
    virtual ~DriftChamberHit();

    // Allocators
    const DriftChamberHit& operator=(const DriftChamberHit &right);
    int operator==(const DriftChamberHit &right) const;
    inline void *operator new(size_t);
    inline void operator delete(void *aHit);

    // Attributes
    virtual const std::map<G4String, G4AttDef>* GetAttDefs() const;
    virtual std::vector<G4AttValue>* CreateAttValues() const;

    // Vis Options
    void Draw();

    // Set functions
    void SetGhost(bool ghost) { fGhost = ghost; };
    void SetDrawOption(int i) { fDrawOption = i; };
    void SetLayerID(G4int z) { fLayerID = z; }
    void SetTime(G4double t) { fTime = t; }
    void SetLocalPos(G4ThreeVector xyz) { fLocalPos = xyz; }
    void SetLocalPosErr(G4ThreeVector xyz) { fLocalPosErr = xyz; }
    void SetWorldPos(G4ThreeVector xyz) { fWorldPos = xyz; }
    void SetWorldPosErr(G4ThreeVector xyz) { fWorldPosErr = xyz; }
    void SetLocalPosTrue(G4ThreeVector xyz) { fLocalPosTrue = xyz; }
    void SetWorldPosTrue(G4ThreeVector xyz) { fWorldPosTrue = xyz; }

    // Get Functions
    inline bool IsGhost() { return fGhost; };
    inline G4double GetTime() const { return fTime; }
    inline G4int GetLayerID() const { return fLayerID; }
    inline G4ThreeVector GetPos() const { return fWorldPos; }
    inline G4ThreeVector GetPosErr() const { return fWorldPosErr; }
    inline G4ThreeVector GetLocalPos() const { return fLocalPos; }
    inline G4ThreeVector GetLocalPosErr() const { return fLocalPosErr; }
    inline G4ThreeVector GetWorldPos() const { return fWorldPos; }
    inline G4ThreeVector GetWorldPosErr() const { return fWorldPosErr; }
    inline G4ThreeVector GetLocalPosTrue() const { return fLocalPosTrue; }
    inline G4ThreeVector GetWorldPosTrue() const { return fWorldPosTrue; }

private:

    G4int fLayerID;
    G4double fTime;

    G4ThreeVector fLocalPosTrue;
    G4ThreeVector fLocalPos;
    G4ThreeVector fLocalPosErr;

    G4ThreeVector fWorldPosTrue;
    G4ThreeVector fWorldPos;
    G4ThreeVector fWorldPosErr;

    bool fGhost;
    uint fDrawOption;

};

// ----------------------------------------------------------------------
// Allocators
typedef G4THitsCollection<DriftChamberHit> DriftChamberHitsCollection;

extern G4ThreadLocal G4Allocator<DriftChamberHit>* DriftChamberHitAllocator;

inline void* DriftChamberHit::operator new(size_t)
{
    if (!DriftChamberHitAllocator)
        DriftChamberHitAllocator = new G4Allocator<DriftChamberHit>;
    return (void*)DriftChamberHitAllocator->MallocSingle();
}

inline void DriftChamberHit::operator delete(void* aHit)
{
    DriftChamberHitAllocator->FreeSingle((DriftChamberHit*) aHit);
}
// ----------------------------------------------------------------------
#endif
