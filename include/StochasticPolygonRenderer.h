//
//  StochasticPolygonRenderer.h
//
//
//  Created by Kun Zhao on 2015-11-25 20:36:47.
//
//

#ifndef KVS__STOCHASTIC_POLYGON_RENDERER_H_INCLUDE
#define KVS__STOCHASTIC_POLYGON_RENDERER_H_INCLUDE

#include <kvs/Module>
#include <kvs/ProgramObject>
#include <kvs/VertexBufferObject>
#include <kvs/IndexBufferObject>
#include <kvs/StochasticRenderingEngine>
#include <kvs/StochasticRendererBase>
#include <kvs/PolygonObject>

namespace kun
{

class PolygonObject;

/*===========================================================================*/
/**
 *  @brief  Stochastic polygon renderer class.
 */
/*===========================================================================*/
class StochasticPolygonRenderer : public kvs::StochasticRendererBase
{
    kvsModule( kun::StochasticPolygonRenderer, Renderer );
    kvsModuleBaseClass( kvs::StochasticRendererBase );

public:

    class Engine;

public:

    StochasticPolygonRenderer();
    void setPolygonOffset( const float polygon_offset );
};

/*===========================================================================*/
/**
 *  @brief  Engine class for stochastic polygon renderer.
 */
/*===========================================================================*/
class StochasticPolygonRenderer::Engine : public kvs::StochasticRenderingEngine
{
private:

    bool m_has_normal; ///< check flag for the normal array
    bool m_has_connection; ///< check flag for the connection array
    size_t m_random_index; ///< index used for refering the random texture
    float m_polygon_offset; ///< polygon offset
    kvs::ProgramObject m_shader_program; ///< shader program
    kvs::VertexBufferObject m_vbo; ///< vertex buffer object
    kvs::IndexBufferObject m_ibo; ///< index buffer object

public:

    Engine();
    void release();
    void create( kvs::ObjectBase* object, kvs::Camera* camera, kvs::Light* light );
    void update( kvs::ObjectBase* object, kvs::Camera* camera, kvs::Light* light );
    void setup( kvs::ObjectBase* object, kvs::Camera* camera, kvs::Light* light );
    void draw( kvs::ObjectBase* object, kvs::Camera* camera, kvs::Light* light );

    void setPolygonOffset( const float offset )
    {
        m_polygon_offset = offset;
    }

private:

    void create_shader_program();
    void create_buffer_object( const kvs::PolygonObject* polygon );
};

} // end of namespace kvs

#endif // KVS__STOCHASTIC_POLYGON_RENDERER_H_INCLUDE
