#!/bin/sh
job_id=$1
read_database=$2
read_id_table=zinc_${job_id}
write_table=${read_database}_${job_id}
target_name=$3
node_num=$4
slots=`expr 1 \* $node_num`
ulimit -u unlimited
#source env
#source /WORK/app/condor/cn16354-mangager_submit/condor.sh
source /WORK/app/condor/work/cn16370/condor.sh

#*****write condor_submit****#
#export condor_shell_dir=/HOME/nscc-gz_vscreening_1/WORKSPACE/compute/condor
export condor_shell_dir=/HOME/nscc-gz_pinchen/compute/condor
mkdir ${condor_shell_dir}/${job_id}
export condor_run_dir=${condor_shell_dir}/${job_id}

echo "DOCK_ROOT 	      = /HOME/nscc-gz_pinchen/compute/bin/vina_wrapper" >${condor_run_dir}/${job_id}_dock.sub
echo "universe                = vanilla " >>${condor_run_dir}/${job_id}_dock.sub
echo "executable              = \$(DOCK_ROOT)/vina_dock">>${condor_run_dir}/${job_id}_dock.sub
echo "arguments               = \"$job_id $read_database $read_id_table $write_table $target_name\"">>${condor_run_dir}/${job_id}_dock.sub
echo "log                     = ${condor_run_dir}/dock.log ">>${condor_run_dir}/${job_id}_dock.sub
echo "output                  = ${condor_run_dir}/output.txt">>${condor_run_dir}/${job_id}_dock.sub
echo "error                   = ${condor_run_dir}/errors.txt">>${condor_run_dir}/${job_id}_dock.sub
echo "should_transfer_files   = IF_NEEDED">>${condor_run_dir}/${job_id}_dock.sub
echo "when_to_transfer_output = ON_EXIT">>${condor_run_dir}/${job_id}_dock.sub
echo "environment             = \"LD_LIBRARY_PATH=/HOME/nscc-gz_pinchen/compute/bin/vina_wrapper/lib:/WORK/app/Openbabel/2.3.2-python/lib:\$LD_LIBRARY_PATH\"  ">>${condor_run_dir}/${job_id}_dock.sub
echo "queue $slots">>${condor_run_dir}/${job_id}_dock.sub
#end write

#submit job
condor_submit ${condor_run_dir}/${job_id}_dock.sub

