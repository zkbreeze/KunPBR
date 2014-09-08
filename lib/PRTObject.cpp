//
//  PRTObject.cpp
//
//
//  Created by Kun Zhao on 2014-09-07 21:06:02.
//
//

#include "PRTObject.h"

namespace kun
{
    
    PRTObject::PRTObject( void )
    {
    }
    
    PRTObject::PRTObject( std::string filename )
    {
        this->read( filename );
        this->setMinNutrition( 0.0 );
        this->setMaxNutrition( 300.0 );
    }
    
    void PRTObject::read( std::string filename )
    {
        std::ifstream infile( filename.c_str() );
        infile.seekg( 0, infile.end );
        size_t length = infile.tellg();
        infile.seekg( 0, infile.beg );
        m_size = length / 101;
        
        m_x = new float[m_size];
        m_y = new float[m_size];
        m_z = new float[m_size];
        m_nutri = new float[m_size];
        m_depth = new float[m_size];
        m_i = new int[m_size];
        m_j = new int[m_size];
        m_k = new int[m_size];
        m_ID = new int[m_size];
        m_fishID = new char[m_size];
        
        for (size_t i = 0 ; i < m_size; i++)
        {
            infile >> m_x[i];
            infile >> m_y[i];
            infile >> m_z[i];
            infile >> m_nutri[i];
            infile >> m_depth[i];
            infile >> m_i[i];
            infile >> m_j[i];
            infile >> m_k[i];
            infile >> m_ID[i];
            infile >> m_fishID[i];
        }
    }
    
    size_t PRTObject::size()
    {
        return m_size;
    }
    
    float* PRTObject::x()
    {
        return m_x;
    }
    
    float* PRTObject::y()
    {
        return m_y;
    }
    
    float* PRTObject::z()
    {
        return m_z;
    }
    
    float* PRTObject::nutrition()
    {
        return m_nutri;
    }
    
    void PRTObject::setMinNutrition( float min )
    {
        m_min_nutrition = min;
    }
    
    void PRTObject::setMaxNutrition( float max )
    {
        m_max_nutrition = max;
    }
        
    kun::PointObject* PRTObject::toKUNPointObject( void )
    {
        kun::PointObject* point = new kun::PointObject();
        
        float* coords_buffer = new float[m_size * 3];
        float* sizes_buffer = new float[m_size];
        float range = m_max_nutrition - m_min_nutrition;
        
        for( size_t i = 0; i < m_size; i++ )
        {   
            if( m_nutri[i] < m_min_nutrition ) m_nutri[i] = m_min_nutrition;
            if( m_nutri[i] > m_max_nutrition ) m_nutri[i] = m_max_nutrition;
            m_nutri[i] -= m_min_nutrition;
            m_nutri[i] /= range;
            sizes_buffer[i] = m_nutri[i];

            coords_buffer[i * 3] = m_x[i];
            coords_buffer[i * 3 + 1] = m_y[i];
            coords_buffer[i * 3 + 2] = 0.0;
        }
        
        kvs::ValueArray<kvs::Real32>coords( coords_buffer, m_size * 3 );
        kvs::ValueArray<kvs::Real32>sizes( sizes_buffer, m_size );
        
        point->setCoords( coords );
        point->setSizes( sizes );
        
        return point;
    }

    kvs::PointObject* PRTObject::toKVSPointObject( void )
    {
        kvs::PointObject* point = new kvs::PointObject();
        
        float* coords_buffer = new float[m_size * 3];
        unsigned char* colors_buffer = new unsigned char[m_size * 3];
        
        for( size_t i = 0; i < m_size; i++ )
        {   
            coords_buffer[i * 3] = m_x[i];
            coords_buffer[i * 3 + 1] = m_y[i];
            coords_buffer[i * 3 + 2] = 0.0;

            colors_buffer[i * 3] = 100;
            colors_buffer[i * 3 + 1] = 100;
            colors_buffer[i * 3 + 2] = 100;
        }
        
        kvs::ValueArray<kvs::Real32>coords( coords_buffer, m_size * 3 );
        kvs::ValueArray<kvs::UInt8>colors( colors_buffer, m_size * 3 );
        
        point->setCoords( coords );
        point->setColors( colors );
        
        return point;
    }
}
