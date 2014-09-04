//
//	main.cpp
//
//
//	Created by Kun Zhao on 2014/08/29.
//
//

#include <kvs/glut/Application>
#include <kvs/glut/Screen>
#include <kvs/StructuredVolumeObject>
#include <kvs/StructuredVolumeImporter>
#include <kvs/UnstructuredVolumeObject>
#include <kvs/UnstructuredVolumeImporter>
#include <kvs/TransferFunction>
#include <kvs/ColorMap>
#include <kvs/OpacityMap>
#include <kvs/glut/TransferFunctionEditor>
#include <kvs/RendererManager>
#include <kvs/CommandLine>
#include "CellByCellUniformSampling.h"
#include "ParticleBasedRendererGLSL.h"
#include "JetImporter.h"

#include "PointImporter.h"
#include <kvs/KVSMLObjectPoint>
#include "PointExporter.h"

#include "load_ucd.h"
#include "PointObject.h"

#include <kvs/Timer>
#include <kvs/ColorImage>
#include "SnapKey.h"

#define TETRA 4
#define PRISM 6


bool ShadingFlag = true;

struct timeval start,end,start2;

namespace
{
	float base_opacity = 0.2;
}

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
		if(ShadingFlag == false)
		{
			renderer->disableShading();
		}
		renderer->setTransferFunction( transferFunction() );
		renderer->setBaseOpacity( ::base_opacity );
		screen()->redraw();
	}
};

kun::PointObject* CreatePointObject( kvs::VolumeObjectBase* volume, size_t subpixel_level, kvs::TransferFunction tfunc )
{
	kvs::Timer time;
	time.start();
	kun::PointObject* point = new kun::CellByCellUniformSampling( volume, subpixel_level, 0.5, tfunc, 0.0f );
	time.stop();
	std::cout << "Particle generation time: " << time.msec() << " msec." << std::endl;
	std::cout << "Particle number: " << point->numberOfVertices() << std::endl;
	point->print( std::cout );
	return point;
}

