for i in `seq 0 1`;
do 
    condor_qsub runscript.sh ${i}
done
