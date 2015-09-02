//
//  PointObject.h
//
//
//  Created by Kun Zhao on 2014-09-04 16:11:48.
//
//

#ifndef KUN__POINTOBJECT_H_INCLUDE
#define KUN__POINTOBJECT_H_INCLUDE

#include <ostream>
#include <kvs/GeometryObjectBase>
#include <kvs/ValueArray>
#include <kvs/Type>
#include <kvs/Vector3>
#include <kvs/RGBColor>
#include <kvs/Module>
#include <kvs/Indent>
#include <kvs/Deprecated>
#include <kvs/LineObject>
#include <kvs/PolygonObject>
#include <kvs/AnyValueArray>

namespace kun
{

/*==========================================================================*/
/**
 *  Point object class.
 */
/*==========================================================================*/
class PointObject : public kvs::GeometryObjectBase
{
    kvsModule( kun::PointObject, Object );
    kvsModuleBaseClass( kvs::GeometryObjectBase );

private:

    kvs::ValueArray<kvs::Real32> m_sizes;

    /*ADD*/
    kvs::AnyValueArray m_values;
    mutable kvs::Real64 m_min_value;
    mutable kvs::Real64 m_max_value;
    mutable bool m_has_min_max_values;
    size_t m_veclen;

public:

    PointObject();
    explicit PointObject( const kvs::LineObject& line );
    explicit PointObject( const kvs::PolygonObject& polygon );

    void add( const PointObject& other );
    void shallowCopy( const PointObject& other );
    void deepCopy( const PointObject& other );
    void clear();
    void print( std::ostream& os, const kvs::Indent& indent = kvs::Indent(0) ) const;

    void setSizes( const kvs::ValueArray<kvs::Real32>& sizes ) { m_sizes = sizes; }
    void setSize( const kvs::Real32 size );

    size_t numberOfSizes() const { return m_sizes.size(); }
    size_t numberOfValues() const { return m_values.size(); }

    kvs::Real32 size( const size_t index = 0 ) const { return m_sizes[index]; }
    const kvs::ValueArray<kvs::Real32>& sizes() const { return m_sizes; }

    /*ADD*/
    void setVeclen( const size_t veclen ) { m_veclen = veclen; }
    size_t veclen() const { return m_veclen; }
    void setValues( const kvs::AnyValueArray values ) { m_values = values; }
    kvs::AnyValueArray values() const { return m_values; }

    /*ADD*/
    void shuffle();
    void setMinMaxValues( const kvs::Real64 min_value, const kvs::Real64 max_value ) const;
    void updateMinMaxValues() const;
    void updateMinMaxExternalCoords();

    void setMinMaxRange( kvs::Vector3f min, kvs::Vector3f max ); // Clip the min max range of the point data

    /*ADD*/
    kvs::Real64 minValue() const { return m_min_value; }
    kvs::Real64 maxValue() const { return m_max_value; }

    // /*ADD*/
    // // Read or write KVSML file
    // void read( const std::string filename );
    // void write( const std::string filename );

public:
    KVS_DEPRECATED( PointObject(
                        const kvs::ValueArray<kvs::Real32>& coords,
                        const kvs::ValueArray<kvs::UInt8>& colors,
                        const kvs::ValueArray<kvs::Real32>& normals,
                        const kvs::ValueArray<kvs::Real32>& sizes ) )
    {
        this->setCoords( coords );
        this->setColors( colors );
        this->setNormals( normals );
        this->setSizes( sizes );
    }

    KVS_DEPRECATED( PointObject(
                        const kvs::ValueArray<kvs::Real32>& coords,
                        const kvs::ValueArray<kvs::UInt8>& colors,
                        const kvs::ValueArray<kvs::Real32>& normals,
                        const kvs::Real32 size ) )
    {
        this->setCoords( coords );
        this->setColors( colors );
        this->setNormals( normals );
        this->setSize( size );
    }

    KVS_DEPRECATED( PointObject(
                        const kvs::ValueArray<kvs::Real32>& coords,
                        const kvs::RGBColor& color,
                        const kvs::ValueArray<kvs::Real32>& normals,
                        const kvs::ValueArray<kvs::Real32>& sizes ) )
    {
        this->setCoords( coords );
        this->setColor( color );
        this->setNormals( normals );
        this->setSizes( sizes );
    }

    KVS_DEPRECATED( PointObject(
                        const kvs::ValueArray<kvs::Real32>& coords,
                        const kvs::ValueArray<kvs::Real32>& normals,
                        const kvs::ValueArray<kvs::Real32>& sizes ) )
    {
        this->setCoords( coords );
        this->setNormals( normals );
        this->setSizes( sizes );
    }

    KVS_DEPRECATED( PointObject(
                        const kvs::ValueArray<kvs::Real32>& coords,
                        const kvs::RGBColor& color,
                        const kvs::ValueArray<kvs::Real32>& normals,
                        const kvs::Real32 size ) )
    {
        this->setCoords( coords );
        this->setColor( color );
        this->setNormals( normals );
        this->setSize( size );
    }

    KVS_DEPRECATED( PointObject(
                        const kvs::ValueArray<kvs::Real32>& coords,
                        const kvs::ValueArray<kvs::UInt8>& colors,
                        const kvs::ValueArray<kvs::Real32>& sizes ) )
    {
        this->setCoords( coords );
        this->setColors( colors );
        this->setSizes( sizes );
    }

    KVS_DEPRECATED( PointObject(
                        const kvs::ValueArray<kvs::Real32>& coords,
                        const kvs::RGBColor& color,
                        const kvs::ValueArray<kvs::Real32>& sizes ) )
    {
        this->setCoords( coords );
        this->setColor( color );
        this->setSizes( sizes );
    }

    KVS_DEPRECATED( PointObject(
                        const kvs::ValueArray<kvs::Real32>& coords,
                        const kvs::ValueArray<kvs::UInt8>&  colors,
                        const kvs::Real32 size ) )
    {
        this->setCoords( coords );
        this->setColors( colors );
        this->setSize( size );
    }

    KVS_DEPRECATED( PointObject(
                        const kvs::ValueArray<kvs::Real32>& coords,
                        const kvs::RGBColor& color,
                        const kvs::Real32 size ) )
    {
        this->setCoords( coords );
        this->setColor( color );
        this->setSize( size );
    }

    KVS_DEPRECATED( PointObject(
                        const kvs::ValueArray<kvs::Real32>& coords ) )
    {
        this->setCoords( coords );
        this->setColor( kvs::RGBColor( 255, 255, 255 ) );
        this->setSize( 1 );
    }

    KVS_DEPRECATED( PointObject& operator += ( const PointObject& other ) )
    {
        this->add( other );
        return( *this );
    }

    KVS_DEPRECATED( size_t nsizes() const ) { return this->numberOfSizes(); }
    KVS_DEPRECATED( friend std::ostream& operator << ( std::ostream& os, const PointObject& object ) );
};

} // end of namespace kvs

#endif // KUN__POINTOBJECT_H_INCLUDE

