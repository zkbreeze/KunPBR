//
//  Correlation.h
//
//
//  Created by Kun Zhao on 2015-12-07 15:08:48.
//
//

#ifndef KUN__CORRELATION_H_INCLUDE
#define KUN__CORRELATION_H_INCLUDE
 
#include <iostream>
#include <kvs/ValueArray>

namespace kun
{

class Correlation
{

public:
	Correlation();
	// Correlation( float* parameterA, float* parameterB, size_t size );
	~Correlation();

	static float calculate( const kvs::ValueArray<float>& sequences1, const kvs::ValueArray<float>& sequences2 );
	static float calculateLag( const kvs::ValueArray<float>& sequences1, const kvs::ValueArray<float>& sequences2, unsigned int lag );
	static std::vector<kvs::ValueArray<float> > arraysToSequences( const std::vector<kvs::ValueArray<float> >& arrays );
};
	
} // end of namespace kun
 
#endif // KUN__CORRELATION_H_INCLUDE