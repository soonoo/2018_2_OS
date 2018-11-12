TARGET = filegen schedtest
MAX_PROCESSES = 15000

all: $(TARGET)

filegen:
	gcc -DMAX_PROCESSES=$(MAX_PROCESSES) -ofilegen filegen.c

schedtest:
	gcc -DMAX_PROCESSES=$(MAX_PROCESSES) -oschedtest schedtest.c -lrt

clean:
	rm -rf *o $(TARGET)

