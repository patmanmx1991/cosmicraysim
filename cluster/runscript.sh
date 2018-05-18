
# Get Inputs
RUNID=$1

# EDIT : CHANGE THESE LINES TO POINT TO WHERE THE GEOMETRY MACRO AND WORK DIRECTORIES ARE 
GEOMETRY="/home/steer/cosmicraysim/macros"
WORKDIR="/home/steer/cosmicraysim/build/work/outputs"

# Let Condor know where everything is
source setup.sh
source setupgeant4.sh

# Make new temp scratch area
scratchdir=$(source make_temp_scratch.sh)

# Move to scratch
cd $scratchdir

# Run Job
echo cosmicraysim -n 1000 -o tunnel    -g $GEOMETRY/detector_ca_tunnel.geo --run ${RUNID}
cosmicraysim -n 1000 -o tunnel    -g $GEOMETRY/detector_ca_tunnel.geo --run ${RUNID}

# Copy Outputs back to WORK Area
cp -rv ./* $WORKDIR/
cd -

# Remove temp directory
rm -rv $scratchdir
