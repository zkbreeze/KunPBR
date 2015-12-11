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

OBJObject::OBJObject():
m_has_normal( false ),
m_color( kvs::RGBColor::White() )
{
}

OBJObject::OBJObject( std::string filename ):
m_has_normal( false ),
m_color( kvs::RGBColor::White() )
{
	this->read( filename );
}

bool OBJObject::read( std::string filename )
{
	std::ifstream ifs( filename.c_str(), std::ifstream::in );
	if ( !ifs.is_open() ) 
	{
		std::cerr<< "Error to open file!" << std::endl;
		return false;
	}
	std::cout << filename << " file is opened." << std::endl;

	char* buf = new char[256];
	std::vector<float> normals;
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
				// // coordinate transform for 1f_in data.
				// coord_temp1 -= ( 106105 + 73 );
				// coord_temp2 -= ( 158414 + 110 );

				m_coords.push_back( coord_temp1 );
				m_coords.push_back( coord_temp2 );
				m_coords.push_back( coord_temp3 );

				break;

				case 'n': // normal file
				m_has_normal = true;
				float normal_temp1, normal_temp2, normal_temp3;
				if ( std::sscanf( buf + 2, "%f %f %f", &normal_temp1, &normal_temp2, &normal_temp3 ) != 3 )
				{
					std::cerr << "Normal data is wrong" << std::endl;
					return false;
				}
				normals.push_back( normal_temp1 );
				normals.push_back( normal_temp2 );
				normals.push_back( normal_temp3 );
				break;
			}
			break;

			case 'f': // connection data
			int connection_temp1, connection_temp2, connection_temp3;
			int nor_temp1, nor_temp2, nor_temp3;
			if ( std::sscanf( buf + 2, "%d//%d %d//%d %d//%d", &connection_temp1, &nor_temp1, &connection_temp2, &nor_temp2, &connection_temp3, &nor_temp3 ) != 6 )
			{
				if( std::sscanf( buf + 2, "%d %d %d", &connection_temp1, &connection_temp2, &connection_temp3 ) != 3 )
				{
					std::cerr << "Connection data is wrong" << std::endl;
					return false;
				}
			}
			m_connections.push_back( connection_temp1 - 1 );
			m_connections.push_back( connection_temp2 - 1 );
			m_connections.push_back( connection_temp3 - 1 );

			if( m_has_normal )
			{
				m_normals.push_back( normals[(nor_temp1 - 1 ) * 3] + normals[(nor_temp2 - 1) * 3] + normals[(nor_temp3 - 1 ) * 3] );
				m_normals.push_back( normals[(nor_temp1 - 1 ) * 3 + 1] + normals[(nor_temp2 - 1) * 3 + 1] + normals[(nor_temp3 - 1 ) * 3 + 1] );
				m_normals.push_back( normals[(nor_temp1 - 1 ) * 3 + 2] + normals[(nor_temp2 - 1) * 3 + 2] + normals[(nor_temp3 - 1 ) * 3 + 2] );				
			}

			break;
		}
	}

	return true;
}

kvs::PolygonObject* OBJObject::toKVSPolygonObject()
{
	kvs::PolygonObject* polygon = new kvs::PolygonObject();

	kvs::UInt8* color_buf = new kvs::UInt8[ m_coords.size() ];
	for( size_t i = 0; i < m_coords.size() / 3; i++ )
	{
		color_buf[i * 3] = m_color.r();
		color_buf[i * 3 + 1] = m_color.g();
		color_buf[i * 3 + 2] = m_color.b();
	}

	kvs::ValueArray<float> coords( m_coords.data(), m_coords.size() );
	kvs::ValueArray<float> normals( m_normals.data(), m_normals.size() );
	kvs::ValueArray<kvs::UInt32> connections( m_connections.data(), m_connections.size() );
	kvs::ValueArray<kvs::UInt8> colors( color_buf, m_coords.size() );

	polygon->setCoords( coords );
	polygon->setColors( colors );
	if( m_has_normal ) polygon->setNormals( normals );
	polygon->setConnections( connections );
	polygon->setPolygonType( kvs::PolygonObject::Triangle );
	polygon->setColorType( kvs::PolygonObject::VertexColor );
	polygon->setNormalTypeToPolygon();

	polygon->updateMinMaxCoords();

	return polygon;
}

// bool OBJObject::write( std::string filename )	
// {}
} // end of namespace kun
