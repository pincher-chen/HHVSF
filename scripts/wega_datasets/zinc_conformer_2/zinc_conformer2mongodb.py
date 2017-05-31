#! /usr/bin/env python
# -*- coding: utf-8 -*-

if __name__ == '__main__':
  import os
  import sys
  import pymongo
  import pybel
#  import openbabel

#mol2_file_name = raw_input("Please input the file name of the mol2 file: ")

#mongodb_addr = raw_input("Please input the uri of the mongodb: ")

#db_name = raw_input("Please input db name: ")

#collection_name = raw_input("Please input the collection name: ")

  mongodb_addr="cn16377:16377"
  db_name="zinc_conformer_2"
  collection_name="zinc_data"

  file_list=os.listdir(mol_file_dir)
  mol_file_list=[]
  for file_name in file_list:
    if os.path.isfile(mol_file_dir+"/"+file_name) :
      if os.path.splitext(file_name)[1] == '.sdf' :
        mol_file_list.append(file_name)

  client=pymongo.MongoClient(mongodb_addr)
  db=eval("client."+db_name)
  collection=eval("db."+collection_name)
#  collection.create_index([('idxd',pymongo.ASCENDING)],unique=True) #index within dataset
  coll_count = collection.find().count()

  count=1+coll_count
  print mol_file_list
  for mol_file_name in mol_file_list:
    print mol_file_name + " start to import..."
    f=open(mol_file_name,"r+")
    size = os.stat(mol_file_name).st_size
    molfile=f.read(size)
    collection.insert({"idxd":count,"sdf_name":mol_file_name,"sdf_data":molfile})
    count+=1
