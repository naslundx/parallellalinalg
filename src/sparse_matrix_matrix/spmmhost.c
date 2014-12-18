#include <stdio.h>
#include <stdlib.h>
#include <stdcl.h>
#include <time.h>

int main(int argc, char **argv)
{
	/* some variables we need */
	int i,j;
	unsigned int n, proc=16, nnz=10;
	int idata, index;
	float fdata;
	char ch;
	FILE *fpa, *fpb;

	/* read arguments */
	if (argc < 6)
	{
		printf("Use: ./spmm [FILENAME MATRIX A] [FILENAME MATRIX B] [MATRIX SIZE] [CORES TO USE (1-16)] [NONZERO ELEMENTS]");
		return 0;
	}
	
	n = atoi(argv[3]);
	proc = atoi(argv[4]);
	nnz = atoi(argv[5]);

	clock_t start = clock();
	double t_alloc, t_clalloc, t_calc, t_full;
	
	/* allocate device-shareable memory */
	int 	*ai = (int*)  clmalloc(stdacc,(n+1)*sizeof(int),0);
	int 	*bi = (int*)  clmalloc(stdacc,(n+1)*sizeof(int),0);
	int 	*ci = (int*)  clmalloc(stdacc,(n+1)*sizeof(int),0);
	int 	*aj = (int*)  clmalloc(stdacc,nnz  *sizeof(int),0);
	float 	*av = (float*)clmalloc(stdacc,nnz  *sizeof(float),0);
	int 	*bj = (int*)  clmalloc(stdacc,nnz  *sizeof(int),0);
	float 	*bv = (float*)clmalloc(stdacc,nnz  *sizeof(float),0);
	int 	*cj = (int*)  clmalloc(stdacc,2*nnz*sizeof(int),0);
	float 	*cv = (float*)clmalloc(stdacc,2*nnz*sizeof(float),0);

	t_alloc = (clock() - start) / (double) CLOCKS_PER_SEC;

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

	/* setup matrix b */
	if ((fpb = fopen(argv[2], "r")) == NULL) {
        fprintf(stdout, "Error: Can't open file!\n");
        return -1;  }

    index=-1;
	while (++index < n+1)
	{
		fscanf(fpb, "%d%c", &idata, &ch);
		bi[index] = idata;
		//printf("%d\n", bi[index]);
	}

	getc(fpb);
	index=-1;
	while (++index < nnz)
	{
		fscanf(fpb, "%d%c", &idata, &ch);
		bj[index] = idata;
		//printf("%d\n", bj[index]);
	}

	getc(fpb);
	index=-1;
	while (++index < nnz)
	{
		fscanf(fpb, "%f%c", &fdata, &ch);
		bv[index] = fdata;
		//printf("%f\n", bv[index]);
	}
	fclose(fpb);
	printf("Matrix B loaded.\n");

	/* print a and b */
	/*for (i=0; i<n+1; i++)
	{
		ai[i] = i;
		bi[i] = i;
		if (i<n)
		{
			aj[i] = i;
			bj[i] = i;
			av[i] = (float)i;
			bv[i] = (float)i;
		}
	}*/

	/*
	printf("\nai: ");
	for(i=0;i<n+1;i++) printf("%i ",ai[i]);
	printf("\naj: ");
	for(i=0;i<nnz;i++) printf("%i ",aj[i]);
	printf("\nav: ");
	for(i=0;i<nnz;i++) printf("%.0f ",av[i]);

	printf("\nbi: ");
	for(i=0;i<n+1;i++) printf("%i ",bi[i]);
	printf("\nbj: ");
	for(i=0;i<nnz;i++) printf("%i ",bj[i]);
	printf("\nbv: ");
	for(i=0;i<nnz;i++) printf("%.0f ",bv[i]);
	*/

	/* setup a blank matrix c */
	for (i=0; i<n+1; i++)
	{
		ci[i] = ai[i]*2;
	}
	for (i=0; i<2*nnz; i++)
	{
		cj[i] = 0;
		cv[i] = 0.0f;
	}
	
	/* sync data with device memory */
	clmsync(stdacc,0,ai,CL_MEM_DEVICE|CL_EVENT_NOWAIT);
	clmsync(stdacc,0,aj,CL_MEM_DEVICE|CL_EVENT_NOWAIT);
	clmsync(stdacc,0,av,CL_MEM_DEVICE|CL_EVENT_NOWAIT);
	clmsync(stdacc,0,bi,CL_MEM_DEVICE|CL_EVENT_NOWAIT);
	clmsync(stdacc,0,bj,CL_MEM_DEVICE|CL_EVENT_NOWAIT);
	clmsync(stdacc,0,bv,CL_MEM_DEVICE|CL_EVENT_NOWAIT);
	clmsync(stdacc,0,ci,CL_MEM_DEVICE|CL_EVENT_NOWAIT);
	clmsync(stdacc,0,cj,CL_MEM_DEVICE|CL_EVENT_NOWAIT);
	clmsync(stdacc,0,cv,CL_MEM_DEVICE|CL_EVENT_NOWAIT);

	t_clalloc = (clock() - start) / (double) CLOCKS_PER_SEC - t_alloc;
	
	/* perform calculation */
	printf("Calculating...");
	clndrange_t ndr = clndrange_init1d( 0, n, proc );
	clexec(stdacc,0,&ndr,spmmkernel,n,proc,ai,aj,av,bi,bj,bv,ci,cj,cv);
	printf("Done.\n");

	t_calc = (clock() - start) / (double) CLOCKS_PER_SEC - t_alloc - t_clalloc;
	
	/* sync data with host memory */
	clmsync(stdacc,0,ci,CL_MEM_HOST|CL_EVENT_NOWAIT);
	clmsync(stdacc,0,cj,CL_MEM_HOST|CL_EVENT_NOWAIT);
	clmsync(stdacc,0,cv,CL_MEM_HOST|CL_EVENT_NOWAIT);
	
	/* block until co-processor is done */
	clwait(stdacc,0,CL_ALL_EVENT);

	/* print result */
	/*printf("\nci: ");
	for(i=0;i<n+1;i++) printf("%i ",ci[i]);
	printf("\ncj: ");
	for(i=0;i<2*nnz;i++) printf("%i ",cj[i]);
	printf("\ncv: ");
	for(i=0;i<2*nnz;i++) printf("%.0f ",cv[i]);
	*/

	/* deallocate memory and exit */
	clfree(ai);
	clfree(aj);
	clfree(av);
	clfree(bi);
	clfree(bj);
	clfree(bv);
	clfree(ci);
	clfree(cj);
	clfree(cv);

	t_full = (clock() - start) / (double) CLOCKS_PER_SEC;

	printf("\n\nt_alloc = %f\nt_clalloc = %f\nt_calc = %f\nt_full = %f\n", t_alloc, t_clalloc, t_calc, t_full);

	return 0;
}