
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


#include"ParticleBasedRendererGLSL.h"

#include <kvs/PointImporter>

#include "PointObject.h"

#include <kvs/ParticleBasedRenderer>

#include <sys/time.h>

#define TETRA 4

#define PRISM 6


kvs::PointObject** object;
kvs::PointObject** object_fine;
bool isHaveFinePoint = false;

kvs::glut::Timer* glut_timer;
std::vector<std::string> fine_filename;
std::string objectname = "ParticleObject";
kvs::TransferFunction tfunc( 256 );

kvs::RGBColor label_color = kvs::RGBColor( 0, 0, 0 );

int    msec;
int    nsteps;
int    time_step;

size_t repetition;
size_t fine_repetition;

bool ShadingFlag = true;

struct timeval start,end,start2;

namespace
{
    float particle_scale = 1.0;
    float max_alpha = 0.79212;
}
namespace { const std::string ObjectName( "ParticleObject" ); }
namespace { const std::string RendererName( "ParticleRenderer" ); }

class TransferFunctionEditor : public kvs::glut::TransferFunctionEditor
{

public:

    TransferFunctionEditor( kvs::ScreenBase* screen ) :
    kvs::glut::TransferFunctionEditor( screen )
    {
    }
    
    void apply( void )
    {

        gettimeofday(&start, NULL);
        kvs::glut::Screen* glut_screen = static_cast<kvs::glut::Screen*>( screen() );
        kvs::RendererBase* r = glut_screen->scene()->rendererManager()->renderer();
        kun::ParticleBasedRenderer* renderer = static_cast<kun::ParticleBasedRenderer*>( r );

        if(ShadingFlag == false)
        {
            renderer->disableShading();
        }
        tfunc.setOpacityMap(transferFunction().opacityMap());
        tfunc.setColorMap(transferFunction().colorMap() );
        renderer->setTransferFunction( transferFunction() );
        renderer->setParticleScale( ::particle_scale );
        printf("apply pushed \n");
        gettimeofday(&start2, NULL);
        screen()->redraw();
        gettimeofday(&end, NULL);
        printf("更新時間 %f [s] \n",end.tv_sec-start.tv_sec + (end.tv_usec-start.tv_usec)*1.0E-6);
        printf("screen()->redraw() %f [s] \n",end.tv_sec-start2.tv_sec + (end.tv_usec-start2.tv_usec)*1.0E-6);
        printf("finish redraw\n");
    }
};

TransferFunctionEditor* editor = NULL;

void initialize( std::string filename )
{
    time_step = 0;
//    filename = "5/Density";

    msec = 20;
    nsteps = 0;
    kvs::Directory directory( filename );
    const kvs::FileList files = directory.fileList();
    int file_length = files.size();
    std::vector<std::string> file_name;
    for ( int i = 0; i < file_length; i++ )
    {
        const kvs::File file = files[i];
        if( file.extension() == "kvsml" )
        {
            nsteps++;
            file_name.push_back( file.filePath() );
        }
    }
    
    object = new kvs::PointObject*[nsteps];
    std::cout << nsteps << std::endl;
    
    kvs::Timer time;
    time.start();
    for ( int i = 0; i < nsteps; i++ )
    { 
        object[i] = new kvs::PointImporter( file_name[i] );
        object[i]->setName( ::ObjectName );
        std::cout << "\r" << i << std::flush;        
    }
    time.stop();
    std::cout << "\r" << "                           " << std::flush;
    std::cout << "\r" << "Finish Reading." << std::endl;
    std::cout <<"Loading time: " <<time.msec()<<"msec"<<std::endl;
    time_step = 0;
}


class Label : public kvs::glut::Label
{
public:

