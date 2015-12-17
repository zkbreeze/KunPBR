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

float calculateMean( const kvs::ValueArray<float>& sequences )
{
	float sum = 0.0;
	for( size_t i = 0; i < sequences.size(); i++ ) sum += sequences.data()[i];
	return ( sum / sequences.size() );
}

}

namespace kun
{
	
float Correlation::calculate( const kvs::ValueArray<float>& sequences1, const kvs::ValueArray<float>& sequences2 )
{
	if( sequences1.size() != sequences2.size() )
	{
		std::cerr << "Error to calculate the correlation." << std::endl;
		exit( 0 );
	}

	float meanA = ::calculateMean( sequences1 );
	float meanB = ::calculateMean( sequences2 );

	float cov = 0.0;
	float deviationA = 0.0;
	float deviationB = 0.0;
	unsigned int size = sequences1.size();
	for( size_t i = 0; i < size; i++ ) 
	{
		float diffA = sequences1.data()[i] - meanA;
		float diffB = sequences2.data()[i] - meanB;
		cov += diffA * diffB;
		deviationA += diffA * diffA;
		deviationB += diffB * diffB;
	}

	return cov / ( std::sqrt( deviationA ) * std::sqrt( deviationB ) ) ;
}

float Correlation::calculateLag( const kvs::ValueArray<float>& sequences1, const kvs::ValueArray<float>& sequences2, unsigned int lag )
{
	if( sequences1.size() != sequences2.size() )
	{
		std::cerr << "Error to calculate the correlation." << std::endl;
		exit( 0 );
	}

	kvs::ValueArray<float> lag_seq1( sequences1.data(), sequences1.size() - lag );
	kvs::ValueArray<float> lag_seq2( sequences2.data() + lag, sequences1.size() - lag );

	float result = calculate( lag_seq1, lag_seq2 );
	return result;
}

} // end of namespace kun