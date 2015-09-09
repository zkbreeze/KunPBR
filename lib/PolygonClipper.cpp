//
//  PolygonClipper.cpp
//
//
//  Created by Kun Zhao on 2015-09-08 16:30:08.
//
//

#include "PolygonClipper.h"

namespace kun
{

PolygonClipper::PolygonClipper( kvs::PolygonObject* polygon, kvs::Vec3 min, kvs::Vec3 max )
{
	ClipBox( polygon, min, max );	
}

kvs::PolygonObject* PolygonClipper::ClipBox(kvs::PolygonObject *polygon, kvs::Vec3 min, kvs::Vec3 max)
{
	Plane* plane_min_x = Plane::ConstructXPlane( min.x() ); ClipPlane( polygon, plane_min_x, kvs::Vec3( 1, 0, 0 ) );
	Plane* plane_min_y = Plane::ConstructYPlane( min.y() ); ClipPlane( polygon, plane_min_y, kvs::Vec3( 0, 1, 0 ) );
	Plane* plane_min_z = Plane::ConstructZPlane( min.z() ); ClipPlane( polygon, plane_min_z, kvs::Vec3( 0, 0, 1 ) );
	Plane* plane_max_x = Plane::ConstructXPlane( max.x() ); ClipPlane( polygon, plane_max_x, kvs::Vec3( -1, 0, 0 ) );
	Plane* plane_max_y = Plane::ConstructYPlane( max.y() ); ClipPlane( polygon, plane_max_y, kvs::Vec3( 0, -1, 0 ) );
	Plane* plane_max_z = Plane::ConstructZPlane( max.z() ); ClipPlane( polygon, plane_max_z, kvs::Vec3( 0, 0, -1 ) );
}

kvs::PolygonObject* PolygonClipper::ClipPlane( kvs::PolygonObject* polygon, kun::Plane* plane, kvs::Vec3 plane_normal)
{
	const float* pcoord = polygon->coords().data();
	const float* pnormal = polygon->normals().data();
	const unsigned char* pcolor = polygon->colors().data();
	const unsigned int* pconnection = polygon->connections().data();

	std::vector<float> coords;
	std::vector<float> normals;
	std::vector<unsigned char> colors;
	std::vector<unsigned int> connections;

	unsigned int count = 0;
	unsigned int* table = new unsigned int[polygon->numberOfVertices()];

	// Delete the vertices out of scope
	for( size_t i = 0; i < polygon->numberOfVertices(); i++ )
	{
		int index = i * 3;
		kvs::Vec3 vertex( pcoord[index], pcoord[index + 1], pcoord[index + 2] );
		if( plane->pointDistance( vertex, plane_normal ) >= 0 )
		{
			coords.push_back( pcoord[index] );
			coords.push_back( pcoord[index + 1] );
			coords.push_back( pcoord[index + 2] );

			normals.push_back( pnormal[index] );
			normals.push_back( pnormal[index + 1] );
			normals.push_back( pnormal[index + 2] );

			colors.push_back( pcolor[index] );
			colors.push_back( pcolor[index + 1] );
			colors.push_back( pcolor[index + 2] );

			table[i] = i - count;
		}
		else
			count++;
	}

	int pcon = coords.size() / 3; // added connections

	// Create new triangle for the clipped faces
	for( size_t i = 0; i < ( polygon->numberOfConnections() / 3 ); i++ )
	{
		int connection_index = i * 3;
		int coord_index_1 = pconnection[connection_index] * 3;
		int coord_index_2 = pconnection[connection_index + 1] * 3;
		int coord_index_3 = pconnection[connection_index + 2] * 3;

		kvs::Vec3 V1( pcoord[coord_index_1], pcoord[coord_index_1 + 1], pcoord[coord_index_1 + 2] );
		kvs::Vec3 V2( pcoord[coord_index_2], pcoord[coord_index_2 + 1], pcoord[coord_index_2 + 2] );
		kvs::Vec3 V3( pcoord[coord_index_3], pcoord[coord_index_3 + 1], pcoord[coord_index_3 + 2] );

		kvs::Vec3 intersection1;
		kvs::Vec3 intersection2; // Intersections between triangle and plane

		if( plane->intersectTriangle( V1, V2, V3, intersection1, intersection2 ) ) // Intersection
		{
			if( plane->pointDistance( V1, plane_normal ) > 0 ) // V1 inside
			{
				if( plane->pointDistance( V2, plane_normal ) > 0 ) // V1, V2 inside
				{
					coords.push_back( intersection1.x() );
					coords.push_back( intersection1.y() );
					coords.push_back( intersection1.z() );

					coords.push_back( intersection2.x() );
					coords.push_back( intersection2.y() );
					coords.push_back( intersection2.z() );

					// Assign the nearest normals
					normals.push_back( pnormal[coord_index_1] );
					normals.push_back( pnormal[coord_index_1 + 1] );
					normals.push_back( pnormal[coord_index_1 + 2] );

					normals.push_back( pnormal[coord_index_2] );
					normals.push_back( pnormal[coord_index_2 + 1] );
					normals.push_back( pnormal[coord_index_2 + 2] );

					// Assign the nearest colors
					colors.push_back( pcolor[coord_index_1] );
					colors.push_back( pcolor[coord_index_1 + 1] );
					colors.push_back( pcolor[coord_index_1 + 2] );

					colors.push_back( pcolor[coord_index_2] );
					colors.push_back( pcolor[coord_index_2 + 1] );
					colors.push_back( pcolor[coord_index_2 + 2] );

					// Create twos triangles for the clipped quadrilateral
					connections.push_back( table[connection_index] );
					connections.push_back( table[connection_index + 1] );
					connections.push_back( pcon++ );

					connections.push_back( table[connection_index + 1] );
					connections.push_back( pcon );
					connections.push_back( pcon++ );
				}
				else if( plane->pointDistance( V3, plane_normal ) > 0 ) // V1, V3 inside
				{
					coords.push_back( intersection1.x() );
					coords.push_back( intersection1.y() );
					coords.push_back( intersection1.z() );

					coords.push_back( intersection2.x() );
					coords.push_back( intersection2.y() );
					coords.push_back( intersection2.z() );

					// Assign the nearest normals
					normals.push_back( pnormal[coord_index_1] );
					normals.push_back( pnormal[coord_index_1 + 1] );
					normals.push_back( pnormal[coord_index_1 + 2] );

					normals.push_back( pnormal[coord_index_3] );
					normals.push_back( pnormal[coord_index_3 + 1] );
					normals.push_back( pnormal[coord_index_3 + 2] );

					// Assign the nearest colors
					colors.push_back( pcolor[coord_index_1] );
					colors.push_back( pcolor[coord_index_1 + 1] );
					colors.push_back( pcolor[coord_index_1 + 2] );

					colors.push_back( pcolor[coord_index_3] );
					colors.push_back( pcolor[coord_index_3 + 1] );
					colors.push_back( pcolor[coord_index_3 + 2] );

					// Create twos triangles for the clipped quadrilateral
					connections.push_back( table[connection_index] );
					connections.push_back( table[connection_index + 2] );
					connections.push_back( pcon++ );

					connections.push_back( table[connection_index + 2] );
					connections.push_back( pcon );
					connections.push_back( pcon++ );
				}
				else // Only V1 inside
				{
					coords.push_back( intersection1.x() );
					coords.push_back( intersection1.y() );
					coords.push_back( intersection1.z() );

					coords.push_back( intersection2.x() );
					coords.push_back( intersection2.y() );
					coords.push_back( intersection2.z() );

					// Assign the nearest normals
					normals.push_back( pnormal[coord_index_1] );
					normals.push_back( pnormal[coord_index_1 + 1] );
					normals.push_back( pnormal[coord_index_1 + 2] );

					normals.push_back( pnormal[coord_index_1] );
					normals.push_back( pnormal[coord_index_1 + 1] );
					normals.push_back( pnormal[coord_index_1 + 2] );

					// Assign the nearest colors
					colors.push_back( pcolor[coord_index_1] );
					colors.push_back( pcolor[coord_index_1 + 1] );
					colors.push_back( pcolor[coord_index_1 + 2] );

					colors.push_back( pcolor[coord_index_1] );
					colors.push_back( pcolor[coord_index_1 + 1] );
					colors.push_back( pcolor[coord_index_1 + 2] );

					// Create twos triangles for the clipped quadrilateral
					connections.push_back( table[connection_index] );
					connections.push_back( pcon++ );
					connections.push_back( pcon++ );
				}
			}
			else if( plane->pointDistance( V2, plane_normal ) > 0 ) // V2 inside
			{
				if( plane->pointDistance( V3, plane_normal ) > 0 ) // V2, V3 inside
				{
					coords.push_back( intersection1.x() );
					coords.push_back( intersection1.y() );
					coords.push_back( intersection1.z() );

					coords.push_back( intersection2.x() );
					coords.push_back( intersection2.y() );
					coords.push_back( intersection2.z() );

					// Assign the nearest normals
					normals.push_back( pnormal[coord_index_2] );
					normals.push_back( pnormal[coord_index_2 + 1] );
					normals.push_back( pnormal[coord_index_2 + 2] );

					normals.push_back( pnormal[coord_index_3] );
					normals.push_back( pnormal[coord_index_3 + 1] );
					normals.push_back( pnormal[coord_index_3 + 2] );

					// Assign the nearest colors
					colors.push_back( pcolor[coord_index_2] );
					colors.push_back( pcolor[coord_index_2 + 1] );
					colors.push_back( pcolor[coord_index_2 + 2] );

					colors.push_back( pcolor[coord_index_3] );
					colors.push_back( pcolor[coord_index_3 + 1] );
					colors.push_back( pcolor[coord_index_3 + 2] );

					// Create twos triangles for the clipped quadrilateral
					connections.push_back( table[connection_index + 1] );
					connections.push_back( table[connection_index + 2] );
					connections.push_back( pcon++ );

					connections.push_back( table[connection_index + 2] );
					connections.push_back( pcon );
					connections.push_back( pcon++ );
				}
				else // Only V2 inside
				{
					coords.push_back( intersection1.x() );
					coords.push_back( intersection1.y() );
					coords.push_back( intersection1.z() );

					coords.push_back( intersection2.x() );
					coords.push_back( intersection2.y() );
					coords.push_back( intersection2.z() );

					// Assign the nearest normals
					normals.push_back( pnormal[coord_index_2] );
					normals.push_back( pnormal[coord_index_2 + 1] );
					normals.push_back( pnormal[coord_index_2 + 2] );

					normals.push_back( pnormal[coord_index_2] );
					normals.push_back( pnormal[coord_index_2 + 1] );
					normals.push_back( pnormal[coord_index_2 + 2] );

					// Assign the nearest colors
					colors.push_back( pcolor[coord_index_2] );
					colors.push_back( pcolor[coord_index_2 + 1] );
					colors.push_back( pcolor[coord_index_2 + 2] );

					colors.push_back( pcolor[coord_index_2] );
					colors.push_back( pcolor[coord_index_2 + 1] );
					colors.push_back( pcolor[coord_index_2 + 2] );

					// Create twos triangles for the clipped quadrilateral
					connections.push_back( table[connection_index + 1] );
					connections.push_back( pcon++ );
					connections.push_back( pcon++ );
				}
			}
			else if( plane->pointDistance( V3, plane_normal ) ) // Only V3 inside
			{
				coords.push_back( intersection1.x() );
				coords.push_back( intersection1.y() );
				coords.push_back( intersection1.z() );

				coords.push_back( intersection2.x() );
				coords.push_back( intersection2.y() );
				coords.push_back( intersection2.z() );

				// Assign the nearest normals
				normals.push_back( pnormal[coord_index_3] );
				normals.push_back( pnormal[coord_index_3 + 1] );
				normals.push_back( pnormal[coord_index_3 + 2] );

				normals.push_back( pnormal[coord_index_3] );
				normals.push_back( pnormal[coord_index_3 + 1] );
				normals.push_back( pnormal[coord_index_3 + 2] );

				// Assign the nearest colors
				colors.push_back( pcolor[coord_index_3] );
				colors.push_back( pcolor[coord_index_3 + 1] );
				colors.push_back( pcolor[coord_index_3 + 2] );

				colors.push_back( pcolor[coord_index_3] );
				colors.push_back( pcolor[coord_index_3 + 1] );
				colors.push_back( pcolor[coord_index_3 + 2] );

				// Create twos triangles for the clipped quadrilateral
				connections.push_back( table[connection_index + 2] );
				connections.push_back( pcon++ );
				connections.push_back( pcon++ );
			}
		}
		else if( plane->pointDistance( V1, plane_normal ) + plane->pointDistance( V2, plane_normal ) + plane->pointDistance( V3, plane_normal ) >=0 ) 
		// No intersection and the triangle is inside the plane
		{
			connections.push_back( table[connection_index] );
			connections.push_back( table[connection_index + 1] );
			connections.push_back( table[connection_index + 2] );
		}
	}

	kvs::ValueArray<float> coords_array( coords.data(), coords.size() );
	kvs::ValueArray<float> normals_array( normals.data(), normals.size() );
	kvs::ValueArray<kvs::UInt32> connections_array( connections.data(), connections.size() );
	kvs::ValueArray<kvs::UInt8> colors_array( colors.data(), colors.size() );

	polygon->setCoords( coords_array );
	polygon->setColors( colors_array );
	polygon->setNormals( normals_array );
	polygon->setConnections( connections_array );
	polygon->setPolygonType( kvs::PolygonObject::Triangle );
	polygon->setColorType( kvs::PolygonObject::VertexColor );
	polygon->setNormalType( kvs::PolygonObject::VertexNormal );
}


} // end of namespace kun