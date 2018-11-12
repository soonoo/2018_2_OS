LDFLAGS = -pthread
MAX_PROCESSES = 2048
TARGET = numgen fork thread

all: $(TARGET)

numgen:
	gcc -DMAX_PROCESSES=$(MAX_PROCESSES) -onumgen numgen.c

fork:
	gcc -DMAX_PROCESSES=$(MAX_PROCESSES) -ofork fork.c -lrt

thread:
	gcc -DMAX_PROCESSES=$(MAX_PROCESSES) -pthread -othread thread.c -lrt

clean:
	rm -f $(TARGET)

