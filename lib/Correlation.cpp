//
//  Correlation.cpp
//
//
//  Created by Kun Zhao on 2015-12-07 15:14:58.
//
//

#include "Correlation.h"
#include <cmath>

namespace
{

float calculateMean( float* parameter, size_t size )
{
	float sum = 0.0;
	for( size_t i = 0; i < size; i++ ) sum += parameter[i];
	return ( sum / size );
}

}

namespace kun
{
	
float Correlation::calculate( float* parameterA, float* parameterB, size_t size )
{
	float meanA = ::calculateMean( parameterA, size );
	float meanB = ::calculateMean( parameterB, size );

	float cov = 0.0;
	float deviationA = 0.0;
	float deviationB = 0.0;
	for( size_t i = 0; i < size; i++ ) 
	{
		float diffA = parameterA[i] - meanA;
		float diffB = parameterB[i] - meanB;
		cov += diffA * diffB;
		deviationA += diffA * diffA;
		deviationB += diffB * diffB;
	}

	return cov / ( std::sqrt( deviationA ) * std::sqrt( deviationB ) ) ;
}

float Correlation::calculateLag(float *parameterA, float *parameterB, size_t size, size_t lag )
{
	float result = calculate( parameterA, parameterB + lag, size - lag );
	return result;
}

} // end of namespace kun