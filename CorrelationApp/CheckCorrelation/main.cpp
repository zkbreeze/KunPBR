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

	kvs::ValueArray<float> sequences1;
	sequences1.allocate( 27 );
	kvs::ValueArray<float> sequences2;
	sequences2.allocate( 27 );

	for( size_t i = 0; i < 27; i++ )
	{
		ifs.getline( buf, 256 );	
		std::sscanf( buf, "%f,%f", &sequences1.data()[i], &sequences2.data()[i] );
		std::cout << sequences1.data()[i] << ", " << sequences2.data()[i] <<std::endl;
	}

	float correlation =	kun::Correlation::calculate( sequences1, sequences2 );
	std::cout << correlation << std::endl;
}