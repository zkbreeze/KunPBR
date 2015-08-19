//
//  OBJObject.h
//
//
//  Created by Kun Zhao on 2015-07-31 16:38:10.
//
//

#ifndef KUN__OBJOBJECT_H_INCLUDE
#define KUN__OBJOBJECT_H_INCLUDE

#include <kvs/PolygonObject>
#include <kvs/ValueArray>

namespace kun
{
	
class OBJObject
{
	std::string m_filename;
	std::vector<kvs::Real32> m_coords;
	std::vector<kvs::UInt8> m_colors;
	std::vector<kvs::Real32> m_normals;
	std::vector<kvs::UInt32> m_connections;
	kvs::Vector3f m_min; // Clip edge for the polygon [min]
	kvs::Vector3f m_max; // Clip edge for the polygon [max]
	bool m_is_minmax;

public:
	OBJObject( void );
	OBJObject( std::string filename );
	~OBJObject();

	bool read( std::string filename );
	void setRange( kvs::Vector3f min, kvs::Vector3f max ){ m_is_minmax = true; m_min = min; m_max = max; }
	kvs::PolygonObject* toKVSPolygonObject();
};


} // end of namespace kun
 
#endif // KUN__OBJOBJECT_H_INCLUDE