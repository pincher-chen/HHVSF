# HHVSF: A Framework for Accelerating High-throughput Virtual Drug Screening on High-Performance Computers
High-performance High-throughput Virtual Screening Framework (HHVSF) is a general framework, and supports the high-throughput applications of large-scale, multitasking and small sized IO files running on HPC resources. This framework contains task management and data management systems, which can handle thousands of tasks, manage a large volume of lots-of-small files, and reduce the long processing time for analyzing.


<img src="https://github.com/pincher-chen/HHVSF/blob/master/data/screenshots/picture.png" width="80%" />

# How to run HHVSF
$run_*_job.sh argument_1 argument_2 argument_3 argument_4
+ argument_1: job id
+ argument_2: target name or query ligand
+ argument_3: database name
+ argument_4: core number
## Command example
run_vina_job.sh a002 5f5w zinc_Pur1000w_sort_heavy 500 <br />
run_wega_job.sh a072 oneQuery.sd zinc_conformer_100w 100

