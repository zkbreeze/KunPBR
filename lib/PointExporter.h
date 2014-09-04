//
//  PointExporter.h
//
//
//  Created by Kun Zhao on 2014-09-04 16:41:44.
//
//

#ifndef KUN__POINTEXPORTER_H_INCLUDE
#define KUN__POINTEXPORTER_H_INCLUDE
 
#include "PointObject.h"
#include <kvs/KVSMLObjectPoint>
#include <kvs/ExporterBase>


namespace kun
{

/*===========================================================================*/
/**
 *  @brief  Point exporter class.
 */
/*===========================================================================*/
template <typename FileFormatType>
class PointExporter : public kvs::ExporterBase, public FileFormatType
{
    kvsModule( kun::PointExporter<FileFormatType>, Exporter );
    kvsModuleBaseClass( kvs::ExporterBase );

public:
    FileFormatType* exec( const kvs::ObjectBase* ) { return NULL; }
};

/*===========================================================================*/
/**
 *  @brief  Point exporter class as KVSMLObjectPoint format.
 */
/*===========================================================================*/
template <>
class PointExporter<kvs::KVSMLObjectPoint> :
        public kvs::ExporterBase,
        public kvs::KVSMLObjectPoint
{
public:
    PointExporter( const kun::PointObject* object );
    kvs::KVSMLObjectPoint* exec( const kvs::ObjectBase* object );
};

} // end of namespace kun
 
#endif // KUN__POINTEXPORTER_H_INCLUDE