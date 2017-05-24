#include <bson.h>
#include <bcon.h>
#include <mongoc.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

char* readFile(const char* filename)
{
        long fSize;
        char* buffer;
        size_t result;
        FILE* fp = fopen(filename,"rb");
 
        if(fp == NULL) return NULL;
        //if(fp == NULL) continue;
        fseek(fp, 0, SEEK_END);
        fSize = ftell(fp);
        rewind(fp);
 
        buffer = (char*) malloc(sizeof(char)*(fSize+1));
        if(buffer == NULL) return NULL;
        result = fread(buffer, 1, fSize, fp);
        buffer[fSize] = '\0';
        if(result != fSize) {
                if(buffer) free(buffer);
                return NULL;
        }
 
        return buffer;
}


double get_score(const char *file, int row, int col)
{
   double score;
   FILE * pFile;
   int len;
   char mystring [100];
   char tem_str [100];
   const char *sep = " ";
   char *p;
   int i=0, j=0;
//   time_t start, stop;
//   double execute_time;

   pFile = fopen (file, "r");
   if (pFile == NULL) {
     perror ("Error opening file");
     exit(0);
     //continue;
   }
   else {
     for (i = 0;i< row; i++){
       if ( fgets (mystring , 100 , pFile) != NULL );
    }
    p = strtok(mystring, sep);
    for (j = 0; j < col; j++){
      p = strtok(NULL, sep);
    }
    //printf ("%.3f",atof(p));
    score = atof(p);

  }
  fclose (pFile);
  return score;
}

