//
//  main.cpp
//
//
//  Created by Kun Zhao on 2015-08-19 14:00:03.
//
//

#include "DensityCalculator.h"
#include "WriteStructuredVolume.h"
#include "TsunamiObject.h"
#include "PointImporter.h"
#include <kvs/glut/Application>
#include <kvs/glut/Screen>
#include <kvs/RayCastingRenderer>
#include <kvs/CommandLine>
#include <kvs/Timer>
#include "ParticleBasedRendererGLSLPoint.h"
#include <kvs/TransferFunction>
#include <kvs/glut/TransferFunctionEditor>
#include <kvs/RendererManager>
#include "SnapKey.h"
#include <kvs/RGBFormulae>
#include <kvs/StochasticRenderingCompositor>
#include <kvs/StochasticPolygonRenderer>
#include "OBJObject.h"
#include <kvs/PolygonObject>
#include <kvs/glut/Slider>
#include <kvs/ObjectManager>
#include <kvs/PaintEventListener>

namespace
{
	kvs::StructuredVolumeObject* density_volume = NULL;	
	int repetition_level = 36;
	kvs::Timer fps_time;
}

class FPS : public kvs::PaintEventListener
{

public:
	void update()
	{	
		::fps_time.stop();
		std::cout << "\r" << "FPS: " << fps_time.fps() << std::flush;
		::fps_time.start();
	}
};


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
		kvs::RendererBase* r = glut_screen->scene()->rendererManager()->renderer( "PointRenderer" );
		kun::ParticleBasedRendererPoint* renderer = static_cast<kun::ParticleBasedRendererPoint*>( r );
		renderer->setShader( kvs::Shader::Phong( 0.6, 0.4, 0, 1 ) );
		renderer->setDensityVolume( ::density_volume );
		renderer->setRepetitionLevel( ::repetition_level );
		renderer->setTransferFunction( transferFunction() );
		std::cout << "TF adjust time: " << renderer->timer().msec() << std::endl;
		screen()->redraw();
	}
};

class PolygonSlider : public kvs::glut::Slider
{
public:

	PolygonSlider( kvs::glut::Screen* screen ) : 
	kvs::glut::Slider( screen ){}

	void valueChanged( void )
	{
		kvs::glut::Screen* glut_screen = static_cast<kvs::glut::Screen*>( screen() );
		kvs::ObjectBase* o = glut_screen->scene()->objectManager()->object( "Polygon" );
		kvs::PolygonObject* polygon = static_cast<kvs::PolygonObject*>( o );
		polygon->setOpacity( this->value() );

		kvs::StochasticPolygonRenderer* polygon_renderer = new kvs::StochasticPolygonRenderer();
		polygon_renderer->setShader( kvs::Shader::Phong( 0.6, 0.4, 0, 1 ) );
		polygon_renderer->setName( "PolygonRenderer" );
		glut_screen->scene()->rendererManager()->change( "PolygonRenderer", polygon_renderer, false );
		screen()->redraw();
	}
};

