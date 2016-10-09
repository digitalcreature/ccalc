all: clean
	gcc *.c -o ccalc

clean:
	rm -f ccalc
