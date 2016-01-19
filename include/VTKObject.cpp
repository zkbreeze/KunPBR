//
//  VTKObject.cpp
//
//
//  Created by Kun Zhao on 2015-12-01 16:52:03.
//
//

#include "VTKObject.h"
#include <iostream>
#include <fstream>
#include <kvs/ValueArray>
#include <kvs/PolygonObject>

namespace kun
{

VTKObject::VTKObject( std::string filename )
{
	this->read( filename );
}

bool VTKObject::read( std::string filename )
{
	std::ifstream ifs( filename.c_str(), std::ifstream::in );
	if ( !ifs.is_open() ) 
	{
		std::cerr<< "Error to open file!" << std::endl;
		return false;
	}
	std::cout << "VTK File is opened." << std::endl;

	char* buf = new char[256];
	char* arttibute = new char[256];
	int size;
	char* type = new char[256];

	ifs.getline( buf, 256 ); // Header
	ifs.getline( buf, 256 ); // Title
	ifs.getline( buf, 256 ); // Data type, either ASCII or BINARY
	ifs.getline( buf, 256 ); // Geometry/topology: STRUCTURED_POINTS STRUCTURED_GRID UNSTRUCTURED_GRID POLYDATA RECTILINEAR_GRID FIELD

	// Read the point
	ifs.getline( buf, 256 ); // Dataset attributes
	if( std::sscanf( buf, "%s %d %s", arttibute, &m_nvertices, type ) != 3 )
	{
		std::cerr << "Data type of POINT is wrong" << std::endl;
		return false;
	}
	std::cout << "Reading: " << arttibute << std::endl;
	m_coords = new float[m_nvertices * 3];
	for( size_t i = 0; i < m_nvertices; i++ )
	{
		ifs.getline( buf, 256 );
		float buf1, buf2, buf3;
		if( std::sscanf( buf, "%f %f %f", &buf1, &buf2, &buf3 ) != 3 )
		{
			std::cerr << "Coord data is wrong" << std::endl;
			return false;			
		}
		m_coords[i * 3] = buf1;
		m_coords[i * 3 + 1] = buf2;
		m_coords[i * 3 + 2] = buf3;
	}

	// Read the cell
	ifs.getline( buf, 256 ); // Dataset attributes
	if( std::sscanf( buf, "%s %d %d", arttibute, &m_nconnections, &size ) != 3 )
	{
		std::cerr << "Data type of CELL is wrong" << std::endl;
		return false;
	}
	std::cout << "Reading: " << arttibute << std::endl;
	m_connections = new unsigned int[m_nconnections * 3];
	for( size_t i = 0; i < m_nconnections; i++ )
	{
		ifs.getline( buf, 256 );
		unsigned int buf1, buf2, buf3;
		if( std::sscanf( buf, "%*d %d %d %d", &buf1, &buf2, &buf3 ) != 3 )
		{
			std::cerr << "Connection data is wrong" << std::endl;
			return false;			
		}
		m_connections[i * 3] = buf1;
		m_connections[i * 3 + 1] = buf2;
		m_connections[i * 3 + 2] = buf3;
	}

	return true;
}

kvs::PolygonObject* VTKObject::toKVSPolygonObject()
{
	kvs::PolygonObject* polygon = new kvs::PolygonObject();

	kvs::UInt8* color_buf = new kvs::UInt8[ m_nvertices * 3 ];
	for( size_t i = 0; i < m_nvertices * 3; i++ ) color_buf[i] = 150; // Assign the building as white

	kvs::ValueArray<float> coords( m_coords, m_nvertices * 3 );
	kvs::ValueArray<unsigned int> connections( m_connections, m_nconnections * 3 );
	kvs::ValueArray<kvs::UInt8> colors( color_buf, m_nvertices * 3 );

	polygon->setCoords( coords );
	polygon->setColors( colors );
	polygon->setConnections( connections );
	polygon->setPolygonType( kvs::PolygonObject::Triangle );
	polygon->setColorType( kvs::PolygonObject::VertexColor );

	polygon->updateMinMaxCoords();

	return polygon;
}


} // end of namespace kun