    Label( kvs::ScreenBase* screen ):
    kvs::glut::Label( screen )
    {
        setTextColor( label_color  );
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
        glut_timer->stop();
        time_step = int( this->value() );
        kvs::glut::Screen* glut_screen = static_cast<kvs::glut::Screen*>( screen() );
        kun::ParticleBasedRenderer* renderer = new kun::ParticleBasedRenderer();

        if(ShadingFlag == false)
        {
            renderer->disableShading();
        }
        else
        {
            renderer->setShader( kvs::Shader::Phong( 0.5, 0.5, 0.8, 15.0 ) );
        }

        renderer->setName( ::RendererName );
        renderer->setParticleScale( ::particle_scale );
        renderer->setTransferFunction( tfunc );

        
        kvs::PointObject* object_current = NULL;

        if ( isHaveFinePoint )
        {
            object_current = new kvs::PointImporter( fine_filename[time_step] );
            object_current->setName( ::ObjectName );
            renderer->setRepetitionLevel( fine_repetition );
        }
        else
        {
            object_current = object[time_step];
            renderer->setRepetitionLevel( repetition );  
        }

        glut_screen->scene()->objectManager()->change( ::ObjectName, object[time_step], false );
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
        kvs::RendererBase* r = glut_screen->scene()->rendererManager()->renderer();
        kun::ParticleBasedRenderer* renderer = static_cast<kun::ParticleBasedRenderer*>(r);

        renderer->setName( ::RendererName );
        if(ShadingFlag == false)
        {
            renderer->disableShading();
        }
        else
        {
            renderer->setShader( kvs::Shader::Phong( 0.5, 0.5, 0.8, 15.0 ) );
        }
        renderer->setTransferFunction( tfunc );
        renderer->setRepetitionLevel( repetition );
        renderer->setParticleScale( ::particle_scale );
        // renderer->setLODRepetitionLevel( 4 );

        // Set the transfer function editor
        // kvs::StructuredVolumeObject* pointdummy = new kvs::StructuredVolumeObject();
        // pointdummy->setGridType( kvs::StructuredVolumeObject::Uniform );
        // pointdummy->setVeclen( 1 );
        // pointdummy->setResolution( kvs::Vector3ui( 1, 1, object[time_step]->numberOfVertices() ) );
        // pointdummy->setValues( object[time_step]->sizes() );
        // pointdummy->updateMinMaxValues();

        // editor->setVolumeObject( pointdummy );
        // editor->show();

        glut_screen->scene()->objectManager()->change( ::ObjectName, object[time_step++], false );
        //glut_screen->scene()->rendererManager()->change( ::RendererName, renderer, true );
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
                    if ( isHaveFinePoint )
                    {
                        kvs::glut::Screen* glut_screen = static_cast<kvs::glut::Screen*>( screen() );
                        kun::ParticleBasedRenderer* renderer = new kun::ParticleBasedRenderer();
                        kvs::PointObject* object_current = new kvs::PointImporter( fine_filename[time_step - 1] );
                        object_current->setName( ::ObjectName );
                        std::cout << "Finish loading " << fine_filename[time_step - 1] <<std::endl;

                        renderer->setName( ::RendererName );
                        renderer->setParticleScale( ::particle_scale );
                        renderer->setTransferFunction( tfunc );
                        renderer->setRepetitionLevel( fine_repetition );

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
    printf("this program needs more than two steps\n");
    kvs::glut::Application app( argc, argv );
    kvs::glut::Screen screen( &app );
    KeyPressEvent     key_press_event;
    TimerEvent        timer_event;
    glut_timer = new kvs::glut::Timer( msec );

    kvs::CommandLine param( argc, argv );
    param.addHelpOption();
    param.addOption( "point", "KVSML Point Data Filename", 1, true );
    param.addOption( "point_fine", "KVSML Fine Point Data Filename", 1, false );
    param.addOption( "nos", "No Shading", 0, false );
    param.addOption( "o", "base opacity", 1, true );
    param.addOption( "rep", "repetition level", 1, true );
    param.addOption( "rep_fine", "fine repetition level", 1, false );
    param.addOption( "trans", "set initial transferfunction", 1, false );
    
    if ( !param.parse() ) return 1;

    repetition = param.optionValue<size_t>( "rep" );
    float base_opacity = param.optionValue<float>( "o" );

    // Base transfer function
    if(param.hasOption("trans"))
    {
        tfunc = kvs::TransferFunction( param.optionValue<std::string>( "trans" ) );
    }
    


    initialize( param.optionValue<std::string>( "point" ).c_str() );

    kun::ParticleBasedRenderer* renderer = new kun::ParticleBasedRenderer();
    if ( base_opacity < ::max_alpha )
        ::particle_scale = std::sqrt( std::log( 1 - ::max_alpha ) / std::log( 1 - base_opacity ) );
    std::cout << "Particle Scale: " << ::particle_scale << std::endl;
    renderer->setParticleScale( ::particle_scale );
    renderer->setTransferFunction( tfunc );
    if ( param.hasOption( "nos" ) )
    {
        ShadingFlag = false;
    }
    if( ShadingFlag == false )
    {
        renderer->disableShading();
    }

    kvs::PointObject* object_first = NULL;

    // If there are fine point object
    if ( param.hasOption( "point_fine" ) )
    {
        isHaveFinePoint = true;
        kvs::Directory directory( param.optionValue<std::string>( "point_fine" ) );
        const kvs::FileList files = directory.fileList();
        int file_length = files.size();

        for ( int i = 0; i < file_length; i++ )
        {
            const kvs::File file = files[i];
            if( file.extension() == "kvsml" )
            {
                fine_filename.push_back( file.filePath() );
            }
        }
        object_first = new kvs::PointImporter( fine_filename[0] );
        object_first->setName( ObjectName );

        fine_repetition = param.optionValue<size_t>( "rep_fine" );
        renderer->setRepetitionLevel( fine_repetition );
    }
    else
    {
        object_first = object[0];
        renderer->setRepetitionLevel(param.optionValue<int>( "rep" ) );
    }

    //screen.scene()->camera()->setPosition( kvs::Vector3f(0, 0, 3), kvs::Vector3f(1, 0, 0) );
    
    screen.registerObject( object_first, renderer );
    //screen.setBackgroundColor( kvs::RGBColor( 0, 0, 0) );

    screen.addEvent( &key_press_event );
    screen.addTimerEvent( &timer_event, glut_timer );

    screen.show();
    // Set the transfer function editor
    kvs::StructuredVolumeObject* pointdummy = new kvs::StructuredVolumeObject();
    pointdummy->setGridType( kvs::StructuredVolumeObject::Uniform );
    pointdummy->setVeclen( 1 );
    pointdummy->setResolution( kvs::Vector3ui( 1, 1, object_first->numberOfVertices() ) );
    pointdummy->setValues( object_first->sizes() );
    pointdummy->updateMinMaxValues();
    std::cout << "Number of points for the first time step: " << pointdummy->numberOfNodes() <<std::endl;
    
    editor = new TransferFunctionEditor( &screen );
    editor->setVolumeObject( pointdummy );
    editor->setTransferFunction( tfunc );
    editor->show();

    //lable
    label = new Label( &screen );
    label->setX( screen.width() * 0.25 );
    label->setY( screen.height() - 80 );
    label->show();
    
    // slider
    slider = new TimeSlider( &screen );
    slider->setSliderColor( label_color  );
    slider->setX( screen.width() * 0.25 );
    slider->setY( screen.height() - 80 );
    slider->setWidth( screen.width() / 2 );
    slider->setValue( 0.0 );
    slider->setRange( 0.0, nsteps );
    slider->setMargin( 15 );
    slider->setCaption("");
    slider->setTextColor( label_color  );
    slider->show();
    
    glut_timer->start( msec );
    glut_timer->stop();
    return app.run();
}
