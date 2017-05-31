# HHVSF: A Framework for Accelerating High-throughput Virtual Drug Screening on High-Performance Computers
High-performance High-throughput Virtual Screening Framework (HHVSF) is a general framework, and supports the high-throughput applications of large-scale, multitasking and small sized IO files running on HPC resources. This framework contains task management and data management systems, which can handle thousands of tasks, manage a large volume of lots-of-small files, and reduce the long processing time for analyzing.

The following is the the hardwares and relevant operations in HHVSF:

<img src="https://github.com/pincher-chen/HHVSF/blob/master/data/screenshots/picture.png" width="70%" />

# How to compile
## For HTCondor
Multi-level scheduling method is used to maintain the computing resources.The first level scheduler applies for a number of resources to the second level for task distribution. The second level scheduler can refine the computing resources and then distributes the tasks. Here, we choose HTCondor as the second level scheduler, and We configure the HTCondor with one core per slot to provide more flexible task scheduling.

Before you set up a Condor pool, you need to know the three different roles a machine can play in a your pool:
+ Central manager -- The central manager collects information about the resources available to the pool, and negotiates between a machine that is submitting a job and the machine that will execute the job. Only one machine in a pool can play this role.
+ Execute machine -- Any machine (including the central manager) configured to execute jobs submitted to the pool.
+ Submit machine -- Any machine (including the central manager) configure to submit jobs to the pool.

###  Set up a Central manger and submit machine
```
$ export HOSTNAME=cn16376.12.10.133.152.com
$ ./condor_install --prefix=~/sf_install/condor-5/8.5.7-mangager_submit --type=manager,submit --verbose 
$ source ~/sf_install/condor-5/8.5.7-mangager_submit/condor.sh
 
# etc/condor_config is default setting.
 
$ cat local.cn16374/condor_config.local 
FLOCK_FROM = cn16375,cn16376
ALLOW_WRITE = cn16374, 12.10.133.150
 
$ condor_master
```

### set up serval execute manchines
We take cn16375 and cn16376 for examples.
```
$ export HOSTNAME=cn16377.12.10.133.153.com
$ ./condor_install --prefix=~/sf_install/condor-5/8.5.7-execute --type=execute --central-manager=cn16374 --verbose 
$  source ~/sf_install/condor-5/8.5.7-execute/condor.sh
 
 
$ cat local.cn16375/condor_config.local 
 
ALLOW_WRITE = cn16374, 12.10.133.150
FLOCK_TO = cn16374
$ export HOSTNAME=cn16376.12.10.133.152.com 
$ ./condor_install --prefix=/HOME/nscc-gz_pinchen/sf_install/condor-5/8.5.7-execute-1 --type=execute --central-manager=cn16374 --verbose 
$ source /HOME/nscc-gz_pinchen/sf_install/condor-5/8.5.7-execute-1/condor.sh
 
 
$ cat local.cn16376/condor_config.local 
 
ALLOW_WRITE = cn16374, 12.10.133.150
FLOCK_TO = cn16374
```




## For MongoDB
MongoDB is used as the data repository engine. "Sharding" mothod is used for distributing data across multiple machines to support deployments with very large data sets and high throughput operations. The zinc_ligand_1 ~ 5 databases are prepared for Audock_vina, and the zinc_ligand_2 ~ 5 databases were extracted from zinc_ligand_1 in accordance with a certain proportion. The zinc_conformer_1~2 databases are prepared for WEGA, and the zinc_conformer_2 are extracted from zinc_conformer_1 randomly.

## For wrappers
We transform MTC into HTC by wrapping ADV or WEGA program with MongoDB C driver (version 1.4.2) as a worker. Each worker accesses database preemptively to get input files until all data is traversed. MongoDB provides atomic operation with “inc” to ensure data security when multitudinous workers start concurrently, so that each worker can get unique job. After the worker obtains data from the database, the data is written to a file and stored on the local file system implemented in RAMDISK.
```
$ cd ~/bin/vina_wrapper # modify the makefile, and provide the path of MongoDB C driver.
$ make
$ cd ~/bin/wega_wrapper # modify the makefile, and provide the path of MongoDB C driver.
$ make
```


# How to run HHVSF

$run_*_job.sh argument_1 argument_2 argument_3 argument_4
+ argument_1: job id
+ argument_2: target name or query ligand
+ argument_3: database name
+ argument_4: core number
## Command example
run_vina_job.sh a002 5f5w zinc_Pur1000w_sort_heavy 500 <br />
run_wega_job.sh a072 oneQuery.sd zinc_conformer_100w 100

