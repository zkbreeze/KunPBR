//
//  main.cpp
//
//
//  Created by Kun Zhao on 2015-12-18 16:14:40.
//
//

#include <kvs/glut/Application>
#include <kvs/glut/Screen>
#include <kvs/StructuredVolumeObject>
#include <kvs/RayCastingRenderer>
#include "Correlation.h"
#include <kvs/glut/Slider>
#include <kvs/ObjectManager>
#include <kvs/RendererManager>
#include <kvs/TransferFunction>
#include <kvs/glut/TransferFunctionEditor>
#include <kvs/glut/Label>
#include <kvs/RGBFormulae>
#include <kvs/KeyPressEventListener>
#include <kvs/OpacityMap>
#include <kvs/ColorMap>
#include <kvs/Xorshift128>
#include <iostream>
#include <fstream>
#include <kvs/SharedPointer>
#include <kvs/HydrogenVolumeData>
#include <kvs/Directory>
#include <kvs/File>
#include <kvs/StructuredVolumeImporter>
#include <kvs/CommandLine>

namespace
{
	int time_lag = 0;
	int max_lag = 10;
	kvs::TransferFunction tfunc = kvs::RGBFormulae::Jet( 256 );
	kvs::Vec3ui resolution;
}

kvs::StructuredVolumeObject* createVolumeObject( const kvs::ValueArray<float>& value_array )
{
	kvs::StructuredVolumeObject* object = new kvs::StructuredVolumeObject();
	object->setName( "Object" );
	object->setGridTypeToUniform();
	object->setResolution( resolution );
	object->setVeclen( 1 );
	object->setValues( value_array );
	object->updateMinMaxCoords();
	object->updateMinMaxValues();

	return object;
}

std::vector<kvs::StructuredVolumeObject*> loadTimeVaryingVolume( const std::string& filename, const int step = 0 )
{
	std::vector<kvs::StructuredVolumeObject*> time_varying_volume;

	kvs::Directory directory( filename );
	const kvs::FileList files = directory.fileList();
	int file_length = files.size();
	std::vector<std::string> file_name;

	int nsteps = 0;
	for ( int i = 0; i < file_length; i++ )
	{
	    const kvs::File file = files[i];
	    if( file.extension() == "kvsml" )
	    {
	        nsteps++;
	        file_name.push_back( file.filePath() );
	    }
	}

	if( step ) 
	{
	    if( step > nsteps ) exit( 0 );
	    else
	        nsteps = step;
	}

	std::cout << "There are " << nsteps << " time steps" << std::endl;

	kvs::Timer time;
	time.start();
	for ( int i = 0; i < nsteps; i++ )
	{
		kvs::StructuredVolumeObject* object;
	    object = new kvs::StructuredVolumeImporter( file_name[i] );
	    time_varying_volume.push_back( object );
	    std::cout << "\r" << i << std::flush;        
	}
	time.stop();
	std::cout << "Reading time of the time varying volume: " << time.msec() << " msec" << std::endl;

	::resolution = time_varying_volume[0]->resolution();
	return time_varying_volume;
}

std::vector<kvs::StructuredVolumeObject*> createRandomVolume( int total_time_step )
{
	std::vector<kvs::StructuredVolumeObject*> volume_set;
	for( unsigned int i = 0; i < total_time_step; i++ )
	{
		unsigned int nvertices = ::resolution.x() * ::resolution.y() * ::resolution.z();
		float range = 100.0; // Min max range of the range volume
		float* pvalue = new float[nvertices];
		for( unsigned int j = 0; j < nvertices; j++ )
		{
			kvs::Xorshift128 rng;
			rng.setSeed( i * nvertices + j );
			pvalue[j] = rng.rand() * range;
		}
		kvs::ValueArray<float> values( pvalue, nvertices );
		kvs::StructuredVolumeObject* object = createVolumeObject( values );
		volume_set.push_back( object );
	}
	return volume_set;
}

