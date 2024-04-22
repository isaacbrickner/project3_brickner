pc: pc.c eventbuf.c eventbuf.h
	gcc -Wall -Wextra -pthread -o pc pc.c eventbuf.c

clean:
	rm -f pc
