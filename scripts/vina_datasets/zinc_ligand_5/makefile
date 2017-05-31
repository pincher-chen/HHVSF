MONGO_ROOT=/WORK/app/mongo-api/c/1.4.2-v2
#MONGO_ROOT=/NSFCGZ/app/mongo-api/c/1.4.2
LIBS := -L$(MONGO_ROOT)/lib -lbson-1.0 -lmongoc-1.0 -lsasl2
CFLAGS := -I$(MONGO_ROOT)/include/libbson-1.0 -I$(MONGO_ROOT)/include/libmongoc-1.0
#DEBUG := -ggdb
OPTS := -O2 
CC := gcc 
object=get_1000w_ligands
#object=get_torsions_heavy_update_one
#object=update
#WARNINGS := -Wall -Werror

%.o: %.c %.h
	$(CC) -o main.o -c $(OPTS) $(CFLAGS) *.c

main: main.o
	$(CC) -o $@ $(OPTS) $(LIBS) main.o
$(object):$(object).o
	$(CC) -o $@ $(OPTS) $(LIBS) $(object).o
clean:
	rm -rf $(object) $(object).o
