#include <bson.h>
#include <mongoc.h>
#include <stdio.h>
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
    mongoc_client_t *client;
    mongoc_collection_t *collection;
    mongoc_cursor_t *cursor;
    bson_error_t error;
    bson_oid_t oid;
    bson_t *doc;
    char *file_in = NULL;

    char *read_id_table = NULL;
    read_id_table = (char*) malloc(sizeof(char)*64);
    read_id_table = argv[1];
    
//    FILE *fp;
//    fp = fopen("test.mol2","r");

    mongoc_init ();

    client = mongoc_client_new ("mongodb://cn16366:16366/");
    collection = mongoc_client_get_collection (client, "id_create", read_id_table);

    doc = bson_new ();
//    file_in = readFile ("test.mol2");
    bson_oid_init (&oid, NULL);
//    BSON_APPEND_OID (doc, "_id", &oid);
//    BSON_APPEND_UTF8 (doc, "hello", "world");
//    BSON_APPEND_UTF8 (doc, "file", file_in);
    doc = BCON_NEW ("_id",BCON_OID(&oid),
                    "index_id",BCON_INT32(1));   

    if (!mongoc_collection_insert (collection, MONGOC_INSERT_NONE, doc, NULL, &error)) {
        fprintf (stderr, "%s\n", error.message);
    }
    else {
        printf ("data insert successfully.\n");
    }

    bson_destroy (doc);
    mongoc_collection_destroy (collection);
    mongoc_client_destroy (client);
    mongoc_cleanup ();

    return 0;
}
