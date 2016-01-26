//
//  main.cpp
//
//
//  Created by Kun Zhao on 2016-01-22 20:45:05.
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
#include "PointObject.h"
#include "ParticleBasedRenderer.h"

namespace
{
	int time_lag = 0;
	kvs::TransferFunction tfunc = kvs::RGBFormulae::Jet( 256 );
	kvs::Vec3ui resolution;
	kvs::ValueArray<float> coords;
	int repetition = 10;
	int max_lag = 50;
	float bo = 0.1; // base opacity
}

kun::PointObject* createPointObject( const kvs::ValueArray<float>& values )
{
	kun::PointObject* point = new kun::PointObject();
	point->setName( "Object" );
	point->setVeclen( 1 );
	point->setCoords( ::coords );
	point->setValues( values );
	point->updateMinMaxValues();
	point->updateMinMaxCoords();
	return point;
}

std::vector<kvs::ValueArray<float> > loadArrays( const std::string& distname, const int step = 0 )
{
	std::vector<kvs::ValueArray<float> > arrays;

	kvs::Directory directory( distname );
	const kvs::FileList files = directory.fileList();
	int file_length = files.size();
	std::vector<std::string> filename;

	int nsteps = 0;
	for ( int i = 0; i < file_length; i++ )
	{
	    const kvs::File file = files[i];
	    if( file.extension() == "dat" )
	    {
	        nsteps++;
	        filename.push_back( file.filePath() );
	    }
	}

	if( step ) 
	{
	    if( step > nsteps ) exit( 0 );
	    else
	        nsteps = step;
	}

	::max_lag = nsteps * 0.3;

	std::cout << "Start to load arrays." << std::endl;
	std::cout << "There are " << nsteps << " time steps" << std::endl;

	kvs::Timer time;
	time.start();
	float* pvalue = NULL;
	for ( int i = 0; i < nsteps; i++ )
	{
		std::ifstream ifs( filename[i].c_str(), std::ios::in );
		ifs.seekg( 0, ifs.end );
		unsigned int size = ifs.tellg() / 4;
		ifs.seekg( 0, ifs.beg );
		pvalue = new float[size];
		ifs.read( (char*)pvalue, sizeof(float) * size );
		ifs.close();
		arrays.push_back( kvs::ValueArray<float>( pvalue, size ) );
	    std::cout << "\r" << i << std::flush;        
	}
	time.stop();
	std::cout << "Reading time of the arrays: " << time.msec() << " msec" << std::endl;

	return arrays;
}

kun::PointObject* calculateCorrelationPoint( const std::vector<kvs::ValueArray<float> >& sequences1, const std::vector<kvs::ValueArray<float> >& sequences2, int time_lag = 0 )
{
	if( sequences1.size() != sequences2.size() )
	{
		std::cerr << "Error of two sequences size (not same)." << std::endl;
		exit( 0 );
	}

	unsigned int size = sequences1.size();
	float* pvalue = new float[size];
	for( unsigned int i = 0; i < size; i++ )
	{
		if( time_lag )
			pvalue[i] = kun::Correlation::calculateLag( sequences1[i], sequences2[i], ::time_lag );
		else
			pvalue[i] = kun::Correlation::calculate( sequences1[i], sequences2[i] );
	}

	return createPointObject( kvs::ValueArray<float>( pvalue, size ) );
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
		kun::ParticleBasedRenderer* renderer = static_cast<kun::ParticleBasedRenderer*>( r );
		renderer->setTransferFunction( transferFunction() );
		renderer->setBaseOpacity( ::bo );
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

	std::vector<kvs::ValueArray<float> > m_sequences1;
	std::vector<kvs::ValueArray<float> > m_sequences2;

public:

	CorrelationSlider( kvs::glut::Screen* screen ) : 
	kvs::glut::Slider( screen ){}

	void setSequences( const std::vector<kvs::ValueArray<float> >& sequences1, const std::vector<kvs::ValueArray<float> >& sequences2 ){ m_sequences1 = sequences1; m_sequences2 = sequences2; }
	void valueChanged( void )
	{
		kvs::glut::Screen* glut_screen = static_cast<kvs::glut::Screen*>( screen() );
		::time_lag = this->value();
		
		kun::PointObject* object = calculateCorrelationPoint( m_sequences1, m_sequences2, ::time_lag );

		kun::ParticleBasedRenderer* renderer = new kun::ParticleBasedRenderer();
		renderer->setName( "Renderer" );
		renderer->setRepetitionLevel( ::repetition );
		renderer->setBaseOpacity( ::bo );
		renderer->setParticleScale( 2.0 );
		renderer->setTransferFunction( ::tfunc );

		glut_screen->scene()->objectManager()->change( "Object", object, true );
		glut_screen->scene()->replaceRenderer( "Renderer", renderer, true );

		screen()->redraw();
	}
};

