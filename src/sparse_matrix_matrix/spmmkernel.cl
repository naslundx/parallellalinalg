#include <stdcl.h>

void spmmkernel(
	unsigned int n, 
	unsigned int proc,
	int *ai,
	int *aj,
	float *av,
	int *bi,
	int *bj,
	float *bv,
	int *ci,
	int *cj,
	float *cv
	)
{
	// MODEL:
	// C(i,:) = C(i,:) + A(i,k) * B(k,:)

	// The sparse matrices are stored in SRW format with ai, aj, av for matrix A
	// and bi, bj, bv for matrix B
	// and ci, cj, cv for matrix C

	// The matrices are of size n x n.
	// There are proc Epiphany cores running.

	// Allocate variables we need
	const int BUFSIZE = 10;
	int bufcj[BUFSIZE];
	float bufcv[BUFSIZE];
	int i,j,k,m,jindex,kindex,astart,aend,cmin,cmax,clength,cindex,q,thisproc;
	int bstart,bend;
	float avalue, bvalue;
	float tmp = 0.0f;

	// This is process i (OpenCL handles this)
	thisproc = get_global_id(0);

	// Which rows should be taken care of?
	int istart = thisproc*(n/proc),
		iend = (thisproc+1)*(n/proc);

	// We can't do more than the matrix itself
	if (iend > n)
	{
		iend = n;
	}

	// Loop over all rows in this interval
	for (i=istart; i<iend; i++)
	{
		// Where does this line start and end?
		astart = ai[i];
		aend = ai[i+1];

		// We update C on row i, so we need:
		cmin = ci[i];
		cmax = ci[i+1];
		clength = 0; // Use clength to keep track of how many nonzeros there are on this row
		cindex;

		// If length of C is greater than BUFSIZE, crash
		if (cmax-cmin >= BUFSIZE)
		{
			//TODO: This should crash
			break;
		}
		else
		{
			// Clean the mem buffer
			for (q=0; q<BUFSIZE; q++)
			{
				bufcj[q] = 0;
				bufcv[q] = 0.0f;
			}
		}
		
		// for k=0:(n-1)
		// Note that if astart==aend, the row is empty, and this loop never runs
		for(kindex=astart; kindex<aend; kindex++) 
		{
			// Which element of A?
			k = aj[kindex];
			avalue = av[kindex];

			// We are on the k:th row of B (and A(i,k) is nonzero)
			bstart = bi[k];
			bend = bi[k+1];

			// for all elements in b(k,:)...
			for (jindex=bstart; jindex<bend; jindex++)
			{
				// Have we used too much space in this row of C?
				if (clength >= cmax-cmin)
				{
					// TODO: This should crash
					break;
				}

				// Get current value in b(k,:)
				j = bj[jindex];
				bvalue = bv[j];

				// Scan the current row of C to see if this column is there
				// note that if cmin==clength, this loop never runs
				cindex = -1;
				for (m=0; m<clength; m++)
				{
					if (bufcj[m] == jindex)
					{
						// Yes, there is an entry for C(i,j)
						cindex = m;
						break;
					}
				}

				if (cindex == -1)
				{
					// Set this value
					bufcj[clength] = jindex;
					bufcv[clength] = avalue * bvalue;

					// Does not exist, add another entry
					clength++;
				}
				else
				{
					// Exists, update with the new value
					bufcv[cindex] += avalue * bvalue;
				}
			}
		}

		// Move data from mem buffer to shared memory
		for (q=0; q<clength; q++)
		{
			cj[cmin+q] = bufcj[q];
			cv[cmin+q] = bufcv[q];
		}
	}
}