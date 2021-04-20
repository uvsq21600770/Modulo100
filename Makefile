run: TD2
	./TD2
	gnuplot aff.gplt

TD2: TD2.c
	gcc -Wall -o TD2 TD2.c -lm

clean:
	rm -f TD2