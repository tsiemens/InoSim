INO := Sample.ino

all:
	mkdir -p build
	cd build; cmake -D INO_PROG_FILE=$(INO) .. && make install

clean:
	cd build && make clean

.PHONY: clean
