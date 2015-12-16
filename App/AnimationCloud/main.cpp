//
//  main.cpp
//
//
//  Created by Kun Zhao on 2015-10-30 14:12:12.
//
//

#include <kvs/glut/Application>
#include <kvs/glut/Screen>
#include <kvs/StructuredVolumeImporter>
#include <kvs/StructuredVolumeObject>
#include <kvs/UnstructuredVolumeObject>
#include <kvs/TransferFunction>
#include <kvs/ColorMap>
#include <kvs/OpacityMap>
#include <kvs/glut/TransferFunctionEditor>
#include <kvs/RendererManager>
#include <kvs/CommandLine>
#include <kvs/glut/RadioButton>
#include <kvs/glut/RadioButtonGroup>
#include <kvs/HydrogenVolumeData>
#include <kvs/ObjectManager>
#include <kvs/glut/Slider>
#include <kvs/glut/Label>
#include <kvs/Directory>
#include <kvs/FileList>
#include <kvs/glut/Timer>
#include <kvs/TimerEventListener>
#include "ParticleBasedRenderer.h"
#include "PointImporter.h"
#include "PointObject.h"
#include "FPS.h"
#include "CloudObject.h"

namespace
{
    kun::PointObject** object = NULL;

    kvs::glut::Timer* glut_timer;
    std::vector<std::string> file_name;
    kvs::TransferFunction tfunc( 256 );

    kvs::RGBColor label_color = kvs::RGBColor( 255, 255, 255 );

    int    msec;
    int    nsteps;
    int    time_step;

    int    parameter = 0;

    size_t repetition = 5;

    bool ShadingFlag = true;

    float base_opacity = 0.2;

    const std::string ObjectName( "ParticleObject" ); 
    const std::string RendererName( "ParticleRenderer" );
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

        if(::ShadingFlag == false)
        {
            renderer->disableShading();
        }
        ::tfunc.setOpacityMap(transferFunction().opacityMap());
        ::tfunc.setColorMap(transferFunction().colorMap() );
        renderer->setTransferFunction( transferFunction() );
        renderer->setBaseOpacity( ::base_opacity );
        renderer->setRepetitionLevel( ::repetition );
        std::cout << "TF renderer time: " << renderer->timer().msec() << std::endl;
        screen()->redraw();
    }
};

TransferFunctionEditor* editor = NULL;

void initialize( std::string filename, size_t input_timestep = 0 )
{
    ::time_step = 0;
    ::msec = 20;
    
    kvs::Directory directory( filename );
    const kvs::FileList files = directory.fileList();
    int file_length = files.size();
    ::nsteps = file_length / 8;

    if( input_timestep )
        ::nsteps = input_timestep;
    
    ::object = new kun::PointObject*[::nsteps];
    std::cout << ::nsteps << std::endl;
    
    kvs::Timer time;
    time.start();
    for ( int i = 0; i < ::nsteps; i++ )
    {
    	char* buf = new char[10];
    	sprintf( buf, "%04d", i * 60 );
    	std::string name = filename + "sdbin_all0000" + buf + "_";
    	kun::CloudObject* cloud = new kun::CloudObject( name, 8 );
    	::object[i] = cloud->toKUNPointObject( ::parameter );
        ::object[i]->setName( ::ObjectName );
        delete cloud;
        std::cout << "\r" << i << std::flush;        
    }
    time.stop();
    std::cout << "\r" << "                           " << std::flush;
    std::cout << "\r" << "Finish Reading." << std::endl;
    std::cout <<"Loading time: " << time.msec() << "msec" <<std::endl;
    ::time_step = 0;
}


class Label : public kvs::glut::Label
{
public:

    Label( kvs::ScreenBase* screen ):
    kvs::glut::Label( screen )
    {
        setTextColor( ::label_color  );
        setMargin( 10 );
    }
    
    void screenUpdated( void )
    {
        char* buf = new char[256];
        sprintf( buf, "Time step : %03d", ::time_step );
        setText( std::string( buf ).c_str() );
    }
};

class TimeSlider : public kvs::glut::Slider
{
public:

    TimeSlider( kvs::glut::Screen* screen ):
    kvs::glut::Slider( screen ){};
    
    void valueChanged( void )
    {
        ::glut_timer->stop();
        ::time_step = int( this->value() );
        kvs::glut::Screen* glut_screen = static_cast<kvs::glut::Screen*>( screen() );
        kun::ParticleBasedRenderer* renderer = new kun::ParticleBasedRenderer();

        if(::ShadingFlag == false)
        {
            renderer->disableShading();
        }

        renderer->setName( ::RendererName );
        renderer->setBaseOpacity( ::base_opacity );
        renderer->setTransferFunction( ::tfunc );
        renderer->setRepetitionLevel( ::repetition );

        kun::PointObject* object_current = NULL;
        object_current = ::object[::time_step];

        glut_screen->scene()->objectManager()->change( ::ObjectName, object_current, false );
        glut_screen->scene()->rendererManager()->change( ::RendererName, renderer, true );
        glut_screen->redraw();    
    }
};

Label* label;
TimeSlider* slider;

