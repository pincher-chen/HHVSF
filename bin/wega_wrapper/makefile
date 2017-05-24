#MONGO_ROOT=/HOME/nscc-gz_pinchen/sf_install/mongo-c-driver
MONGO_ROOT=/WORK/app/mongo-api/c/1.4.2
LIBS := -L$(MONGO_ROOT)/lib -lbson-1.0 -lmongoc-1.0 -lsasl2
CFLAGS := -I$(MONGO_ROOT)/include/libbson-1.0 -I$(MONGO_ROOT)/include/libmongoc-1.0
#DEBUG := -ggdb
OPTS := -O2
#CC := gcc
CC := g++
object = wega_wrapper
#object=test2
#WARNINGS := -Wall -Werror

%.o: %.c %.h
	$(CC) -o main.o -c $(OPTS) $(CFLAGS) *.c

main: main.o
	$(CC) -o $@ $(OPTS) $(LIBS) main.o
$(object):$(object).o
	$(CC) -o $@ $(OPTS) $(LIBS) $(object).o
clean:
	rm -rf $(object) $(object).o
