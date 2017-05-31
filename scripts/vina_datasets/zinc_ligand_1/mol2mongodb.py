#! /usr/bin/env python
# -*- coding: utf-8 -*-

if __name__ == '__main__':
  import os
  import pymongo
  import pybel
#  import openbabel

#mol2_file_name = raw_input("Please input the file name of the mol2 file: ")

#db_name = raw_input("Please input db name: ")

#collection_name = raw_input("Please input the collection name: ")

  mol_file_dir="~/ZINC_DB/mol2_all/"
  mongodb_addr="cn16377:16377"
  db_name="zinc_ligand_1"
  collection_name="zinc"

  file_list=os.listdir(mol_file_dir)
  mol_file_list=[]

  client=pymongo.MongoClient(mongodb_addr)
  db=eval("client."+db_name)
  collection=eval("db."+collection_name)

  count=1
  for mol_file_name in mol_file_list:
    mol_file=pybel.readfile("mol2",mol_file_dir+"/"+mol_file_name)

    while True:
      try:
        tmp=mol_file.next()
#        if collection.find({"mol_name":tmp.title}).count()>0 :
#          print "Warning: Duplicated ligand ",tmp.title," skipped."
#          continue
#        collection.insert_one({"idxd":count,"mol_name":tmp.title,"file":tmp.write(format="mol2"),"tag_1":mol_file_name,"num_atoms":len(tmp.atoms)})
        collection.insert({"mol_name":tmp.title,"file":tmp.write(format="mol2"),"tag_1":mol_file_name,"num_atoms":len(tmp.atoms)})
        if((count%5000)==0):
          print "count=",count
        count+=1
      except StopIteration:
        break
