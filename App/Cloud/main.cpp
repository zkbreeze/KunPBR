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
#include "ParticleBasedRenderer.h"

#include "CloudObject.h"
#include "PointObject.h"

namespace
{
	int repetition_level = 30;
	float base_opacity = 0.2;
} // end of namespace kun

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
		kun::ParticleBasedRenderer* renderer = static_cast<kun::ParticleBasedRenderer*>( r );
		// renderer->setEnabledSizesMode();
		// renderer->setShader( kvs::Shader::Phong( 0.6, 0.4, 0, 1 ) );
		renderer->setBaseOpacity( ::base_opacity );
		renderer->setTransferFunction( transferFunction() );
		renderer->setRepetitionLevel( ::repetition_level );
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
	param.addOption( "rep", "Set repetition level", 1, false );
	param.addOption( "o", "Set base opacity", 1, false );
	if( !param.parse() ) exit( 0 );

	kun::CloudObject* cloud = new kun::CloudObject();
	bool isRead;
	if( param.hasOption( "f" ) ) isRead = cloud->read( param.optionValue<std::string>( "f" ) );
	if( param.hasOption( "p" ) ) isRead = cloud->read( param.optionValue<std::string>( "p" ), param.optionValue<size_t>( "n" ) );
	if( isRead == false ) 
	{
		std::cerr << "File is not load successfully." << std::endl;
		exit( 1 );
	}
	if( param.hasOption( "rep" ) ) ::repetition_level = param.optionValue<int>( "rep" );
	if( param.hasOption( "o" ) ) ::base_opacity = param.optionValue<float>( "o" );

	kun::PointObject* point = cloud->toKUNPointObject( 0 );
	point->rotate( kvs::Matrix33f::RotationX( -90 ), kvs::Vec3::All( 0.0 ) );
	point->print( std::cout );

	kvs::TransferFunction tfunc( 256 );
	kun::ParticleBasedRenderer* renderer = new kun::ParticleBasedRenderer();
	// renderer->setEnabledSizesMode();
	renderer->setBaseOpacity( ::base_opacity );
	renderer->setTransferFunction( tfunc );
	renderer->setRepetitionLevel( ::repetition_level );
	// renderer->setParticleScale( 10.0 );

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