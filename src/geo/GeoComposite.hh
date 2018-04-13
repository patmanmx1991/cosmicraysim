// #ifndef __GeoComposite_HH__
// #define __GeoComposite_HH__

// #include "GeoObject.hh"

// class G4LogicalVolume;
// class G4VPhysicalVolume;

// namespace COSMIC {

// /// Composite geometry template object for making large geometries
// /// containing many contributions
// ///
// /// Each composite must have a VOLUME header assigned to this geometries
// /// physical and logical pointers.
// class GeoComposite : public GeoObject {
// public:

//   /// Empty constructor
//   GeoComposite(){};

//   /// Construct function that sets each sub object up.
//   /// Children MUST override this function.
//   void Construct(DBLink* table) = 0;  


// protected:
//   std::vector<GeoObject*> fSubObjects; ///< Vector of all sub objects. 
// };

// } // namespace COSMIC
// #endif