//
//  CloudObject.h
//
//
//  Created by Kun Zhao on 2015-10-20 19:06:57.
//
//

#ifndef KUN__CLOUDOBJECT_H_INCLUDE
#define KUN__CLOUDOBJECT_H_INCLUDE
 
#include <string>
#include <iostream>
#include <fstream>
#include "PointObject.h"

namespace kun
{
	
class CloudObject
{

private:
	int m_num;
	int m_numasl;
	double* m_x;
	double* m_y;
	double* m_z;
	double* m_vz;
	double* m_r;
	double* m_asl;
	long* m_n;

public:
	CloudObject();
	CloudObject( std::string filename );
	CloudObject( std::string filename, size_t pe ); // pe is the MPI process number

	bool read( std::string filename );
	bool read( std::string filename, size_t pe );

	int numberOfPoints(){ return m_num; }

	kun::PointObject* toKUNPointObject( int Parameter_ID );
};

} // end of namespace kun
 
#endif // KUN__CLOUDOBJECT_H_INCLUDE