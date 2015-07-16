//
//  ParticleBasedRendererGLSLPoint.cpp
//
//
//  Created by Kun Zhao on 2015-07-08 12:06:47.
//
//

#include "ParticleBasedRendererGLSLPoint.h"
#include <cmath>
#include <kvs/OpenGL>
#include "PointObject.h"
#include <kvs/Camera>
#include <kvs/Light>
#include <kvs/Assert>
#include <kvs/Math>
#include <kvs/MersenneTwister>
#include <kvs/Xorshift128>
#include <float.h>
#include "RadiusCalculator.h"


namespace
{

/*===========================================================================*/
/**
 *  @brief  Returns shuffled array.
 *  @param  values [in] value array
 *  @param  seed   [in] seed value for random number generator
 */
/*===========================================================================*/
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


}


namespace kun
{

/*===========================================================================*/
/**
 *  @brief  Constructs a new ParticleBasedRendererPoint class.
 */
/*===========================================================================*/
ParticleBasedRendererPoint::ParticleBasedRendererPoint():
    StochasticRendererBase( new Engine() )
{
    // The tranformation flag should be enabled in order to calculate the
    // object depth when calling the create-function of the engine. This is
    // only available for stochastic composition of multiple objects.
    // BaseClass::setEnabledTransformation( true );
}

/*===========================================================================*/
/**
 *  @brief  Constructs a new ParticleBasedRendererPoint class.
 *  @param  m [in] initial modelview matrix
 *  @param  p [in] initial projection matrix
 *  @param  v [in] initial viewport
 */
/*===========================================================================*/
ParticleBasedRendererPoint::ParticleBasedRendererPoint( const kvs::Mat4& m, const kvs::Mat4& p, const kvs::Vec4& v ):
    StochasticRendererBase( new Engine( m, p, v ) )
{
}

/*===========================================================================*/
/**
 *  @brief  Returns true if the particle shuffling is enabled
 *  @return true, if the shuffling is enabled
 */
/*===========================================================================*/
bool ParticleBasedRendererPoint::isEnabledShuffle() const
{
    return static_cast<const Engine&>( engine() ).isEnabledShuffle();
}

/*===========================================================================*/
/**
 *  @brief  Sets enable-flag for the particle shuffling.
 *  @param  enable [in] enable-flag
 */
/*===========================================================================*/
void ParticleBasedRendererPoint::setEnabledShuffle( const bool enable )
{
    static_cast<Engine&>( engine() ).setEnabledShuffle( enable );
}

/*===========================================================================*/
/**
 *  @brief  Enable the particle shuffling.
 */
/*===========================================================================*/
void ParticleBasedRendererPoint::enableShuffle()
{
    static_cast<Engine&>( engine() ).enableShuffle();
}

/*===========================================================================*/
/**
 *  @brief  Disable the particle shuffling.
 */
/*===========================================================================*/
void ParticleBasedRendererPoint::disableShuffle()
{
    static_cast<Engine&>( engine() ).disableShuffle();
}
    
void ParticleBasedRendererPoint::setParticleScale( float scale )
{
    static_cast<Engine&>( engine() ).setScale( scale );
}
    
void ParticleBasedRendererPoint::setTransferFunction( kvs::TransferFunction tfunc )
{
    static_cast<Engine&>( engine() ).setTransferFunction( tfunc );
}

// void ParticleBasedRendererPoint::setBaseOpacity( float base_opacity )
// {
//     static_cast<Engine&>( engine() ).setBaseOpacity( base_opacity );
// }

void ParticleBasedRendererPoint::setDensityVolume( kvs::StructuredVolumeObject* volume )
{
    static_cast<Engine&>( engine() ).setDensityVolume( volume );
}

    
/*===========================================================================*/
/**
 *  @brief  Constructs a new Engine class.
 */
/*===========================================================================*/
ParticleBasedRendererPoint::Engine::Engine():
    m_has_normal( false ),
    m_enable_shuffle( true ),
    m_random_index( 0 ),
    m_initial_modelview( kvs::Mat4::Zero() ),
    m_initial_projection( kvs::Mat4::Zero() ),
    m_initial_viewport( kvs::Vec4::Zero() ),
    m_initial_object_depth( 0 ),
    m_vbo( NULL ),
    m_particle_scale( 1.0 )
{
}

/*===========================================================================*/
/**
 *  @brief  Constructs a new Engine class.
 *  @param  m [in] initial modelview matrix
 *  @param  p [in] initial projection matrix
 *  @param  v [in] initial viewport
 */
/*===========================================================================*/
ParticleBasedRendererPoint::Engine::Engine( const kvs::Mat4& m, const kvs::Mat4& p, const kvs::Vec4& v ):
    m_has_normal( false ),
    m_enable_shuffle( true ),
    m_random_index( 0 ),
    m_initial_modelview( m ),
    m_initial_projection( p ),
    m_initial_viewport( v ),
    m_initial_object_depth( 0 ),
    m_vbo( NULL ),
    m_particle_scale( 1.0 )
{
}

/*===========================================================================*/
/**
 *  @brief  Destroys the Engine class.
 */
/*===========================================================================*/
ParticleBasedRendererPoint::Engine::~Engine()
{
    if ( m_vbo ) delete [] m_vbo;
}

/*===========================================================================*/
/**
 *  @brief  Releases the GPU resources.
 */
/*===========================================================================*/
void ParticleBasedRendererPoint::Engine::release()
{
    m_shader_program.release();
    for ( size_t i = 0; i < repetitionLevel(); i++ ) m_vbo[i].release();
}
    
void ParticleBasedRendererPoint::Engine::setScale( float scale )
{
    m_particle_scale = scale;
}

void ParticleBasedRendererPoint::Engine::setTransferFunction( kvs::TransferFunction tfunc )
{
    m_tfunc = tfunc;
    if ( m_transfer_function_texture.isLoaded() )
    {
        m_transfer_function_texture.release();
    }
}

// void ParticleBasedRendererPoint::Engine::setBaseOpacity( float base_opacity )
// {
//     m_base_opacity = base_opacity;
// }

void ParticleBasedRendererPoint::Engine::setDensityVolume( kvs::StructuredVolumeObject* volume )
{
    m_density_volume = volume;
}

/*===========================================================================*/
/**
 *  @brief  Create shaders, VBO, and framebuffers.
 *  @param  point [in] pointer to the point object
 *  @param  camera [in] pointer to the camera
 *  @param  light [in] pointer to the light
 */
/*===========================================================================*/
void ParticleBasedRendererPoint::Engine::create( kvs::ObjectBase* object, kvs::Camera* camera, kvs::Light* light )
{
    kun::PointObject* point = kun::PointObject::DownCast( object );
    m_has_normal = point->normals().size() > 0;
    if ( !m_has_normal ) setEnabledShading( false );

    float PI = 3.14159265359;
    float sampling_step = 0.5;
    m_max_alpha = 1.0 - std::exp( - ( PI * sampling_step )  );

    // Create resources.
    attachObject( object );
    createRandomTexture();
    this->create_shader_program();
    this->create_buffer_object( point );

    // Initial values for calculating the object depth.
    if ( kvs::Math::IsZero( m_initial_modelview[3][3] ) )
    {
        m_initial_modelview = kvs::OpenGL::ModelViewMatrix();
    }

    if ( kvs::Math::IsZero( m_initial_projection[3][3] ) )
    {
        m_initial_projection = kvs::OpenGL::ProjectionMatrix();
    }

    if ( kvs::Math::IsZero( m_initial_viewport[2] ) )
    {
        m_initial_viewport[2] = static_cast<float>( camera->windowWidth() );
        m_initial_viewport[3] = static_cast<float>( camera->windowHeight() );
    }

    const kvs::Vec4 I( point->objectCenter(), 1.0f );
    const kvs::Vec4 O = m_initial_projection * m_initial_modelview * I;
    m_initial_object_depth = O.z();
}

/*===========================================================================*/
/**
 *  @brief  Update.
 *  @param  point [in] pointer to the point object
 *  @param  camera [in] pointer to the camera
 *  @param  light [in] pointer to the light
 */
/*===========================================================================*/
void ParticleBasedRendererPoint::Engine::update( kvs::ObjectBase* object, kvs::Camera* camera, kvs::Light* light )
{
    m_initial_viewport[2] = static_cast<float>( camera->windowWidth() );
}

/*===========================================================================*/
/**
 *  @brief  Setup.
 *  @param  reset_count [in] flag for the repetition count
 */
/*===========================================================================*/
void ParticleBasedRendererPoint::Engine::setup( kvs::ObjectBase* object, kvs::Camera* camera, kvs::Light* light )
{
    resetRepetitions(); // counter must be reset with no referrence to 'reset_count'

    kvs::OpenGL::Enable( GL_DEPTH_TEST );
    kvs::OpenGL::Enable( GL_VERTEX_PROGRAM_POINT_SIZE );
    m_random_index = m_shader_program.attributeLocation("random_index");
    m_value_location = m_shader_program.attributeLocation("value");
}

/*===========================================================================*/
/**
 *  @brief  Draw an ensemble.
 *  @param  point [in] pointer to the point object
 *  @param  camera [in] pointer to the camera
 *  @param  light [in] pointer to the light
 */
/*===========================================================================*/
void ParticleBasedRendererPoint::Engine::draw( kvs::ObjectBase* object, kvs::Camera* camera, kvs::Light* light )
{
    kun::PointObject* point = kun::PointObject::DownCast( object );
    
    this->initialize_transfer_function_texture();
    this->initialize_density_volume_texture();

    float radius = kun::RadiusCalculator::CalculateObjectLength( 1.0, *camera, *object );
    // std::cout << radius << std::endl;

    kvs::VertexBufferObject::Binder bind1( m_vbo[ repetitionCount() ] );
    kvs::ProgramObject::Binder bind2( m_shader_program );
    kvs::Texture::Binder unit1( randomTexture(), 0 );
    kvs::Texture::Binder unit2( m_transfer_function_texture, 1 );
    kvs::Texture::Binder unit3( m_density_texture, 2 );
    {
        const kvs::Mat4& m0 = m_initial_modelview;
        const float scale0 = kvs::Vec3( m0[0][0], m0[1][0], m0[2][0] ).length();
        const float width0 = m_initial_viewport[2];
        const float height0 = m_initial_viewport[3];

        const kvs::Mat4 m = kvs::OpenGL::ModelViewMatrix();
        const float scale = kvs::Vec3( m[0][0], m[1][0], m[2][0] ).length();
        const float width = static_cast<float>( camera->windowWidth() );
        const float height = static_cast<float>( camera->windowHeight() );

        const float Cr = ( width / width0 ) * ( height / height0 );
        const float Cs = scale / scale0;
        const float D0 = m_initial_object_depth;
        const float object_depth = Cr * Cs * D0;

        const int level = repetitionLevel();
        const float minx = static_cast<float>( point->minObjectCoord().x() );
        const float miny = static_cast<float>( point->minObjectCoord().y() );
        const float minz = static_cast<float>( point->minObjectCoord().z() );
        const float maxx = static_cast<float>( point->maxObjectCoord().x() );
        const float maxy = static_cast<float>( point->maxObjectCoord().y() );
        const float maxz = static_cast<float>( point->maxObjectCoord().z() );
        const kvs::Vec3 coord_min( minx, miny, minz );
        const kvs::Vec3 coord_max( maxx, maxy, maxz );

        m_shader_program.setUniform( "object_depth", object_depth );
        m_shader_program.setUniform( "random_texture", 0 );
        m_shader_program.setUniform( "transfer_function_texture", 1 );
        m_shader_program.setUniform( "density_volume", 2 );
        m_shader_program.setUniform( "min", coord_min );
        m_shader_program.setUniform( "max", coord_max );
        m_shader_program.setUniform( "random_texture_size_inv", 1.0f / randomTextureSize() );
        m_shader_program.setUniform( "screen_scale", kvs::Vec2( width * 0.5f, height * 0.5f ) );
        m_shader_program.setUniform( "scale", m_particle_scale );
        m_shader_program.setUniform( "max_alpha", m_max_alpha );
        m_shader_program.setUniform( "Rad", radius );
        m_shader_program.setUniform( "repetition_level", level );
        
        const size_t nvertices = point->numberOfVertices();
        const size_t rem = nvertices % repetitionLevel();
        const size_t quo = nvertices / repetitionLevel();
        const size_t count = quo + ( repetitionCount() < rem ? 1 : 0 );
        const size_t coord_size = count * sizeof(kvs::Real32) * 3;
        const size_t normal_size = m_has_normal ? ( count * sizeof(kvs::Real32) * 3 ) : 0;

        const size_t coord_offset = 0;
        const size_t normal_offset = coord_offset + coord_size;
        const size_t value_offset = normal_offset + normal_size;
 
        // Enable coords.
        KVS_GL_CALL( glEnableClientState( GL_VERTEX_ARRAY ) );
        KVS_GL_CALL( glVertexPointer( 3, GL_FLOAT, 0, (GLbyte*)NULL + coord_offset ) );

        // Enable normals.
        if ( m_has_normal )
        {
            KVS_GL_CALL( glEnableClientState( GL_NORMAL_ARRAY ) );
            KVS_GL_CALL( glNormalPointer( GL_FLOAT, 0, (GLbyte*)NULL + normal_offset ) );
        }

        // Enable values.
        KVS_GL_CALL( glEnableVertexAttribArray( m_value_location ) );
        KVS_GL_CALL( glVertexAttribPointer( m_value_location, 1, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL + value_offset ) );

        // Enable random index.
        KVS_GL_CALL( glEnableVertexAttribArray( m_random_index ) );
        KVS_GL_CALL( glVertexAttribPointer( m_random_index, 2, GL_UNSIGNED_SHORT, GL_FALSE, 0, (GLubyte*)NULL + 0 ) );

        // Draw.
        KVS_GL_CALL( glDrawArrays( GL_POINTS, 0, count ) );

        // Disable coords.
        KVS_GL_CALL( glDisableClientState( GL_VERTEX_ARRAY ) );

        // Disable normals.
        if ( m_has_normal )
        {
            KVS_GL_CALL( glDisableClientState( GL_NORMAL_ARRAY ) );
        }

        // Disable values.
        KVS_GL_CALL( glDisableVertexAttribArray( m_value_location ) );

        // Disable random index.
        KVS_GL_CALL( glDisableVertexAttribArray( m_random_index ) );
    }

    // countRepetitions();
}

/*===========================================================================*/
/**
 *  @brief  Creates shader program.
 */
/*===========================================================================*/
void ParticleBasedRendererPoint::Engine::create_shader_program()
{
    std::cout << "Start to create shader program." << std::endl;
    const std::string shader_dir( std::getenv( "KUN_SHADER_DIR" ) );
    std::string vert_shader_source = shader_dir + std::string( "kun_PBR_zooming_point.vert" );
    std::string frag_shader_source = shader_dir + std::string( "kun_PBR_zooming.frag" );
    // std::string vert_shader_source = std::string( "${PROJECT_SOURCE_DIR}/lib/Shader/kun_PBR_zooming.vert");
    // std::string frag_shader_source = std::string( "${PROJECT_SOURCE_DIR}/lib/Shader/kun_PBR_zooming.frag");

    kvs::ShaderSource vert( vert_shader_source );
    kvs::ShaderSource frag( frag_shader_source );

    if ( isEnabledShading() )
    {
        switch ( shader().type() )
        {
        case kvs::Shader::LambertShading: frag.define("ENABLE_LAMBERT_SHADING"); break;
        case kvs::Shader::PhongShading: frag.define("ENABLE_PHONG_SHADING"); break;
        case kvs::Shader::BlinnPhongShading: frag.define("ENABLE_BLINN_PHONG_SHADING"); break;
        default: break; // NO SHADING
        }

        if ( kvs::OpenGL::Boolean( GL_LIGHT_MODEL_TWO_SIDE ) == GL_TRUE )
        {
            frag.define("ENABLE_TWO_SIDE_LIGHTING");
        }
    }

    m_shader_program.build( vert, frag );
    m_shader_program.bind();
    m_shader_program.setUniform( "shading.Ka", shader().Ka );
    m_shader_program.setUniform( "shading.Kd", shader().Kd );
    m_shader_program.setUniform( "shading.Ks", shader().Ks );
    m_shader_program.setUniform( "shading.S",  shader().S );
    m_shader_program.unbind();

}

/*===========================================================================*/
/**
 *  @brief  Creates buffer objects.
 *  @param  point [in] pointer to the point object
 */
/*===========================================================================*/
void ParticleBasedRendererPoint::Engine::create_buffer_object( const kun::PointObject* point )
{
    std::cout << "Start to create buffer object." << std::endl;

    kvs::ValueArray<kvs::Real32> coords = point->coords();
    kvs::ValueArray<kvs::Real32> normals = point->normals();
    kvs::ValueArray<kvs::Real32> values( static_cast<kvs::Real32*>( point->values().data() ), point->numberOfVertices() );

    /*ADD*/
    // Normalize the values
    float* pvalue = values.data();
    for( size_t i = 0; i < point->numberOfVertices(); i++ )
    {
        pvalue[i] = ( pvalue[i] - point->minValue() ) / ( point->maxValue() - point->minValue() );   
    }

    if ( m_enable_shuffle )
    {
        kvs::UInt32 seed = 12345678;
        coords = ::ShuffleArray<3>( point->coords(), seed );
        if ( m_has_normal )
        {
            normals = ::ShuffleArray<3>( point->normals(), seed );
        }
        values = ::ShuffleArray<1>( values, seed );
    }
    
    if ( !m_vbo ) m_vbo = new kvs::VertexBufferObject [ repetitionLevel() ];

    const size_t nvertices = point->numberOfVertices();
    const size_t rem = nvertices % repetitionLevel();
    const size_t quo = nvertices / repetitionLevel();
    for ( size_t i = 0; i < repetitionLevel(); i++ )
    {
        const size_t count = quo + ( i < rem ? 1 : 0 );
        const size_t first = quo * i + kvs::Math::Min( i, rem );
        const size_t coord_size = count * sizeof(kvs::Real32) * 3;
        const size_t normal_size = m_has_normal ? count * sizeof(kvs::Real32) * 3 : 0;
        const size_t value_size = count * sizeof(kvs::Real32);
        
        const size_t byte_size = coord_size + normal_size + value_size;
        m_vbo[i].create( byte_size );

        m_vbo[i].bind();
        m_vbo[i].load( coord_size, coords.data() + first * 3, 0 );
        if ( m_has_normal )
        {
            m_vbo[i].load( normal_size, normals.data() + first * 3, coord_size );
        }
        m_vbo[i].load( value_size, values.data() + first, coord_size + normal_size );
        
        m_vbo[i].unbind();
    }
}
    
void ParticleBasedRendererPoint::Engine::initialize_transfer_function_texture()
{
    const size_t width = m_tfunc.resolution();
    const kvs::ValueArray<kvs::Real32> table = m_tfunc.table();
    
    m_transfer_function_texture.release();
    m_transfer_function_texture.setWrapS( GL_CLAMP_TO_EDGE );
    m_transfer_function_texture.setMagFilter( GL_LINEAR );
    m_transfer_function_texture.setMinFilter( GL_LINEAR );
    m_transfer_function_texture.setPixelFormat( GL_RGBA32F_ARB, GL_RGBA, GL_FLOAT  );
    m_transfer_function_texture.create( width, table.data() );
}

void ParticleBasedRendererPoint::Engine::initialize_density_volume_texture()
{
    m_density_texture.release();

    const size_t width = m_density_volume->resolution().x();
    const size_t height = m_density_volume->resolution().y();
    const size_t depth = m_density_volume->resolution().z();

    m_density_texture.setPixelFormat( GL_ALPHA, GL_ALPHA, GL_FLOAT );
    m_density_texture.setWrapS( GL_CLAMP_TO_BORDER );
    m_density_texture.setWrapT( GL_CLAMP_TO_BORDER );
    m_density_texture.setWrapR( GL_CLAMP_TO_BORDER );
    m_density_texture.setMagFilter( GL_LINEAR );
    m_density_texture.setMinFilter( GL_LINEAR );
    m_density_texture.create( width, height, depth, m_density_volume->values().data() );
}


} // end of kun
