/*
 *   Statistics routines.
 */

void calculateStatistics( int length , double results[] , 
                          double* min , double* max , double* average , double* median )
{
	int i;
	BOOLEAN f;
	double temp;
	
	*min = results[0];
	*max = results[0];
	for ( i=0; i<length; i++ )
	{
		if ( results[i] < *min )
		{
			*min = results[i];
		}
		
		if ( results[i] > *max )
		{
			*max = results[i];
		}
	}
	
	for ( i=0; i<length; i++ )
	{
		*average += results[i];
	}
	*average /= length;
	
	i = 0;
	do {
	f = FALSE;
		for ( i=0; i<(length-1); i++ )
		{
			if ( results[i] > results[i+1] )
			{
			temp = results[i];
			results[i] = results[i+1];
			results[i+1] = temp;
			f = TRUE;
			}
		}
	} while (f);
	
	int j = length / 2;
	if ( (length %2) == 0 )
	{
		*median = ( results[j-1] + results[j] ) / 2.0;  // average of middle pair
	}
	else
	{
		*median = results[j];  // middle or single element
	}
	
	
}

