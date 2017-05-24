#include <bson.h>
#include <bcon.h>
#include <mongoc.h>
#include <stdio.h>
#include <unistd.h>
#include <fstream>
#include <string>
#include <iostream>
#include <map>

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
//    readWEGAResDataFile(argv[1],argv[2]);
//    return 0;  

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
   printf("%s %s %s %s\n",argv[1],argv[2],argv[3],argv[4]);
   char *query_ligand = NULL;
   query_ligand  = (char*) malloc(sizeof(char)*64);
   query_ligand = argv[5];

    client = mongoc_client_new ("mongodb://cn16377:16377/");
    client1 = mongoc_client_new ("mongodb://cn16366:16366/");
    collection = mongoc_client_get_collection (client, read_db, "zinc_data");
    collection0 = mongoc_client_get_collection (client1, "id_create", id_table);
    collection1 = mongoc_client_get_collection (client1, "wega_result", write_table);
    
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
        //fprintf (stdout, "%" PRIu64 "\n", count);
    }
    bson_destroy (query_count);

/* get every document from database. */
    int index_id = 0, current_id = 0;


    char *sdf_name = NULL;
    bson_t *query0;
    bson_t *update0;
    sdf_name = (char*) malloc(sizeof(char)*128);

    bson_error_t error0;
    bson_t reply0;

    printf("before modify the current_id is %d\n",current_id);

    query0  = BCON_NEW (("index_id"),
                     "{", "$gt", BCON_INT32(0), "}"
                     );
    update0 = BCON_NEW("$inc", "{", "index_id", BCON_INT32(1),"}");


    //start loop
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

        //从数据库中取出sdf文件；
        printf ("after modify the current is %d\n", current_id);
        query = BCON_NEW (("idxd"),BCON_INT32 (current_id));

        cursor = mongoc_collection_find (collection, MONGOC_QUERY_NONE, 0, 0, 0, query, NULL, NULL);
         while (mongoc_cursor_next (cursor, &doc)) {
             bson_iter_init(&itb, doc);
             if(bson_iter_find(&itb,"sdf_name")){
                 value = bson_iter_value(&itb);
                 if(value->value_type == BSON_TYPE_UTF8) {
                     sprintf(sdf_name, "%s",value->value.v_utf8.str);
                 }
             }
             //判断数据库中是否含有sdf_data,有的退出，无则进行格式转换；
             bson_iter_init(&itb,doc);
             if(bson_iter_find(&itb,"sdf_data")) {
                 //注意此处仍需bosn_iter_init(),否则itb仍然是上一次值；
                 //bson_iter_init(&itb,doc);
                 //if(bson_iter_find(&itb,"pdbqt_content")) {
                     value = bson_iter_value(&itb);
                     if(value->value_type == BSON_TYPE_UTF8) {
          
                         char *file_name=NULL;
                         file_name = (char*) malloc(sizeof(char)*64);
                         //sprintf(file_name, "/HOME/nscc-gz_vscreening_1/WORKSPACE/compute/wega_wrapper/tmp/%s", sdf_name);
                         sprintf(file_name, "/tmp/%s", sdf_name);
                         fp = fopen(file_name,"w");
                         fwrite(value->value.v_utf8.str, sizeof(char), value->value.v_utf8.len, fp);
                         fclose(fp);
                         //printf("step 2 is ok.\n");
                         free(file_name);
                     }
                // }

                 //open pdbqt file and do vina computing
                 //return 0;
                 //continue;
                 char *file_dir_in = NULL;
                 char *file_dir_out = NULL;
                 //char *mol_file_dir = NULL;

                 char *cmd = NULL;
                 char *cmd2 = NULL;
                 //char *cmd3 = NULL;
    
                 file_dir_in = (char*) malloc(sizeof(char)*128);
                 file_dir_out = (char*) malloc(sizeof(char)*128);
                 //mol_file_dir = (char*) malloc(sizeof(char)*128);
                 cmd = (char*) malloc(sizeof(char)*1028);
                 cmd2 = (char*) malloc(sizeof(char)*1028);
                 //cmd3 = (char*) malloc(sizeof(char)*1028);
    
                 sprintf(file_dir_in,"/tmp/%s",sdf_name);
                 sprintf(file_dir_out,"/tmp/%s_out.sd",sdf_name);
                 sprintf(cmd,"/HOME/nscc-gz_pinchen/sf_install/wega/wega_nscc-gz_pinchen.exe \
                 -sd %s -qsd /HOME/nscc-gz_pinchen/sf_install/wega/%s -M 2 -TN 100 -CC true -outsd %s \
                 ",file_dir_in, query_ligand, file_dir_out);

                 printf(" Running %s\n", cmd);
                 system(cmd);
                 //continue;
                 //analyse outfile,insert data to database
                 char *out_file_text = NULL;
                 char *out_file_sd = NULL;
                 char *out_file_aligned = NULL;
     
                 out_file_text = (char*) malloc(sizeof(char)*64);
                 out_file_sd = (char*) malloc(sizeof(char)*64);
                 out_file_aligned = (char*) malloc(sizeof(char)*64);

                 sprintf(out_file_text,"/tmp/%s_out.txt",sdf_name);
                 sprintf(out_file_sd,"/tmp/%s_out.sd",sdf_name);
                 sprintf(out_file_aligned,"/tmp/%s_out_Aligned.sd",sdf_name);
                 //printf("start to deal with %s and %s.\n",out_file_text,out_file_aligned); 
                 //readWEGAResDataFile(out_file_text,out_file_aligned);
                 /******************/
                 
                //  read WEGA result score file
                //char *scoreFile = NULL;
                //scoreFile = (char*) malloc(sizeof(char)*64);
                //scoreFile = out_file_text;
                //char *SDFile = NULL;
                //SDFile = (char*) malloc(sizeof(char)*64);
                //SDFile = out_file_aligned;

                std::ifstream scoreFin;
                scoreFin.open(out_file_text);
                if (!scoreFin.is_open ())
                {
                  std::cerr << "read WEGA result error: score file is not open yet\n";
                  exit (1);
                }

                char zincMolName[32];
                char queryMolName[256];
                char scoreTmp[32];
                double score = 1.0;

                std::map<std::string, double> res_name_score_map;
                int count_tmp = 0;
                std::string tmpstr;
                std::getline(scoreFin,tmpstr); // get rid of first line of column name
                do {
                   std::getline(scoreFin,tmpstr);
                   //std::cout << tmpstr << "\n";
                   sscanf(tmpstr.c_str(), "%[^,],%[^,],%lf", zincMolName, queryMolName, &score);
                   //std::cout << zincMolName << "\n";
                   //std::cout << queryMolName << "\n";
                   //std::cout << score << "\n";
                   res_name_score_map[zincMolName] = score;
                   count_tmp++;
               } while(!scoreFin.eof());
               scoreFin.close();
               //    return;
               //  read WEGA result aligned SD file
               std::ifstream molFin;
               molFin.open(out_file_aligned);
               if (!molFin.is_open ())
               {
                  std::cerr << "read WEGA result error: aligned SD file is not open yet\n";
                  exit (1);
               }

              //std::map<std::string, std::string> res_name_mol_map;
              count_tmp = 0;


              std::string molname;
              while(std::getline(molFin,molname) && !molFin.eof()){
              std::string mol;
              mol = molname + "\n";
              do { // read until "M  END"
                 std::getline(molFin,tmpstr);
                 mol += tmpstr + "\n";
              } while(tmpstr.substr(0,6) != std::string("M  END") && !molFin.eof());

               
              do { // skip to "$$$$"
                 std::getline(molFin,tmpstr);
              } while(tmpstr.substr(0,4) != std::string("$$$$") && !molFin.eof());

             // save to mongodb
             //std::cout << molname << "\n" << mol << "\n" << res_name_score_map[molname] << "\n"; 
             bson_t *doc2;
             doc2 = bson_new();
             bson_error_t error2;
             bson_oid_t oid; 

             bson_oid_init (&oid, NULL);
             BSON_APPEND_OID (doc2, "_id", &oid);
             BSON_APPEND_DOUBLE(doc2,"score", res_name_score_map[molname]);
             BSON_APPEND_UTF8(doc2, "molname", molname.c_str());
             BSON_APPEND_UTF8(doc2, "moldata", mol.c_str());
             BSON_APPEND_UTF8(doc2, "jobid", job_id);

             //printf("Start to insert data to mongo...\n");
             if (!mongoc_collection_insert (collection1, MONGOC_INSERT_NONE, doc2, NULL, &error2)) {
                fprintf (stderr, "%s\n", error2.message);
             }
             else {
             printf ("data insert successfully.\n");
             //FILE *fp1;
             //fp1=fopen("/HOME/nscc-gz_vscreening_1/WORKSPACE/compute/zinc1.log","a");
             //fprintf(fp1,"%s\n",sdf_name);
             //fclose(fp1);
             }
       
           // bson_destroy (doc2);
           // mongoc_collection_destroy (collection1);
      
            //res_name_mol_map[molname] = mol 
            count_tmp++;
            //free(mol_name);
            //free(moldata);
            }
            //} while(!molFin.eof());
            molFin.close();
            /********************/
            sprintf(cmd2,"rm %s %s %s %s",file_dir_in, out_file_text,out_file_sd,out_file_aligned);
            printf("Running %s\n",cmd2);
            system(cmd2);

 
                 //free(scoreFile);
                // free(SDFile)
                /*******/
                // free(out_file_text);
                // free(out_file_sd);
                // free(out_file_aligned);

                 //free(cmd);
                // free(cmd2);
                 //free(cmd3);
                 //free(file_in);
                // free(file_dir_in);
                // free(file_dir_out);
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
    //free(sdf_name);
   // free(file_name);

   // free(job_id);
   // free(read_db);
   // free(id_table);
   // free(write_table);
   // free(query_ligand);

    bson_destroy (query);
    //bson_destroy (doc);
    //bson_destroy (doc2);
    mongoc_cursor_destroy (cursor);
    mongoc_collection_destroy (collection);
    mongoc_collection_destroy (collection0);
    mongoc_collection_destroy (collection1);
    mongoc_client_destroy (client);
    mongoc_client_destroy (client1);
    mongoc_cleanup ();

    return 0;
}