CorrelationSlider* slider;

class KeyPressEvent : public kvs::KeyPressEventListener
{
	std::vector<kvs::ValueArray<float> > m_sequences1;
	std::vector<kvs::ValueArray<float> > m_sequences2;

public:
	void setSequences( const std::vector<kvs::ValueArray<float> >& sequences1, const std::vector<kvs::ValueArray<float> >& sequences2 ){ m_sequences1 = sequences1; m_sequences2 = sequences2; }
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
        kun::PointObject* object = calculateCorrelationPoint( m_sequences1, m_sequences2, ::time_lag );

        kun::ParticleBasedRenderer* renderer = new kun::ParticleBasedRenderer();
        renderer->setName( "Renderer" );
        renderer->setRepetitionLevel( ::repetition );
        renderer->setBaseOpacity( ::bo );
        renderer->setParticleScale( 2.0 );
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
	param.addOption( "nsteps", "How many steps to load", 1, false );
	param.addOption( "d1", "Directory name of the first arrays", 1, true );
	param.addOption( "d2", "Directory name of the seconde arrays", 1, true );
	param.addOption( "b", "Base opacity.", 1, false );
	param.addHelpOption();
	if( !param.parse() ) exit( 0 );

	int nsteps = 99; // By default
	if( param.hasOption( "nsteps" ) ) nsteps = param.optionValue<int>( "nsteps" );
	if( param.hasOption( "b" ) ) ::bo = param.optionValue<float>( "b" );

	std::string dist1 = param.optionValue<std::string>( "d1" );
	std::string dist2 = param.optionValue<std::string>( "d2" );
	std::vector<kvs::ValueArray<float> > arrays1 = loadArrays( dist1, nsteps );	
	std::vector<kvs::ValueArray<float> > arrays2 = loadArrays( dist2, nsteps );

	// load coords
	std::ifstream ifs( std::string( dist1 + "/coords.crd" ).c_str(), std::ios::in );
	// ifs.seekg( 0, ifs.end );
	// int size = ifs.tellg() / 4;
	// ifs.seekg( 0, ifs.beg );
	int size = arrays1[0].size() * 3;
	float* pcoord = new float[size];
	ifs.read( (char*)pcoord, sizeof(float) * size );
	ifs.close();
	::coords = kvs::ValueArray<float>( pcoord, size );

	std::vector<kvs::ValueArray<float> > sequences1 = kun::Correlation::arraysToSequences( arrays1 );
	std::vector<kvs::ValueArray<float> > sequences2 = kun::Correlation::arraysToSequences( arrays2 );

	kun::PointObject* object = calculateCorrelationPoint( sequences1, sequences2 );
	object->print( std::cout );

	kun::ParticleBasedRenderer* renderer = new kun::ParticleBasedRenderer();
	renderer->setName( "Renderer" );
	renderer->setRepetitionLevel( ::repetition );
	renderer->setBaseOpacity( ::bo );
	renderer->setParticleScale( 2.0 );
	kvs::OpacityMap omap;
	omap.addPoint( 0, 0.6 );
	omap.addPoint( 125, 0.05 );
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

	screen.registerObject( object, renderer );
	screen.setBackgroundColor( kvs::RGBColor::Black() );
	screen.setSize( 1024, 768 );
	screen.show();

	// Transfer function editor
	kvs::StructuredVolumeObject* pointdummy = new kvs::StructuredVolumeObject();
	pointdummy->setGridType( kvs::StructuredVolumeObject::Uniform );
	pointdummy->setVeclen( 1 );
	pointdummy->setResolution( kvs::Vector3ui( 1, 1, object->numberOfVertices() ) );
	pointdummy->setValues( object->values() );
	pointdummy->updateMinMaxValues();
	pointdummy->updateMinMaxCoords();
	
	TransferFunctionEditor* editor = new TransferFunctionEditor( &screen );
	editor->setVolumeObject( pointdummy );
	editor->setTransferFunction( ::tfunc );
	editor->show();

	Label* label = new Label( &screen );
	label->setX( screen.width() * 0.25 );
	label->setY( screen.height() - 80 );
	label->show();
	
	slider = new CorrelationSlider( &screen );
	slider->setSequences( sequences1, sequences2 );
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
	key->setSequences( sequences1, sequences2 );
	screen.setEvent( key );

	return app.run();
}