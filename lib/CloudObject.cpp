//
//  CloudObject.cpp
//
//
//  Created by Kun Zhao on 2015-10-20 19:07:09.
//
//

#include "CloudObject.h"
#include <kvs/ValueArray>
#include <kvs/Endian>

namespace kun
{
	
CloudObject::CloudObject( std::string filename )
{
	this->read( filename );
}

CloudObject::CloudObject( std::string filename, size_t pe )
{
	this->read( filename, pe );
}

bool CloudObject::read( std::string filename )
{
	std::ifstream file( filename.c_str(), std::ifstream::binary );
	if( file.fail() )
	{
		std::cerr << "Cannot read the file." << std::endl;
		return false;
	}

	file.seekg( 4 ); // Fortran header
	file.read( (char*)&m_num, sizeof(int) ); m_num = 87825; // The particle number in the data is wrong, so the number is set by hand.
	std::cout << "Particle number: " << m_num << std::endl;
	file.read( (char*)&m_numasl, sizeof(int) );
	std::cout << "ASL number: " << m_numasl << std::endl;

	m_x = new double[m_num];
	m_y = new double[m_num];
	m_z = new double[m_num];
	m_vz = new double[m_num];
	m_r = new double[m_num];
	m_asl = new double[m_num * m_numasl];
	m_n = new long[m_num];

	file.read( (char*)m_x, sizeof(double) * m_num );
	file.read( (char*)m_y, sizeof(double) * m_num );
	file.read( (char*)m_z, sizeof(double) * m_num );
	file.read( (char*)m_vz, sizeof(double) * m_num );
	file.read( (char*)m_r, sizeof(double) * m_num );
	file.read( (char*)m_asl, sizeof(double) * m_num * m_numasl );
	file.read( (char*)m_n, sizeof(long) * m_num );

	file.close();

	std::cout << filename << " file is load successfully." << std::endl;
	return true;
}

bool CloudObject::read( std::string filename, size_t pe )
{
	std::ifstream* file = new std::ifstream[pe];
	m_num = 0;
	int total_num_asl = 0;
	int* num = new int[pe];
	int* num_asl = new int[pe];

	for( int i = 0; i < pe; i++ )
	{
		char* buf = new char[10];
		sprintf( buf , "%02d", i );
		std::string file_name = filename + "pe00" + buf + ".bin";

		file[i].open( file_name.c_str(), std::ifstream::binary );
		if( file[i].fail() )
		{
			std::cout << "Cannot read the file " << file_name << std::endl;
			return false;
		}

		file[i].seekg( 4 ); // Fortran header
		file[i].read( (char*)&num[i], sizeof(int) ); num[i] = 87825; // The particle number in the data is wrong, so the number is set by hand.
		file[i].read( (char*)&num_asl[i], sizeof(int) );
		m_num += num[i];
		total_num_asl += num[i] * num_asl[i];
	}

	m_x = new double[m_num];
	m_y = new double[m_num];
	m_z = new double[m_num];
	m_vz = new double[m_num];
	m_r = new double[m_num];
	m_asl = new double[total_num_asl];
	m_n = new long[m_num];

	int index = 0;
	int index_asl = 0;
	for( size_t i = 0; i < pe; i++ )
	{
		file[i].read( (char*)(m_x + index), sizeof(double) * num[i] );
		file[i].read( (char*)(m_y + index), sizeof(double) * num[i] );
		file[i].read( (char*)(m_z + index), sizeof(double) * num[i] );
		file[i].read( (char*)(m_vz + index), sizeof(double) * num[i] );
		file[i].read( (char*)(m_r + index), sizeof(double) * num[i] );
		file[i].read( (char*)(m_asl + index_asl), sizeof(double) * num[i] * num_asl[i] );
		file[i].read( (char*)(m_n + index), sizeof(long) * num[i] );

		index += num[i];
		index_asl += num[i] * num_asl[i];
	}

	std::cout << " Files are load successfully." << std::endl;
	return true;
}

kun::PointObject* CloudObject::toKUNPointObject( int Parameter_ID )
{
	kun::PointObject* point = new kun::PointObject();
	float* coord_buffer = new float[m_num * 3];
	float* size_buffer = new float[m_num];
	float* value_buffer = new float[m_num];

	for( size_t i = 0; i < m_num; i++ )
	{
		coord_buffer[i * 3] = (float)m_x[i];
		coord_buffer[i * 3 + 1] = (float)m_y[i];
		coord_buffer[i * 3 + 2] = (float)m_z[i];

		if( 1.0e-06 >= m_r[i] && m_r[i] >= 50.0e-06  )
		{
			float buf = (float)m_r[i] * 1.0e06;
			size_buffer[i] = std::sqrt( buf );
		}
		else
			size_buffer[i] = 0.0;
	
		// size_buffer[i] = (float)m_r[i] + 0.1;

		switch ( Parameter_ID ) 
		{
			case 0: value_buffer[i] = (float)m_r[i]; break;
			case 1: value_buffer[i] = (float)m_vz[i]; break;
			case 2: value_buffer[i] = (float)m_n[i]; break;
			case 3: value_buffer[i] = (float)m_r[i] * m_r[i] * m_n[i]; break;
		}
	}

	kvs::ValueArray<kvs::Real32> coords( coord_buffer, m_num * 3 );
	kvs::ValueArray<kvs::Real32> sizes( size_buffer, m_num );
	kvs::ValueArray<kvs::Real32> values( value_buffer, m_num );

	point->setVeclen( 1 );
	point->setCoords( coords );
	point->setSizes( sizes );
	point->setValues( values );
	point->updateMinMaxCoords();
	point->updateMinMaxValues();

	return point;
}

} // end of namespace kun