/* sparsematvecmult_kern.cl */
#include <stdcl.h>

void sparsematvecmult_kern(
	unsigned int n, 
	unsigned int proc,
	int *ai,
	int *aj,
	float *av,
	float *b,
	float *c
	)
{
	// MODEL: c(i) = A(i,:) * b(:), for all nonzero values
	
	// Allocate variables we need
	int i,j,k,astart,aend;
	float tmp;

	// We are the ith process (OpenCL handles this)
	int procindex = get_global_id(0);

	// Which lines will we use?
	int istart = procindex*(n/proc),
		iend = (procindex+1)*(n/proc);

	// We can't do more than the matrix
	if (iend > n)
		iend = n;

	// Loop over all assigned lines
	for (i=istart; i<iend; i++)
	{
		tmp = 0.0f;

		// Where does this line start and end?
		astart = ai[i];
		aend = ai[i+1];

		// For all elements on this row...
		// note: if astart==aend the line is empty and this loop is never run
		for (j=astart; j<aend; j++)
		{
			// Get the column
			k = aj[j];

			// Add to sum (A(i,k) * b(k))
			tmp += av[j] * b[k];
		}

		// Update to global memory
		c[i] = tmp;
	}	
}