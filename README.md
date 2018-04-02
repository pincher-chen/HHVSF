# HHVSF: A Framework for Accelerating High-throughput Virtual Drug Screening on High-Performance Computers
High-performance High-throughput Virtual Screening Framework (HHVSF) is a general framework, and supports the high-throughput applications of large-scale, multitasking and small sized IO files running on HPC resources. This framework contains task management and data management systems, which can handle thousands of tasks, manage a large volume of lots-of-small files, and reduce the long processing time for analyzing.

The following is the the hardwares and relevant operations in HHVSF:

<img src="https://github.com/pincher-chen/HHVSF/blob/master/data/screenshots/picture.png" width="70%" />

# 1. Code structure
```
$ tree -d
.
|-- bin                  # precompiled executes (for example vina, wega) and source code executes (for example vina_wrapper,       wega_wrapper).
|   |-- vina_wrapper
|   |   `-- lib
|   `-- wega_wrapper
|       `-- lib
|-- condor               # scripts for submit job and each job log files.
|   |-- a001
|   |-- a002
|   |-- a003
|   |-- a004
|   |-- a005
|   |-- a007
...........
|-- data                 # data for screening, for example, targets or querying ligands.
|   |-- 2j3m
|   |-- 5f5w
|   `-- screenshots
|-- mongo                # scripts for mongo's operations.
|   `-- insert
`-- scripts              # scripts for preparing datasets for virtual screening.
    |-- vina_datasets
    |   |-- sort
    |   |-- zinc_ligand_1
    |   |-- zinc_ligand_2
    |   |-- zinc_ligand_3
    |   |-- zinc_ligand_4
    |   `-- zinc_ligand_5
    `-- wega_datasets
        |-- zinc_conformer_1
        `-- zinc_conformer_2
```

# 2. How to compile
## 2.1 For HTCondor
Multi-level scheduling method is used to maintain the computing resources.The first level scheduler applies for a number of resources to the second level for task distribution. The second level scheduler can refine the computing resources and then distributes the tasks. Here, we choose HTCondor as the second level scheduler, and We configure the HTCondor with one core per slot to provide more flexible task scheduling.

Before you set up a Condor pool, you need to know the three different roles a machine can play in a your pool:
+ Central manager -- The central manager collects information about the resources available to the pool, and negotiates between a machine that is submitting a job and the machine that will execute the job. Only one machine in a pool can play this role.
+ Execute machine -- Any machine (including the central manager) configured to execute jobs submitted to the pool.
+ Submit machine -- Any machine (including the central manager) configure to submit jobs to the pool.

###  2.1.1 Set up a central manger and submit machine
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

### 2.1.2 Set up serval execute manchines
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


## 2.2 For MongoDB
MongoDB is used as the data repository engine. "Sharding" mothod is used for distributing data across multiple machines to support deployments with very large data sets and high throughput operations. The zinc_ligand_1 ~ 5 databases are prepared for Audock_vina, and the zinc_ligand_2 ~ 5 databases were extracted from zinc_ligand_1 in accordance with a certain proportion. The zinc_conformer_1~2 databases are prepared for WEGA, and the zinc_conformer_2 are extracted from zinc_conformer_1 randomly.

### 2.2.1 set up nodes for sharding
+ cn16377:Query Routers
+ cn16378:Shard1 + Config Server
+ cn16379:Shard2 + Config Server
+ cn16380:Shard3 + Config Server

```
#step 1
[~@cn16378 mongoDB]$ numactl --interleave=all mongod --configsvr --port 16378 --dbpath mongo_data/shard/config0 --logpath mongo_data/shard/log/config0.log --logappend --fork --httpinterface --rest
about to fork child process, waiting until server is ready for connections.
forked process: 2075807
child process started successfully, parent exiting
 
[~@cn16379 mongoDB]$ numactl --interleave=all mongod --configsvr --port 16379 --dbpath mongo_data/shard/config1 --logpath mongo_data/shard/log/config1.log --logappend --fork --httpinterface --rest
about to fork child process, waiting until server is ready for connections.
forked process: 2837036
child process started successfully, parent exiting
 
[~@cn16380 mongoDB]$ numactl --interleave=all mongod --configsvr --port 16380 --dbpath mongo_data/shard/config2 --logpath mongo_data/shard/log/config2.log --logappend --fork --httpinterface --rest
about to fork child process, waiting until server is ready for connections.
forked process: 2707866
child process started successfully, parent exiting
```

```
#step 2
[~@cn16377 mongoDB]$ numactl --interleave=all mongos --port 16377 --configdb cn16378:16378,cn16379:16379,cn16380:16380 --fork --logpath mongo_data/shard/log/route.log --chunkSize 500  --httpinterface
about to fork child process, waiting until server is ready for connections.
forked process: 3007685
child process started successfully, parent exiting
```

```
#step 3
[~@cn16378 mongoDB]$ numactl --interleave=all mongod --shardsvr --replSet shard0 --port 26378 --dbpath mongo_data/shard/s0 --logpath mongo_data/shard/log/s0.log --logappend --fork --httpinterface --rest
about to fork child process, waiting until server is ready for connections.
forked process: 2075861
child process started successfully, parent exiting
 
[~@cn16378 mongoDB]$ mongo --port 26378
> rs.initiate()
{
	"info2" : "no configuration specified. Using a default configuration for the set",
	"me" : "cn16378:26378",
	"ok" : 1
}
 
[~@cn16379 mongoDB]$ numactl --interleave=all mongod --shardsvr --replSet shard1 --port 26379 --dbpath mongo_data/shard/s1 --logpath mongo_data/shard/log/s1.log --logappend --fork --httpinterface --rest
about to fork child process, waiting until server is ready for connections.
forked process: 2837087
child process started successfully, parent exiting
 
