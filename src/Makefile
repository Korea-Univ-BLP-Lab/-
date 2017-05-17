CC = g++
#For Optimization
CFLAGS = -O2
#For debugging
#CFLAGS = -g

RM= rm -f
CP= cp

LIB= ../lib/
BIN= ../bin/
MYBIN= /home/dglee/bin/

.cpp.o: 

	$(CC) -c $(CFLAGS) $<

all: search_substring cpall

lex: 
	flex -osearch_regex.cpp search_regex.l

search_substring: search_substring_test.o search_regex.o search_substring.o FST.o GetFST.o
	$(CC) -o $(BIN)$@ search_substring_test.o search_regex.o search_substring.o FST.o GetFST.o

cpall: cpbin
cpbin:
	${CP} $(BIN)search_substring $(MYBIN)

clean:  rmobj rmbin
rmobj:
	${RM} *.o
rmbin:
	${RM} $(BIN)search_substring
