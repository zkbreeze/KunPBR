//
//  PointExporter.cpp
//
//
//  Created by Kun Zhao on 2014-09-04 16:42:37.
//
//

#include "PointExporter.h"
#include <kvs/Message>


namespace kun
{

/*===========================================================================*/
/**
 *  @brief  Constructs a new PointExporter class for KVMLObjectPoint format.
 *  @param  object [in] pointer to the input point object
 */
/*===========================================================================*/
PointExporter<kun::KVSMLObjectKunPoint>::PointExporter( const kun::PointObject* object )
{
    this->exec( object );
}

/*===========================================================================*/
/**
 *  @brief  Executes the export process.
 *  @param  object [in] pointer to the input object
 *  @return pointer to the KVSMLObjectPoint format
 */
/*===========================================================================*/
kun::KVSMLObjectKunPoint* PointExporter<kun::KVSMLObjectKunPoint>::exec( const kvs::ObjectBase* object )
{
    BaseClass::setSuccess( true );

    if ( !object )
    {
        BaseClass::setSuccess( false );
        kvsMessageError("Input object is NULL.");
        return NULL;
    }

    const kun::PointObject* point = kun::PointObject::DownCast( object );
    if ( !point )
    {
        BaseClass::setSuccess( false );
        kvsMessageError("Input object is not point object.");
        return NULL;
    }

    this->setCoords( point->coords() );
    this->setColors( point->colors() );
    this->setNormals( point->normals() );
    this->setSizes( point->sizes() );
    this->setValues( point->values() );

    return this;
}

} // end of namespace kun
