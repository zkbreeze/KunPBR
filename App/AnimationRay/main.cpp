//
//  main.cpp
//
//
//  Created by Kun Zhao on 2014-09-04 14:45:55.
//
//

#include <kvs/glut/Application>
#include <kvs/glut/Screen>
#include <kvs/glut/Slider>
#include <kvs/glut/Label>
#include <kvs/glut/LegendBar>
#include <kvs/RayCastingRenderer>
#include <kvs/StructuredVolumeObject>
#include <kvs/StructuredVolumeImporter>
#include <kvs/StructuredVolumeExporter>
#include <kvs/KVSMLObjectStructuredVolume>
#include <kvs/CommandLine>
#include <float.h>

#include <kvs/Directory>
#include <kvs/FileList>
#include <kvs/glut/Timer>
#include <kvs/objectManager>
#include <kvs/rendererManager>
#include <kvs/TimerEventListener>
#include "JetImporter.h"

kvs::StructuredVolumeObject** object;
kvs::glut::Timer* glut_timer;
std::string filename;
std::string objectname = "object";
kvs::TransferFunction tfunc( 256 );
float MIN = FLT_MAX;
float MAX = -FLT_MAX;

bool isJetData = false;

int    msec;
int    nsteps;
int    time_step;

void initialize( std::string filename, int start_of_step = 0, int end_of_step = 0 )
{
    time_step = 0;
//    filename = "5/Density";
    int input_number_of_steps = end_of_step - start_of_step;

    msec = 20;
    nsteps = 0;
    kvs::Directory directory( filename );
    const kvs::FileList files = directory.fileList();
    int file_length = files.size();
    std::vector<std::string> file_name;
    for ( int i = 0; i < file_length; i++ )
    {
        const kvs::File file = files[i];
        if( file.extension() == "kvsml" || file.extension() == "dat" )
        {
            nsteps++;
            file_name.push_back( file.filePath() );
        }
    }

    if( input_number_of_steps ) 
    {
        if( input_number_of_steps > nsteps ) exit( 0 );
        else
            nsteps = input_number_of_steps;
    }

    object = new kvs::StructuredVolumeObject*[nsteps];
    std::cout << "There are " << nsteps << " time steps" << std::endl;

    kvs::Timer time;
    time.start();
    for ( int i = 0; i < nsteps; i++ )
    {
        if( isJetData ) object[i] = new kun::JetImporter( file_name[i + start_of_step] ); 
        else
            object[i] = new kvs::StructuredVolumeImporter( file_name[i + start_of_step] );
        object[i]->setName( "object" );
        std::cout << "\r" << i << std::flush;        
    }
    time.stop();
    std::cout << "\r" << "                           " << std::flush;
    std::cout << "\r" << "Finish Reading." << std::endl;
    std::cout << "Loading time: " << time.msec() << "msec" << std::endl;
    time_step = 0;
}

class Label : public kvs::glut::Label
{
public:
    
    Label( kvs::ScreenBase* screen ):
    kvs::glut::Label( screen )
    {
        setTextColor( kvs::RGBColor( 0, 0, 0 ) );
        setMargin( 10 );
    }
    
    void screenUpdated( void )
    {
        char* buf = new char[256];
        sprintf( buf, "Time step : %03d", time_step );
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
        kvs::glut::Screen* glut_screen = static_cast<kvs::glut::Screen*>( screen() );

        glut_timer->stop();
        time_step = int( this->value() );
        kvs::glsl::RayCastingRenderer* renderer = new kvs::glsl::RayCastingRenderer();
        renderer->setName( "renderer" );
        renderer->enableShading();
        renderer->setShader( kvs::Shader::Phong( 0.5, 0.5, 0.8, 15.0 ) );
//        tfunc.setRange( 10.0, object[time_step]->maxValue() );
        renderer->setTransferFunction( tfunc );
        
        glut_screen->scene()->objectManager()->change( "object", object[time_step], false );
        glut_screen->scene()->rendererManager()->change( "renderer", renderer, true );
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

        kvs::glsl::RayCastingRenderer* renderer = new kvs::glsl::RayCastingRenderer();
        renderer->setName( "renderer" );
        renderer->enableShading();
        renderer->setShader( kvs::Shader::Phong( 0.5, 0.5, 0.8, 15.0 ) );
        renderer->setTransferFunction( tfunc );
        
        glut_screen->scene()->objectManager()->change( "object", object[time_step++], false );
        glut_screen->scene()->rendererManager()->change( "renderer", renderer, true );
        slider->setValue( (float)time_step );
        std::cout << "\r" << time_step <<std::flush;
        glut_screen->redraw();
        if( time_step == nsteps ) 
        {
            time_step = 0;
            glut_timer->stop();
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
                if ( glut_timer-> isStopped() )
                {
                    glut_timer->start();
                    screen()->redraw();
                }
                else
                {
                    glut_timer->stop();
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
    
    kvs::CommandLine param( argc, argv );
    param.addHelpOption();
    param.addOption( "f", "Folder name of the KVSML objects", 1, false );
    param.addOption( "j", "Folder name of the jet data", 1, false );
    param.addOption( "t", "Transfer function", 1, false );
    param.addOption( "s", "Start time step you want to render", 1, false );
    param.addOption( "e", "End time step you want to render", 1, false );

    if( !param.parse() ) exit( 0 );

    std::string input_file;
    if( param.hasOption( "f" ) ) input_file = param.optionValue<std::string>( "f" );
    if( param.hasOption( "j" ) ) 
    {
        isJetData = true;
        input_file = param.optionValue<std::string>( "j" );
    }
    if( param.hasOption( "t" ) ) tfunc = kvs::TransferFunction( param.optionValue<std::string>( "t" ) );

    if( param.hasOption( "s" ) && param.hasOption( "e") ) initialize( input_file, param.optionValue<int>( "s" ), param.optionValue<int>( "e" ) );
    else
        initialize( input_file );

    KeyPressEvent     key_press_event;
    TimerEvent        timer_event;

    kvs::glut::Screen screen( &app );
    glut_timer = new kvs::glut::Timer( msec );
    
    kvs::glsl::RayCastingRenderer* renderer = new kvs::glsl::RayCastingRenderer();
    renderer->setName( "renderer" );
    renderer->enableShading();
    renderer->setShader( kvs::Shader::Phong( 0.5, 0.5, 0.8, 15.0 ) );
    renderer->setTransferFunction( tfunc );
    screen.registerObject( object[0], renderer );

    screen.addEvent( &key_press_event );
    screen.addTimerEvent( &timer_event, glut_timer );
    screen.setGeometry( 0, 0, 800, 600 );
//    screen.background()->setColor( kvs::RGBColor( 0, 0, 0 ) );
    screen.setTitle( "AnimationRayCasting" );
    screen.show();
    
    //lable
    label = new Label( &screen );
    label->setX( screen.width() * 0.25 );
    label->setY( screen.height() - 80 );
    label->show();
    
    // slider
    slider = new TimeSlider( &screen );
    slider->setSliderColor( kvs::RGBColor( 0, 0, 0 ) );
    slider->setX( screen.width() * 0.25 );
    slider->setY( screen.height() - 80 );
    slider->setWidth( screen.width() / 2 );
    slider->setValue( 0.0 );
    slider->setRange( 0.0, nsteps );
    slider->setMargin( 15 );
    slider->setCaption("");
    slider->setTextColor( kvs::RGBColor( 0, 0, 0 ) );
    slider->show();
    
    glut_timer->start( msec );
    glut_timer->stop();
    
    return( app.run() );
}