int main( int argc, char** argv )
{
	kvs::glut::Application app( argc, argv );
	kvs::glut::Screen screen( &app );

	kvs::CommandLine param( argc, argv );
	param.addHelpOption();

	SnapKey key;
	screen.addEvent( &key );

	// Parameter
	param.addOption( "s", "Subpixel Level", 1, false );
	param.addOption( "o", "base opacity", 1, false );
	param.addOption( "nos", "No Shading", 0, false );
	param.addOption( "trans", "set initial transferfunction", 1, false );

	// Data input
	param.addOption( "j", "Jet Data Filename", 1, false );
	param.addOption( "u", "Unstructured Volume Data Filename", 1, false );
	param.addOption( "k", "Structured Volume Data Filename", 1, false );
	param.addOption( "point", "KVSML Point Data Filename", 1, false );
	param.addOption( "tetra", "Input tetra Volume Object (binary_ucd)", 1, false );
	param.addOption( "prism", "Input prism Volume Object (binary_ucd)", 1, false );
	param.addOption( "both", "Input prism and tetra Volume Object (binary_ucd)", 1, false );
	param.addOption( "u-prism-ball", "KVSML Data Filename. around ball only", 1, false );

	// Output
	param.addOption( "writepoint", "Output point object filename", 1, false );
	if ( !param.parse() ) return 1;

	size_t subpixel_level = 1;
	kvs::TransferFunction tfunc_base( 256 );

	// Parameter
	if ( param.hasOption( "s" ) )
		subpixel_level = param.optionValue<size_t>( "s" ) ;
	if ( param.hasOption( "o" ) )
		base_opacity = param.optionValue<float>( "o" );
	if ( param.hasOption( "nos" ) )
	{
		ShadingFlag = false;
	}
	if(param.hasOption("trans"))
	{
		tfunc_base = kvs::TransferFunction(param.optionValue<std::string>( "trans" ));
	}
	else
	{
		kvs::OpacityMap omap( 256 );
		omap.addPoint( 0, base_opacity );
		omap.addPoint( 255, base_opacity );
		omap.create();
		tfunc_base.setOpacityMap( omap );
	}

	kun::PointObject* point = NULL;

		// Data Input
	if( param.hasOption( "point" ) )
	{
		point = new kun::PointImporter(param.optionValue<std::string>( "point" )	);
	}
	else if ( param.hasOption( "u" ) )
	{
		kvs::UnstructuredVolumeObject* volume = new kvs::UnstructuredVolumeImporter( param.optionValue<std::string>( "u" ) );
		point = CreatePointObject( volume, subpixel_level, tfunc_base );
	}
	else if ( param.hasOption( "k" ) )
	{
		kvs::StructuredVolumeObject* volume = new kvs::StructuredVolumeImporter( param.optionValue<std::string>( "k" ) );
		point = CreatePointObject( volume, subpixel_level, tfunc_base );
	}
	else if ( param.hasOption( "j" ) )
	{
		kvs::StructuredVolumeObject* volume = new kun::JetImporter( param.optionValue<std::string>( "j" )	);
		point = CreatePointObject( volume, subpixel_level, tfunc_base );
	}
	else if ( param.hasOption( "u-prism-ball" ) )
	{
		kvs::UnstructuredVolumeObject* volume = new kvs::UnstructuredVolumeImporter( param.optionValue<std::string>( "u-prism-ball" ) );
		volume->setMinMaxObjectCoords(kvs::Vec3(-30, -30, -30), kvs::Vec3(30, 30, 30) );
		volume->setMinMaxExternalCoords(kvs::Vec3(-30, -30, -30), kvs::Vec3(30, 30, 30) );
		point = CreatePointObject( volume, subpixel_level, tfunc_base );
	}
	else if ( param.hasOption( "tetra" ) )
	{
		kvs::UnstructuredVolumeObject* volume = CreateUnstructuredVolumeObject( param.optionValue<std::string>( "tetra" ).c_str() ,TETRA);
		point = CreatePointObject( volume, subpixel_level, tfunc_base );
	}
	else if ( param.hasOption( "prism" ) )
	{
		kvs::UnstructuredVolumeObject* volume = CreateUnstructuredVolumeObject( param.optionValue<std::string>( "prism" ).c_str() ,PRISM);
		point = CreatePointObject( volume, subpixel_level, tfunc_base );
	}
	else if ( param.hasOption( "both" ) )
	{
		kvs::UnstructuredVolumeObject* volume = CreateUnstructuredVolumeObject( param.optionValue<std::string>( "both" ).c_str() ,TETRA);
		point = CreatePointObject( volume, subpixel_level, tfunc_base );

		kvs::UnstructuredVolumeObject* volume2 = CreateUnstructuredVolumeObject( param.optionValue<std::string>( "both" ).c_str() ,PRISM);

		kun::PointObject* point2 = CreatePointObject( volume2, subpixel_level, tfunc_base );
		delete(volume2);
		point->add(*point2);
	}
	else 
	{
		std::cerr << "No input file !!!!!!!" << std::endl;
		exit(0);
	}

	// Data output. If there is output, the program will be ended
	if ( param.hasOption( "writepoint" ) )
	{

		kvs::KVSMLObjectPoint* kvsml = new kun::PointExporter<kvs::KVSMLObjectPoint>(point);
		kvsml -> setWritingDataType(kvs::KVSMLObjectPoint::ExternalBinary);
		std::string point_filename = param.optionValue<std::string>( "writepoint" );
		kvsml -> write( point_filename.c_str());
		std::cout << "Finish writing " << point_filename << std::endl;

		exit(0);
	}

	// Rendering
	kun::ParticleBasedRenderer* renderer = new kun::ParticleBasedRenderer();
	if( ShadingFlag == false)
	{
		renderer->disableShading();
	}

	kvs::TransferFunction tfunc( 256 );
	renderer->setTransferFunction( tfunc );
	if ( param.hasOption( "rep" ) )
	{
		renderer->setRepetitionLevel(param.optionValue<int>( "rep" ) );
	}
	else
	{
		renderer->setRepetitionLevel( subpixel_level * subpixel_level );
	}

	renderer->setBaseOpacity( ::base_opacity );
	screen.registerObject( point, renderer );
	screen.show();

	// Set the transfer function editor
	kvs::StructuredVolumeObject* object = new kvs::StructuredVolumeObject();
	object->setGridType( kvs::StructuredVolumeObject::Uniform );
	object->setVeclen( 1 );
	object->setResolution( kvs::Vector3ui( 1, 1, point->numberOfVertices() ) );
	object->setValues( point->sizes() );
	object->updateMinMaxValues();

	TransferFunctionEditor editor( &screen );
	editor.setVolumeObject( object );
	editor.setTransferFunction( tfunc );
	editor.show();

	return app.run();
}
