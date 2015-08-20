//
//  DensityCalculator.cpp
//
//
//  Created by Kun Zhao on 2015-06-21 22:18:09.
//
//

#include "DensityCalculator.h"
#include "float.h"

namespace kun
{

DensityCalculator::DensityCalculator( void )
{
	m_use_given_grid = false;
	m_max_grid = 200;
}

DensityCalculator::DensityCalculator( kun::PointObject* point )
{
	this->setPointObject( point );
	m_use_given_grid = false;
	m_max_grid = 200;
}

DensityCalculator::~DensityCalculator( void )
{
	delete m_table;
}

void DensityCalculator::exec()
{
	float size_x;
	float size_y;
	float size_z;

	if( m_use_given_grid == false )
	{
		float range_x = m_point->maxObjectCoord().x() - m_point->minObjectCoord().x();
		float range_y = m_point->maxObjectCoord().y() - m_point->minObjectCoord().y();
		float range_z = m_point->maxObjectCoord().z() - m_point->minObjectCoord().z();
		float range_max = range_x;
		if( range_max < range_y ) range_max = range_y;
		if( range_max < range_z ) range_max = range_z;
		if( range_max == range_x ) { m_x = m_max_grid; m_y = ( range_y / range_x ) * m_x; m_z = ( range_z / range_x ) * m_x; }
		if( range_max == range_y ) { m_y = m_max_grid; m_z = ( range_z / range_y ) * m_y; m_x = ( range_x / range_y ) * m_y; }
		if( range_max == range_z ) { m_z = m_max_grid; m_x = ( range_x / range_z ) * m_z; m_y = ( range_y / range_z ) * m_z; }

		size_x = (float)range_x / m_x;
		size_y = (float)range_y / m_y;
		size_z = (float)range_z / m_z;
	}

	m_cube_volume = size_x * size_y * size_z;

	m_table = new int[m_x * m_y * m_z];
	for( size_t i = 0; i < m_x * m_y * m_z; i++ ) m_table[i] = 0;

	const float* coord = m_point->coords().data();
	for( size_t i = 0; i < m_point->numberOfVertices(); i++ )
	{
		size_t index_x = ( coord[i * 3] - m_point->minObjectCoord().x() ) / size_x;
		size_t index_y = ( coord[i * 3 + 1] - m_point->minObjectCoord().y() ) / size_y;
		size_t index_z = ( coord[i * 3 + 2] - m_point->minObjectCoord().z() ) / size_z;

		m_table[index_x + index_y * m_x + index_z * m_x * m_y] += 1;
	}
}
	
kvs::StructuredVolumeObject* DensityCalculator::outputDensityVolume()
{
	this->exec();

	size_t length = m_x * m_y * m_z;
	kvs::AnyValueArray values;
	values.allocate<float>( length );
	float* buf = static_cast<float*>( values.data() );
	for( size_t i = 0; i < length; i++ ) buf[i] = (float)m_table[i] / m_cube_volume;
	
	kvs::StructuredVolumeObject* volume = new kvs::StructuredVolumeObject();
	const kvs::StructuredVolumeObject::GridType grid_type = kvs::StructuredVolumeObject::Uniform;
	volume->setGridType( grid_type );
	volume->setResolution( kvs::Vec3ui( m_x, m_y, m_z ) );
	volume->setVeclen( 1 );
	volume->setValues( values );
	volume->updateMinMaxCoords();
	volume->updateMinMaxValues();
	
	return( volume );
}

} // end of namespace kun