class TimerEvent : public kvs::TimerEventListener
{
    void update( kvs::TimeEvent* event )
    {
        kvs::glut::Screen* glut_screen = static_cast<kvs::glut::Screen*>( screen() );
        kun::ParticleBasedRenderer* renderer = new kun::ParticleBasedRenderer();

        renderer->setName( ::RendererName );
        if( ::ShadingFlag == false )
        {
            renderer->disableShading();
        }
        renderer->setTransferFunction( ::tfunc );
        renderer->setRepetitionLevel( ::repetition );
        renderer->setBaseOpacity( ::base_opacity );

        glut_screen->scene()->objectManager()->change( ::ObjectName, ::object[::time_step++], false );
        glut_screen->scene()->replaceRenderer( ::RendererName, renderer, true );
        slider->setValue( (float)::time_step );
        glut_screen->redraw();

        if( ::time_step == ::nsteps ) 
        {
            ::time_step = 0;
            ::glut_timer->stop();
        }
    }
};

class KeyPressEvent : public kvs::KeyPressEventListener
{
    void update( kvs::KeyEvent* event )
    {
        switch ( event->key() )
        {
            case kvs::Key::s:
            {
                if ( ::glut_timer-> isStopped() )
                {
                    ::glut_timer->start();
                    screen()->redraw();
                }
                else
                {
                    ::glut_timer->stop();
                    screen()->redraw();
                }
                break;
                
            }
        }
    }
};

int main( int argc, char** argv )
{
    kvs::glut::Application app( argc, argv );
    kvs::glut::Screen screen( &app );
    KeyPressEvent     key_press_event;
    TimerEvent        timer_event;
    kun::FPS          fps;
    ::glut_timer = new kvs::glut::Timer( ::msec );

    kvs::CommandLine param( argc, argv );
    param.addHelpOption();
    param.addOption( "nos", "No Shading", 0, false );
    param.addOption( "c", "Cloud Folder", 1, false );
    param.addOption( "o", "base opacity", 1, false );
    param.addOption( "rep", "repetition level", 1, false );
    param.addOption( "trans", "set initial transferfunction", 1, false );
    param.addOption( "nsteps", "set n time steps to load", 1, false );
    param.addOption( "p", "parameter", 1, false );
    
    if ( !param.parse() ) exit( 0 );

    ::repetition = param.optionValue<size_t>( "rep" );
    ::base_opacity = param.optionValue<float>( "o" );

    // Base transfer function
    if( param.hasOption( "trans" ) )
        ::tfunc = kvs::TransferFunction( param.optionValue<std::string>( "trans" ) );
    if ( param.hasOption( "nos" ) ) ::ShadingFlag = false;
    if( param.hasOption( "p" ) ) ::parameter = param.optionValue<int>( "p" );

    // Time-varying data loading
    size_t input_timestep = 0;
    if( param.hasOption( "nsteps" ) ) input_timestep = param.optionValue<size_t>( "nsteps" );

    initialize( param.optionValue<std::string>( "c" ), input_timestep );

    kun::ParticleBasedRenderer* renderer = new kun::ParticleBasedRenderer();
    renderer->setName( ::RendererName );

    renderer->setBaseOpacity( ::base_opacity );
    renderer->setTransferFunction( ::tfunc );
    renderer->setRepetitionLevel( ::repetition );

    if( ::ShadingFlag == false ) renderer->disableShading();

    kun::PointObject* object_first = NULL;

    object_first = ::object[0];

    //screen.scene()->camera()->setPosition( kvs::Vector3f(0, 0, 3), kvs::Vector3f(1, 0, 0) );
    object_first->print( std::cout );
    screen.registerObject( object_first, renderer );
    screen.setBackgroundColor( kvs::RGBColor( 0, 0, 0) );
    screen.show();
    std::cout << "TEST" << std::endl;

    screen.addEvent( &fps );
    screen.addEvent( &key_press_event );
    screen.addTimerEvent( &timer_event, ::glut_timer );

    // Set the transfer function editor
    kvs::StructuredVolumeObject* pointdummy = new kvs::StructuredVolumeObject();
    pointdummy->setGridType( kvs::StructuredVolumeObject::Uniform );
    pointdummy->setVeclen( 1 );
    pointdummy->setResolution( kvs::Vector3ui( 1, 1, object_first->numberOfVertices() ) );
    pointdummy->setValues( object_first->values() );
    pointdummy->updateMinMaxValues();
    std::cout << "Number of points for the first time step: " << pointdummy->numberOfNodes() <<std::endl;
    
    editor = new TransferFunctionEditor( &screen );
    editor->setVolumeObject( pointdummy );
    editor->setTransferFunction( ::tfunc );
    editor->show();

    //lable
    label = new Label( &screen );
    label->setX( screen.width() * 0.25 );
    label->setY( screen.height() - 80 );
    label->show();
    
    // slider
    slider = new TimeSlider( &screen );
    slider->setSliderColor( ::label_color );
    slider->setX( screen.width() * 0.25 );
    slider->setY( screen.height() - 80 );
    slider->setWidth( screen.width() / 2 );
    slider->setValue( 0.0 );
    slider->setRange( 0.0, ::nsteps );
    slider->setMargin( 15 );
    slider->setCaption("");
    slider->setTextColor( ::label_color  );
    slider->show();
    
    ::glut_timer->start( ::msec );
    ::glut_timer->stop();
    return app.run();
}
