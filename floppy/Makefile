OPTS=-I inc/ -static -O3

all: test

test:
	gcc $(OPTS) open_and_sig.c -o sig
	gcc $(OPTS) multiple_tasks_waiting.c -o mtw 

clean:
	rm -f test

