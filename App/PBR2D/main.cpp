//
//  main.cpp
//
//
//  Created by Kun Zhao on 2014-11-13 16:27:22.
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
#include "CellByCellUniformSampling2D.h"
#include "ParticleBasedRendererGLSL2D.h"
#include "JetImporter.h"

#include "KVSMLObjectKunPoint.h"
#include "PointImporter.h"
#include "PointExporter.h"

#include "load_ucd.h"
#include "PointObject.h"

#include <kvs/Timer>
#include <kvs/ColorImage>
#include "SnapKey.h"
#include "FPS.h"

#include "loaducd.h"
#include "TsunamiObject.h"

#define TETRA 4
#define PRISM 6


namespace
{
	bool ShadingFlag = true;
	float base_opacity = 0.2;
}

kun::PointObject* CreatePointObject2D( kvs::VolumeObjectBase* volume1, kvs::VolumeObjectBase* volume2, size_t subpixel_level, kvs::TransferFunction tfunc, bool shuffle = 0, bool use_kun_sampling_step = 0 )
{
	kvs::Timer time;
	time.start();
	kun::CellByCellUniformSampling2D* sampler = new kun::CellByCellUniformSampling2D();
	sampler->setSubpixelLevel( subpixel_level );
	sampler->setSamplingStep( 0.5 );
	sampler->setTransferFunction( tfunc );
	sampler->setObjectDepth( 0.0 );
	sampler->setSecondVolume( volume2 );
	if( shuffle ) sampler->setShuffleParticles();
	if( use_kun_sampling_step ) sampler->setKunSamplingStep();
	kun::PointObject* point = sampler->exec( volume1);
	time.stop();
	std::cout << "Particle generation time: " << time.msec() << " msec." << std::endl;
	std::cout << "Particle number: " << point->numberOfVertices() << std::endl;
	return point;
}

