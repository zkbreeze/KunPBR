//
//  KVSMLObjectKunPoint.cpp
//
//
//  Created by Kun Zhao on 2014-10-03 11:22:45.
//
//

#include "KVSMLObjectKunPoint.h"
#include <Core/FileFormat/KVSML/DataReader.h>
#include <Core/FileFormat/KVSML/DataWriter.h>
#include <Core/FileFormat/KVSML/PointObjectTag.h>
#include <Core/FileFormat/KVSML/VertexTag.h>
#include <Core/FileFormat/KVSML/CoordTag.h>
#include <Core/FileFormat/KVSML/ColorTag.h>
#include <Core/FileFormat/KVSML/NormalTag.h>
#include <Core/FileFormat/KVSML/SizeTag.h>
#include <Core/FileFormat/KVSML/ValueTag.h>
#include <Core/FileFormat/KVSML/DataArrayTag.h>
#include <Core/FileFormat/KVSML/DataValueTag.h>
#include <kvs/XMLDocument>
#include <kvs/XMLDeclaration>
#include <kvs/XMLElement>
#include <kvs/XMLComment>
#include <kvs/ValueArray>
#include <kvs/File>
#include <kvs/Type>
#include <kvs/File>
#include <kvs/IgnoreUnusedVariable>
#include <iostream>
#include <fstream>
#include <sstream>


