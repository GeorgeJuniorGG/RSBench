#include "rsbench.h"

int * generate_n_poles( Input input, uint64_t * seed )
{
	int total_resonances = input.avg_n_poles * input.n_nuclides;

	int * R = (int *) malloc( input.n_nuclides * sizeof(int));
	
	// Ensure all nuclides have at least 1 resonance
	for( int i = 0; i < input.n_nuclides; i++ )
		R[i] = 1;

	// Sample the rest
	for( int i = 0; i < total_resonances - input.n_nuclides; i++ )
		R[LCG_random_int(seed) % input.n_nuclides]++;
	
	/* Debug	
	for( int i = 0; i < input.n_nuclides; i++ )
		printf("R[%d] = %d\n", i, R[i]);
	*/
	
	return R;
}

int * generate_n_windows( Input input, uint64_t * seed )
{
	int total_resonances = input.avg_n_windows * input.n_nuclides;

	int * R = (int *) malloc( input.n_nuclides * sizeof(int));
	
	// Ensure all nuclides have at least 1 resonance
	for( int i = 0; i < input.n_nuclides; i++ )
		R[i] = 1;

	// Sample the rest
	for( int i = 0; i < total_resonances - input.n_nuclides; i++ )
		R[LCG_random_int(seed) % input.n_nuclides]++;
	
	/* Debug	
	for( int i = 0; i < input.n_nuclides; i++ )
		printf("R[%d] = %d\n", i, R[i]);
	*/
	
	return R;
}

Pole * generate_poles( Input input, int * n_poles, uint64_t * seed, int * max_num_poles )
{
	// Pole Scaling Factor -- Used to bias hitting of the fast Faddeeva
	// region to approximately 99.5% (i.e., only 0.5% of lookups should
	// require the full eval).
	double f = 152.5;
	RSComplex f_c = {f, 0};

	int max_poles = -1;
	
	for( int i = 0; i < input.n_nuclides; i++ )
	{
		if( n_poles[i] > max_poles)
			max_poles = n_poles[i];
	}
	*max_num_poles = max_poles;

	// Allocating 2D matrix as a 1D contiguous vector
	Pole * R = (Pole *) malloc( input.n_nuclides * max_poles * sizeof(Pole));
	
	// fill with data
	for( int i = 0; i < input.n_nuclides; i++ )
		for( int j = 0; j < n_poles[i]; j++ )
		{
			double r = LCG_random_double(seed);
			double im = LCG_random_double(seed);
			RSComplex t1 = {r, im};
			R[i * max_poles + j].MP_EA = c_mul(f_c,t1);
			r = LCG_random_double(seed);
			im = LCG_random_double(seed);
			RSComplex t2 = {f*r, im};
			R[i * max_poles + j].MP_RT = t2;
			r = LCG_random_double(seed);
			im = LCG_random_double(seed);
			RSComplex t3 = {f*r, im};
			R[i * max_poles + j].MP_RA = t3;
			r = LCG_random_double(seed);
			im = LCG_random_double(seed);
			RSComplex t4 = {f*r, im};
			R[i * max_poles + j].MP_RF = t4;
			R[i * max_poles + j].l_value = LCG_random_int(seed) % input.numL;
		}
	
	/* Debug
	for( int i = 0; i < input.n_nuclides; i++ )
		for( int j = 0; j < n_poles[i]; j++ )
			printf("R[%d][%d]: Eo = %lf lambda_o = %lf Tn = %lf Tg = %lf Tf = %lf\n", i, j, R[i * max_poles + j].Eo, R[i * max_poles + j].lambda_o, R[i * max_poles + j].Tn, R[i * max_poles + j].Tg, R[i * max_poles + j].Tf);
	*/

	return R;
}

Window ** generate_window_params( Input input, int * n_windows, int * n_poles, uint64_t * seed )
{
	// Allocating 2D contiguous matrix
	Window ** R = (Window **) malloc( input.n_nuclides * sizeof( Window *));
	Window * contiguous = (Window *) malloc( input.n_nuclides * input.avg_n_windows * sizeof(Window));

	int k = 0;
	for( int i = 0; i < input.n_nuclides; i++ )
	{
		R[i] = &contiguous[k];
		k += n_windows[i];
	}
	
	// fill with data
	for( int i = 0; i < input.n_nuclides; i++ )
	{
		int space = n_poles[i] / n_windows[i];
		int remainder = n_poles[i] - space * n_windows[i];
		int ctr = 0;
		for( int j = 0; j < n_windows[i]; j++ )
		{
			R[i][j].T = LCG_random_double(seed);
			R[i][j].A = LCG_random_double(seed);
			R[i][j].F = LCG_random_double(seed);
			R[i][j].start = ctr; 
			R[i][j].end = ctr + space - 1;

			ctr += space;

			if ( j < remainder )
			{
				ctr++;
				R[i][j].end++;
			}
		}
	}

	return R;
}

double ** generate_pseudo_K0RS( Input input, uint64_t * seed )
{
	double ** R = (double **) malloc( input.n_nuclides * sizeof( double * ));
	double * contiguous = (double *) malloc( input.n_nuclides * input.numL * sizeof(double));

	for( int i = 0; i < input.n_nuclides; i++ )
		R[i] = &contiguous[i*input.numL];

	for( int i = 0; i < input.n_nuclides; i++)
		for( int j = 0; j < input.numL; j++ )
			R[i][j] = LCG_random_double(seed);

	return R;
}
