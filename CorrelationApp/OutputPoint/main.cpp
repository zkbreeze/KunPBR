//
//  main.cpp
//
//
//  Created by Kun Zhao on 2015-12-21 16:13:07.
//
//

#include <kvs/glut/Application>
#include <kvs/glut/Screen>
#include "Correlation.h"
#include <kvs/glut/Slider>
#include <kvs/ObjectManager>
#include <kvs/RendererManager>
#include <iostream>
#include <fstream>
#include <kvs/TransferFunction>
#include <kvs/glut/TransferFunctionEditor>
#include <kvs/glut/Label>
#include <kvs/RGBFormulae>
#include <kvs/KeyPressEventListener>
#include <kvs/OpacityMap>
#include <kvs/ColorMap>
#include "UcdObject.h"
#include <kvs/CommandLine>
#include <kvs/LineObject>
#include <kvs/ExtractEdges>
#include <kvs/ExtractVertices>
#include "WriteUnstructuredVolume.h"
#include "PointObject.h"
#include "ParticleBasedRenderer.h"
#include <fstream>

int main( int argc, char** argv )
{
	kvs::glut::Application app( argc, argv );
	kvs::glut::Screen screen( &app );

	kvs::CommandLine param( argc, argv );
	param.addOption( "f", "One file name", 1, true );
	param.addOption( "p", "Choose parameter, 1: pressure, 2: density, 3: temperature", 1, false );
	param.addOption( "out", "Output file name", 1, false );
	param.addOption( "part", "Part to output the point", 1, false );
	param.addHelpOption();
	if( !param.parse() ) exit( 0 );

	std::string filename =  param.optionValue<std::string>( "f" );
	kun::UcdObject* ucd = new kun::UcdObject( filename );
	kvs::UnstructuredVolumeObject* object = new kvs::UnstructuredVolumeObject();
	if( param.hasOption( "p" ) )
	{
		int ID = param.optionValue<int>( "p" );
		if( ID == 1 ) object = ucd->toPressureSphere();
		if( ID == 2 ) object = ucd->toDensitySphere();
		if( ID == 3 ) object = ucd->toTemperatureSphere();
	}
	else
		object = ucd->toPressureSphere();

	// object->print( std::cout );
	// WriteUnstructuredVolume( object, "test.kvsml");
	// exit( 0 );

	// kvs::LineObject* proxy = new kvs::ExtractEdges( object );
	// kvs::PointObject* proxy = new kvs::ExtractVertices( object );

	float part = 0.1;
	if( param.hasOption( "part" ) ) part = param.optionValue<float>( "part" );

	kun::PointObject* point = new kun::PointObject();
	point->setVeclen( 1 );
	point->setCoords( object->coords() );
	point->setValues( object->values() );
	point->updateMinMaxValues();
	point->updateMinMaxCoords();

	point->shuffle();
	point = point->toPartPoint( part );
	// point->print( std::cout );

	if( param.hasOption( "out" ) )
	{
		std::string output_name = param.optionValue<std::string>( "out" );
		std::ofstream ofs( output_name.c_str(), std::ios::out );
		ofs.write( (char*)point->values().data(), sizeof(float) * point->numberOfVertices() );
		ofs.close();

		// // Coords; only once
		// std::string output_coords( "coords.crd" );
		// std::ofstream ofs_coords( output_coords.c_str(), std::ios::out );
		// ofs_coords.write( (char*)point->coords().data(), sizeof(float) * point->numberOfVertices() * 3 );
		// ofs_coords.close();

		std::cout << "Finish writing." << std::endl;
		exit( 0 );
	}

	// Test for the out put data.
	// std::string input_point( "pressure000.dat" );
	// std::string input_coords( "coords.dat" );
	// std::ifstream ifs_point( input_point.c_str(), std::ios::in );
	// std::ifstream ifs_coords( input_coords.c_str(), std::ios::in );
	// float* pvalue = new float[point->numberOfVertices()];
	// float* pcoord = new float[point->numberOfVertices() * 3];
	// ifs_point.read( (char*)pvalue, sizeof(float) * point->numberOfVertices() );
	// ifs_coords.read( (char*)pcoord, sizeof(float) * point->numberOfVertices() * 3 );
	// kvs::ValueArray<float> values( pvalue, point->numberOfVertices() );
	// kvs::ValueArray<float> coords( pcoord, point->numberOfVertices() * 3 );
	// kun::PointObject* new_point = new kun::PointObject();
	// new_point->setVeclen( 1 );
	// new_point->setCoords( coords );
	// new_point->setValues( values );
	// new_point->updateMinMaxValues();
	// new_point->updateMinMaxCoords();
	// new_point->print( std::cout );

	kun::ParticleBasedRenderer* renderer = new kun::ParticleBasedRenderer();
	renderer->setBaseOpacity( 0.05 );
	renderer->setRepetitionLevel( 10 );
	renderer->setParticleScale( 3.0 );

	screen.registerObject( point, renderer );
	screen.setBackgroundColor( kvs::RGBColor::Black() );
	screen.show();

	return app.run();
}