//
//  OBJImporter.h
//
//
//  Created by Kun Zhao on 2015-07-31 16:38:10.
//
//

#ifndef KUN__OBJIMPORTER_H_INCLUDE
#define KUN__OBJIMPORTER_H_INCLUDE

#include <kvs/PolygonObject>
#include <kvs/ValueArray>

namespace kun
{
	
class OBJImporter
{
	std::vector<kvs::Real32> m_coords;
	std::vector<kvs::Real32> m_normals;
	std::vector<kvs::UInt32> m_connections;

public:
	OBJImporter( void );
	OBJImporter( std::string filename );
	~OBJImporter();

	bool read( std::string filename );
	kvs::PolygonObject* toKVSPolygonObject();
};


} // end of namespace kun
 
#endif // KUN__OBJIMPORTER_H_INCLUDE