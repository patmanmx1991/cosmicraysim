
# Get Inputs
RUNID=$1
RUNLENGTH=$2
CHUNKLENGTH=$3

GEOMETRY="/home/chris/Code/geant4.10.03.p03/work/cosmicraysim/macros/ca"
WORKDIR="/home/chris/Code/geant4.10.03.p03/work/cosmicraysim/build/outputs"

# Let Condor know where everything is
source setup.sh
# source setupgeant4.sh

# Make new temp scratch area
# scratchdir=$(source make_temp_scratch.sh)

# Move to scratch
# cd $scratchdir

# Run Job
echo "Running target and background simulation..."
cosmicraysim -t ${RUNLENGTH} -c ${CHUNKLENGTH} -o ca_d25_s0p5_signal -g ${GEOMETRY}/ca_variables_d25_s0p5.geo -g ${GEOMETRY}/ca_world_rock.geo -g ${GEOMETRY}/ca_detector_system.geo -g ${GEOMETRY}/ca_air_tunnels.geo -g ${GEOMETRY}/ca_source_flux.geo --run ${RUNID} &

echo "Running background only simulation..."
cosmicraysim -t ${RUNLENGTH} -c ${CHUNKLENGTH} -o ca_d25_s0p5_background -g ${GEOMETRY}/ca_variables_d25_s0p5.geo -g ${GEOMETRY}/ca_world_rock.geo -g ${GEOMETRY}/ca_detector_system.geo -g ${GEOMETRY}/ca_source_flux.geo --run ${RUNID} &

echo "Running opensky simulation..."
cosmicraysim -t ${RUNLENGTH} -c ${CHUNKLENGTH} -o ca_d25_s0p5_norm -g ${GEOMETRY}/ca_variables_d25_s0p5.geo -g ${GEOMETRY}/ca_world_air.geo -g ${GEOMETRY}/ca_detector_system.geo -g ${GEOMETRY}/ca_source_flux.geo --run ${RUNID} &

# Copy Outputs back to WORK Area
# cp -rv ./* $WORKDIR/
# cd -

# Remove temp directory
# rm -rv $scratchdir
