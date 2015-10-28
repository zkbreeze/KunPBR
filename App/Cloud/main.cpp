//
//  main.cpp
//
//
//  Created by Kun Zhao on 2015-10-28 15:02:48.
//
//

#include <kvs/glut/Application>
#include <kvs/glut/Screen>
#include <kvs/TransferFunction>
#include <kvs/glut/TransferFunctionEditor>
#include <kvs/RendererManager>
#include <kvs/ObjectManager>
#include <kvs/CommandLine>
#include <kvs/HydrogenVolumeData>
#include "ParticleBasedRendererGLSLPoint.h"

#include "CloudObject.h"
#include "PointObject.h"

class TransferFunctionEditor : public kvs::glut::TransferFunctionEditor
{

public:
	TransferFunctionEditor( kvs::ScreenBase* screen ) :
	kvs::glut::TransferFunctionEditor( screen )
	{
	}

	void apply( void )
	{
		kvs::glut::Screen* glut_screen = static_cast<kvs::glut::Screen*>( screen() );
		kvs::RendererBase* r = glut_screen->scene()->rendererManager()->renderer();
		kun::ParticleBasedRendererPoint* renderer = static_cast<kun::ParticleBasedRendererPoint*>( r );
		renderer->setEnabledSizesMode();
		// renderer->setShader( kvs::Shader::Phong( 0.6, 0.4, 0, 1 ) );
		renderer->setTransferFunction( transferFunction() );
		renderer->setRepetitionLevel( 1 );
		std::cout << "TF adjust time: " << renderer->timer().msec() << std::endl;
		screen()->redraw();
	}
};

int main( int argc, char** argv )
{
	kvs::glut::Application app( argc, argv );
	kvs::glut::Screen screen( &app );

	kvs::CommandLine param( argc, argv );
	param.addHelpOption();
	// param.addOption( "d", "Input the folder name", 1, false );
	param.addOption( "f", "Input filename", 1, false );
	param.addOption( "p", "Part name of the MPI file", 1, false );
	param.addOption( "n", "Number of processes", 1, false );
	if( !param.parse() ) return 1;

	kun::CloudObject* cloud = new kun::CloudObject();
	if( param.hasOption( "f" ) ) cloud->read( param.optionValue<std::string>( "f" ) );
	if( param.hasOption( "p" ) ) cloud->read( param.optionValue<std::string>( "p" ), param.optionValue<size_t>( "n" ) );

	kun::PointObject* point = cloud->toKUNPointObject( 1 );

	kvs::TransferFunction tfunc( 256 );
	kun::ParticleBasedRendererPoint* renderer = new kun::ParticleBasedRendererPoint();
	renderer->setEnabledSizesMode();
	renderer->setTransferFunction( tfunc );
	renderer->setRepetitionLevel( 1 );
	renderer->setParticleScale( 10.0 );

	screen.registerObject( point, renderer );
	screen.setBackgroundColor( kvs::RGBColor::Black() );
	screen.show();

	// Set the transfer function editor
	kvs::StructuredVolumeObject* object = new kvs::StructuredVolumeObject();
	object->setGridType( kvs::StructuredVolumeObject::Uniform );
	object->setVeclen( 1 );
	object->setResolution( kvs::Vector3ui( 1, 1, point->numberOfVertices() ) );
	object->setValues( point->values() );
	object->updateMinMaxValues();

	TransferFunctionEditor editor( &screen );
	editor.setVolumeObject( object );
	editor.setTransferFunction( tfunc );
	editor.show();

	return app.run();
}