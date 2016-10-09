all: clean
	gcc *.c -o ccalc -lm

clean:
	rm -f ccalc
