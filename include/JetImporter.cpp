//
//  JetImporter.cpp
//  
//
//  Created by Kun Zhao on 12/06/06.
//  Copyright (c) 2012 Kun Zhao. All rights reserved.
//

#include "JetImporter.h"

#include <kvs/File>
#include <kvs/Message>
#include <kvs/Endian>
#include <cstdlib>
#include <cstring>

namespace kun
{

JetImporter::JetImporter( void )
{
}

JetImporter::JetImporter( 
                         const std::string& filename, 
                         size_t ResolutionX,
                         size_t ResolutionY,
                         size_t ResolutionZ
                         )   
{
    m_resolution = kvs::Vector3ui( ResolutionX, ResolutionY, ResolutionZ );
    this->import( filename );
}
    
JetImporter::JetImporter( const std::string& filename )
{
    m_resolution = kvs::Vector3ui( 480, 720, 120 );
    this->import( filename );
}
    
JetImporter::~JetImporter( void )
{
}

const kvs::Vector3ui& JetImporter::resolution( void )
{
    return( m_resolution );
}

const kvs::AnyValueArray& JetImporter::values( void )
{
    return( m_values );
}

const bool JetImporter::import( const std::string& filename )
{
    FILE *Jetfile = fopen( filename.c_str(), "rb" );
    size_t nx = m_resolution.x();
    size_t ny = m_resolution.y();
    size_t nz = m_resolution.z();
    size_t length = nx * ny * nz;
    
    m_values.allocate<float>( length );
    float* buf = static_cast<float*>( m_values.data() );
    fread( buf, sizeof( float ), length, Jetfile );
    fclose(Jetfile) ;
    
    const kvs::StructuredVolumeObject::GridType grid_type = kvs::StructuredVolumeObject::Uniform;
    
    setGridType( grid_type );
    setResolution( m_resolution );
    setVeclen( 1 );
    setValues( m_values );
    updateMinMaxCoords();
    updateMinMaxValues();
    
    return( true );
}

const bool JetImporter::write( const std::string& filename )
{
    return( true );
    //now it is not needed
}

}
