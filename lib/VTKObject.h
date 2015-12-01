//
//  VTKObject.h
//
//
//  Created by Kun Zhao on 2015-12-01 16:46:39.
//
//

#ifndef KUN__VTKOBJECT_H_INCLUDE
#define KUN__VTKOBJECT_H_INCLUDE

#include <kvs/PolygonObject>
#include <kvs/ValueArray>

namespace kun
{

class VTKObject
{

	float* m_coords;
	unsigned int* m_connections;
	int m_nvertices;
	int m_nconnections;

public:
	VTKObject();
	VTKObject( std::string filename );
	~VTKObject();

	bool read( std::string filename );
	kvs::PolygonObject* toKVSPolygonObject();
};


} // end of namespace kun
 
#endif // KUN__VTKOBJECT_H_INCLUDE