int
main (int   argc,
        char *argv[])
{
//    for mongo connection
    mongoc_client_t *client;
    mongoc_client_t *client1;
    mongoc_collection_t *collection;
    mongoc_collection_t *collection0;
    mongoc_collection_t *collection1;

//    for query, iter and print
    mongoc_cursor_t *cursor;
    const bson_t *doc;
    bson_t *query;
    bson_t *query_count;
    char *str;
    const bson_value_t *value;
    bson_iter_t itb, itb_sub;
//    for file IO
    FILE *fp;
    int64_t count;
    bson_error_t error;

    mongoc_init ();

    //argv[1]
    //time_t start, stop;
    //double execute_time;
    int num_atoms = 0;
 
    char *job_id = NULL;
    job_id = (char*) malloc(sizeof(char)*64);
    job_id = argv[1];
    //argv[2]
    char *read_db = NULL;
    read_db = (char*) malloc(sizeof(char)*64);
    read_db = argv[2];
    //argv[3]
    char *id_table = NULL;
    id_table = (char*) malloc(sizeof(char)*64);
    id_table = argv[3];
    //argv[4]
    char *write_table = NULL;
    write_table = (char*) malloc(sizeof(char)*64);
    write_table = argv[4];
   //argv[5]
   char *target = NULL;
   target = (char*) malloc(sizeof(char)*64);
   target = argv[5];

    client = mongoc_client_new ("mongodb://cn16377:16377/");
    client1 = mongoc_client_new ("mongodb://cn16366:16366/");
    collection = mongoc_client_get_collection (client, read_db, "zinc_data");
    collection0 = mongoc_client_get_collection (client1, "id_create", id_table);
    collection1 = mongoc_client_get_collection (client1, "vina_result", write_table);
    
   // free(job_id);
   // free(read_db);
   // free(id_table);
   // free(write_db);
   // free(target);
/*    char *cmd0 = NULL;
    cmd0 = (char*) malloc(sizeof(char)*128);
    sprintf(cmd0,"cp /HOME/nscc-gz_pinchen/bin/mongo/vina /tmp/;cp -r /HOME/nscc-gz_pinchen/bin/mongo/docking_data/5f5w /tmp/");
    system(cmd0);
    free(cmd0);
*/
/*get collections count.*/
    query_count = BCON_NEW (("idxd"),
                     "{", "$gt", BCON_INT32(0), "}"
                     );
    count = mongoc_collection_count (collection, MONGOC_QUERY_NONE, query_count, 0, 0, NULL, &error);


    if (count < 0) {
 //     str = bson_as_json (&error, -1, NULL);
        fprintf (stderr, "%s\n", str);
        bson_free (str);
    } 
    else {
        fprintf (stdout, "%" PRIu64 "\n", count);
    }
    bson_destroy (query_count);

/* get every document from database. */
    int index_id = 0, current_id = 0;


    char *index_name = NULL;
    bson_t *query0;
    bson_t *update0;
    index_name = (char*) malloc(sizeof(char)*64);

    bson_error_t error0;
    bson_t reply0;

    printf("before modify the current_id is %d\n",current_id);

    query0  = BCON_NEW (("index_id"),
                     "{", "$gt", BCON_INT32(0), "}"
                     );
    update0 = BCON_NEW("$inc", "{", "index_id", BCON_INT32(1),"}");

    //enablesharding
    /*
    bson_error_t error11;
    bson_error_t error12;
    bson_t reply;
    char *str0;

    bson_t *command;
    bson_t *command1;

    command = BCON_NEW ("enablesharding", BCON_UTF8 ("vina_result"));
    command1 = BCON_NEW ( "shardcollection",BCON_UTF8("vina_result.temp_result"),
                          "key","{","score",BCON_INT32 (1),"}");

//    if(!mongoc_client_command_simple (client1, "admin", command, NULL, &reply, &error11)){
//      fprintf (stderr, "%s\n", error11.message);
//      str0 = bson_as_json (&reply, NULL);
//      printf ("%s\n", str0);
//      return EXIT_FAILURE;
//    }
    mongoc_client_command_simple (client1, "admin", command, NULL, &reply, &error11);
    
    if(error11.message == "sharding already enabled for database vina_result"){
     
    }
    else {      
      if(!mongoc_client_command_simple (client1, "admin", command1, NULL, &reply, &error12)){
        fprintf (stderr, "%s\n",error11.message);
      }
   }  
    */
    //start loop
    //while (current_id <= count+20330347) {
    while (current_id <= count) {

        if(!mongoc_collection_find_and_modify (collection0, query0, NULL, update0, NULL, false, false, false, &reply0, &error0)) {
			printf("Fail to get index_id: %s\n",error0.message);
	    }
	    else {
            if((bson_iter_init(&itb,&reply0))&&(bson_iter_find_descendant(&itb, "value.index_id",&itb_sub))) {
		        value = bson_iter_value(&itb_sub);
			    if(value->value_type == BSON_TYPE_INT32) {
				    current_id = value->value.v_int32;
			    }
			    else printf("Type error.\n");
		    }
		    else {
		        printf("Error finding value.index_id.\n");
		        //continue;
		    }

	    }

        //从数据库中取出mol2文件；
        printf ("after modify the current is %d\n", current_id);
        query = BCON_NEW (("idxd"),BCON_INT32 (current_id));

        cursor = mongoc_collection_find (collection, MONGOC_QUERY_NONE, 0, 0, 0, query, NULL, NULL);
        while (mongoc_cursor_next (cursor, &doc)) {
             bson_iter_init(&itb, doc);
             if(bson_iter_find(&itb,"index_name")){
                 value = bson_iter_value(&itb);
                 if(value->value_type == BSON_TYPE_UTF8) {
                     sprintf(index_name, "%s",value->value.v_utf8.str);
                 }
             }
             //判断数据库中是否含有pqbqt_content,有的退出，无则进行格式转换；
             bson_iter_init(&itb,doc);
             if(bson_iter_find(&itb,"pdbqt_file")) {
                 //注意此处仍需bosn_iter_init(),否则itb仍然是上一次值；
                 //bson_iter_init(&itb,doc);
                 //if(bson_iter_find(&itb,"pdbqt_content")) {
                     value = bson_iter_value(&itb);
                     if(value->value_type == BSON_TYPE_UTF8) {
          
                         char *file_name=NULL;
                         file_name = (char*) malloc(sizeof(char)*64);
                         sprintf(file_name, "/tmp/%s.pdbqt", index_name);
                         fp = fopen(file_name,"w");
                         fwrite(value->value.v_utf8.str, sizeof(char), value->value.v_utf8.len, fp);
                         fclose(fp);
                     }
                // }
                bson_iter_init(&itb, doc);
                if(bson_iter_find(&itb,"num_atoms")){
                   value = bson_iter_value(&itb);
                   if(value->value_type == BSON_TYPE_INT32) {
                      num_atoms=value->value.v_int32;
                   }
                }
                

                 //open pdbqt file and do vina computing
                 char *file_dir_in = NULL;
                 char *file_dir_out = NULL;
                 char *mol_file_dir = NULL;

                 char *cmd = NULL;
                 char *cmd2 = NULL;
                 char *cmd3 = NULL;
    
                 file_dir_in = (char*) malloc(sizeof(char)*128);
                 file_dir_out = (char*) malloc(sizeof(char)*128);
                 mol_file_dir = (char*) malloc(sizeof(char)*128);
                 cmd = (char*) malloc(sizeof(char)*1028);
                 cmd2 = (char*) malloc(sizeof(char)*1028);
                 cmd3 = (char*) malloc(sizeof(char)*1028);
    
                 sprintf(file_dir_in,"/tmp/%s.pdbqt",index_name);
                 sprintf(file_dir_out,"/tmp/%s.out.pdbqt",index_name);
                 sprintf(cmd,"/HOME/nscc-gz_pinchen/compute/bin/vina --cpu 1 \
                 --config /HOME/nscc-gz_pinchen/compute/data/%s/%s.txt --ligand %s \
                 --receptor /HOME/nscc-gz_pinchen/compute/data/%s/%s.pdbqt --out %s \
                 ",target, target, file_dir_in, target, target, file_dir_out);

                 printf(" Running %s\n", cmd);
                 //time(&start);
                 system(cmd);
                 //time(&stop);
                 //execute_time = (double)(stop - start);

                 //get score number from pdbqt result file
                 double score = 0.0;
                 int score_row = 2;
                 int score_col = 3;
                 score = get_score (file_dir_out, score_row, score_col);

                 //convert pdbqt result file to mol file
                 
                 printf("start to babel\n");
                 sprintf(mol_file_dir,"/tmp/%s.mol",index_name);
                 sprintf(cmd2,"source /WORK/app/Openbabel/2.3.2-python/bin/env.sh;obabel -ipdbqt %s -omol -O %s;sed -i '1c %s' %s",file_dir_out, mol_file_dir, index_name, mol_file_dir);
                 //printf("end babel\n");
                 printf(" Running %s\n", cmd2);
                 system(cmd2);
                 
                 //insert mol file and score result back into mongodb

                 bson_t *doc2;
                 doc2 = bson_new();
                 bson_error_t error2;
                 bson_oid_t oid;
                 //bson_t reply;
                 char *file_in = (char*) malloc(sizeof(char)*20560);
                 //file_in = readFile (mol_file_dir);
                 file_in = readFile (mol_file_dir);
                 //doc2 = BCON_NEW("_id",BCON_OID(&oid),
                 //                "mol_name",index_name,
                  //               "mol_data",BCON_UTF8(file_in));
                  //               "score", score);
                  //doc2 = bson_new();
                  bson_oid_init(&oid,NULL);
                  BSON_APPEND_OID (doc2, "_id", &oid);
                  BSON_APPEND_DOUBLE(doc2,"score",score);
		  BSON_APPEND_UTF8(doc2, "molname", index_name);
		  BSON_APPEND_UTF8(doc2, "moldata", file_in);
                  BSON_APPEND_UTF8(doc2, "jobid", job_id);
                  //BSON_APPEND_DOUBLE(doc2,"dock_time",execute_time);
                  BSON_APPEND_INT32(doc2,"num_atoms",num_atoms);

		  //create index;
		  mongoc_index_opt_t opt;
                  bson_error_t error4;
		 // bson_error_t error3;
		 // bson_t keys;
		 /*
                  bson_error_t error11;
                  bson_error_t error12;
                  bson_t reply;	
                  char *str;	
              
                  bson_t *command;
                  bson_t *command1;	

                 command = BCON_NEW ("enablesharding", BCON_UTF8 ("vina_result"));
                 command1 = BCON_NEW ( "shardcollection",BCON_UTF8("vina_result.temp_result"),
                          "key","{","score",BCON_INT32 (1),"}");

                 if(!mongoc_client_command_simple (client1, "admin", command, NULL, &reply, &error11)){
                   fprintf (stderr, "%s\n", error11.message);
                //   return EXIT_FAILURE;
              //   }
                 
                 str = bson_as_json (&reply, NULL);
                 printf ("%s\n", str);
                 return EXIT_FAILURE;
                 } 
                 
                 if(!mongoc_client_command_simple (client1, "admin", command1, NULL, &reply, &error12)){
                   fprintf (stderr, "%s\n", error12.message);
                 return EXIT_FAILURE;
                 }
                 */
                 /*
		  bson_init(&keys);
	          mongoc_index_opt_init(&opt);
	          bson_append_double(&keys, "score", -1, 1);
                  
                
	         if (!mongoc_collection_create_index(collection1, &keys,&opt, &error5)){
	                 fprintf (stderr, "%s\n", error5.message);
			      }
			      else {
                      printf ("data insert successfully.\n");
                 }

                 */
                 printf("start to insert.\n");
                 if (!mongoc_collection_insert (collection1, MONGOC_INSERT_NONE, doc2, NULL, &error4)) {
                     fprintf (stderr, "%s\n", error4.message);
                 }
                 else {
                      printf ("data insert successfully.\n");
                 }
                 sprintf(cmd3,"rm -rf %s %s %s",file_dir_in, file_dir_out, mol_file_dir);
                 system(cmd3);


                 //free(cmd);
                 //free(cmd2);
                 //free(cmd3);
                 //free(file_in);
                 //free(file_dir_in);
                 //free(file_dir_out);
             }
        }
    }
    
//    bson_destroy (doc);
  /*  char *cmd4 = NULL;
    cmd4 = (char*) malloc(sizeof(char)*128);
    sprintf(cmd4,"rm /tmp/vina;rm -r /tmp/5f5w");
    system(cmd4);
    free(cmd4);
*/
    //free(index_name);

    //free(job_id);
    //free(read_db);
    //free(id_table);
    //free(write_table);
    //free(target);

    bson_destroy (query);
    mongoc_cursor_destroy (cursor);
    mongoc_collection_destroy (collection);
    mongoc_client_destroy (client);
    mongoc_cleanup ();

    return 0;
}
