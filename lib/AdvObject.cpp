//
//  AdvObject.cpp
//
//
//  Created by Kun Zhao on 2015-11-17 17:32:22.
//
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <string.h>
#include "AdvObject.h"

namespace kun
{
	
AdvObject::AdvObject( std::string filename )
{
	this->read( filename );
}

void AdvObject::read( std::string filename )
{
	AdvDocFile *advfile;
	AdvDocument *doc;
	adv_off_t  off;
	float* val = new float[3];
	int a,i;

	std::cout << "Read file:" << filename << std::endl;

	advfile = adv_dio_file_open( filename.c_str(), "r" );
	doc = adv_dio_open_by_property( advfile,NULL,"label","Position",NULL);
	adv_dio_get_property_int32(doc,"num_items", &a);
	m_number = (int)a;
	std::cout << "Number of particles: " << m_number << std::endl;
	adv_dio_close(doc);
	adv_dio_file_close(advfile);

	m_x = new float[m_number];
	m_y = new float[m_number];
	m_z = new float[m_number];
	m_u = new float[m_number];
	m_v = new float[m_number];
	m_w = new float[m_number];
	m_type = new int[m_number];

	advfile = adv_dio_file_open( filename.c_str(), "r" );
	doc = adv_dio_open_by_property( advfile, NULL, "label", "Position", NULL );
	off = 0;

	for( i = 0; i < m_number; i++ )
	{
		off +=  adv_dio_read_float32(doc, off, &val[0]);
		off +=  adv_dio_read_float32(doc, off, &val[1]);
		off +=  adv_dio_read_float32(doc, off, &val[2]);
		m_x[i] = (float)val[0];
		m_y[i] = (float)val[1];
		m_z[i] = (float)val[2];

		if( std::isnan( (float)m_x[i] ) ) std::cout << "x NAN value: " << i << " " << m_x[i] << std::endl;
		if( std::isnan( (float)m_y[i] ) ) std::cout << "y NAN value: " << i << " " << m_y[i] << std::endl;
		if( std::isnan( (float)m_z[i] ) ) std::cout << "z NAN value: " << i << " " << m_z[i] << std::endl;

	}

	adv_dio_close(doc);

	doc = adv_dio_open_by_property( advfile, NULL, "label", "Velocity", NULL );
	off = 0;
	for( i = 0; i < m_number; i++ )
	{
		off +=  adv_dio_read_float32(doc, off, &val[0]);
		off +=  adv_dio_read_float32(doc, off, &val[1]);
		off +=  adv_dio_read_float32(doc, off, &val[2]);
		m_u[i] = (float)val[0];
		m_v[i] = (float)val[1];
		m_w[i] = (float)val[2];
	}

	adv_dio_close(doc);

	doc = adv_dio_open_by_property( advfile, NULL, "label", "ParticleType", NULL );
	off = 0;
	for( i = 0; i < m_number; i++ )
	{
		off +=  adv_dio_read_int32(doc, off, &a);
		m_type[i] = a;
	}
	adv_dio_close(doc);

	adv_dio_file_close(advfile);

}

kun::PointObject* AdvObject::toKUNPointObject()
{
	kun::PointObject* point = new kun::PointObject();

	float* coords_buffer = new float[m_number * 3];
	float* values_buffer = new float[m_number];

	for( size_t i = 0; i < m_number; i++ )
	{   
		// Absolute value
		values_buffer[i] = std::sqrt( m_u[i] * m_u[i] + m_v[i] * m_v[i] + m_w[i] * m_w[i] );

		coords_buffer[i * 3] = m_x[i];
		coords_buffer[i * 3 + 1] = m_y[i];
		coords_buffer[i * 3 + 2] = m_z[i];
	}

	kvs::ValueArray<kvs::Real32>coords( coords_buffer, m_number * 3 );
	kvs::ValueArray<kvs::Real32>values( values_buffer, m_number );

	point->setVeclen( 1 );
	point->setCoords( coords );
	point->setValues( values );
	point->updateMinMaxValues();
	point->updateMinMaxCoords();

	return point;
}

} // end of namespace kun