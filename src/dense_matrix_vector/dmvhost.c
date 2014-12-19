#include <stdio.h>
#include <stdlib.h>
#include <stdcl.h>
#include <time.h>

inline int parity( int x ) { return ( (x%2)? +1 : -1 ); }

int main(int argc, char **argv)
{
	int i,j;
	unsigned int n, proc;

	if (argc < 3)
	{
		printf("Use: ./dmv [MATRIX SIZE] [CORES TO USE (1-16)");
		return 0;
	}

	n = atoi(argv[1]);
	proc = atoi(argv[2]);

	clock_t start;
	double t_alloc, t_clalloc, t_calc, t_full;
	start = clock();
	
	/* allocate device-shareable memory */
	float* aa = (float*)clmalloc(stdacc,n*n*sizeof(float),0);
	float* b = (float*)clmalloc(stdacc,n*sizeof(float),0);
	float* c = (float*)clmalloc(stdacc,n*sizeof(float),0);

	t_alloc = (clock() - start) / (double)CLOCKS_PER_SEC;
	
	/* initialize matrix aa[] and vector b[], and zero result vector c[] */
	for(i=0; i<n; i++) for(j=0; j<n; j++) aa[i*n+j] = (1.0/n/n)*i*j*parity(i*j);
	printf("Matrix A generated.\n");
	for(i=0; i<n; i++) b[i] = (1.0/n)*i*parity(i);
	printf("Vector B generated.\n");
	for(i=0; i<n; i++) c[i] = 0.0f;
	
	/* sync data with device memory */
	clmsync(stdacc,0,aa,CL_MEM_DEVICE|CL_EVENT_NOWAIT);
	clmsync(stdacc,0,b,CL_MEM_DEVICE|CL_EVENT_NOWAIT);
	clmsync(stdacc,0,c,CL_MEM_DEVICE|CL_EVENT_NOWAIT);

	t_clalloc = (clock() - start) / (double)CLOCKS_PER_SEC - t_alloc;
	
	/* perform calculation */
	clndrange_t ndr = clndrange_init1d( 0, proc, proc );
	clexec(stdacc,0,&ndr,dmvkernel,proc,n,aa,b,c);

	/* sync data with host memory */
	clmsync(stdacc,0,c,CL_MEM_HOST|CL_EVENT_NOWAIT);
	
	/* block until co-processor is done */
	clwait(stdacc,0,CL_ALL_EVENT);

	t_calc = (clock() - start) / (double)CLOCKS_PER_SEC - t_alloc - t_clalloc;

	/*for(i=0;i<n;i++) 
		printf("%d %f %f\n",i,b[i],c[i]);*/
	
	clfree(aa);
	clfree(b);
	clfree(c);

	t_full = (clock() - start) / (double)CLOCKS_PER_SEC;

	printf("\nt_alloc = %f\nt_clalloc = %f\nt_calc=%f\nt_full=%f", t_alloc, t_clalloc, t_calc, t_full);

	return 0;
}