std::vector<kvs::ValueArray<float> > toCorrelationSequences( std::vector<kvs::StructuredVolumeObject*> time_varying_volume, int best_time_lag, float offset )
{
	std::cout << "Converting the volume to correlation sequences." << std::endl;
	unsigned int size = time_varying_volume.size();
	unsigned int nvertices = time_varying_volume[0]->numberOfNodes();

	std::vector<kvs::ValueArray<float> > sequences;
	for( unsigned int i = 0; i < nvertices; i++ )
	{
		float* pvalue1 = new float[size];
		float* pvalue2 = new float[size];
		kvs::Xorshift128 rng_pn;
		rng_pn.setSeed( i );
		float pos_neg = ( ( rng_pn.rand() - 0.5 ) > 0 ) ? 1.0 : -1.0;
		for( unsigned int j = 0; j < size; j++ )
		{
			kvs::Xorshift128 rng;
			rng.setSeed( i * size + j );
			const float* values_ori = static_cast<const float*>( time_varying_volume[j]->values().data() );
			float range = time_varying_volume[j]->maxValue() - time_varying_volume[j]->minValue();
			pvalue1[j] = values_ori[i];
			pvalue2[(j + best_time_lag) % size] = pos_neg * values_ori[i] + ( rng.rand() - 0.5 ) * range * offset;
		}
		sequences.push_back( kvs::ValueArray<float>( pvalue1, size ) );
		sequences.push_back( kvs::ValueArray<float>( pvalue2, size ) );
		std::cout << "\r" << i << std::flush;
	}

	std::cout << std::endl;

	if( sequences.size() !=	nvertices * 2 )
	{
		std::cerr << "Error to create the correlation sequences" << std::endl;
		exit( 0 );
	}

	return sequences;
}

kvs::StructuredVolumeObject* calculateCorrelationVolume( const std::vector<kvs::ValueArray<float> >& sequences, int time_lag = 0 )
{
	unsigned int size = sequences.size() / 2;
	float* pvalue = new float[size];
	for( unsigned int i = 0; i < size; i++ )
	{
		if( time_lag )
			pvalue[i] = kun::Correlation::calculateLag( sequences[i * 2], sequences[i * 2 + 1], ::time_lag );
		else
			pvalue[i] = kun::Correlation::calculate( sequences[i * 2], sequences[i * 2 + 1] );
	}

	kvs::StructuredVolumeObject* object = createVolumeObject( kvs::ValueArray<float>( pvalue, size ) );
	return object;
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
		kvs::RendererBase* r = glut_screen->scene()->rendererManager()->renderer( "Renderer" );
		kvs::glsl::RayCastingRenderer* renderer = static_cast<kvs::glsl::RayCastingRenderer*>( r );
		renderer->setTransferFunction( transferFunction() );
		::tfunc = transferFunction();
		screen()->redraw();
	}
};

class Label : public kvs::glut::Label
{
public:

    Label( kvs::ScreenBase* screen ):
    kvs::glut::Label( screen )
    {
        setTextColor( kvs::RGBColor::White()  );
        setMargin( 10 );
    }
    
    void screenUpdated( void )
    {
        char* buf = new char[256];
        sprintf( buf, "Time lag : %d", ::time_lag );
        setText( std::string( buf ).c_str() );
    }
};

class CorrelationSlider : public kvs::glut::Slider
{

	std::vector<kvs::ValueArray<float> > m_sequences;

public:

	CorrelationSlider( kvs::glut::Screen* screen ) : 
	kvs::glut::Slider( screen ){}

	void setSequences( const std::vector<kvs::ValueArray<float> >& sequences ){ m_sequences = sequences; }
	void valueChanged( void )
	{
		kvs::glut::Screen* glut_screen = static_cast<kvs::glut::Screen*>( screen() );
		::time_lag = this->value();
		
		kvs::StructuredVolumeObject* object = calculateCorrelationVolume( m_sequences, ::time_lag );

		kvs::glsl::RayCastingRenderer* renderer = new kvs::glsl::RayCastingRenderer();
		renderer->setName( "Renderer" );
		renderer->setTransferFunction( ::tfunc );

		glut_screen->scene()->objectManager()->change( "Object", object, true );
		glut_screen->scene()->replaceRenderer( "Renderer", renderer, true );

		screen()->redraw();
	}
};

CorrelationSlider* slider;

class KeyPressEvent : public kvs::KeyPressEventListener
{
	std::vector<kvs::ValueArray<float> > m_sequences;

public:
	void setSequences( const std::vector<kvs::ValueArray<float> >& sequences ){ m_sequences = sequences; }
    void update( kvs::KeyEvent* event )
    {
        switch ( event->key() )
        {
            case kvs::Key::Right:
            {
            	if( ::time_lag < ::max_lag )
            	{
            		::time_lag++;
            	}
            	break;
            }
            case kvs::Key::Left:
            {
            	if( ::time_lag > 0 )
            	{
            		::time_lag--;
            	}
            	break;
            }
        }
        kvs::glut::Screen* glut_screen = static_cast<kvs::glut::Screen*>( screen() );
        kvs::StructuredVolumeObject* object = calculateCorrelationVolume( m_sequences, ::time_lag );

        kvs::glsl::RayCastingRenderer* renderer = new kvs::glsl::RayCastingRenderer();
        renderer->setName( "Renderer" );
        renderer->setTransferFunction( ::tfunc );

        glut_screen->scene()->objectManager()->change( "Object", object, true );
        glut_screen->scene()->replaceRenderer( "Renderer", renderer, true );

        slider->setValue( ::time_lag );
        screen()->redraw();
    }
};

