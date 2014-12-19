#include <stdcl.h>

void dmvkernel( unsigned int proc, unsigned int n, float* aa, float* b, float* c )
{
	int i,j;
	float tmp;
	int index = get_global_id(0);
	int imin = index*(n/proc),
		imax = (index+1)*(n/proc);

	// We can't do more than the matrix
	if (imax > n)
		imax = n;

	// Run the basic algorithm for dense matrix-vector multiplication over each row
	for(i = imin; i < imax; i++)
	{
		tmp = 0.0f;

		for(j=0; j<n; j++) 
		{
			tmp += aa[i*n+j] * b[j];
		}

		c[i] = tmp;
	}
}