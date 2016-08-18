//
//  WritePolygon.h
//
//
//  Created by Kun Zhao on 2016-06-13 09:36:50.
//
//

#ifndef KUN__WRITEPOLYGON_H_INCLUDE
#define KUN__WRITEPOLYGON_H_INCLUDE
 
#include <kvs/PolygonObject>
#include <kvs/KVSMLObjectPolygon>
#include <kvs/PolygonExporter>

bool WritePolygon( kvs::PolygonObject* object, std::string outname )
{
    kvs::KVSMLObjectPolygon* output_volume = new kvs::PolygonExporter<kvs::KVSMLObjectPolygon>( object );
    output_volume->setWritingDataType( kvs::KVSMLObjectPolygon::ExternalBinary );
    output_volume->write( outname.c_str() );
    std::cout << "Finish writing Polygon of " << outname << std::endl;
    return true;
}
 
#endif // KUN__WRITEPOLYGON_H_INCLUDE