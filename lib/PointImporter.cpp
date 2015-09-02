//
//  PointImporter.cpp
//
//
//  Created by Kun Zhao on 2014-09-04 16:39:56.
//
//

#include "PointImporter.h"
#include <kvs/DebugNew>
#include "KVSMLObjectKunPoint.h"
#include <kvs/Math>
#include <kvs/Vector3>
#include <string>
#include <kvs/File>
#include <fstream>
#include <iostream>

namespace kun
{

/*==========================================================================*/
/**
 *  @brief  Constructs a new PointImporter class.
 */
/*==========================================================================*/
 PointImporter::PointImporter()
 {
 }

/*===========================================================================*/
/**
 *  @brief  Constructs a new PointImporter class.
 *  @param  filename [in] input filename
 */
/*===========================================================================*/
 PointImporter::PointImporter( const std::string& filename )
 {
    if ( kun::KVSMLObjectKunPoint::CheckExtension( filename ) )
    {
        kun::KVSMLObjectKunPoint* file_format = new kun::KVSMLObjectKunPoint( filename );
        if( !file_format )
        {
            BaseClass::setSuccess( false );
            kvsMessageError("Cannot read '%s'.",filename.c_str());
            return;
        }

        if( file_format->isFailure() )
        {
            BaseClass::setSuccess( false );
            kvsMessageError("Cannot read '%s'.",filename.c_str());
            delete file_format;
            return;
        }

        this->import( file_format );
        delete file_format;
    }
    else
    {
        BaseClass::setSuccess( false );
        kvsMessageError("Cannot import '%s'.",filename.c_str());
        return;
    }
}

// Add. To load the point object the repetition part.
PointImporter::PointImporter( const std::string& filename, const float fraction )
{
    if ( kun::KVSMLObjectKunPoint::CheckExtension( filename ) )
    {
        const kvs::File file( filename );

        // value
        std::string value_name = file.pathName() + "/" + file.baseName() + "_value.dat";
        std::ifstream value( value_name, std::ifstream::binary );
        value.seekg( 0, std::ios::end );
        size_t end = value.tellg();
        value.seekg( 0, std::ios::beg );
        size_t length = ( end / sizeof( float ) ) * fraction;
        kvs::AnyValueArray values;
        values.allocate<kvs::Real32>( length );
        value.read( (char*)values.data(), length * sizeof( float ) );

        // coord
        std::string coord_name = file.pathName() + "/" + file.baseName() + "_coord.dat";
        std::ifstream coord( coord_name, std::ifstream::binary );
        kvs::ValueArray<kvs::Real32>coords( length * 3 );
        coord.read( (char*)coords.data(), length * 3 * sizeof( float ) );

        // normal
        std::string normal_name = file.pathName() + "/" + file.baseName() + "_normal.dat";
        std::ifstream normal( normal_name, std::ifstream::binary);
        kvs::ValueArray<kvs::Real32>normals( length * 3 );
        normal.read( (char*)normals.data(), length * 3 * sizeof( float ) );

        SuperClass::setValues( values );
        SuperClass::setCoords( coords );
        SuperClass::setNormals( normals );
    }
    else
    {
        BaseClass::setSuccess( false );
        kvsMessageError("Cannot import '%s'.",filename.c_str());
        return;
    }
}

/*==========================================================================*/
/**
 *  @brief  Constructs a new PointImporter class.
 *  @param file_format [in] pointer to the file format
 */
/*==========================================================================*/
 PointImporter::PointImporter( const kvs::FileFormatBase* file_format )
 {
    this->exec( file_format );
}

/*===========================================================================*/
/**
 *  @brief  Destructs the PointImporter class.
 */
/*===========================================================================*/
 PointImporter::~PointImporter()
 {
 }

/*===========================================================================*/
/**
 *  @brief  Executes the import process.
 *  @param  file_format [in] pointer to the file format class
 *  @return pointer to the imported point object
 */
/*===========================================================================*/
 PointImporter::SuperClass* PointImporter::exec( const kvs::FileFormatBase* file_format )
 {
    if ( !file_format )
    {
        BaseClass::setSuccess( false );
        kvsMessageError("Input file format is NULL.");
        return NULL;
    }

    if ( const kun::KVSMLObjectKunPoint* point = dynamic_cast<const kun::KVSMLObjectKunPoint*>( file_format ) )
    {
        this->import( point );
    }
    else
    {
        BaseClass::setSuccess( false );
        kvsMessageError("Input file format is not supported.");
        return NULL;
    }

    return this;
}

/*==========================================================================*/
/**
 *  @brief  Imports KVSML format data.
 *  @param  kvsml [in] pointer to the KVSML format data
 */
/*==========================================================================*/
 void PointImporter::import( const kun::KVSMLObjectKunPoint* kvsml )
 {
    if ( kvsml->objectTag().hasExternalCoord() )
    {
        const kvs::Vector3f min_coord( kvsml->objectTag().minExternalCoord() );
        const kvs::Vector3f max_coord( kvsml->objectTag().maxExternalCoord() );
        SuperClass::setMinMaxExternalCoords( min_coord, max_coord );
    }

    if ( kvsml->objectTag().hasObjectCoord() )
    {
        const kvs::Vector3f min_coord( kvsml->objectTag().minObjectCoord() );
        const kvs::Vector3f max_coord( kvsml->objectTag().maxObjectCoord() );
        SuperClass::setMinMaxObjectCoords( min_coord, max_coord );
    }

    SuperClass::setCoords( kvsml->coords() );
    // SuperClass::setColors( kvsml->colors() );
    SuperClass::setNormals( kvsml->normals() );
    SuperClass::setSizes( kvsml->sizes() );
    SuperClass::setValues( kvsml->values() );
    SuperClass::setVeclen( kvsml->veclen() );
    SuperClass::updateMinMaxValues();
    SuperClass::updateMinMaxCoords();
    this->set_min_max_coord();

    BaseClass::setSuccess( true );
}

/*==========================================================================*/
/**
 *  @brief  Calculates the min/max coordinate values.
 */
/*==========================================================================*/
 void PointImporter::set_min_max_coord()
 {
    const float min_x = SuperClass::coords()[0];
    const float min_y = SuperClass::coords()[1];
    const float min_z = SuperClass::coords()[2];
    kvs::Vector3f min_coord( min_x, min_y, min_z );
    kvs::Vector3f max_coord( min_coord );
    const size_t dimension = 3;
    const size_t nvertices = SuperClass::coords().size() / dimension;
    size_t index3 = 3;
    for ( size_t i = 1; i < nvertices; i++, index3 += 3 )
    {
        const float x = SuperClass::coords()[index3];
        const float y = SuperClass::coords()[index3+1];
        const float z = SuperClass::coords()[index3+2];

        min_coord.x() = kvs::Math::Min( min_coord.x(), x );
        min_coord.y() = kvs::Math::Min( min_coord.y(), y );
        min_coord.z() = kvs::Math::Min( min_coord.z(), z );

        max_coord.x() = kvs::Math::Max( max_coord.x(), x );
        max_coord.y() = kvs::Math::Max( max_coord.y(), y );
        max_coord.z() = kvs::Math::Max( max_coord.z(), z );
    }

    this->setMinMaxObjectCoords( min_coord, max_coord );
    this->setMinMaxExternalCoords( min_coord, max_coord );
}

} // end of namespace kun