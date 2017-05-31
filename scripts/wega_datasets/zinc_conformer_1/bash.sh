#!/bin/sh
#module load Python/2.7.9-fPIC
python_dir=/WORK/app/Python/2.7.9-fPIC
export PATH=${python_dir}/bin:$PATH
export LD_LIBRARY_PATH=${python_dir}/lib:$LD_LIBRARY_PATH
which python

split_dir=/HOME/nscc-gz_vscreening_1/WORKSPACE/data/zinc_conformer/data_split
for file in `cd  ${split_dir};ls 6_p0* -d`;do
#  cd $file 
  echo "Hello! I am in $file.."
#    for sub_file in `ls ./`;do
#      echo $sub_file
#      echo "$sub_file start to ..."
      cd ${split_dir}/$file
      ${split_dir}/zinc_conformer2mongodb.py $file
#    done
#  cd ..
done
  
