#!/bin/sh
split_path=/HOME/nscc-gz_vscreening_1/sf_install/mayachemtools/bin
data_path=/HOME/nscc-gz_vscreening_1/WORKSPACE/data/zinc_conformer
out_path=/HOME/nscc-gz_vscreening_1/WORKSPACE/data/zinc_conformer/data_split
for((i=72;i<87;i++));do
  mkdir ${out_path}/6_p0.${i}_caesar
  yhrun -n 1 ${split_path}/SplitSDFiles.pl -m Files -n 1100 -r ${out_path}/6_p0.${i}_caesar/6_p0-${i} -o ${data_path}/6_p0.${i}_caesar.sdf &
done
wait
