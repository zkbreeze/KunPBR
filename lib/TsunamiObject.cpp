//
//  TsunamiObject.cpp
//
//
//  Created by Kun Zhao on 2014-10-01 18:04:58.
//
//

#include "TsunamiObject.h"
namespace kun
{

	TsunamiObject::TsunamiObject( void )
	{
	}

	TsunamiObject::TsunamiObject( std::string filename )
	{
		this->read( filename );
	}

	void TsunamiObject::read( std::string filename )
	{
		std::ifstream infile( filename.c_str() );
		char* temp = new char[100];
		infile >> temp;
		infile >> m_size;
		std::cout << m_size << std::endl;

		m_x = new float[m_size];
		m_y = new float[m_size];
		m_z = new float[m_size];
		m_u = new float[m_size];
		m_v = new float[m_size];
		m_w = new float[m_size];
		m_depth = new float[m_size];
		m_eddy_x = new float[m_size];
		m_eddy_y = new float[m_size];
		m_eddy_z = new float[m_size];

		for (size_t i = 0 ; i < m_size; i++)
		{
			infile >> m_x[i];
			infile >> m_y[i];
			infile >> m_z[i];
			infile >> m_u[i];
			infile >> m_v[i];
			infile >> m_w[i];
			infile >> m_depth[i];
			infile >> m_eddy_x[i];
			infile >> m_eddy_y[i];
			infile >> m_eddy_z[i];
		}
	}

	kun::PointObject* TsunamiObject::toKUNPointObject( size_t Parameter_ID )
	{
		kun::PointObject* point = new kun::PointObject();

		float* coords_buffer = new float[m_size * 3];
		float* values_buffer = new float[m_size];

		for( size_t i = 0; i < m_size; i++ )
		{   
			// Absolute value
			if( Parameter_ID == 1 )
			{
				values_buffer[i] = std::sqrt( m_u[i] * m_u[i] + m_v[i] * m_v[i] + m_w[i] * m_w[i] );
			}
			if( Parameter_ID == 2 )
			{
				values_buffer[i] = m_depth[i];
			}
			if( Parameter_ID == 3 )
			{
				values_buffer[i] = std::sqrt( m_eddy_x[i] * m_eddy_x[i] + m_eddy_y[i] * m_eddy_y[i] + m_eddy_z[i] * m_eddy_z[i] );
			}

			coords_buffer[i * 3] = m_x[i];
			coords_buffer[i * 3 + 1] = m_y[i];
			coords_buffer[i * 3 + 2] = m_z[i];
		}

		kvs::ValueArray<kvs::Real32>coords( coords_buffer, m_size * 3 );
		kvs::ValueArray<kvs::Real32>values( values_buffer, m_size );

		point->setVeclen( 1 );
		point->setCoords( coords );
		point->setValues( values );
		point->updateMinMaxValues();
		point->updateMinMaxCoords();

		const float* coord = point->coord().data();
		std::cout << coord[3] << std::endl;

		return point;
	}

	kvs::PointObject* TsunamiObject::toKVSPointObject( void )
	{
		kvs::PointObject* point = new kvs::PointObject();

		// float* coords_buffer = new float[m_size * 3];
		// unsigned char* colors_buffer = new unsigned char[m_size * 3];

		// for( size_t i = 0; i < m_size; i++ )
		// {   
		// 	coords_buffer[i * 3] = m_x[i];
		// 	coords_buffer[i * 3 + 1] = m_y[i];
		// 	coords_buffer[i * 3 + 2] = 0.0;

		// 	colors_buffer[i * 3] = 100;
		// 	colors_buffer[i * 3 + 1] = 100;
		// 	colors_buffer[i * 3 + 2] = 100;
		// }

		// kvs::ValueArray<kvs::Real32>coords( coords_buffer, m_size * 3 );
		// kvs::ValueArray<kvs::UInt8>colors( colors_buffer, m_size * 3 );

		// point->setCoords( coords );
		// point->setColors( colors );

		return point;
	}
} // end of namespace kun
