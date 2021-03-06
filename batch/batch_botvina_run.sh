#!/bin/bash

format='+%Y/%m/%d-%H:%M:%S'

date $format

job_num=$(($SLURM_ARRAY_TASK_ID))

filelist=$lists_dir/$(ls $lists_dir | sed "${job_num}q;d")

input_files=`head -n 1 $filelist`

cd $output_dir
mkdir -p $job_num
cd $job_num

echo "loading " $hadesroot
source $hadesroot

echo "executing $build_dir/HT2AT -i $input_files -o output.root -c $config_file -m"
$build_dir/HT2AT -i $input_files -o output.root -c $config_file -m

directory=`dirname $input_files`

file=$directory/../geant_out1.root

echo "executing $build_dir/HT2AT -i $file -o wall_output.root -c $config_file --first-event 1"
$build_dir/convert_wall -i $file -o wall_output.root -c $config_file -N -1 --first-event 1

echo JOB FINISHED!
date $format