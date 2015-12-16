//
//  main.cpp
//
//
//  Created by Kun Zhao on 2015-10-08 10:21:07.
//
//

#include <kvs/glut/Application>
#include <kvs/glut/Screen>
#include "ParticleTransparentRendererGLSL.h"
#include "PointObject.h"
#include "PointImporter.h"
#include <kvs/TransferFunction>
#include <kvs/RGBFormulae>

int main( int argc, char** argv )
{
	kvs::glut::Application app( argc, argv );
	kvs::glut::Screen screen( &app );

	kun::PointObject* point = new kun::PointImporter( argv[1] );
	kun::ParticleTransparentRenderer* renderer = new kun::ParticleTransparentRenderer();

	kvs::TransferFunction tfunc = kvs::RGBFormulae::Ocean( 256 );
	renderer->setTransferFunction( tfunc );

	screen.registerObject( point, renderer );
	// screen.setBackgroundColor( kvs::RGBColor::Black() );

	screen.show();

	return app.run();
}