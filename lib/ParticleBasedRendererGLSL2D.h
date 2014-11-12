//
//  ParticleBasedRendererGLSL2D.h
//
//
//  Created by Kun Zhao on 2014-10-24 13:34:28.
//
//

#ifndef KUN__PARTICLEBASEDRENDERERGLSL2D_H_INCLUDE
#define KUN__PARTICLEBASEDRENDERERGLSL2D_H_INCLUDE
 
#include <kvs/RendererBase>
#include <kvs/Module>
#include <kvs/ProgramObject>
#include <kvs/VertexBufferObject>
#include <kvs/Deprecated>
#include <kvs/EnsembleAverageBuffer>
#include <kvs/StochasticRendererBase>
#include <kvs/StochasticRenderingEngine>
#include <kvs/Texture2D>
#include <kvs/TransferFunction>

namespace kun { class PointObject; }

namespace kun
{

/*===========================================================================*/
/**
 *  @brief  Particle-based renderer class.
 */
/*===========================================================================*/
 class ParticleBasedRenderer2D : public kvs::StochasticRendererBase
 {
    kvsModule( kun::ParticleBasedRenderer2D, Renderer );
    kvsModuleBaseClass( kvs::StochasticRendererBase );

public:

    class Engine;

public:

    ParticleBasedRenderer2D();
    ParticleBasedRenderer2D( const kvs::Mat4& m, const kvs::Mat4& p, const kvs::Vec4& v );
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
    void setBaseOpacity( float base_opacity );

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
 class ParticleBasedRenderer2D::Engine : public kvs::StochasticRenderingEngine
 {
 private:

    bool m_has_normal; ///< check flag for the normal array
    bool m_enable_shuffle; ///< flag for shuffling particles
    size_t m_random_index; ///< index used for refering the random texture
    size_t m_value_location; ///< index used for refering the size array
    size_t m_value2_location;
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
    // transfer function 2D
    float* m_tfunc_2d;
    kvs::Texture2D m_transfer_function_texture_2d;
    float m_tfunc_2d_width;
    float m_tfunc_2d_height;

    // base opacity
    float m_base_opacity;
    

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
    void setBaseOpacity( float base_opacity );

private:

    void create_shader_program();
    void create_buffer_object( const kun::PointObject* point );
    
    void initialize_transfer_function_texture();
};

} // end of namespace kun
 
#endif // KUN__PARTICLEBASEDRENDERERGLSL2D_H_INCLUDE