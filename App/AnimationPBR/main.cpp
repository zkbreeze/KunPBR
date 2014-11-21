
//
//  main.cpp
//
//
//  Created by Kun Zhao on 2014/08/30.
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
#include "ParticleBasedRendererGLSL.h"
#include "PointImporter.h"
#include "PointObject.h"
#include <kvs/ParticleBasedRenderer>
#include "FPS.h"

#define TETRA 4
#define PRISM 6


namespace
{
    kun::PointObject** object;
    bool isLODRendering = false;

    kvs::glut::Timer* glut_timer;
    std::vector<std::string> file_name;
    kvs::TransferFunction tfunc( 256 );

    kvs::RGBColor label_color = kvs::RGBColor( 0, 0, 0 );

    int    msec;
    int    nsteps;
    int    time_step;

    size_t repetition;
    size_t repetition_low;

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
        std::cout << "TF renderer time: " << renderer->timer().msec() << std::endl;
        screen()->redraw();
    }
};

TransferFunctionEditor* editor = NULL;

void initialize( std::string filename, float fraction = 0.0, size_t input_timestep = 0 )
{
    ::time_step = 0;
    ::msec = 20;
    ::nsteps = 0;
    kvs::Directory directory( filename );
    const kvs::FileList files = directory.fileList();
    int file_length = files.size();

    for ( int i = 0; i < file_length; i++ )
    {
        const kvs::File file = files[i];
        if( file.extension() == "kvsml" )
        {
            ::nsteps++;
            file_name.push_back( file.filePath() );
        }
    }

    if( input_timestep )
        ::nsteps = input_timestep;
    
    ::object = new kun::PointObject*[::nsteps];
    std::cout << ::nsteps << std::endl;
    
    kvs::Timer time;
    time.start();
    for ( int i = 0; i < ::nsteps; i++ )
    {
    if( fraction ) 
        ::object[i] = new kun::PointImporter( file_name[i], fraction );
    else
        ::object[i] = new kun::PointImporter( file_name[i] );
        ::object[i]->setName( ::ObjectName );
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
        if ( ::isLODRendering )
        {
            object_current = new kun::PointImporter( ::file_name[::time_step] );
            object_current->setName( ::ObjectName );
        }
        else
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
        if ( ::isLODRendering )
            renderer->setRepetitionLevel( ::repetition_low );
        else
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
                    if ( ::isLODRendering )
                    {
                        kvs::glut::Screen* glut_screen = static_cast<kvs::glut::Screen*>( screen() );
                        kun::ParticleBasedRenderer* renderer = new kun::ParticleBasedRenderer();
                        kun::PointObject* object_current = new kun::PointImporter( ::file_name[::time_step - 1] );
                        object_current->setName( ::ObjectName );
                        std::cout << std::endl;
                        std::cout << "Finish loading " << ::file_name[::time_step - 1] <<std::endl;

                        renderer->setName( ::RendererName );
                        renderer->setBaseOpacity( ::base_opacity );
                        renderer->setTransferFunction( ::tfunc );
                        renderer->setRepetitionLevel( ::repetition );

                        glut_screen->scene()->objectManager()->change( ::ObjectName, object_current, false );
                        glut_screen->scene()->rendererManager()->change( ::RendererName, renderer, true );
                    }
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
    param.addOption( "point", "KVSML Point Data Filename", 1, true );
    param.addOption( "nos", "No Shading", 0, false );
    param.addOption( "o", "base opacity", 1, true );
    param.addOption( "rep", "repetition level", 1, true );
    param.addOption( "rep_low", "low repetition level used for animation", 1, false );
    param.addOption( "trans", "set initial transferfunction", 1, false );
    param.addOption( "nsteps", "set n time steps to load", 1, false );
    
    if ( !param.parse() ) return 1;

    ::repetition = param.optionValue<size_t>( "rep" );
    ::base_opacity = param.optionValue<float>( "o" );

    if( param.hasOption( "rep_low" ) )
    {
        ::isLODRendering= true;
        ::repetition_low = param.optionValue<size_t>( "rep_low" );
    }

    std::cout << "fraction: " << (float)::repetition_low / ::repetition << std::endl;

    // Base transfer function
    if( param.hasOption( "trans" ) )
        ::tfunc = kvs::TransferFunction( param.optionValue<std::string>( "trans" ) );
    if ( param.hasOption( "nos" ) ) ::ShadingFlag = false;

    // Time-varying data loading
    size_t input_timestep = 0;
    if( param.hasOption( "nsteps" ) ) input_timestep = param.optionValue<size_t>( "nsteps" );
    if( ::isLODRendering )
        initialize( param.optionValue<std::string>( "point" ), (float)::repetition_low / ::repetition, input_timestep );
    else
        initialize( param.optionValue<std::string>( "point" ), 0.0, input_timestep );

    kun::ParticleBasedRenderer* renderer = new kun::ParticleBasedRenderer();
    renderer->setName( ::RendererName );

    renderer->setBaseOpacity( ::base_opacity );
    renderer->setTransferFunction( ::tfunc );
    renderer->setRepetitionLevel( ::repetition );

    if( ::ShadingFlag == false ) renderer->disableShading();

    kun::PointObject* object_first = NULL;

    // load the point for the first time step
    if ( param.hasOption( "rep_low" ) )
    {
        object_first = new kun::PointImporter( ::file_name[0] );
        object_first->setName( ::ObjectName );
    }
    else
        object_first = ::object[0];

    //screen.scene()->camera()->setPosition( kvs::Vector3f(0, 0, 3), kvs::Vector3f(1, 0, 0) );
    
    screen.registerObject( object_first, renderer );
    screen.setBackgroundColor( kvs::RGBColor( 255, 255, 255) );

    screen.addEvent( &fps );
    screen.addEvent( &key_press_event );
    screen.addTimerEvent( &timer_event, ::glut_timer );

    screen.show();
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
