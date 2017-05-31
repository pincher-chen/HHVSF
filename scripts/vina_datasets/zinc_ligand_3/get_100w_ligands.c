#include <bson.h>
#include <bcon.h>
#include <mongoc.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

char* readFile(const char* filename)
{
        long fSize;
        char* buffer;
        size_t result;
        FILE* fp = fopen(filename,"rb");
 
        if(fp == NULL) return NULL;
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


int
main (int   argc,
      char *argv[])
{
   mongoc_collection_t *collection;
   mongoc_collection_t *collection1;
   mongoc_client_t *client;
   bson_error_t error;
   bson_error_t error1;
   bson_t *query;
   bson_t *update;
   bson_t reply;
   char *str;
   mongoc_cursor_t *cursor;
   const bson_t *doc;
   bson_t *doc2;
   bson_oid_t oid;
   bson_iter_t itb;
   FILE *fp;

   mongoc_init ();

   client = mongoc_client_new ("mongodb://cn16377:16377/");
   collection = mongoc_client_get_collection (client, "zinc_ligand_all", "zinc_data");
   collection1 = mongoc_client_get_collection (client, "zinc_ligand_100w", "zinc_data");

    //建立分片
    bson_t *command;
    bson_t *command1;
    const bson_value_t *value;

    char *file_name = NULL;
    char *mol2_file = NULL;
    char *index_name = NULL;
    char *pdbqt_file = NULL;
    int num_atoms = 0;
    int active_torsion = 0;
    int heavy_atom = 0;
    file_name = (char*) malloc(sizeof(char)*10240);
    mol2_file = (char*) malloc(sizeof(char)*1024000);
    index_name = (char*) malloc(sizeof(char)*10240);
    pdbqt_file = (char*) malloc(sizeof(char)*1024000);


    command = BCON_NEW ("enablesharding", BCON_UTF8 ("zinc_ligand_100w")
                        );
    command1 = BCON_NEW ( "shardcollection",BCON_UTF8("zinc_ligand_100w.zinc_data"),
                          "key","{","idxd",BCON_INT32 (1),"}");
   if(!mongoc_client_command_simple (client, "admin", command, NULL, &reply, &error)){
      fprintf (stderr, "%s\n", error.message);
      return EXIT_FAILURE;
    }

    if(!mongoc_client_command_simple (client, "admin", command1, NULL, &reply, &error1)){
      fprintf (stderr, "%s\n", error1.message);
      return EXIT_FAILURE;
    }
  
   //循环取数据
   int32_t i = 1, count = 1;
   //#pragma omp parallel for ordered,2000 for 1w;200 for 10w;20 for 100w; 2 for 1000w
   for (i = 1;i < 20430347;i = i + 20){
    
/*    char *file_name = NULL;
    char *mol2_file = NULL;
    char *index_name = NULL;
    char *pdbqt_file = NULL;
    file_name = (char*) malloc(sizeof(char)*1024);
    mol2_file = (char*) malloc(sizeof(char)*10240);
    index_name = (char*) malloc(sizeof(char)*1024);
    pdbqt_file = (char*) malloc(sizeof(char)*10240);
*/
     query = BCON_NEW ("idxd", BCON_INT32 (i));
     cursor = mongoc_collection_find (collection, MONGOC_QUERY_NONE, 0, 0, 0, query, NULL, NULL);

     while (mongoc_cursor_more (cursor) && mongoc_cursor_next (cursor, &doc)) {
      //BSON_APPEND_UTF8 (doc2, "file", doc);
      //BSON_APPEND_INT32 (doc2,"count",count);
      //find file_name
      //printf("start to cursor.\n");
      bson_iter_init(&itb, doc);
      if(bson_iter_find(&itb,"file_name")){
         value = bson_iter_value(&itb);
         if(value->value_type == BSON_TYPE_UTF8) {
         sprintf(file_name, "%s",value->value.v_utf8.str);
         }
      }
      
      //find index_name
      //printf("find index name.\n");
      bson_iter_init(&itb, doc);
      if(bson_iter_find(&itb,"index_name")){
         value = bson_iter_value(&itb);
         if(value->value_type == BSON_TYPE_UTF8) {
         sprintf(index_name, "%s",value->value.v_utf8.str);
         }
      }
      //find num_atoms
      bson_iter_init(&itb, doc);
      if(bson_iter_find(&itb,"num_atoms")){
         value = bson_iter_value(&itb);
         if(value->value_type == BSON_TYPE_INT32) {
             num_atoms = value->value.v_int32;

         }
      }
  
       
      //find mol2_name
      
      //printf("find mol2_name.\n");
      bson_iter_init(&itb, doc);
      if(bson_iter_find(&itb,"mol2_file")){
         value = bson_iter_value(&itb);
         if(value->value_type == BSON_TYPE_UTF8) {
           sprintf(mol2_file, "%s",value->value.v_utf8.str);
           //char *mol2_file_name=NULL;
          // mol2_file_name = (char*) malloc(sizeof(char)*1024);
           //sprintf(mol2_file, "%s",value->value.v_utf8.str);
           /*sprintf(mol2_file_name, "/tmp/%s.mol2", index_name);
           fp = fopen(mol2_file_name,"w");
           fwrite(value->value.v_utf8.str, sizeof(char), value->value.v_utf8.len, fp);
           fclose(fp);*/
           //free(mol2_file_name);
         }
      }   
     
      //find pdbqt_file
      
      //printf("find pdbqt file.\n");
      bson_iter_init(&itb, doc);
      if(bson_iter_find(&itb,"pdbqt_file")){
         value = bson_iter_value(&itb);
         if(value->value_type == BSON_TYPE_UTF8) {
            sprintf(pdbqt_file, "%s",value->value.v_utf8.str);
           /*char *pdbqt_file_name=NULL;
           pdbqt_file_name = (char*) malloc(sizeof(char)*1024);
           sprintf(pdbqt_file_name, "/tmp/%s.pdbqt", index_name);
           fp = fopen(pdbqt_file_name,"w");
           fwrite(value->value.v_utf8.str, sizeof(char), value->value.v_utf8.len, fp);
           fclose(fp);
           free(pdbqt_file_name);*/
         }
      } 
       else {
         printf("Can not find pdbqt file.\n");
         printf("This i is: %d",i);
       }
  
      bson_iter_init(&itb, doc);
      if(bson_iter_find(&itb,"active_torsion")){
         value = bson_iter_value(&itb);
         if(value->value_type == BSON_TYPE_INT32) {
             active_torsion = value->value.v_int32;

         }
      }


      bson_iter_init(&itb, doc);
      if(bson_iter_find(&itb,"heavy_atom")){
         value = bson_iter_value(&itb);
         if(value->value_type == BSON_TYPE_INT32) {
             heavy_atom = value->value.v_int32;

         }
      }
//     }
      //BSON_APPEND_UTF8 (doc2, "file", doc);
      //printf("end to find.\n");
      //printf("file name is %s\n",file_name);

      //char *file_dir_mol2 = NULL;
      //char *file_dir_pdbqt = NULL;
      //char *cmd = NULL;
      //file_dir_mol2 = (char*) malloc(sizeof(char)*1024);
      //file_dir_pdbqt = (char*) malloc(sizeof(char)*1024);
      //cmd = (char*) malloc(sizeof(char)*1024);
      //sprintf(file_dir_mol2,"/tmp/%s.mol2",index_name);
      //sprintf(file_dir_pdbqt,"/tmp/%s.pdbqt",index_name);
      //sprintf(cmd,"/bin/rm -rf %s %s",file_dir_mol2, file_dir_pdbqt);
    

      //mol2_file = readFile (file_dir_mol2);
      //pdbqt_file = readFile (file_dir_pdbqt);

      doc2 = bson_new ();
      bson_oid_init (&oid, NULL);
      BSON_APPEND_OID (doc2, "_id", &oid);
      BSON_APPEND_INT32 (doc2,"idxd",count);
      BSON_APPEND_UTF8 (doc2, "file_name", file_name);
      BSON_APPEND_UTF8 (doc2, "mol2_file", mol2_file);
      BSON_APPEND_UTF8 (doc2, "index_name", index_name);
      BSON_APPEND_UTF8 (doc2, "pdbqt_file", pdbqt_file);
      BSON_APPEND_INT32 (doc2,"num_atoms",num_atoms);
      BSON_APPEND_INT32 (doc2,"active_torsion",active_torsion);
      BSON_APPEND_INT32 (doc2,"heavy_atom",heavy_atom);
      
      //printf("start to insert.\n");
      //printf("file_name is %s",file_name);
      if(!mongoc_collection_insert (collection1, MONGOC_INSERT_NONE, doc2, NULL, &error1)){
        fprintf (stderr, "%s\n", error1.message);
      }
      //printf ("data insert successfully.\n");
      //printf (" Running %s\n",cmd);
      //system(cmd);

      //free(cmd);
      //free(file_dir_mol2);
      //free(file_dir_pdbqt);
      //free(pdbqt_file_name);
      //free(mol2_file_name);
      //free(file_name);
      //free(mol2_file);
      //free(index_name);
      //free(pdbqt_file);
      #pragma omp atomic
      count = count + 1;
      if( count % 1000 == 0)printf ("count is %d.\n", count);
     }
     if(count == 1000001){
     break;
     }

   }
   //free(cmd);
   //free(file_dir_mol2);
   //free(file_dir_pdbqt);
   free(file_name);
   free(mol2_file);
   free(index_name);
   free(pdbqt_file);

   bson_destroy (query);
   mongoc_cursor_destroy (cursor);
   mongoc_collection_destroy (collection);
   mongoc_collection_destroy (collection1);
   mongoc_client_destroy (client);

   mongoc_cleanup ();

   return 0;
}




