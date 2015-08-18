//
//  main.cpp
//
//
//  Created by Kun Zhao on 2015-08-03 18:44:55.
//
//

#include "OBJImporter.h"
#include <kvs/glut/Application>
#include <kvs/glut/Screen>

int main( int argc, char** argv )
{
	kvs::glut::Application app( argc, argv );
	kvs::glut::Screen screen( &app );

	kun::OBJImporter* obj = new kun::OBJImporter( argv[1] );
	kvs::PolygonObject* polygon = obj->toKVSPolygonObject();

	screen.registerObject( polygon );
	screen.show();

	return app.run();
}
