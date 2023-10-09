PROJECT =  mysync
HEADERS =  $(PROJECT).h
OBJ     =  mysync.o List.o hashtable.o


C11     =  gcc -std=c11
CFLAGS  =  -Wall -Werror 


$(PROJECT) : $(OBJ)
	$(C11) $(CFLAGS) -o $(PROJECT) $(OBJ)


%.o : %.c $(HEADERS)
	$(C11) $(CFLAGS) -c $<

clean:
	rm -f $(PROJECT) $(OBJ)
