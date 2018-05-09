TIME="`date +%Y%m%d%H%M%S`"
RAND=$RANDOM
JOBID=$$
ID="steertemp_${TIME}_${RAND}_${JOBID}"
mkdir /scratch/steer/$ID
echo /scratch/steer/$ID
