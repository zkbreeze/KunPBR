#include <kvs/glut/Application>
#include <kvs/glut/Screen>
#include <kvs/CellByCellUniformSampling>
#include <kvs/ParticleBasedRenderer>
#include <kvs/StructuredVolumeObject>
#include <kvs/StructuredVolumeImporter>
#include "JetImporter.h"
#include <kvs/UnstructuredVolumeObject>
#include <kvs/UnstructuredVolumeImporter>
#include <kvs/KeyPressEventListener>
#include <kvs/CommandLine>
#include "PointImporter.h"
#include "load_ucd.h"
#include "SnapKey.h"

#define TETRA 4
#define PRISM 6

kun::PointObject* CreatePointObject( kvs::VolumeObjectBase* volume, size_t subpixel_level, kvs::TransferFunction tfunc )
{
	kvs::Timer time;
	time.start();
	kun::PointObject* point = new kvs::CellByCellUniformSampling( volume, subpixel_level, 0.5, tfunc, 0.0f );
	time.stop();
	std::cout << "Particle generation time: " << time.msec() << " msec." << std::endl;
	std::cout << "Particle number: " << point->numberOfVertices() << std::endl;
	return point;
}

int main(int argc, char** argv)
{
	kvs::glut::Application app( argc, argv );
	kvs::glut::Screen screen( &app );

	SnapKey key_event;
	screen.addEvent( &key_event );

	kvs::CommandLine param( argc, argv );
	// Parameter
	param.addOption( "s", "Subpixel Level", 1, false );
	// Data input
	param.addOption( "j", "Jet Data Filename", 1, false );
	param.addOption( "u", "Unstructured Volume Data Filename", 1, false );
	param.addOption( "k", "Structured Volume Data Filename", 1, false );
	param.addOption( "point", "KVSML Point Data Filename", 1, false );
	param.addOption( "tetra", "Input tetra Volume Object (binary_ucd)", 1, false );
	param.addOption( "prism", "Input prism Volume Object (binary_ucd)", 1, false );
	param.addOption( "both", "Input prism and tetra Volume Object (binary_ucd)", 1, false );
	param.addOption( "u-prism-ball", "KVSML Data Filename. around ball only", 1, false );
	if ( !param.parse() ) return 1;

	size_t subpixel_level = 1;
	kvs::TransferFunction tfunc_base( 256 );

	// Parameter
	if ( param.hasOption( "s" ) )
		subpixel_level = param.optionValue<size_t>( "s" ) ;

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

	// Rendering
	kvs::glsl::ParticleBasedRenderer* renderer = new kvs::glsl::ParticleBasedRenderer();
	renderer->setRepetitionLevel( subpixel_level * subpixel_level );
	screen.registerObject( point, renderer );
	screen.show();

	return app.run();

}