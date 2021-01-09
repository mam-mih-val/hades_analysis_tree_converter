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

echo "executing $build_dir/HT2AT -i $input_files -o output.root -c $config_file"
$build_dir/HT2AT -i $input_files -o output.root -c $config_file

echo JOB FINISHED!
date $format