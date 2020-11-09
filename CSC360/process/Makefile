.phony all:

all: rsi processes 
rsi: rsi.c
	gcc rsi.c -lreadline -lhistory -o PMan
processes: inf.c foo.c bar.c
	gcc foo.c -o foo
	gcc bar.c -o bar
.PHONY clean:
clean:
	-rm -rf *.o *.exe PMan
