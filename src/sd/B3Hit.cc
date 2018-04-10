#include "sd/B3Hits.hh"
#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

G4ThreadLocal G4Allocator<B3Hits> *HitAllocator=0;

//==============================================================

///constructor: does nothing
B3Hits::B3Hits() : 
 G4VHit() 
{;}

//==============================================================

///destructor: does nothing
B3Hits::~B3Hits() {;}

//==============================================================

///copy constructor (copies the content of the argument in the new object)
B3Hits::B3Hits(const B3Hits &right) : 
  G4VHit()
{
  fEnergy = right.fEnergy;
  fZ = right.fZ;
  fY = right.fY;
}

//==============================================================

///redefinition of the copy operator (now one can assign and 
///copy objects of B3Hits

const B3Hits& B3Hits::operator=(const B3Hits& right)
{
  fEnergy = right.fEnergy;
  fZ = right.fZ;  
  fY = right.fY;
  return *this;
}


//==============================================================

///Define equality between two B3Hits. They are equal only if their 
///content is exactly the same
int B3Hits::operator==(const B3Hits& right) const
{
  return ((fEnergy == right.fEnergy) && (fZ == right.fZ) && fY == right.fY) ? 1 : 0;
}

