#include "PoCAUtils.hh"

namespace COSMIC{

void PoCAUtils::GetPoCAFromTracks(G4ThreeVector& poca, G4double& angle, G4double& distance,
                                  G4ThreeVector posA, G4ThreeVector momA,
                                  G4ThreeVector posB, G4ThreeVector momB) {

    // Get the scatter angle
    angle = acos( (momB.dot(momA))/(momB.mag()*momA.mag()) ) * 180. / (2.*3.142);

    // Get the PoCA
    G4ThreeVector w0 = posA - posB;
    double a = momA.dot(momA);
    double b = momA.dot(momB);
    double c = momB.dot(momB);
    double d = momA.dot(w0);
    double e = momB.dot(w0);

    double sc = ((b*e) - (c*d))/((a*c)-(b*b));
    double tc = ((a*e) - (b*d))/((a*c)-(b*b));

    poca = 0.5 * (posA + sc*momA + posB + tc*momB);
    distance = (posA + sc*momB - posB - tc*momB).mag();

    return;
}

} // - namespace COSMIC
