//
//  OBJObject.cpp
//
//
//  Created by Kun Zhao on 2015-07-31 16:41:49.
//
//

#include "OBJObject.h"
#include <iostream>
#include <fstream>
#include <kvs/ValueArray>
#include <kvs/PolygonObject>

namespace kun
{

OBJObject::OBJObject( std::string filename )
{
	m_is_minmax= false;
	m_filename = filename;
}

bool OBJObject::read( std::string filename )
{
	std::ifstream ifs( filename.c_str(), std::ifstream::in );
	if ( !ifs.is_open() ) 
	{
		std::cerr<< "Error to open file!" << std::endl;
		return false;
	}
	std::cout << "File is opened." << std::endl;

	char* buf = new char[256];
	while ( ifs.getline( buf, 256 ) )
	{
		switch( buf[0] )
		{
			case 'v':
			switch( buf[1] )
			{
				case ' ': // vertex file
				float coord_temp1, coord_temp2, coord_temp3;
				if ( std::sscanf( buf + 2, "%f %f %f", &coord_temp1, &coord_temp2, &coord_temp3 ) != 3 )
				{
					std::cerr << "Coord data is wrong" << std::endl;
					return false;
				}
				if ( m_is_minmax )
				{
					if ( coord_temp1 < m_min.x() ) coord_temp1 = m_min.x();
					if ( coord_temp1 > m_max.x() ) coord_temp1 = m_max.x();
					if ( coord_temp2 < m_min.y() ) coord_temp2 = m_min.y();
					if ( coord_temp2 > m_max.y() ) coord_temp2 = m_max.y();
					if ( coord_temp3 < m_min.z() ) coord_temp3 = m_min.z();
					if ( coord_temp3 > m_max.z() ) coord_temp3 = m_max.z();
				}
				m_coords.push_back( coord_temp1 );
				m_coords.push_back( coord_temp2 );
				m_coords.push_back( coord_temp3 );

				break;

				case 'n': // normal file
				float normal_temp1, normal_temp2, normal_temp3;
				if ( std::sscanf( buf + 2, "%f %f %f", &normal_temp1, &normal_temp2, &normal_temp3 ) != 3 )
				{
					std::cerr << "Normal data is wrong" << std::endl;
					return false;
				}
				m_normals.push_back( normal_temp1 );
				m_normals.push_back( normal_temp2 );
				m_normals.push_back( normal_temp3 );
				break;
			}
			break;

			case 'f': // connection data
			int connection_temp1, connection_temp2, connection_temp3;
			int nor_temp1, nor_temp2, nor_temp3;
			if ( std::sscanf( buf + 2, "%d//%d %d//%d %d//%d", &connection_temp1, &nor_temp1, &connection_temp2, &nor_temp2, &connection_temp3, &nor_temp3 ) != 6 )
			{
				std::cerr << "Connection data is wrong" << std::endl;
				return false;
			}
			m_connections.push_back( connection_temp1 - 1 );
			m_connections.push_back( connection_temp2 - 1 );
			m_connections.push_back( connection_temp3 - 1 );
			break;
		}
	}
	return true;
}

kvs::PolygonObject* OBJObject::toKVSPolygonObject()
{
	this->read( m_filename ); 

	kvs::PolygonObject* polygon = new kvs::PolygonObject();

	kvs::UInt8* color_buf = new kvs::UInt8[ m_coords.size() ];
	for( size_t i = 0; i < m_coords.size(); i++ ) color_buf[i] = 255; // Assign the building as white

	kvs::ValueArray<float> coords( m_coords.data(), m_coords.size() );
	kvs::ValueArray<float> normals( m_normals.data(), m_normals.size() );
	kvs::ValueArray<kvs::UInt32> connections( m_connections.data(), m_connections.size() );
	kvs::ValueArray<kvs::UInt8> colors( color_buf, m_coords.size() );

	polygon->setCoords( coords );
	polygon->setColors( colors );
	polygon->setNormals( normals );
	polygon->setConnections( connections );
	polygon->setPolygonType( kvs::PolygonObject::Triangle );
	polygon->setColorType( kvs::PolygonObject::VertexColor );
	polygon->setNormalType( kvs::PolygonObject::VertexNormal );

	return polygon;

}

	
} // end of namespace kun
