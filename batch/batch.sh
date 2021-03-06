#!/bin/bash

file_list=$1
output_dir=$2
config_file=$3

hadesroot=/cvmfs/hades.gsi.de/install/6.12.06/hydra2-5.5a/defall.sh

current_dir=$(pwd)
partition=main
time=8:00:00
build_dir=$current_dir/../build

lists_dir=${output_dir}/lists/
log_dir=${output_dir}/log

mkdir -p $output_dir
mkdir -p $log_dir
mkdir -p $lists_dir

split -l 500 -d -a 3 --additional-suffix=.list "$file_list" $lists_dir

n_runs=$(ls $lists_dir/*.list | wc -l)

job_range=1-$n_runs

echo file list=$file_list
echo output_dir=$output_dir
echo log_dir=$log_dir
echo lists_dir=$lists_dir
echo n_runs=$n_runs
echo job_range=$job_range

sbatch -J HT2AT -p $partition -t $time -a $job_range -e ${log_dir}/%A_%a.e -o ${log_dir}/%A_%a.o --export=output_dir=$output_dir,file_list=$file_list,hadesroot=$hadesroot,lists_dir=$lists_dir,build_dir=$build_dir,config_file=$config_file -- /cvmfs/vae.gsi.de/debian8/containers/debian8-user_container_20210211T1503.sif /lustre/nyx/hades/user/mmamaev/hades_analysis_tree_converter/batch/batch_run.sh
