source /usr/local/geant4/setup.sh 10.2.b01 
source /usr/local/t2k-software/ROOT-NUISANCE/root/bin/thisroot.sh
export G4MAKE="-DGeant4_DIR=$G4LIB"
export G4MAKE_MODULES="-DCMAKE_MODULE_PATH=/usr/local/geant4/10.2.b01/lib64/Geant4-10.2.0/Modules/"
export CXXFLAGS="$CXXFLAGS -std=c++11"
alias g4make="cmake $G4MAKE $G4MAKE_MODULES CMAKE_CXX_STANDARD=11 "