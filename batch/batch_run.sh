#!/bin/bash

format='+%Y/%m/%d-%H:%M:%S'

date $format

job_num=$(($SLURM_ARRAY_TASK_ID))

filelist=$lists_dir/$(ls $lists_dir | sed "${job_num}q;d")

while read line; do
    input_files=$input_files","$line
done < $filelist

cd $output_dir
mkdir -p $job_num
cd $job_num

echo "loading " $hadesroot
source $hadesroot

echo "executing $build_dir/HT2AT -i $input_files -o output.root -c $config_file -m"
#singularity exec \
#          -B /cvmfs/hadessoft.gsi.de/install/debian8/install:/cvmfs/hades.gsi.de/install \
#          -B /cvmfs/hadessoft.gsi.de/install/debian8/oracle:/cvmfs/it.gsi.de/oracle \
#          -B /lustre \
#          /cvmfs/vae.gsi.de/debian8/containers/user_container-production.sif \
#          $build_dir/HT2AT -i $input_files -o output.root -c $config_file -m

$build_dir/HT2AT -i $input_files -o output.root -c $config_file -m

echo JOB FINISHED!
date $format