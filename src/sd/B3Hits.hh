#ifndef B3Hits_h
#define B3Hits_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "tls.hh"


class B3Hits : public G4VHit
{
public:
  ///constructor
  B3Hits();

  ///destructor
  ~B3Hits();

  ///copy constructor
  B3Hits(const B3Hits&);

  ///assignment operator
  const B3Hits& operator=(const B3Hits&);

  ///equality operator
  int operator==(const B3Hits&) const;
  
  ///memory allocation
  inline void* operator new(size_t);

  ///memory de-allocation
  inline void  operator delete(void*);

  ///method inherited from the G4VHit base interface
  void Draw(){;};

  ///method inherited from the G4VHit base interface
  void Print(){;};
  
public:
  ///Setters and getters for the parameters that are stored in the hit
  void SetEnergyDeposit(G4double edep){fEnergy = edep;};
  G4double GetEnergyDeposit(){return fEnergy;};
  void SetZ(G4double z){fZ = z;};
  void SetY(G4double y){fY = y;};
  G4double GetZ(){return fZ;};
  G4double GetY(){return fY;};
      
private:
  ///parameters that are stored into the hit

  ///Energy released in the interaction
  G4double fEnergy;
  ///z coordinate
  G4double fZ;
  G4double fY;
};

///B3HitsCollection is an alias to indicate a template of 
///G4THitsCollection having elements of type B3Hits.
typedef G4THitsCollection<B3Hits> B3HitsCollection;

extern G4ThreadLocal G4Allocator<B3Hits> *HitAllocator;


inline void* B3Hits::operator new(size_t)
{
  if (!HitAllocator)
    HitAllocator = new G4Allocator<B3Hits>;
  void *aHit;
  aHit = (void *) HitAllocator->MallocSingle();
  return aHit;
}


inline void B3Hits::operator delete(void *aHit)
{
  if (!HitAllocator)
    HitAllocator = new G4Allocator<B3Hits>;
  HitAllocator->FreeSingle((B3Hits*) aHit);
}


#endif
