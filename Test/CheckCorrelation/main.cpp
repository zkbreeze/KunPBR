//
//  main.cpp
//
//
//  Created by Kun Zhao on 2015-12-11 15:22:50.
//
//

#include <iostream>
#include <fstream>
#include "Correlation.h"

// Load test csv data
int main( int argc, char** argv )
{
	std::ifstream ifs( argv[1], std::ifstream::in );
	char* buf = new char[256];
	ifs.getline( buf, 256 ); // Skip
	ifs.getline( buf, 256 ); // Skip

	float* test_value_1 = new float[27];
	float* test_value_2 = new float[27];

	for( size_t i = 0; i < 27; i++ )
	{
		ifs.getline( buf, 256 );	
		std::sscanf( buf, "%f,%f", &test_value_1[i], &test_value_2[i] );
	}

	float correlation =	kun::Correlation::calculate( test_value_1, test_value_2, 27 );
	std::cout << correlation << std::endl;
}