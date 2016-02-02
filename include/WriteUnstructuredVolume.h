//
//  WriteUnstructuredVolume.h
//
//
//  Created by Kun Zhao on 2016-01-20 16:01:27.
//
//

#ifndef KUN__WRITEUNSTRUCTUREDVOLUME_H_INCLUDE
#define KUN__WRITEUNSTRUCTUREDVOLUME_H_INCLUDE
 

#include <kvs/UnstructuredVolumeObject>
#include <kvs/KVSMLObjectUnstructuredVolume>
#include <kvs/UnstructuredVolumeExporter>

bool WriteUnstructuredVolume ( kvs::UnstructuredVolumeObject* object, std::string outname )
{
    kvs::KVSMLObjectUnstructuredVolume* output_volume = new kvs::UnstructuredVolumeExporter<kvs::KVSMLObjectUnstructuredVolume>( object );
    output_volume->setWritingDataType( kvs::KVSMLObjectUnstructuredVolume::ExternalBinary );
    output_volume->write( outname.c_str() );
    std::cout << "Finish writing Unstructured volume of " << outname << std::endl;
    return true;
}
 
#endif // KUN__WRITEUNSTRUCTUREDVOLUME_H_INCLUDE