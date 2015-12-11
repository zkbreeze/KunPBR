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
#include <kvs/RGBColor>

namespace kun
{
	
class OBJObject
{
	std::vector<kvs::Real32> m_coords;
	std::vector<kvs::UInt8> m_colors;
	std::vector<kvs::Real32> m_normals;
	std::vector<kvs::UInt32> m_connections;

	bool m_has_normal;
	kvs::RGBColor m_color;

public:
	OBJObject();
	OBJObject( std::string filename );
	~OBJObject();

	bool read( std::string filename );
	void setColor( kvs::RGBColor color ){ m_color = color; }
	kvs::PolygonObject* toKVSPolygonObject();

	// bool write( std::string filename ){};
};


} // end of namespace kun
 
#endif // KUN__OBJOBJECT_H_INCLUDE