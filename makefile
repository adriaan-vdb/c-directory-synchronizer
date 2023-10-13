PROJECT =  mysync
HEADERS =  $(PROJECT).h
OBJ     =  mysync.o List.o hashtable.o glob2regex.o


C11     =  gcc -std=c11
CFLAGS  =  -Wall -Werror


debug: CFLAGS += -g
debug: $(PROJECT)

$(PROJECT) : $(OBJ)
	$(C11) $(CFLAGS) -o $(PROJECT) $(OBJ)


%.o : %.c $(HEADERS)
	$(C11) $(CFLAGS) -c $<

clean:
	rm -f $(PROJECT) $(OBJ)
