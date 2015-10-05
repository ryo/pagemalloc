
malloc.so: page_malloc.c
	cc -Wall -O2 -fPIC -shared page_malloc.c -o malloc.so

clean:
	rm -f malloc.so
