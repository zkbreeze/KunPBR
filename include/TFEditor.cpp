//
//  TFEditor.cpp
//
//
//  Created by Kun Zhao on 2016-08-17 13:30:48.
//
//

#include "TFEditor.h"

namespace kun
{

void RayTFEditor::apply()
{
	kvs::glut::Screen* glut_screen = static_cast<kvs::glut::Screen*>( screen() );
	kvs::RendererBase* r = glut_screen->scene()->rendererManager()->renderer();
	kvs::glsl::RayCastingRenderer* renderer = static_cast<kvs::glsl::RayCastingRenderer*>( r );
	renderer->setTransferFunction( transferFunction() );
	std::cout << "TF Renderer time: " << renderer->timer().msec() << std::endl;
	screen()->redraw();
}

void ParticleTFEditor::setKunPointObject( kun::PointObject* point )
{
	m_point = point;
	kvs::StructuredVolumeObject* object = new kvs::StructuredVolumeObject();
	object->setGridType( kvs::StructuredVolumeObject::Uniform );
	object->setVeclen( 1 );
	object->setResolution( kvs::Vector3ui( 1, 1, point->numberOfVertices() ) );
	object->setValues( point->values() );
	object->updateMinMaxValues();
	object->updateMinMaxCoords();
	this->setVolumeObject( object );
}

void ParticleTFEditor::apply()
{
	kvs::glut::Screen* glut_screen = static_cast<kvs::glut::Screen*>( screen() );
	kvs::RendererBase* r = glut_screen->scene()->rendererManager()->renderer();
	kun::ParticleBasedRenderer* renderer = static_cast<kun::ParticleBasedRenderer*>( r );
	renderer->setTransferFunction( transferFunction() );
	renderer->setBaseOpacity( m_base_opactiy );
	std::cout << "TF Renderer time: " << renderer->timer().msec() << std::endl;
	screen()->redraw();
}

} // end of namespace kun