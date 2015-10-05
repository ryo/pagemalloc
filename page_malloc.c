#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include <machine/vmparam.h>

#define	PAGE_ALIGN(x)	((void *)((unsigned long)((x) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1)))

#undef DEBUG
#undef ALIGN_PTRSIZE	/* malloc return pointer aligned address (default) */

struct mallochdr {
	unsigned long magic;
	void *mmappage;
	unsigned long mmapsize;
	unsigned long mallocsize;
};
#define	HEADERSIZE	sizeof(struct mallochdr)
#define	MPTR2MHDR(p)	((struct mallochdr *)(((unsigned long)(p) & (HEADERSIZE - 1)) ? ((unsigned long)(p) & ~(HEADERSIZE - 1)) : (unsigned long)(p)) -1)

#define	MALLOC_MAGIC	0xbe708063

void *
malloc(size_t size)
{
	void *pagep, *p;
	struct mallochdr *h;
	unsigned long mapsize;

#ifdef	DEBUG
	fprintf(stderr, "malloc %llu\r\n", (unsigned long long)size);
	fflush(stderr);
#endif

#ifdef ALIGN_PTRSIZE
	if ((size & (sizeof(void *) - 1)) != 0)
		size = (size + sizeof(void *) - 1) & ~(sizeof(void *) - 1);
#endif


	mapsize = (unsigned long)PAGE_ALIGN(size + sizeof(struct mallochdr) + PAGE_SIZE);
	pagep = p = mmap(NULL, mapsize, PROT_READ|PROT_WRITE, MAP_ANON, -1, 0);

	if (p == MAP_FAILED) {
		errno = ENOMEM;
		return NULL;
	}

	p = (char *)p + mapsize - PAGE_SIZE;	// p = last page
	mprotect(p, PAGE_SIZE, PROT_NONE);		// set last page read/write protected

	p = (char *)p - size;

	h = MPTR2MHDR(p);

	h->magic = MALLOC_MAGIC;
	h->mmappage = pagep;
	h->mmapsize = mapsize;
	h->mallocsize = size;

#ifdef	DEBUG
	fprintf(stderr, "TRACE: %p: malloc %llu -> %p\r\n", p, (unsigned long long)size, p);
	fflush(stderr);
#endif

	return p;
}

void *
calloc(size_t number, size_t size)
{
	size_t len = number * size;
	void *p;

#ifdef	DEBUG
	fprintf(stderr, "calloc %llu, %llu\r\n", (unsigned long long)number, (unsigned long long)size);
	fflush(stderr);
#endif

	p = malloc(len);
	if (p) {
		memset(p, 0, len);
	}

	return p;
}

void *
realloc(void *ptr, size_t size)
{
	struct mallochdr *h;
	void *p;

	p = malloc(size);
	if (p) {
		if (ptr) {
			h = MPTR2MHDR(ptr);

			if (size > h->mallocsize) {
				memcpy(p, ptr, h->mallocsize);
			} else {
				memcpy(p, ptr, size);
			}
		}
	} else {
		return NULL;
	}

	free(ptr);

	return p;
}

void
free(void *ptr)
{
	struct mallochdr *h;
	if (ptr) {
		h = MPTR2MHDR(ptr);
		munmap(h->mmappage, h->mmapsize);

#ifdef	DEBUG
		fprintf(stderr, "TRACE: %p: free %p\r\n", ptr, ptr);
		fflush(stderr);
#endif
	}
}
