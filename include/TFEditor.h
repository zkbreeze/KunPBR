//
//  TFEditor.h
//
//
//  Created by Kun Zhao on 2016-08-16 18:16:42.
//
//

#ifndef KUN__TFEDITOR_H_INCLUDE
#define KUN__TFEDITOR_H_INCLUDE
 
#include <kvs/glut/TransferFunctionEditor>
#include <kvs/RendererManager>
#include <kvs/ObjectManager>
#include <kvs/RayCastingRenderer>
#include "ParticleBasedRenderer.h"
#include "PointObject.h"

namespace kun
{

class RayTFEditor : public kvs::glut::TransferFunctionEditor
{

public:
	RayTFEditor( kvs::glut::Screen* screen ) : 
	kvs::glut::TransferFunctionEditor( screen ){}

	void apply();
};

class ParticleTFEditor : public kvs::glut::TransferFunctionEditor
{

	float m_base_opactiy;
	kun::PointObject* m_point;

public:
	ParticleTFEditor( kvs::glut::Screen* screen ) : 
	kvs::glut::TransferFunctionEditor( screen ){}

	void setBaseOpacity( float opacity ){ m_base_opactiy = opacity; }
	void setKunPointObject( kun::PointObject* point );
	void apply();
};

} // end of namespace kun

 
#endif // KUN__TFEDITOR_H_INCLUDE