int main( int argc, char** argv )
{
	kvs::glut::Application app( argc, argv );
	kvs::glut::Screen screen( &app );

	kvs::CommandLine param( argc, argv );
	param.addOption( "t", "Best time lag [1~6]", 1, false );
	param.addOption( "nsteps", "How many steps to load", 1, false );
	param.addOption( "resx", "Resolution x of the random volume set", 1, false );
	param.addOption( "resy", "Resolution y of the random volume set", 1, false );
	param.addOption( "resz", "Resolution z of the random volume set", 1, false );
	param.addOption( "offset", "Offset to control the correlation", 1, false );
	param.addOption( "d", "Directory name of the time varying data", 1, false );
	param.addHelpOption();
	if( !param.parse() ) exit( 0 );

	int nsteps = 20; // By default
	if( param.hasOption( "nsteps" ) ) nsteps = param.optionValue<int>( "nsteps" );

	// Time-varying data 
	unsigned int resx = 15, resy = 15, resz = 15; // By default
	std::vector<kvs::StructuredVolumeObject*> time_varying_volume;
	if( param.hasOption( "d" ) )
		time_varying_volume = loadTimeVaryingVolume( param.optionValue<std::string>( "d" ), nsteps );
	else
	{
		if( param.hasOption( "resx" ) ) resx = param.optionValue<unsigned int>( "resx" );
		if( param.hasOption( "resy" ) ) resy = param.optionValue<unsigned int>( "resy" );
		if( param.hasOption( "resz" ) ) resz = param.optionValue<unsigned int>( "resz" );
		::resolution = kvs::Vec3ui( resx, resy, resz );
		time_varying_volume = createRandomVolume( nsteps );
	}

	int best_lag = 3; // By default
	if( param.hasOption( "t" ) ) best_lag = param.optionValue<int>( "t" );
	float offset = 0.1; // By default
	if( param.hasOption( "offset" ) ) offset = param.optionValue<float>( "offset" );
	std::vector<kvs::ValueArray<float> > sequences = toCorrelationSequences( time_varying_volume, best_lag, offset );
	kvs::StructuredVolumeObject* object = calculateCorrelationVolume( sequences ); // First correlation volume
	// object->print( std::cout );

	kvs::glsl::RayCastingRenderer* renderer = new kvs::glsl::RayCastingRenderer();
	renderer->setName( "Renderer" );
	kvs::OpacityMap omap;
	omap.addPoint( 0, 0.6 );
	omap.addPoint( 125, 0.1 );
	omap.addPoint( 255, 0.6 );
	omap.create();
	kvs::ColorMap cmap;
	cmap.addPoint( 0, kvs::RGBColor::Blue() );
	cmap.addPoint( 125, kvs::RGBColor::White() );
	cmap.addPoint( 255, kvs::RGBColor::Red() );
	cmap.create();
	::tfunc.setOpacityMap( omap );
	::tfunc.setColorMap( cmap );
	::tfunc.setRange( -1.0, 1.0 );
	renderer->setTransferFunction( ::tfunc );
	renderer->setSamplingStep( resx / 30.0 );

	screen.registerObject( object, renderer );
	screen.setBackgroundColor( kvs::RGBColor::Black() );
	screen.setSize( 1024, 768 );
	screen.show();

	TransferFunctionEditor* editor = new TransferFunctionEditor( &screen );
	editor->setVolumeObject( object );
	editor->setTransferFunction( ::tfunc );
	editor->show();

	Label* label = new Label( &screen );
	label->setX( screen.width() * 0.25 );
	label->setY( screen.height() - 80 );
	label->show();
	
	slider = new CorrelationSlider( &screen );
	slider->setSequences( sequences );
	slider->setSliderColor( kvs::RGBColor::White() );
	slider->setX( screen.width() * 0.25 );
	slider->setY( screen.height() - 80 );
	slider->setWidth( screen.width() / 2 );
	slider->setValue( 0 );
	slider->setRange( 0, ::max_lag );
	slider->setMargin( 15 );
	slider->setCaption("");
	slider->setTextColor( kvs::RGBColor::White() );
	slider->show();

	KeyPressEvent* key = new KeyPressEvent();
	key->setSequences( sequences );
	screen.setEvent( key );

	return app.run();
}