#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>


struct mallochdr {
	unsigned long	magic;
	void *			mmappage;
	unsigned long	mmapsize;
	unsigned long	mallocsize;
};
#define	MPTR2MHDR(p)	((struct mallochdr *)(((unsigned long)(p) & 7) ? ((unsigned long)(p) & ~7): (unsigned long)(p))-1)

int
main(argc,argv)
	int argc;
	char *argv[];
{
	unsigned char *p;
	int i;


	for (i=0;i<10;i++) {
		p = malloc(1024+i);
		printf("p=%p\n",p);
	}

	p = malloc(1024);

	{
		struct mallochdr *h;
		h = MPTR2MHDR(p);

		printf("magic = 0x%08x\n",h->magic);
		printf("page  = 0x%08x\n",h->mmappage);
		printf("psize = 0x%08x\n",h->mmapsize);
		printf("msize = 0x%08x\n",h->mallocsize);
	}

	printf("%02x\n",p[1024]);

	free(p);

	printf("Occur SEGV?\n");
	printf("%02x\n",p[0]);


}

