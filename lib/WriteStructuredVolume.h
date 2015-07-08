//
//  WriteStructuredVolume.h
//
//
//  Created by Kun Zhao on 2015-07-01 13:57:44.
//
//

#ifndef KUN__WRITESTRUCTUREDVOLUME_H_INCLUDE
#define KUN__WRITESTRUCTUREDVOLUME_H_INCLUDE

#include <kvs/StructuredVolumeObject>
#include <kvs/KVSMLObjectStructuredVolume>
#include <kvs/StructuredVolumeExporter>

void WriteStructuredVolume ( kvs::StructuredVolumeObject* object, std::string outname )
{
    kvs::KVSMLObjectStructuredVolume* output_volume = new kvs::StructuredVolumeExporter<kvs::KVSMLObjectStructuredVolume>( object );
    output_volume->setWritingDataType( kvs::KVSMLObjectStructuredVolume::ExternalBinary );
    output_volume->write( outname.c_str() );
    std::cout << "finish writing Strucutured volume of " << outname << std::endl;
}
 
#endif // KUN__WRITESTRUCTUREDVOLUME_H_INCLUDE