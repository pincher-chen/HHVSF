#! /usr/bin/env python
# -*- coding: utf-8 -*-

if __name__ == '__main__':
  import os
  import pymongo
  import pybel
#  import openbabel

#mol2_file_name = raw_input("Please input the file name of the mol2 file: ")

#mongodb_addr = raw_input("Please input the uri of the mongodb: ")

#db_name = raw_input("Please input db name: ")

#collection_name = raw_input("Please input the collection name: ")

  mol_file_dir="~/ZINC_DB/sd_all/"
  mongodb_addr="cn16377:16377"
  db_name="zinc_ligand_1"
  collection_name="zinc_data"

  file_list=os.listdir(mol_file_dir)
  mol_file_list=[]
  for file_name in file_list:
    if os.path.isfile(mol_file_dir+"/"+file_name) :
      if os.path.splitext(file_name)[1] == '.sd' :
        mol_file_list.append(file_name)

  client=pymongo.MongoClient(mongodb_addr)
  db=eval("client."+db_name)
  collection=eval("db."+collection_name)
  collection.create_index([('idxd',pymongo.ASCENDING)],unique=True) #index within dataset

  count=1
  for mol_file_name in mol_file_list:
    mol_file=pybel.readfile("sd",mol_file_dir+"/"+mol_file_name)

    while True:
      try:
        tmp=mol_file.next()
#        if collection.find({"mol_name":tmp.title}).count()>0 :
#          print "Warning: Duplicated ligand ",tmp.title," skipped."
#          continue
#        p_charges = tmp.data['PartialCharges']
#        print tmp.data.keys()
#        collection.insert_one({"idxd":count,"zinc_id":tmp.title,"sd_file":tmp.write(format="sd"),"molwt":tmp.molwt,"log_p":tmp.data['LOGP'],"h_acceptors":tmp.data['H_ACCEPTORS'],"h_donors":tmp.data['H_DONORS'],"n_rb":tmp.data['ROTATABLE_BONDS'],"log_s":tmp.data['LOGS'],"t_psa":tmp.data['TPSA'],"formula":tmp.formula,"charge":tmp.charge})
        collection.insert({"sd_name":tmp.title,"file":tmp.write(format="sd"),"tag_1":sd_file_name,"num_atoms":len(tmp.atoms)})
        if((count%100)==0):
	  print "count=",count
        count+=1
      except StopIteration:
        break

