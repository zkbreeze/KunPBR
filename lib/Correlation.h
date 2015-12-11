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

namespace kun
{

class Correlation
{

public:
	Correlation();
	// Correlation( float* parameterA, float* parameterB, size_t size );
	~Correlation();

	static float calculate( float* parameterA, float* parameterB, size_t size );
	static float calculateLag( float* parameterA, float* parameterB, size_t size, size_t lag );
};
	
} // end of namespace kun
 
#endif // KUN__CORRELATION_H_INCLUDE