namespace kun
{

bool KVSMLObjectKunPoint::ReadValueData(
    const kvs::XMLNode::SuperClass* parent,
    const size_t nvertices,
    kvs::AnyValueArray* values )
{
    // <Value>
    kvs::kvsml::ValueTag value_tag;
    if ( value_tag.isExisted( parent ) )
    {
        if ( !value_tag.read( parent ) )
        {
            kvsMessageError( "Cannot read <%s>.", value_tag.name().c_str() );
            return false;
        }

        m_veclen = value_tag.veclen();

        // <DataArray>
        const size_t nelements = nvertices * m_veclen;
        kvs::kvsml::DataArrayTag data_tag;
        if ( !data_tag.read( value_tag.node(), nelements, values ) )
        {
            kvsMessageError( "Cannot read <%s> for <%s>.",
               data_tag.name().c_str(),
               value_tag.name().c_str() );
            return false;
        }
    }

    return true;
}

bool KVSMLObjectKunPoint::WriteValueData(
    kvs::XMLNode::SuperClass* parent,
    const kvs::kvsml::WritingDataType writing_type,
    const std::string& filename,
    const kvs::AnyValueArray values )
{
    // <Value>
    if ( values.size() > 0 )
    {
        kvs::kvsml::ValueTag value_tag;
        value_tag.setVeclen( m_veclen );
        if( m_has_minmax_values )
        {
            value_tag.setMinValue( m_min_value );
            value_tag.setMaxValue( m_max_value );
        }

        if ( !value_tag.write( parent ) )
        {
            kvsMessageError( "Cannot write <%s>.", value_tag.name().c_str() );
            return false;
        }

        // <DataArray>
        kvs::kvsml::DataArrayTag data_tag;
        if ( writing_type == kvs::kvsml::ExternalAscii )
        {
            data_tag.setFile( kvs::kvsml::DataArray::GetDataFilename( filename, "value" ) );
            data_tag.setFormat( "ascii" );
        }
        else if ( writing_type == kvs::kvsml::ExternalBinary )
        {
            data_tag.setFile( kvs::kvsml::DataArray::GetDataFilename( filename, "value" ) );
            data_tag.setFormat( "binary" );
        }

        const std::string pathname = kvs::File( filename ).pathName();
        if ( !data_tag.write( value_tag.node(), values, pathname ) )
        {
            kvsMessageError( "Cannot write <%s> for <%s>.",
               data_tag.name().c_str(),
               value_tag.name().c_str() );
            return false;
        }

    }

    return true;
}


/*===========================================================================*/
/**
 *  @brief  Checks the file extension.
 *  @param  filename [in] filename
 *  @return true, if the given filename has the supported extension
 */
/*===========================================================================*/
bool KVSMLObjectKunPoint::CheckExtension( const std::string& filename )
{
    const kvs::File file( filename );
    if ( file.extension() == "kvsml" || file.extension() == "KVSML" ||
         file.extension() == "xml"   || file.extension() == "XML" )
    {
        return true;
    }

    return false;
}

/*===========================================================================*/
/**
 *  @brief  Check the file format.
 *  @param  filename [in] filename
 *  @return true, if the KVSMLObjectKunPoint class can read the given file
 */
/*===========================================================================*/
bool KVSMLObjectKunPoint::CheckFormat( const std::string& filename )
{
    kvs::XMLDocument document;
    if ( !document.read( filename ) ) return false;

    // <KVSML>
    const std::string kvsml_tag("KVSML");
    const kvs::XMLNode::SuperClass* kvsml_node = kvs::XMLDocument::FindNode( &document, kvsml_tag );
    if ( !kvsml_node ) return false;

    // <Object>
    const std::string object_tag("Object");
    const kvs::XMLNode::SuperClass* object_node = kvs::XMLNode::FindChildNode( kvsml_node, object_tag );
    if ( !object_node ) return false;

    // <PointObject>
    const std::string point_tag("KunPointObject");
    const kvs::XMLNode::SuperClass* point_node = kvs::XMLNode::FindChildNode( object_node, point_tag );
    if ( !point_node ) return false;

    return true;
}

/*===========================================================================*/
/**
 *  @brief  Constructs a new KVSML point object class.
 */
/*===========================================================================*/
KVSMLObjectKunPoint::KVSMLObjectKunPoint():
    m_writing_type( kvs::kvsml::ExternalBinary ),
    m_veclen( 1 ),
    m_has_minmax_values( false )
{
}

/*===========================================================================*/
/**
 *  @brief  Constructs a new KVSML point object class.
 *  @param  filename [in] filename
 */
/*===========================================================================*/
KVSMLObjectKunPoint::KVSMLObjectKunPoint( const std::string& filename ):
    m_writing_type( kvs::kvsml::ExternalBinary ),
    m_veclen( 1 ),
    m_has_minmax_values( false )
{
    this->read( filename );
}

/*===========================================================================*/
/**
 *  @brief  Prints file information.
 *  @param  os [in] output stream
 *  @param  indent [in] indent for each line
 */
/*===========================================================================*/
void KVSMLObjectKunPoint::print( std::ostream& os, const kvs::Indent& indent ) const
{
    os << indent << "Filename : " << BaseClass::filename() << std::endl;
    os << indent << "Number of vertices: " << m_coords.size() / 3;
    os << indent << "Value type: " << m_values.typeInfo()->typeName() << std::endl;
}

/*===========================================================================*/
/**
 *  @brief  Read a KVSMl point object file.
 *  @param  filename [in] filename
 *  @return true, if the reading process is successfully
 */
/*===========================================================================*/
bool KVSMLObjectKunPoint::read( const std::string& filename )
{
    BaseClass::setFilename( filename );
    BaseClass::setSuccess( false );

    // XML document.
    kvs::XMLDocument document;
    if ( !document.read( filename ) )
    {
        kvsMessageError( "%s", document.ErrorDesc().c_str() );
        return false;
    }

    // <KVSML>
    m_kvsml_tag.read( &document );

    // <Object>
    if ( !m_object_tag.read( m_kvsml_tag.node() ) )
    {
        kvsMessageError( "Cannot read <%s>.", m_object_tag.name().c_str() );
        return false;
    }

    // <PointObject>
    kvs::kvsml::PointObjectTag point_tag;
    if ( !point_tag.read( m_object_tag.node() ) )
    {
        kvsMessageError( "Cannot read <%s>.", point_tag.name().c_str() );
        return false;
    }

    // <Vertex>
    kvs::kvsml::VertexTag vertex_tag;
    if ( !vertex_tag.read( point_tag.node() ) )
    {
        kvsMessageError( "Cannot read <%s>.", vertex_tag.name().c_str() );
        return false;
    }
    else
    {
        // Parent node.
        const kvs::XMLNode::SuperClass* parent = vertex_tag.node();

        // <Coord>
        const size_t ncoords = vertex_tag.nvertices();
        if ( !kvs::kvsml::ReadCoordData( parent, ncoords, &m_coords ) )
        {
            return false;
        }

        if ( m_coords.size() == 0 )
        {
            kvsMessageError( "Cannot read the coord data." );
            return false;
        }

        // <Color>
        const size_t ncolors = vertex_tag.nvertices();
        if ( !kvs::kvsml::ReadColorData( parent, ncolors, &m_colors ) )
        {
            return false;
        }

        if ( m_colors.size() == 0 )
        {
            // default value (black).
            m_colors.allocate(3);
            m_colors[0] = 0;
            m_colors[1] = 0;
            m_colors[2] = 0;
        }

        // <Normal>
        const size_t nnormals = vertex_tag.nvertices();
        if ( !kvs::kvsml::ReadNormalData( parent, nnormals, &m_normals ) )
        {
            return false;
        }

        // <Size>
        const size_t nsizes = vertex_tag.nvertices();
        if ( !kvs::kvsml::ReadSizeData( parent, nsizes, &m_sizes ) )
        {
            return false;
        }

        /*ADD*/
        // <Value>
        const size_t nvertices = vertex_tag.nvertices();
        if ( !this->ReadValueData( parent, nvertices, &m_values ) )
        {
            return false;
        }

    }

    BaseClass::setSuccess( true );
    return true;
}

/*===========================================================================*/
/**
 *  @brief  Writes the KVSML point object.
 *  @param  filename [in] filename
 *  @return true, if the writing process is done successfully
 */
/*===========================================================================*/
bool KVSMLObjectKunPoint::write( const std::string& filename )
{
    BaseClass::setFilename( filename );
    BaseClass::setSuccess( false );

    kvs::XMLDocument document;
    document.InsertEndChild( kvs::XMLDeclaration("1.0") );
    document.InsertEndChild( kvs::XMLComment(" Generated by kun::KVSMLObjectKunPoint::write() ") );

    // <KVSML>
    kvs::kvsml::KVSMLTag kvsml_tag;
    kvsml_tag.write( &document );

    // <Object type="KunPointObject">
    kvs::kvsml::ObjectTag object_tag;
    object_tag.setType( "KunPointObject" );
    if ( !object_tag.write( kvsml_tag.node() ) )
    {
        kvsMessageError( "Cannot write <%s>.", object_tag.name().c_str() );
        return false;
    }

    // <PointObject>
    kvs::kvsml::PointObjectTag point_tag;
    if ( !point_tag.write( object_tag.node() ) )
    {
        kvsMessageError( "Cannot write <%s>.", point_tag.name().c_str() );
        return false;
    }

    // <Vertex nvertices="xxx">
    const size_t dimension = 3;
    kvs::kvsml::VertexTag vertex_tag;
    vertex_tag.setNVertices( m_coords.size() / dimension );
    if ( !vertex_tag.write( point_tag.node() ) )
    {
        kvsMessageError( "Cannot write <%s>.", vertex_tag.name().c_str() );
        return false;
    }
    else
    {
        // Parent node and writing data type.
        kvs::XMLNode::SuperClass* parent = vertex_tag.node();
        const kvs::kvsml::WritingDataType type = m_writing_type;

        // <Coord>
        if ( !kvs::kvsml::WriteCoordData( parent, type, filename, m_coords ) )
        {
            return false;
        }

        // <Color>
        if ( !kvs::kvsml::WriteColorData( parent, type, filename, m_colors ) )
        {
            return false;
        }

        // <Normal>
        if ( !kvs::kvsml::WriteNormalData( parent, type, filename, m_normals ) )
        {
            return false;
        }

        // <Size>
        if ( !kvs::kvsml::WriteSizeData( parent, type, filename, m_sizes ) )
        {
            return false;
        }

        // <Value>
        if ( !this->WriteValueData( parent, type, filename, m_values ) )
        {
            return false;
        }
    }

    const bool success = document.write( filename );
    BaseClass::setSuccess( success );

    return success;
}

} // end of namespace kun
