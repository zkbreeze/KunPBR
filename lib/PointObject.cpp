//
//  PointObject.cpp
//
//
//  Created by Kun Zhao on 2014-09-04 16:11:38.
//
//

#include "PointObject.h"
#include <cstring>
#include <kvs/LineObject>
#include <kvs/PolygonObject>
#include <kvs/Assert>
#include <kvs/Xorshift128>
#include <kvs/Range>
#include <kvs/VolumeObjectBase>

namespace
{

template <int Dim, typename T>
kvs::ValueArray<T> ShuffleArray( const kvs::ValueArray<T>& values, kvs::UInt32 seed )
{
    KVS_ASSERT( Dim > 0 );
    KVS_ASSERT( values.size() % Dim == 0 );

    kvs::Xorshift128 rng; rng.setSeed( seed );
    kvs::ValueArray<T> ret;
    if ( values.unique() ) { ret = values; }
    else { ret = values.clone(); }

    T* p = ret.data();
    size_t size = ret.size() / Dim;

    for ( size_t i = 0; i < size; ++i )
    {
        size_t j = rng.randInteger() % ( i + 1 );
        for ( int k = 0; k < Dim; ++k )
        {
            std::swap( p[ i * Dim + k ], p[ j * Dim + k ] );
        }
    }
    return ret;
}

template<typename T>
kvs::Range GetMinMaxValues( const kun::PointObject* point )
{
    KVS_ASSERT( point->values().size() != 0 );
    KVS_ASSERT( point->values().size() == point->veclen() * point->numberOfVertices() );

    const T* value = reinterpret_cast<const T*>( point->values().data() );
    const T* const end = value + point->numberOfVertices() * point->veclen();

    if ( point->veclen() == 1 )
    {
        T min_value = *value;
        T max_value = *value;

        while ( value < end )
        {
            min_value = kvs::Math::Min( *value, min_value );
            max_value = kvs::Math::Max( *value, max_value );
            ++value;
        }

        return kvs::Range( static_cast<double>( min_value ), static_cast<double>( max_value ) );
    }
    else
    {
        kvs::Real64 min_value = kvs::Value<kvs::Real64>::Max();
        kvs::Real64 max_value = kvs::Value<kvs::Real64>::Min();

        const size_t veclen = point->veclen();

        while ( value < end )
        {
            kvs::Real64 magnitude = 0.0;
            for ( size_t i = 0; i < veclen; ++i )
            {
                magnitude += static_cast<kvs::Real64>( ( *value ) * ( *value ) );
                ++value;
            }

            min_value = kvs::Math::Min( magnitude, min_value );
            max_value = kvs::Math::Max( magnitude, max_value );
        }

        return kvs::Range( std::sqrt( min_value ), std::sqrt( max_value ) );
    }

}

}

