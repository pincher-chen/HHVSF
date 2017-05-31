var counter=0;
db.zinc_data.find().sort({"heavy_atom":-1}).forEach(function(mydoc){ 
    db2 = connect("cn16377:16377/zinc_ligand_1000w_sort");
    mydoc.idxd = ++counter;
    db2.zinc_data.insert(mydoc);
    //db2.zinc_data.update({"idxd":mydoc.idxd},{$set:{"idxd":count}});
    ////    db2.zinc_data.find({"idxd":mydoc.idxd});
    //    print(counter);
    //    //    count++;
})
