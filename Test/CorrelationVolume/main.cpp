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
#include <kvs/HydrogenVolumeData>
#include <kvs/RayCastingRenderer>
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

namespace
{
	int best_lag = 3;
	int sequence_length = 27;
	int time_lag = 0;
	int max_lag = 10;
	kvs::TransferFunction tfunc = kvs::RGBFormulae::Ocean( 256 );	
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

	float** m_sequences;

public:

	CorrelationSlider( kvs::glut::Screen* screen ) : 
	kvs::glut::Slider( screen ){}

	void setSequences( float** sequences ){ m_sequences = sequences; }
	void valueChanged( void )
	{
		kvs::glut::Screen* glut_screen = static_cast<kvs::glut::Screen*>( screen() );
		::time_lag = this->value();
		
		float* pvalue = new float[8];
		for( size_t i = 0; i < 8; i++ )
			pvalue[i] = kun::Correlation::calculateLag( m_sequences[i * 2], m_sequences[i * 2 + 1], ::sequence_length, ::time_lag );

		kvs::ValueArray<float> values( pvalue, 8 );
		kvs::HydrogenVolumeData* object = new kvs::HydrogenVolumeData( kvs::Vec3ui::All( 2 ) );
		object->setValues( values );
		object->updateMinMaxValues();
		object->setName( "Object" );

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
	float** m_sequences;

public:
	void setSequences( float** sequences ){ m_sequences = sequences; }
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
        	pvalue[i] = kun::Correlation::calculateLag( m_sequences[i * 2], m_sequences[i * 2 + 1], ::sequence_length, ::time_lag );

        kvs::ValueArray<float> values( pvalue, 8 );
        kvs::HydrogenVolumeData* object = new kvs::HydrogenVolumeData( kvs::Vec3ui::All( 2 ) );
        object->setValues( values );
        object->updateMinMaxValues();
        object->setName( "Object" );

        kvs::glsl::RayCastingRenderer* renderer = new kvs::glsl::RayCastingRenderer();
        renderer->setName( "Renderer" );
        renderer->setTransferFunction( ::tfunc );

        glut_screen->scene()->objectManager()->change( "Object", object, true );
        glut_screen->scene()->replaceRenderer( "Renderer", renderer, true );

        slider->setValue( ::time_lag );
        screen()->redraw();
    }
};


float** readCSVtoSequences( char* filename )
{
	std::ifstream ifs( filename, std::ifstream::in );
	char* buf = new char[256];
	ifs.getline( buf, 256 ); // Skip
	ifs.getline( buf, 256 ); // Skip

	float** sequence = new float*[16];
	for( size_t i = 0; i < 16; i++ ) sequence[i] = new float[::sequence_length];

	for( size_t i = 0; i < ::sequence_length; i++ )
	{
		ifs.getline( buf, 256 );
		int j = 0;
		int lag = ( i + ::best_lag ) % ::sequence_length;
		std::sscanf( buf, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f",
		 &sequence[j++][i], &sequence[j++][lag], &sequence[j++][i], &sequence[j++][lag],
		 &sequence[j++][i], &sequence[j++][lag], &sequence[j++][i], &sequence[j++][lag],
		 &sequence[j++][i], &sequence[j++][lag], &sequence[j++][i], &sequence[j++][lag],
		 &sequence[j++][i], &sequence[j++][lag], &sequence[j++][i], &sequence[j++][lag] );
	}

	return sequence;
}


int main( int argc, char** argv )
{
	kvs::glut::Application app( argc, argv );
	kvs::glut::Screen screen( &app );

	float** sequences = readCSVtoSequences( argv[1] );
	float* pvalue = new float[8];
	for( size_t i = 0; i < 8; i++ )
	{
		pvalue[i] = kun::Correlation::calculate( sequences[i * 2], sequences[i * 2 + 1], 27 );
		std::cout << pvalue[i] << std::endl;
	}

	kvs::ValueArray<float> values( pvalue, 8 );

	kvs::HydrogenVolumeData* object = new kvs::HydrogenVolumeData( kvs::Vec3ui::All( 2 ) );
	object->setValues( values );
	object->updateMinMaxValues();
	object->setName( "Object" );

	kvs::glsl::RayCastingRenderer* renderer = new kvs::glsl::RayCastingRenderer();
	renderer->setName( "Renderer" );
	renderer->setTransferFunction( ::tfunc );

	screen.registerObject( object, renderer );
	screen.setBackgroundColor( kvs::RGBColor::Black() );
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