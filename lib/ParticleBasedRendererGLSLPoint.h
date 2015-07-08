//
//  ParticleBasedRendererGLSLPoint.h
//
//
//  Created by Kun Zhao on 2015-07-08 12:06:38.
//
//

#ifndef KUN__PARTICLEBASEDRENDERERGLSLPOINT_H_INCLUDE
#define KUN__PARTICLEBASEDRENDERERGLSLPOINT_H_INCLUDE

#include <kvs/RendererBase>
#include <kvs/Module>
#include <kvs/ProgramObject>
#include <kvs/VertexBufferObject>
#include <kvs/Deprecated>
#include <kvs/EnsembleAverageBuffer>
#include <kvs/StochasticRendererBase>
#include <kvs/StochasticRenderingEngine>
#include <kvs/Texture1D>
#include <kvs/Texture3D>
#include <kvs/TransferFunction>
#include <kvs/StructuredVolumeObject>

namespace kun { class PointObject; }

namespace kun
{

/*===========================================================================*/
/**
 *  @brief  Particle-based renderer class.
 */
/*===========================================================================*/
 class ParticleBasedRendererPoint : public kvs::StochasticRendererBase
 {
    kvsModule( kun::ParticleBasedRendererPoint, Renderer );
    kvsModuleBaseClass( kvs::StochasticRendererBase );

public:

    class Engine;

public:

    ParticleBasedRendererPoint();
    ParticleBasedRendererPoint( const kvs::Mat4& m, const kvs::Mat4& p, const kvs::Vec4& v );
    bool isEnabledShuffle() const;
    void setEnabledShuffle( const bool enable );
    void enableShuffle();
    void disableShuffle();
    const kvs::Mat4& initialModelViewMatrix() const;
    const kvs::Mat4& initialProjectionMatrix() const;
    const kvs::Vec4& initialViewport() const;

    // ADD
    void setParticleScale( float scale );
    void setTransferFunction( kvs::TransferFunction tfunc );
    // void setBaseOpacity( float base_opacity );
    void setDensityVolume( kvs::StructuredVolumeObject* volume );

private:
    // Not supported progressive refinement rendering.
    bool isEnabledRefinement() const;
    void enableRefinement();
    void disableRefinement();
};

/*===========================================================================*/
/**
 *  @brief  Engine class for particle-based renderer.
 */
/*===========================================================================*/
 class ParticleBasedRendererPoint::Engine : public kvs::StochasticRenderingEngine
 {
 private:

    bool m_has_normal; ///< check flag for the normal array
    bool m_enable_shuffle; ///< flag for shuffling particles
    size_t m_random_index; ///< index used for refering the random texture
    size_t m_value_location; ///< index used for refering the size array
    kvs::Mat4 m_initial_modelview; ///< initial modelview matrix
    kvs::Mat4 m_initial_projection; ///< initial projection matrix
    kvs::Vec4 m_initial_viewport; ///< initial viewport
    float m_initial_object_depth; ///< initial object depth
    kvs::ProgramObject m_shader_program; ///< zooming shader program
    kvs::VertexBufferObject* m_vbo; ///< vertex buffer objects for each repetition
    
    float m_particle_scale;
    float m_max_alpha;

    // transfer function
    kvs::TransferFunction m_tfunc;
    kvs::Texture1D m_transfer_function_texture; ///< transfer function texture

    // base opacity
    // float m_base_opacity;
    kvs::StructuredVolumeObject* m_density_volume;    
    kvs::Texture3D m_density_texture;

public:

    Engine();
    Engine( const kvs::Mat4& m, const kvs::Mat4& p, const kvs::Vec4& v );
    virtual ~Engine();
    void release();
    void create( kvs::ObjectBase* object, kvs::Camera* camera, kvs::Light* light );
    void update( kvs::ObjectBase* object, kvs::Camera* camera, kvs::Light* light );
    void setup( kvs::ObjectBase* object, kvs::Camera* camera, kvs::Light* light );
    void draw( kvs::ObjectBase* object, kvs::Camera* camera, kvs::Light* light );

    bool isEnabledShuffle() const { return m_enable_shuffle; }
    void setEnabledShuffle( const bool enable ) { m_enable_shuffle = enable; }
    void enableShuffle() { this->setEnabledShuffle( true ); }
    void disableShuffle() { this->setEnabledShuffle( false ); }
    const kvs::Mat4& initialModelViewMatrix() const { return m_initial_modelview; }
    const kvs::Mat4& initialProjectionMatrix() const { return m_initial_projection; }
    const kvs::Vec4& initialViewport() const { return m_initial_viewport; }
    
    // ADD
    void setScale( float scale );
    void setTransferFunction( kvs::TransferFunction tfunc );
    // void setBaseOpacity( float base_opacity );
    void setDensityVolume( kvs::StructuredVolumeObject* volume );

private:

    void create_shader_program();
    void create_buffer_object( const kun::PointObject* point );
    
    void initialize_transfer_function_texture();
    void initialize_density_volume_texture();
};

} // end of namespace kun

#endif // KUN__PARTICLEBASEDRENDERERGLSLPOINT_H_INCLUDE