int main( int argc, char** argv )
{
	kvs::glut::Application app( argc, argv );
	kvs::glut::Screen screen( &app );

	kvs::CommandLine param( argc, argv );
	param.addHelpOption();

	kun::SnapKey key;
	screen.addEvent( &key );
	kun::FPS fps;
	screen.addEvent( &fps );

	// Parameter
	param.addOption( "s", "Subpixel Level", 1, false );
	param.addOption( "o", "base opacity", 1, false );
	param.addOption( "nos", "No Shading", 0, false );
	param.addOption( "trans", "set initial transfer function", 1, false );
	param.addOption( "sg", "shuffle the generated particles", 0, false );
	param.addOption( "rep", "Input the repetition level", 1, false );

	// Data input
	param.addOption( "point", "Input the point data", 1, false );
	param.addOption( "u1", "1st Unstructured Volume Data Filename", 1, false );
	param.addOption( "u2", "2nd Unstructured Volume Data Filename", 1, false );
	param.addOption( "k1", "1st Structured Volume Data Filename", 1, false );
	param.addOption( "k2", "2nd Structured Volume Data Filename", 1, false );	
	param.addOption( "tetra", "Input tetra Volume Object (binary_ucd)", 1, false );
	param.addOption( "prism", "Input prism Volume Object (binary_ucd)", 1, false );
	param.addOption( "both", "Input prism and tetra Volume Object (binary_ucd)", 1, false );
	param.addOption( "prism-ball", "Input prism Volume Object (binary_ucd) and output the cut ball", 1, false );
	param.addOption( "tsunami", "Input tsunami particle data", 1, false );

	// Output
	param.addOption( "writepoint", "Output point object filename", 1, false );
	if ( !param.parse() ) return 1;

	size_t subpixel_level = 1;
	kvs::TransferFunction tfunc_base( 256 );
	bool shuffle_generated_particles = false;

	// Parameter
	if( param.hasOption( "s" ) ) subpixel_level = param.optionValue<size_t>( "s" ) ;
	if( param.hasOption( "o" ) ) base_opacity = param.optionValue<float>( "o" );
	if( param.hasOption( "nos" ) ) ShadingFlag = false;
	if( param.hasOption( "sg" ) ) shuffle_generated_particles = true;

	if( param.hasOption( "trans" ) ) tfunc_base = kvs::TransferFunction( param.optionValue<std::string>( "trans" ) );
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
		point = new kun::PointImporter( param.optionValue<std::string>( "point" ) );
	}
	else if( param.hasOption( "u1" ) && param.hasOption( "u2" ) )
	{
		kvs::UnstructuredVolumeObject* volume1 = new kvs::UnstructuredVolumeImporter( param.optionValue<std::string>( "u1" ) );
		kvs::UnstructuredVolumeObject* volume2 = new kvs::UnstructuredVolumeImporter( param.optionValue<std::string>( "u2" ) );
		point = CreatePointObject2D( volume1, volume2, subpixel_level, tfunc_base, shuffle_generated_particles );
	}
	else if( param.hasOption( "k1" ) && param.hasOption( "k2") )
	{
		kvs::StructuredVolumeObject* volume1 = new kvs::StructuredVolumeImporter( param.optionValue<std::string>( "k1" ) );
		kvs::StructuredVolumeObject* volume2 = new kvs::StructuredVolumeImporter( param.optionValue<std::string>( "k2" ) );
		point = CreatePointObject2D( volume1, volume2, subpixel_level, tfunc_base, shuffle_generated_particles );
	}
	else if( param.hasOption( "prism-ball" ) )
	{
		kvs::UnstructuredVolumeObject* volume1 = new takami::LoadUcd( param.optionValue<std::string>( "prism-ball" ).c_str(), PRISM, 3 );
		kvs::UnstructuredVolumeObject* volume2 = new takami::LoadUcd( param.optionValue<std::string>( "prism-ball" ).c_str(), PRISM, 1 );
		// volume1->setMinMaxObjectCoords( kvs::Vec3( -30, -30, -30 ), kvs::Vec3( 30, 30, 30 ) );
		// volume1->setMinMaxExternalCoords( kvs::Vec3( -30, -30, -30 ), kvs::Vec3( 30, 30, 30 ) );
		// volume2->setMinMaxObjectCoords( kvs::Vec3( -30, -30, -30 ), kvs::Vec3( 30, 30, 30 ) );
		// volume2->setMinMaxExternalCoords( kvs::Vec3( -30, -30, -30 ), kvs::Vec3( 30, 30, 30 ) );

		point = CreatePointObject2D( volume1, volume2, subpixel_level, tfunc_base, shuffle_generated_particles );
	}
	else if( param.hasOption( "tetra" ) )
	{
		kvs::UnstructuredVolumeObject* volume1 = new takami::LoadUcd( param.optionValue<std::string>( "tetra" ).c_str() ,TETRA, 3 );
		kvs::UnstructuredVolumeObject* volume2 = new takami::LoadUcd( param.optionValue<std::string>( "tetra" ).c_str() ,TETRA, 1 );
		point = CreatePointObject2D( volume1, volume2, subpixel_level, tfunc_base, shuffle_generated_particles );
	}
	else if( param.hasOption( "both" ) )
	{
		kvs::UnstructuredVolumeObject* volume_p1 = new takami::LoadUcd( param.optionValue<std::string>( "both" ).c_str() ,TETRA, 3 );
		kvs::UnstructuredVolumeObject* volume2_p1 = new takami::LoadUcd( param.optionValue<std::string>( "both" ).c_str() ,TETRA, 1 );
		point = CreatePointObject2D( volume_p1, volume2_p1, subpixel_level, tfunc_base, shuffle_generated_particles );

		kvs::UnstructuredVolumeObject* volume_p2 = new takami::LoadUcd( param.optionValue<std::string>( "both" ).c_str(), PRISM, 3 );
		kvs::UnstructuredVolumeObject* volume2_p2 = new takami::LoadUcd( param.optionValue<std::string>( "both" ).c_str(), PRISM, 1 );
		kun::PointObject* point2 = CreatePointObject2D( volume_p2, volume2_p2, subpixel_level, tfunc_base, shuffle_generated_particles );

		point->add( *point2 );
	}
	else if( param.hasOption( "tsunami" ) )
	{
		kun::TsunamiObject* tsunami = new kun::TsunamiObject( param.optionValue<std::string>( "tsunami" ) );
		point = tsunami->toKUNPointObject2Value( 1, 3 );
	}
	else 
	{
		std::cerr << "No input file !!!!!!!" << std::endl;
		exit(0);
	}

	point->print( std::cout );

	// Data output. If there is output, the program will be ended
	if ( param.hasOption( "writepoint" ) )
	{
		point->shuffle();

		kun::KVSMLObjectKunPoint* kvsml = new kun::PointExporter<kun::KVSMLObjectKunPoint>( point );
		kvsml -> setWritingDataType(kvs::kvsml::ExternalBinary);
		std::string point_filename = param.optionValue<std::string>( "writepoint" );
		kvsml -> write( point_filename.c_str());
		std::cout << "Finish writing " << point_filename << std::endl;

		exit(0);
	}

	// Rendering
	kun::ParticleBasedRenderer2D* renderer = new kun::ParticleBasedRenderer2D();
	if( ShadingFlag == false) renderer->disableShading();

    // create the 2d transfer function
    size_t side_size = 10;
    size_t tfunc_size = side_size * side_size;
	kvs::TransferFunction tfunc( tfunc_size );
	kvs::ColorMap cmap( tfunc_size );
	kvs::OpacityMap omap( tfunc_size );
    for ( size_t j = 0; j < side_size; j++ )
    {
    	for ( size_t i = 0; i < side_size; i++ )
    	{
    		int index = i + j * side_size;
    		
    		kvs::UInt8 red = ( 1 - (float)i / side_size ) * 255; // red
    	    kvs::UInt8 green = ( (float)j / side_size ) * 255; // green
    	    kvs::UInt8 blue = 1.0 * 255 ;  // blue
    	    kvs::Real32 opacity = 0.1; //alpha

    	    cmap.addPoint( index, kvs::RGBColor( red, green, blue ) );
    	    omap.addPoint( index, opacity );
    	}
    }
    cmap.create();
    omap.create();
    tfunc.setColorMap( cmap );
    tfunc.setOpacityMap( omap );

	renderer->setTransferFunction( tfunc );
	if( param.hasOption( "low_rep" ) )
		renderer->setRepetitionLevel( param.optionValue<size_t>( "low_rep" ) );
	else if( param.hasOption( "rep" ) )
		renderer->setRepetitionLevel( param.optionValue<size_t>( "rep" ) );
	else
		renderer->setRepetitionLevel( subpixel_level * subpixel_level );

	renderer->setBaseOpacity( ::base_opacity );
	renderer->setValidMinMaxRange( 0.0, 1.0, 0.0, 1.0 );
	screen.registerObject( point, renderer );
	screen.setBackgroundColor( kvs::RGBColor( 255, 255, 255 ) );
	screen.show();

	return app.run();
}
