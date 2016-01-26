//
//  main.cpp
//
//
//  Created by Kun Zhao on 2015-12-10 17:11:16.
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
#include <memory>
#include <iostream>
#include <fstream>
#include <kvs/SharedPointer>

namespace
{
	int best_lag = 3;
	int sequence_length = 27;
	int time_lag = 0;
	int max_lag = 10;
	kvs::TransferFunction tfunc = kvs::RGBFormulae::Jet( 256 );
}

kvs::StructuredVolumeObject* setVolumeObject( const kvs::ValueArray<float>& value_array )
{
	kvs::StructuredVolumeObject* object = new kvs::StructuredVolumeObject();
	object->setName( "Object" );
	object->setGridTypeToUniform();
	object->setResolution( kvs::Vec3ui::All( 2 ) );
	object->setVeclen( 1 );
	object->setValues( value_array );
	object->updateMinMaxCoords();
	object->updateMinMaxValues();

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
		
		float* pvalue = new float[8];
		for( size_t i = 0; i < 8; i++ )
			pvalue[i] = kun::Correlation::calculateLag( m_sequences[i * 2], m_sequences[i * 2 + 1], ::time_lag );

		// kvs::ValueArray<float> values = kvs::ValueArray<float>( pvalue, 8 );
		kvs::StructuredVolumeObject* object = setVolumeObject( kvs::ValueArray<float>( pvalue, 8 ) );

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
        
        float* pvalue = new float[8];
        for( size_t i = 0; i < 8; i++ )
        {
        	pvalue[i] = kun::Correlation::calculateLag( m_sequences[i * 2], m_sequences[i * 2 + 1], ::time_lag );
        	std::cout << pvalue[i] << ", ";
        }
        std::cout << std::endl;

        kvs::ValueArray<float> values( pvalue, 8 );
        kvs::StructuredVolumeObject* object = setVolumeObject( values );

        kvs::glsl::RayCastingRenderer* renderer = new kvs::glsl::RayCastingRenderer();
        renderer->setName( "Renderer" );
        renderer->setTransferFunction( ::tfunc );

        glut_screen->scene()->objectManager()->change( "Object", object, true );
        glut_screen->scene()->replaceRenderer( "Renderer", renderer, true );

        slider->setValue( ::time_lag );
        screen()->redraw();
    }
};

std::vector<kvs::ValueArray<float> > readCSVtoSequences( char* filename )
{
	std::ifstream ifs( filename, std::ifstream::in );
	char* buf = new char[256];
	ifs.getline( buf, 256 ); // Skip
	ifs.getline( buf, 256 ); // Skip

	std::vector<kvs::ValueArray<float> > sequences;
	kvs::ValueArray<float>* value_arryies = new kvs::ValueArray<float>[16];
	for( unsigned int i = 0; i < 16; i++ ) value_arryies[i].allocate( ::sequence_length );

	for( size_t i = 0; i < ::sequence_length; i++ )
	{
		ifs.getline( buf, 256 );
		int lag = ( i + ::best_lag ) % ::sequence_length;
		std::sscanf( buf, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f",
		 &value_arryies[0].data()[i], &value_arryies[1].data()[lag], &value_arryies[2].data()[i], &value_arryies[3].data()[lag],
		 &value_arryies[4].data()[i], &value_arryies[5].data()[lag], &value_arryies[6].data()[i], &value_arryies[7].data()[lag],
		 &value_arryies[8].data()[i], &value_arryies[9].data()[lag], &value_arryies[10].data()[i], &value_arryies[11].data()[lag],
		 &value_arryies[12].data()[i], &value_arryies[13].data()[lag], &value_arryies[14].data()[i], &value_arryies[15].data()[lag] );
	}

	for( size_t i = 0; i < 16; i++ ) sequences.push_back( value_arryies[i] );

	return sequences;
}

int main( int argc, char** argv )
{
	kvs::glut::Application app( argc, argv );
	kvs::glut::Screen screen( &app );

	std::vector<kvs::ValueArray<float> > sequences = readCSVtoSequences( argv[1] );
	float* pvalue = new float[8];
	for( size_t i = 0; i < 8; i++ )
	{
		pvalue[i] = kun::Correlation::calculate( sequences[i * 2], sequences[i * 2 + 1] );
		std::cout << pvalue[i] << ", ";
	}
	std::cout << std::endl;

	kvs::ValueArray<float> values( pvalue, 8 );
	kvs::StructuredVolumeObject* object = setVolumeObject( values );

	kvs::glsl::RayCastingRenderer* renderer = new kvs::glsl::RayCastingRenderer();
	renderer->setName( "Renderer" );
	kvs::OpacityMap omap;
	omap.addPoint( 0.0, 1.0 );
	omap.addPoint( 1.0, 1.0 );
	omap.create();
	kvs::ColorMap cmap;
	cmap.addPoint( 0, kvs::RGBColor::Blue() );
	cmap.addPoint( 125, kvs::RGBColor::White() );
	cmap.addPoint( 255, kvs::RGBColor::Red() );
	cmap.create();
	tfunc.setOpacityMap( omap );
	tfunc.setColorMap( cmap );
	tfunc.setRange( -1.0, 1.0 );
	renderer->setTransferFunction( ::tfunc );

	screen.registerObject( object, renderer );
	screen.setBackgroundColor( kvs::RGBColor::Black() );
	screen.setSize( 1024, 768 );
	screen.show();

	TransferFunctionEditor* editor = new TransferFunctionEditor( &screen );
	editor->setVolumeObject( object );
	editor->setTransferFunction( tfunc );
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