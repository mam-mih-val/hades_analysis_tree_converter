#!/bin/bash

format='+%Y/%m/%d-%H:%M:%S'

date $format

job_num=$(($SLURM_ARRAY_TASK_ID))

filelist=$lists_dir/$job_num.list

while read line; do
    input_files=$input_files","$line
done < $filelist

cd $output_dir
mkdir -p $job_num
cd $job_num

echo "loading " $hadesroot
source $hadesroot

echo "executing $build_dir/src/HT2AT $filelist"
$build_dir/src/HT2AT $filelist

echo JOB FINISHED!
date $format