int main( int argc, char** argv )
{
	kvs::glut::Application app( argc, argv );
	kvs::glut::Screen screen( &app );

	FPS fps;
	screen.addEvent( &fps );
	kun::SnapKey key;
	screen.addEvent( &key );

	kvs::CommandLine param( argc, argv );
	param.addHelpOption();
	param.addOption( "m", "Assign max grid number", 1, false );
	param.addOption( "f", "Input KUN point file name", 1, false );
	param.addOption( "t", "Input tsunami file name", 1, false );
	param.addOption( "rep", "Input repetition level", 1, false );
	param.addOption( "l", "Input the OBJ land object", 1, false );
	param.addOption( "minx", "Input the clip range of min x", 1, false );
	param.addOption( "miny", "Input the clip range of min y", 1, false );
	param.addOption( "minz", "Input the clip range of min z", 1, false );
	param.addOption( "maxx", "Input the clip range of max x", 1, false );
	param.addOption( "maxy", "Input the clip range of max y", 1, false );
	param.addOption( "maxz", "Input the clip range of max z", 1, false );

	if( !param.parse() ) return 1;

	// File import
	kun::PointObject* point = NULL;
	if( param.hasOption( "f" ) ) point = new kun::PointImporter( param.optionValue<std::string>( "f" ) );
	if( param.hasOption( "t" ) ) 
	{
		kun::TsunamiObject* tsunami = new kun::TsunamiObject( param.optionValue<std::string>( "t" ) );
		point = tsunami->toKUNPointObject( 1 );		
	}

	if( param.hasOption( "minx" ) ) 
	{
		float minx = param.optionValue<float>( "minx" );
		float miny = param.optionValue<float>( "miny" );
		float minz = param.optionValue<float>( "minz" );
		float maxx = param.optionValue<float>( "maxx" );
		float maxy = param.optionValue<float>( "maxy" );
		float maxz = param.optionValue<float>( "maxz" );

		point->setMinMaxRange( kvs::Vector3f( minx, miny, minz ), kvs::Vector3f( maxx, maxy, maxz ) );
	}

	point->print( std::cout );
	kvs::Vector3f min = point->minObjectCoord();
	kvs::Vector3f max = point->maxObjectCoord();

	if( param.hasOption( "rep" ) ) ::repetition_level = param.optionValue<int>( "rep" );

	kvs::Timer time;
	time.start();
	kun::DensityCalculator* calculator = new kun::DensityCalculator( point );
	if( param.hasOption( "m" ) ) calculator->setMaxGrid( param.optionValue<int>( "m") );
	density_volume = calculator->outputDensityVolume();
	time.stop();
	std::cout << "Density calculation time: " << time.msec() << "msec" << std::endl;

	kvs::TransferFunction tfunc( 256 );
	tfunc.setColorMap( kvs::RGBFormulae::Ocean( 256 ) );

	kun::ParticleBasedRendererPoint* renderer = new kun::ParticleBasedRendererPoint();
	renderer->setShader( kvs::Shader::Phong( 0.6, 0.4, 0, 1 ) );
	renderer->setDensityVolume( ::density_volume );
	// renderer->setRepetitionLevel( ::repetition_level );
	renderer->setTransferFunction( tfunc );
	renderer->setName( "PointRenderer" );

	// Load land
	kun::OBJObject* obj = new kun::OBJObject( param.optionValue<std::string>( "l" ) );
	obj->setRange( min, max ); // The land data is larger than the tsunami data
	kvs::PolygonObject* polygon = obj->toKVSPolygonObject();
	polygon->setName( "Polygon" );
	// polygon->setOpacity( 100 );

	kvs::StochasticPolygonRenderer* polygon_renderer = new kvs::StochasticPolygonRenderer();
	polygon_renderer->setShader( kvs::Shader::Phong( 0.6, 0.4, 0, 1 ) );
	polygon_renderer->setName( "PolygonRenderer" );
	// polygon_renderer->setPolygonOffset( -1.f );

	screen.registerObject( point, renderer );
	screen.registerObject( polygon, polygon_renderer );
	screen.setBackgroundColor( kvs::RGBColor::Black() );
	// screen.setSize( 1024, 768 );
	screen.show();

	kvs::StochasticRenderingCompositor compositor( screen.scene() );
	compositor.setRepetitionLevel( ::repetition_level );
	compositor.enableLODControl();
	screen.setEvent( &compositor );

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

	PolygonSlider* slider = new PolygonSlider( &screen );
	slider->setSliderColor( kvs::RGBColor::White() );
	slider->setX( screen.width() * 0.25 );
	slider->setY( screen.height() - 80 );
	slider->setWidth( screen.width() / 2 );
	slider->setValue( 255 );
	slider->setRange( 0, 255 );
	slider->setMargin( 15 );
	slider->setCaption("Polygon Opacity");
	slider->setTextColor( kvs::RGBColor::White()  );
	// slider->show();

	return app.run();
}