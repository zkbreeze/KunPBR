//
//  main.cpp
//
//
//  Created by Kun Zhao on 2015-12-15 17:45:54.
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

int main( int argc, char** argv )
{
	kvs::glut::Application app( argc, argv );
	kvs::glut::Screen screen( &app );

	kvs::CommandLine param( argc, argv );
	param.addOption( "d", "Directory of the Ucd data", 1, false );
	param.addOption( "nstep", "Time steps to calculation", 1, false );
	param.addOption( "f", "One file name", 1, false );
	if( !param.parse() ) exit( 0 );

	std::string filename;
	if( param.hasOption( "f" ) ) filename =  param.optionValue<std::string>( "f" );
	kun::UcdObject* ucd = new kun::UcdObject( filename );
	kvs::UnstructuredVolumeObject* object = ucd->toPressureSphere();
	object->print( std::cout );
	kvs::LineObject* proxy = new kvs::ExtractEdges( object );

	screen.registerObject( proxy );
	screen.show();

	return app.run();
}