namespace kun
{

/*===========================================================================*/
/**
 *  @brief  Constructs a new PointObject class.
 */
/*===========================================================================*/
PointObject::PointObject():
m_has_min_max_values( false)
{
    BaseClass::setGeometryType( Point );
    this->setSize( 1 );
}

/*===========================================================================*/
/**
 *  @brief  Constructs a new PointObject class.
 *  @param  line [in] line object
 */
/*===========================================================================*/
PointObject::PointObject( const kvs::LineObject& line )
{
    BaseClass::setGeometryType( Point );
    BaseClass::setCoords( line.coords() );

    if( line.colorType() == kvs::LineObject::VertexColor )
    {
        BaseClass::setColors( line.colors() );
    }
    else
    {
        BaseClass::setColor( line.color() );
    }

    this->setSize( line.size() );

    BaseClass::setMinMaxObjectCoords(
        line.minObjectCoord(),
        line.maxObjectCoord() );

    BaseClass::setMinMaxExternalCoords(
        line.minExternalCoord(),
        line.maxExternalCoord() );
}

/*===========================================================================*/
/**
 *  @brief  Constructs a new PointObject class.
 *  @param  polygon [in] polygon object
 */
/*===========================================================================*/
PointObject::PointObject( const kvs::PolygonObject& polygon )
{
    BaseClass::setGeometryType( Point );
    BaseClass::setCoords( polygon.coords() );

    if( polygon.colorType() == kvs::PolygonObject::VertexColor )
    {
        BaseClass::setColors( polygon.colors() );
    }
    else
    {
        BaseClass::setColor( polygon.color() );
    }

    if( polygon.normalType() == kvs::PolygonObject::VertexNormal )
    {
        BaseClass::setNormals( polygon.normals() );
    }

    this->setSize( 1.0f );

    BaseClass::setMinMaxObjectCoords(
        polygon.minObjectCoord(),
        polygon.maxObjectCoord() );

    BaseClass::setMinMaxExternalCoords(
        polygon.minExternalCoord(),
        polygon.maxExternalCoord() );
}

/*===========================================================================*/
/**
 *  @brief  Add the point object.
 *  @param  other [in] point object
 */
/*===========================================================================*/
void PointObject::add( const PointObject& other )
{
    if ( this->coords().size() == 0 )
    {
        // Copy the object.
        BaseClass::setCoords( other.coords() );
        BaseClass::setNormals( other.normals() );
        BaseClass::setColors( other.colors() );
        this->setSizes( other.sizes() );

        BaseClass::setMinMaxObjectCoords(
            other.minObjectCoord(),
            other.maxObjectCoord() );
        BaseClass::setMinMaxExternalCoords(
            other.minExternalCoord(),
            other.maxExternalCoord() );
    }
    else
    {
        if ( !BaseClass::hasMinMaxObjectCoords() )
        {
            BaseClass::updateMinMaxCoords();
        }

        kvs::Vec3 min_object_coord( BaseClass::minObjectCoord() );
        kvs::Vec3 max_object_coord( BaseClass::maxObjectCoord() );

        min_object_coord.x() = kvs::Math::Min( min_object_coord.x(), other.minObjectCoord().x() );
        min_object_coord.y() = kvs::Math::Min( min_object_coord.y(), other.minObjectCoord().y() );
        min_object_coord.z() = kvs::Math::Min( min_object_coord.z(), other.minObjectCoord().z() );

        max_object_coord.x() = kvs::Math::Max( max_object_coord.x(), other.maxObjectCoord().x() );
        max_object_coord.y() = kvs::Math::Max( max_object_coord.y(), other.maxObjectCoord().y() );
        max_object_coord.z() = kvs::Math::Max( max_object_coord.z(), other.maxObjectCoord().z() );

        BaseClass::setMinMaxObjectCoords( min_object_coord, max_object_coord );
        BaseClass::setMinMaxExternalCoords( min_object_coord, max_object_coord );
        // Integrate the coordinate values.
        kvs::ValueArray<kvs::Real32> coords;
        const size_t ncoords = this->coords().size() + other.coords().size();
        coords.allocate( ncoords );
        kvs::Real32* pcoords = coords.data();

        // x,y,z, ... + x,y,z, ... = x,y,z, ... ,x,y,z, ...
        memcpy( pcoords, this->coords().data(), this->coords().byteSize() );
        memcpy( pcoords + this->coords().size(), other.coords().data(), other.coords().byteSize() );
        BaseClass::setCoords( coords );

        // Integrate the normal vectors.
        kvs::ValueArray<kvs::Real32> normals;
        if ( this->normals().size() > 0 )
        {
            if ( other.normals().size() > 0 )
            {
                // nx,ny,nz, ... + nx,ny,nz, ... = nx,ny,nz, ... ,nx,ny,nz, ...
                const size_t nnormals = this->normals().size() + other.normals().size();
                normals.allocate( nnormals );
                kvs::Real32* pnormals = normals.data();
                memcpy( pnormals, this->normals().data(), this->normals().byteSize() );
                memcpy( pnormals + this->normals().size(), other.normals().data(), other.normals().byteSize() );
            }
            else
            {
                // nx,ny,nz, ... + (none) = nx,ny,nz, ... ,0,0,0, ...
                const size_t nnormals = this->normals().size() + other.coords().size();
                normals.allocate( nnormals );
                kvs::Real32* pnormals = normals.data();
                memcpy( pnormals, this->normals().data(), this->normals().byteSize() );
                memset( pnormals + this->normals().size(), 0, other.coords().byteSize() );
            }
        }
        else
        {
            if ( other.normals().size() > 0 )
            {
                const size_t nnormals = this->coords().size() + other.normals().size();
                normals.allocate( nnormals );
                kvs::Real32* pnormals = normals.data();
                // (none) + nx,ny,nz, ... = 0,0,0, ... ,nz,ny,nz, ...
                memset( pnormals, 0, this->coords().byteSize() );
                memcpy( pnormals + this->coords().size(), other.normals().data(), other.normals().byteSize() );
            }
        }
        BaseClass::setNormals( normals );

        // Integrate the color values.
        kvs::ValueArray<kvs::UInt8> colors;

	if(this->colors().size() == 0 && other.colors().size() == 0 ){
	  goto colorskip;
	}

        if ( this->colors().size() > 1 )
        {
            if ( other.colors().size() > 1 )
            {
                // r,g,b, ... + r,g,b, ... = r,g,b, ... ,r,g,b, ...
                const size_t ncolors = this->colors().size() + other.colors().size();
                colors.allocate( ncolors );
                kvs::UInt8* pcolors = colors.data();
                memcpy( pcolors, this->colors().data(), this->colors().byteSize() );
                memcpy( pcolors + this->colors().size(), other.colors().data(), other.colors().byteSize() );
            }
            else
            {
                // r,g,b, ... + R,G,B = r,g,b, ... ,R,G,B, ... ,R,G,B
                const size_t ncolors = this->colors().size() + other.coords().size();
                colors.allocate( ncolors );
                kvs::UInt8* pcolors = colors.data();
                memcpy( pcolors, this->colors().data(), this->colors().byteSize() );
                pcolors += this->colors().size();
                const kvs::RGBColor color = other.color();
                for ( size_t i = 0; i < other.coords().size(); i += 3 )
                {
                    *(pcolors++) = color.r();
                    *(pcolors++) = color.g();
                    *(pcolors++) = color.b();
                }
            }
        }
        else
        {
            if ( other.colors().size() > 1 )
            {
                // R,G,B + r,g,b, ... = R,G,B, ... ,R,G,B, r,g,b, ...
                const size_t ncolors = this->coords().size() + other.colors().size();
                colors.allocate( ncolors );
                kvs::UInt8* pcolors = colors.data();
                const kvs::RGBColor color = this->color();
                for ( size_t i = 0; i < this->coords().size(); i += 3 )
                {
                    *(pcolors++) = color.r();
                    *(pcolors++) = color.g();
                    *(pcolors++) = color.b();
                }
                memcpy( pcolors, other.colors().data(), other.colors().byteSize() );
            }
            else
            {
                const kvs::RGBColor color1 = this->color();
                const kvs::RGBColor color2 = other.color();
                if ( color1 == color2 )
                {
                    // R,G,B + R,G,B = R,G,B
                    const size_t ncolors = 3;
                    colors.allocate( ncolors );
                    kvs::UInt8* pcolors = colors.data();
                    *(pcolors++) = color1.r();
                    *(pcolors++) = color1.g();
                    *(pcolors++) = color1.b();
                }
                else
                {
                    // R,G,B + R,G,B = R,G,B, ... ,R,G,B, ...
                    const size_t ncolors = this->coords().size() + other.coords().size();
                    colors.allocate( ncolors );
                    kvs::UInt8* pcolors = colors.data();
                    for ( size_t i = 0; i < this->coords().size(); i += 3 )
                    {
                        *(pcolors++) = color1.r();
                        *(pcolors++) = color1.g();
                        *(pcolors++) = color1.b();
                    }
                    for ( size_t i = 0; i < other.coords().size(); i += 3 )
                    {
                        *(pcolors++) = color2.r();
                        *(pcolors++) = color2.g();
                        *(pcolors++) = color2.b();
                    }
                }
            }
        }
        BaseClass::setColors( colors );

    colorskip:
	

        // Integrate the size values.
        kvs::ValueArray<kvs::Real32> sizes;
        if ( this->sizes().size() > 1 )
        {
            if ( other.sizes().size() > 1 )
            {
                // s, ... + s, ... = s, ... ,s, ...
                const size_t nsizes = this->sizes().size() + other.sizes().size();
                sizes.allocate( nsizes );
                kvs::Real32* psizes = sizes.data();
                memcpy( psizes, this->sizes().data(), this->sizes().byteSize() );
                memcpy( psizes + this->sizes().size(), other.sizes().data(), other.sizes().byteSize() );
            }
            else
            {
                // s, ... + S = s, ... ,S, ... ,S
                const size_t nsizes = this->sizes().size() + other.coords().size();
                sizes.allocate( nsizes );
                kvs::Real32* psizes = sizes.data();
                memcpy( psizes, this->sizes().data(), this->sizes().byteSize() );
                psizes += this->colors().size();
                const kvs::Real32 size = other.size();
                for ( size_t i = 0; i < other.coords().size(); i++ )
                {
                    *(psizes++) = size;
                }
            }
        }
        else
        {
            if ( other.sizes().size() > 1 )
            {
                // S + s, ... = S, ... ,S, s, ...
                const size_t nsizes = this->coords().size() + other.sizes().size();
                sizes.allocate( nsizes );
                kvs::Real32* psizes = sizes.data();
                const kvs::Real32 size = this->size();
                for ( size_t i = 0; i < this->coords().size(); i++ )
                {
                    *(psizes++) = size;
                }
                memcpy( psizes, other.sizes().data(), other.sizes().byteSize() );
            }
            else
            {
                const kvs::Real32 size1 = this->size();
                const kvs::Real32 size2 = other.size();
                if ( size1 == size2 )
                {
                    // S + S = S
                    const size_t nsizes = 1;
                    sizes.allocate( nsizes );
                    kvs::Real32* psizes = sizes.data();
                    *(psizes++) = size1;
                }
                else
                {
                    // S + S = S, ... , S, ...
                    const size_t nsizes = this->coords().size() + other.coords().size();
                    sizes.allocate( nsizes );
                    kvs::Real32* psizes = sizes.data();
                    for ( size_t i = 0; i < this->coords().size(); i++ )
                    {
                        *(psizes++) = size1;
                    }
                    for ( size_t i = 0; i < other.coords().size(); i++ )
                    {
                        *(psizes++) = size2;
                    }
                }
            }
        }
        this->setSizes( sizes );
    }
}

/*===========================================================================*/
/**
 *  @brief  Shallow copy the point object.
 *  @param  other [in] point object
 */
/*===========================================================================*/
void PointObject::shallowCopy( const PointObject& other )
{
    BaseClass::shallowCopy( other );
    m_sizes = other.sizes();
}

/*===========================================================================*/
/**
 *  @brief  Deep copy the point object.
 *  @param  other [in] point object
 */
/*===========================================================================*/
void PointObject::deepCopy( const PointObject& other )
{
    BaseClass::deepCopy( other );
    m_sizes = other.sizes().clone();
}

/*===========================================================================*/
/**
 *  @brief  Clear the point object.
 */
/*===========================================================================*/
void PointObject::clear()
{
    BaseClass::clear();
    m_sizes.release();
}

/*===========================================================================*/
/**
 *  @brief  Prints information of the point object.
 *  @param  os [in] output stream
 *  @param  indent [in] indent
 */
/*===========================================================================*/
void PointObject::print( std::ostream& os, const kvs::Indent& indent ) const
{
    os << indent << "Object type : " << "kun::PointObject" << std::endl;
    BaseClass::print( os, indent );
    os << indent << "Number of sizes : " << this->numberOfSizes() << std::endl;
    os << "Number of values:  " << this->numberOfValues() << std::endl;
}

/*===========================================================================*/
/**
 *  @brief  Sets a size value.
 *  @param  size [in] size value
 */
/*===========================================================================*/
void PointObject::setSize( const kvs::Real32 size )
{
    m_sizes.allocate( 1 );
    m_sizes[0] = size;
}

/*===========================================================================*/
/**
 *  @brief  '<<' operator
 */
/*===========================================================================*/
std::ostream& operator << ( std::ostream& os, const PointObject& object )
{
    os << "Object type:  " << "point object" << std::endl;
#ifdef KVS_COMPILER_VC
#if KVS_COMPILER_VERSION_LESS_OR_EQUAL( 8, 0 )
    // @TODO Cannot instance the object that is a abstract class here (error:C2259).
#endif
#else
    os << static_cast<const kvs::GeometryObjectBase&>( object ) << std::endl;
#endif
    os << "Number of sizes:  " << object.numberOfSizes();
    os << "Number of values:  " << object.numberOfValues();

    return os;
}

/*ADD*/
void PointObject::shuffle()
{
    kvs::UInt32 seed = 12345678;

    kvs::ValueArray<kvs::Real32> coords = ::ShuffleArray<3>( this->coords(), seed );
    kvs::ValueArray<kvs::Real32> normals = ::ShuffleArray<3>( this->normals(), seed );
    switch ( this->values().typeID() )
    {
        case kvs::Type::TypeInt8:   
        { 
            kvs::ValueArray<kvs::Int8> values( static_cast<kvs::Int8*>( this->values().data() ), this->values().size() );
            values = ::ShuffleArray<1>( values, seed );
            this->setValues( values );
            break; 
        }
        case kvs::Type::TypeInt16:
        { 
            kvs::ValueArray<kvs::Int16> values( static_cast<kvs::Int16*>( this->values().data() ), this->values().size() );
            values = ::ShuffleArray<1>( values, seed );
            this->setValues( values );
            break; 
        }
        case kvs::Type::TypeInt32:
        { 
            kvs::ValueArray<kvs::Int32> values( static_cast<kvs::Int32*>( this->values().data() ), this->values().size() );
            values = ::ShuffleArray<1>( values, seed );
            this->setValues( values );
            break;
        }
        case kvs::Type::TypeInt64:
        { 
            kvs::ValueArray<kvs::Int64> values( static_cast<kvs::Int64*>( this->values().data() ), this->values().size() );
            values = ::ShuffleArray<1>( values, seed );
            this->setValues( values );
            break; 
        }
        case kvs::Type::TypeUInt8:
        { 
            kvs::ValueArray<kvs::UInt8> values( static_cast<kvs::UInt8*>( this->values().data() ), this->values().size() );
            values = ::ShuffleArray<1>( values, seed );
            this->setValues( values );
            break;
        }
        case kvs::Type::TypeUInt16:
        { 
            kvs::ValueArray<kvs::UInt16> values( static_cast<kvs::UInt16*>( this->values().data() ), this->values().size() );
            values = ::ShuffleArray<1>( values, seed );
            this->setValues( values );
            break; 
        }
        case kvs::Type::TypeUInt32:
        { 
            kvs::ValueArray<kvs::UInt32> values( static_cast<kvs::UInt32*>( this->values().data() ), this->values().size() );
            values = ::ShuffleArray<1>( values, seed );
            this->setValues( values );
            break;
        }
        case kvs::Type::TypeUInt64:
        { 
            kvs::ValueArray<kvs::UInt64> values( static_cast<kvs::UInt64*>( this->values().data() ), this->values().size() );
            values = ::ShuffleArray<1>( values, seed );
            this->setValues( values );
            break; 
        }
        case kvs::Type::TypeReal32:
        { 
            kvs::ValueArray<kvs::Real32> values( static_cast<kvs::Real32*>( this->values().data() ), this->values().size() );
            values = ::ShuffleArray<1>( values, seed );
            this->setValues( values );
            break; 
        }
        case kvs::Type::TypeReal64:
        { 
            kvs::ValueArray<kvs::Real64> values( static_cast<kvs::Real64*>( this->values().data() ), this->values().size() );
            values = ::ShuffleArray<1>( values, seed );
            this->setValues( values );
            break; 
        }
        default: break;
    }


    this->setCoords( coords );
    this->setNormals( normals );

}

/*ADD*/
void PointObject::setMinMaxValues(
    const kvs::Real64 min_value,
    const kvs::Real64 max_value ) const
{
    m_min_value = min_value;
    m_max_value = max_value;
    m_has_min_max_values = true;
}

void PointObject::setMinMaxRange( kvs::Vector3f min, kvs::Vector3f max )
{
    std::vector<kvs::Real32> coords;
    std::vector<kvs::Real32> values; // Only float type values is considered

    const kvs::Real32* pcoords = this->coords().data();
    const kvs::Real32* pvalues = (const kvs::Real32*)this->values().data();

    for( size_t i = 0; i < this->numberOfVertices(); i++ )
    {
        int index = i * 3;
        if( pcoords[index] >= min.x() && pcoords[index] <= max.x()
            && pcoords[index + 1] >= min.y() && pcoords[index + 1] <= max.y() 
            && pcoords[index + 2] >= min.z() && pcoords[index + 2] <= max.z() )
        {
            coords.push_back( pcoords[index] );
            coords.push_back( pcoords[index + 1] );
            coords.push_back( pcoords[index + 2] );

            values.push_back( pvalues[i] );
        }
    }

    kvs::ValueArray<kvs::Real32> new_coords( coords.data(), coords.size() );
    kvs::ValueArray<kvs::Real32> new_values( values.data(), values.size() );
    this->setCoords( new_coords );
    this->setValues( new_values );
    this->updateMinMaxCoords();
    this->updateMinMaxValues();
    this->updateMinMaxExternalCoords();
    std::cout << "The point clip for the min max range was conducted." << std::endl;
}

/*ADD*/
void PointObject::updateMinMaxValues() const
{
    kvs::Range range;
    switch ( m_values.typeID() )
    {
    case kvs::Type::TypeInt8:   { range = ::GetMinMaxValues<kvs::Int8  >( this ); break; }
    case kvs::Type::TypeInt16:  { range = ::GetMinMaxValues<kvs::Int16 >( this ); break; }
    case kvs::Type::TypeInt32:  { range = ::GetMinMaxValues<kvs::Int32 >( this ); break; }
    case kvs::Type::TypeInt64:  { range = ::GetMinMaxValues<kvs::Int64 >( this ); break; }
    case kvs::Type::TypeUInt8:  { range = ::GetMinMaxValues<kvs::UInt8 >( this ); break; }
    case kvs::Type::TypeUInt16: { range = ::GetMinMaxValues<kvs::UInt16>( this ); break; }
    case kvs::Type::TypeUInt32: { range = ::GetMinMaxValues<kvs::UInt32>( this ); break; }
    case kvs::Type::TypeUInt64: { range = ::GetMinMaxValues<kvs::UInt64>( this ); break; }
    case kvs::Type::TypeReal32: { range = ::GetMinMaxValues<kvs::Real32>( this ); break; }
    case kvs::Type::TypeReal64: { range = ::GetMinMaxValues<kvs::Real64>( this ); break; }
    default: break;
    }

    this->setMinMaxValues( range.lower(), range.upper() );
}

void PointObject::updateMinMaxExternalCoords()
{
    this->setMinMaxExternalCoords( this->minObjectCoord(), this->maxObjectCoord() );
}

} // end of namespace kun
