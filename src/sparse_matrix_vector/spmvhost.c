/* sparsematvecmult_host.c */
#include <stdio.h>
#include <stdlib.h>
#include <stdcl.h>
#include <time.h>

int main(int argc, char **argv)
{
	int i,j;
	unsigned int n, proc=16, nnz=10;
	int idata, index;
	float fdata;
	char ch;
	FILE *fpa;
	srand(time(NULL));

	if (argc < 5)
	{
		printf("Use as: ./spmv [FILENAME FOR MATRIX] [MATRIX SIZE] [CORES TO USE (1-16)] [NONZEROS]");
		return 0;
	}
	
	n = atoi(argv[2]);
	proc = atoi(argv[3]);
	nnz = atoi(argv[4]);

	clock_t start = clock();
	double t_alloc, t_clalloc, t_calc, t_full;
	
	/* allocate device-shareable memory */
	int 	*ai = (int*)  clmalloc(stdacc,(n+1)*sizeof(int),0);
	int 	*aj = (int*)  clmalloc(stdacc,nnz  *sizeof(int),0);
	float 	*av = (float*)clmalloc(stdacc,nnz  *sizeof(float),0);
	float 	*b  = (float*)clmalloc(stdacc,n    *sizeof(float),0);
	float 	*c  = (float*)clmalloc(stdacc,n    *sizeof(float),0);

	t_alloc = (clock() - start) / (double) CLOCKS_PER_SEC;

	/* setup matrix a and vector b */
	/*
	/* setup matrix a */	
	if ((fpa = fopen(argv[1], "r")) == NULL) {
        fprintf(stdout, "Error: Can't open file!\n");
        return -1;  }

    index=-1;
	while (++index < n+1)
	{
		fscanf(fpa, "%d%c", &idata, &ch);
		ai[index] = idata;
		//printf("%d\n", ai[index]);
	}

	getc(fpa);
	index=-1;
	while (++index < nnz)
	{
		fscanf(fpa, "%d%c", &idata, &ch);
		aj[index] = idata;
		//printf("%d\n", aj[index]);
	}

	getc(fpa);
	index=-1;
	while (++index < nnz)
	{
		fscanf(fpa, "%f%c", &fdata, &ch);
		av[index] = fdata;
		//printf("%f\n", av[index]);
	}
	fclose(fpa);
	printf("Matrix A loaded.\n");

	/* generate b */
	for (index=0; index<n; index++)
	{
		b[index] = 10.0f*(float)rand()/(float)RAND_MAX;
	}
	printf("Vector B generated.\n");

	/* print a and b */
	/*
	printf("\nai: ");
	for(i=0;i<n+1;i++) printf("%i ",ai[i]);
	printf("\naj: ");
	for(i=0;i<nnz;i++) printf("%i ",aj[i]);
	printf("\nav: ");
	for(i=0;i<nnz;i++) printf("%f ",av[i]);

	printf("\nb: ");
	for(i=0;i<nnz;i++) printf("%f ",b[i]);
	*/

	/* setup a blank vector c */
	for (i=0; i<n; i++)
	{
		c[i] = 0.0f;
	}
	
	/* sync data with device memory */
	clmsync(stdacc,0,ai,CL_MEM_DEVICE|CL_EVENT_NOWAIT);
	clmsync(stdacc,0,aj,CL_MEM_DEVICE|CL_EVENT_NOWAIT);
	clmsync(stdacc,0,av,CL_MEM_DEVICE|CL_EVENT_NOWAIT);
	clmsync(stdacc,0,b ,CL_MEM_DEVICE|CL_EVENT_NOWAIT);
	clmsync(stdacc,0,c ,CL_MEM_DEVICE|CL_EVENT_NOWAIT);

	t_clalloc = (clock() - start) / (double) CLOCKS_PER_SEC - t_alloc;

	/* perform calculation */
	printf("\nCalculating...");
	clndrange_t ndr = clndrange_init1d( 0, n, proc );
	clexec(stdacc,0,&ndr,sparsematvecmult_kern,n,proc,ai,aj,av,b,c);
	printf("Done.");

	t_calc = (clock() - start) / (double) CLOCKS_PER_SEC - t_alloc - t_clalloc;
	
	/* sync data with host memory */
	clmsync(stdacc,0,c,CL_MEM_HOST|CL_EVENT_NOWAIT);
	
	/* block until co-processor is done */
	clwait(stdacc,0,CL_ALL_EVENT);

	/* print result */
	//printf("\nc:\n");
	//for(i=0;i<n;i++) printf("%d: %f\n",i,c[i]);

	/* deallocate memory and exit */
	clfree(ai);
	clfree(aj);
	clfree(av);
	clfree(b);
	clfree(c);

	t_full = (clock() - start) / (double) CLOCKS_PER_SEC;

	printf("\n\nt_alloc = %f\nt_clalloc = %f\nt_calc = %f\nt_full = %f\n", t_alloc, t_clalloc, t_calc, t_full);

	return 0;
}