#!/bin/sh

#environment
#module load mongodb-api/c-1.4.2
export mongo_path=/WORK/app/mongo-api/c/1.4.2
export PATH=${mongo_path}/bin:$PATH
export LD_LIBRARY_PATH=${mongo_path}/lib:$LD_LIBRARY_PATH
#argv
job_id=$1
query_ligand=$2
read_database=$3
#collection=$4
node_num=$4
ulimit -u unlimited
#step 1, alloc compute nodes
#ayhalloc -N 10 &;
#install_condor;

#step 2, initial mongo database
#export mongo_shell_dir=/HOME/nscc-gz_vscreening_1/WORKSPACE/compute/mongo
export mongo_shell_dir=/HOME/nscc-gz_pinchen/compute/mongo
#echo " db.runCommand( { enablesharding :\"wega_result\"})" >${mongo_shell_dir}/${job_id}_enableSharding.js
echo " db.runCommand( { shardcollection : \"babel_result.${read_database}_${job_id}\",key : {\"score\": 1} } );" >>${mongo_shell_dir}/${job_id}_enableSharding.js

mongo cn16366:16366/admin --quiet <${mongo_shell_dir}/${job_id}_enableSharding.js
rm ${mongo_shell_dir}/${job_id}_enableSharding.js
#insert read id table
#/HOME/nscc-gz_vscreening_1/WORKSPACE/compute/mongo/insert_id/insert zinc_${job_id}
/HOME/nscc-gz_pinchen/compute/mongo/insert/insert zinc_${job_id}

#step 3, start condor job
#ssh cn16354 /HOME/nscc-gz_vscreening_1/WORKSPACE/compute/condor/condor_wega_submit.sh $job_id $read_database $query_ligand $node_num 
ssh cn16370 /HOME/nscc-gz_pinchen/compute/condor/condor_wega_submit.sh $job_id $read_database $query_ligand $node_num
##update start time
#${mongo_shell_dir}/update_jobInfo/update_starttime.sh ${job_id}

#step 4, analyze result