[~@cn16379 mongoDB]$ mongo --port 26379
> rs.initiate()
{
	"info2" : "no configuration specified. Using a default configuration for the set",
	"me" : "cn16379:26379",
	"ok" : 1
}
 
[~@cn16380 mongoDB]$ numactl --interleave=all  mongod --shardsvr --replSet shard2 --port 26380 --dbpath mongo_data/shard/s2 --logpath mongo_data/shard/log/s2.log --logappend --fork --httpinterface --rest
about to fork child process, waiting until server is ready for connections.
forked process: 2707916
child process started successfully, parent exiting
 
[~@cn16380 mongoDB]$ mongo --port 26380
> rs.initiate()
{
	"info2" : "no configuration specified. Using a default configuration for the set",
	"me" : "cn16380:26380",
	"ok" : 1
}
```

```
#step 4
[~@cn16377 mongoDB]$ mongo admin --port 16377
MongoDB shell version: 3.2.9
connecting to: 127.0.0.1:16377/admin
mongos> db.runCommand({addshard:"shard0/cn16378:26378"})
{ "shardAdded" : "shard0", "ok" : 1 }
mongos> db.runCommand({addshard:"shard1/cn16379:26379"})
{ "shardAdded" : "shard1", "ok" : 1 }
mongos> db.runCommand({addshard:"shard2/cn16380:26380"})
{ "shardAdded" : "shard2", "ok" : 1 }
 
mongos> sh.status()
--- Sharding Status --- 
  sharding version: {
	"_id" : 1,
	"minCompatibleVersion" : 5,
	"currentVersion" : 6,
	"clusterId" : ObjectId("5840356f79f05c6ba3affbb9")
}
  shards:
	{  "_id" : "shard0",  "host" : "shard0/cn16378:26378" }
	{  "_id" : "shard1",  "host" : "shard1/cn16379:26379" }
	{  "_id" : "shard2",  "host" : "shard2/cn16380:26380" }
  active mongoses:
	"3.2.9" : 1
  balancer:
	Currently enabled:  yes
	Currently running:  no
	Failed balancer rounds in last 5 attempts:  0
	Migration Results for the last 24 hours: 
		No recent migrations
  databases:
```

```
#step 5
[~@cn16377 mongoDB]$ mongo admin --port 16377
mongos> db.runCommand({enablesharding:"testdb"})
{ "ok" : 1 }
mongos> db.runCommand({shardcollection:"testdb.table1",key:{id:1}})
{ "collectionsharded" : "testdb.table1", "ok" : 1 }
mongos> 
 
[~@cn16377 mongoDB]$ mongo cn16377:16377
MongoDB shell version: 3.2.9
connecting to: cn16377:16377/test
mongos> for (var i=1;i<=100000;i++) db.table1.save({id:i,"test1":"testvar1"})
WriteResult({ "nInserted" : 1 })
mongos> db.table1.stats()
{
	"sharded" : true,
	"capped" : false,
	"ns" : "testdb.table1",
	"count" : 100000,
	"size" : 5400000,
	"storageSize" : 1761280,
	"totalIndexSize" : 2023424,
	"indexSizes" : {
		"_id_" : 884736,
		"id_1" : 1138688
	},
	"avgObjSize" : 54,
	"nindexes" : 2,
	"nchunks" : 3,
	"shards" : {
		"shard0" : {
			"ns" : "testdb.table1",
			"count" : 99981,
			"size" : 5398974,
			"avgObjSize" : 54,
			"storageSize" : 1728512,
			"capped" : false,
 
"shard1" : {
			"ns" : "testdb.table1",
			"count" : 1,
			"size" : 54,
			"avgObjSize" : 54,
			"storageSize" : 16384,
			"capped" : false,
 
	"shard2" : {
			"ns" : "testdb.table1",
			"count" : 18,
			"size" : 972,
			"avgObjSize" : 54,
			"storageSize" : 16384,
			"capped" : false,
```

## 2.3 For wrappers
We transform MTC into HTC by wrapping ADV or WEGA program with MongoDB C driver (version 1.4.2) as a worker. Each worker accesses database preemptively to get input files until all data is traversed. MongoDB provides atomic operation with “inc” to ensure data security when multitudinous workers start concurrently, so that each worker can get unique job. After the worker obtains data from the database, the data is written to a file and stored on the local file system implemented in RAMDISK.
```
$ cd ~/bin/vina_wrapper # modify the makefile, and provide the path of MongoDB C driver.
$ make
$ cd ~/bin/wega_wrapper # modify the makefile, and provide the path of MongoDB C driver.
$ make
```


# 3. How to run HHVSF
```
$ pwd
~/HHVSF
$ run_*_job.sh argument_1 argument_2 argument_3 argument_4
```
+ argument_1: job id
+ argument_2: target name or query ligand
+ argument_3: database name
+ argument_4: core number
## 3.1 Command example
```
$ run_vina_job.sh a002 5f5w zinc_Pur1000w_sort_heavy 500 
$ run_wega_job.sh a072 oneQuery.sd zinc_conformer_100w 100
```

# 4. Citing
The implemented algorithm is described in the following paper:    
Chen P., Yan X., Li J., Du Y., Xu J. (2018) HHVSF: A Framework to Accelerate Drug-Based High-Throughput Virtual Screening on High-Performance Computers. In: Yokota R., Wu W. (eds) Supercomputing Frontiers. SCFA 2018. Lecture Notes in Computer Science, vol 10776. Springer, Cham

