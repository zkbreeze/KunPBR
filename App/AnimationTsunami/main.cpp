//
//  main.cpp
//
//
//  Created by Kun Zhao on 2015-08-19 15:12:01.
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
#include <kvs/ParticleBasedRenderer>
#include "DensityCalculator.h"
#include <kvs/RGBFormulae>
#include <kvs/StochasticRenderingCompositor>
#include "StochasticPolygonRenderer.h"
#include "OBJObject.h"
#include <kvs/RGBFormulae>
#include "PolygonClipper.h"
#include "AdvObject.h"
#include "VTKObject.h"

namespace
{
    kun::PointObject** object = NULL;
    kvs::StructuredVolumeObject** density_volume = NULL;

    bool isLODRendering = false;
    float lod = 1.0;

    kvs::glut::Timer* glut_timer;
    std::vector<std::string> file_name;
    kvs::TransferFunction tfunc( kvs::RGBFormulae::Ocean( 256 ) );
    kvs::Shader::Phong phong( 0.6, 0.4, 0, 1 );

    kvs::RGBColor label_color = kvs::RGBColor( 255, 255, 255 );

    int    msec;
    int    nsteps;
    int    time_step;

    size_t repetition = 81;

    int    max_grid = 200;

    bool ShadingFlag = true;

    const std::string ObjectName( "ParticleObject" ); 
    const std::string RendererName( "ParticleRenderer" );

    kvs::Timer fps_time;

    bool isClipping = false;
    kvs::Vector3f min_range, max_range;
}

class FPS : public kvs::PaintEventListener
{

public:
	void update()
	{	
		::fps_time.stop();
		std::cout << "\r" << "FPS: " << fps_time.fps() << std::flush;
		::fps_time.start();
	}
};

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
        kvs::RendererBase* r = glut_screen->scene()->rendererManager()->renderer( ::RendererName );
        kun::ParticleBasedRenderer* renderer = static_cast<kun::ParticleBasedRenderer*>( r );

        renderer->setShader( phong );
        if(::ShadingFlag == false)
        {
            renderer->disableShading();
        }
        ::tfunc = transferFunction();
        renderer->setTransferFunction( transferFunction() );
        renderer->enableDensityMode();
        renderer->setDensityVolume( ::density_volume[::time_step] );
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
    ::nsteps = 0;
    kvs::Directory directory( filename );
    const kvs::FileList files = directory.fileList();
    int file_length = files.size();
    bool isAdv = false;

    for ( int i = 0; i < file_length; i++ )
    {
        const kvs::File file = files[i];
        if( file.extension() == "kvsml" )
        {
            ::nsteps++;
            file_name.push_back( file.filePath() );
        }
        if( file.extension() == "adv" )
        {
            isAdv = true;
            ::nsteps++;
            file_name.push_back( file.filePath() );            
        }
    }

    if( input_timestep )
        ::nsteps = input_timestep;
    
    ::object = new kun::PointObject*[::nsteps];
    ::density_volume = new kvs::StructuredVolumeObject*[::nsteps];
    std::cout << ::nsteps << " time steps." << std::endl;
    
    kvs::Timer time;
    time.start();
    for ( int i = 0; i < ::nsteps; i++ )
    {
        if( isAdv )
        {
            kun::AdvObject* adv = new kun::AdvObject( file_name[i] );
            ::object[i] = adv->toKUNPointObject();
            delete adv;
        }
        else
        {
            ::object[i] = new kun::PointImporter( file_name[i] );            
        }
        if( ::isClipping ) ::object[i]->setMinMaxRange( ::min_range, ::max_range );

        kun::DensityCalculator* calculator = new kun::DensityCalculator( ::object[i] );
        calculator->setMaxGrid( ::max_grid );
        ::density_volume[i] = calculator->outputDensityVolume();
        delete calculator;

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
        renderer->enableDensityMode();
        renderer->setDensityVolume( ::density_volume[::time_step] );
        renderer->setTransferFunction( ::tfunc );
        renderer->setRepetitionLevel( ::repetition );

        kun::PointObject* object_current = ::object[::time_step];

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
        ::time_step++;

        kvs::glut::Screen* glut_screen = static_cast<kvs::glut::Screen*>( screen() );
        kun::PointObject* object_current = new kun::PointObject();
        object_current->setName( ::ObjectName );

        kun::ParticleBasedRenderer* renderer = new kun::ParticleBasedRenderer();
        renderer->setName( ::RendererName );

        if( ::ShadingFlag == false )
        {
            renderer->disableShading();
        }
        renderer->setTransferFunction( ::tfunc );
        if ( ::isLODRendering )
        {
            object_current = ::object[::time_step]->toPartPoint( ::lod );
            // renderer->setRepetitionLevel( ::repetition * ::lod );
            // Supplement for the density.
            renderer->setParticleScale( 1.0 / ::lod );
        }
        else
        {
            object_current = ::object[::time_step];
            // renderer->setRepetitionLevel( ::repetition );            
        }
        object_current->print( std::cout );
        
        renderer->enableDensityMode();
        renderer->setDensityVolume( ::density_volume[::time_step] );

        glut_screen->scene()->objectManager()->change( ::ObjectName, object_current, false );
        glut_screen->scene()->replaceRenderer( ::RendererName, renderer, true );
        slider->setValue( (float)::time_step );
        glut_screen->redraw();

        if( ::time_step == ::nsteps - 1 ) 
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
                        kun::PointObject* object_current = ::object[::time_step];
                        object_current->setName( ::ObjectName );

                        renderer->setName( ::RendererName );
                        renderer->enableDensityMode();
                        renderer->setDensityVolume( ::density_volume[::time_step] );
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

class PolygonSlider : public kvs::glut::Slider
{
public:

	PolygonSlider( kvs::glut::Screen* screen ) : 
	kvs::glut::Slider( screen ){}

	void valueChanged( void )
	{
		kvs::glut::Screen* glut_screen = static_cast<kvs::glut::Screen*>( screen() );
		kvs::ObjectBase* o = glut_screen->scene()->objectManager()->object( "Polygon" );
		kvs::PolygonObject* polygon = static_cast<kvs::PolygonObject*>( o );
		polygon->setOpacity( this->value() );

		kun::StochasticPolygonRenderer* polygon_renderer = new kun::StochasticPolygonRenderer();
		polygon_renderer->setShader( ::phong );
		polygon_renderer->setName( "PolygonRenderer" );
		glut_screen->scene()->rendererManager()->change( "PolygonRenderer", polygon_renderer, false );
		screen()->redraw();
	}
};

int main( int argc, char** argv )
{
    kvs::glut::Application app( argc, argv );
    kvs::glut::Screen screen( &app );
    KeyPressEvent     key_press_event;
    TimerEvent        timer_event;
    FPS               fps;
    ::glut_timer = new kvs::glut::Timer( ::msec );

    kvs::CommandLine param( argc, argv );
    param.addHelpOption();
    param.addOption( "f", "Input KUN Point Data Directory", 1, false );
    param.addOption( "adv", "Input Adv Data Directory", 1, false );
    param.addOption( "l", "Input the OBJ Land File", 1, false );
    param.addOption( "v", "Input the vtk land object", 1, false );
    param.addOption( "nos", "No Shading", 0, false );
    param.addOption( "rep", "Set the Repetition Level", 1, false );
    param.addOption( "lod", "Set the LOD level [0.0 ~ 1.0], which is used LOD animation", 1, false );
    param.addOption( "trans", "Set Initial Transferfunction", 1, false );
    param.addOption( "nsteps", "Set n time steps to load", 1, false );
    param.addOption( "m", "Set the Max Grid for Density Calculation", 1, false );
    param.addOption( "minx", "Input the clip range of min x", 1, false );
    param.addOption( "miny", "Input the clip range of min y", 1, false );
    param.addOption( "minz", "Input the clip range of min z", 1, false );
    param.addOption( "maxx", "Input the clip range of max x", 1, false );
    param.addOption( "maxy", "Input the clip range of max y", 1, false );
    param.addOption( "maxz", "Input the clip range of max z", 1, false );
    if ( !param.parse() ) return 1;

    size_t input_timestep = 0;
    if( param.hasOption( "nsteps" ) ) input_timestep = param.optionValue<size_t>( "nsteps" );
    // Clipping
    if( param.hasOption( "minx" ) ) 
    {
        ::isClipping = true;
        ::min_range = kvs::Vector3f( param.optionValue<float>( "minx" ), param.optionValue<float>( "miny" ), param.optionValue<float>( "minz" ) );
        ::max_range = kvs::Vector3f( param.optionValue<float>( "maxx" ), param.optionValue<float>( "maxy" ), param.optionValue<float>( "maxz" ) );
    }

    if ( param.hasOption( "nos" ) ) ::ShadingFlag = false;
    if( param.hasOption( "rep" ) ) ::repetition = param.optionValue<size_t>( "rep" );
    if( param.hasOption( "lod" ) )
    {
        ::isLODRendering= true;
        ::lod = param.optionValue<float>( "lod" );
    }
    if( param.hasOption( "trans" ) ) ::tfunc = kvs::TransferFunction( param.optionValue<std::string>( "trans" ) );
    if( param.hasOption( "m" ) ) ::max_grid = param.optionValue<int>( "m" );

    // Time-varying data loading
    std::string directory;
    if( param.hasOption( "f" ) ) directory = param.optionValue<std::string>( "f" );
    if( param.hasOption( "adv" ) ) directory = param.optionValue<std::string>( "adv" );
    initialize( directory, input_timestep );
    ::min_range = ::object[0]->minObjectCoord();
    ::max_range = ::object[0]->maxObjectCoord();

    // Land data loading
    kvs::PolygonObject* polygon = NULL;
    if( param.hasOption( "l" ) )
    {
        kun::OBJObject* obj = new kun::OBJObject( param.optionValue<std::string>( "l" ) );
        polygon = obj->toKVSPolygonObject();
    }
    if( param.hasOption( "v" ) )
    {
        kun::VTKObject* vtk = new kun::VTKObject( param.optionValue<std::string>( "v" ) );
        polygon = vtk->toKVSPolygonObject();
    }
    // kun::PolygonClipper::ClipBox( polygon, min, max );
    // kun::PolygonClipper::ClipZPlane( polygon, min.z(), kun::PolygonClipper::UP );
    polygon->setName( "Polygon" );
    kun::PolygonClipper::ClipBoxCourse( polygon, ::min_range, ::max_range );
    float polygon_opacity = 50;
    polygon->setOpacity( polygon_opacity );
    polygon->print( std::cout );

    // Renderer
    kun::ParticleBasedRenderer* renderer = new kun::ParticleBasedRenderer();
    renderer->setName( ::RendererName );
    renderer->enableDensityMode();
    renderer->setDensityVolume( ::density_volume[0] );
    renderer->setTransferFunction( ::tfunc );
    renderer->setRepetitionLevel( ::repetition );

    kun::StochasticPolygonRenderer* polygon_renderer = new kun::StochasticPolygonRenderer();
    polygon_renderer->setShader( phong );
    polygon_renderer->setName( "PolygonRenderer" );
    // polygon_renderer->setPolygonOffset( -1.f );

    if( ::ShadingFlag == false )
    {
        renderer->disableShading();
        polygon_renderer->disableShading();
    }

    screen.registerObject( ::object[0], renderer );
    screen.registerObject( polygon, polygon_renderer );
    screen.setBackgroundColor( kvs::RGBColor::Black() );
    screen.setSize( 1024, 768 );
    screen.show();

    kvs::StochasticRenderingCompositor compositor( screen.scene() );
    compositor.setRepetitionLevel( ::repetition );
    compositor.enableLODControl();
    screen.setEvent( &compositor );

    screen.addEvent( &fps );
    screen.addEvent( &key_press_event );
    screen.addTimerEvent( &timer_event, ::glut_timer );

    // Set the transfer function editor
    kvs::StructuredVolumeObject* pointdummy = new kvs::StructuredVolumeObject();
    pointdummy->setGridType( kvs::StructuredVolumeObject::Uniform );
    pointdummy->setVeclen( 1 );
    pointdummy->setResolution( kvs::Vector3ui( 1, 1, ::object[0]->numberOfVertices() ) );
    pointdummy->setValues( ::object[0]->values() );
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
    
    // Time slider
    slider = new TimeSlider( &screen );
    slider->setSliderColor( ::label_color );
    slider->setX( screen.width() * 0.25 );
    slider->setY( screen.height() - 80 );
    slider->setWidth( screen.width() / 2 );
    slider->setValue( 0.0 );
    slider->setRange( 0.0, ::nsteps - 1 );
    slider->setMargin( 15 );
    slider->setCaption("");
    slider->setTextColor( ::label_color  );
    slider->show();

    // Polygon opacity slider
    PolygonSlider* polygon_slider = new PolygonSlider( &screen );
    polygon_slider->setSliderColor( kvs::RGBColor::White() );
    polygon_slider->setX( screen.width() * 0.25 );
    polygon_slider->setY( 10 );
    polygon_slider->setWidth( screen.width() / 2 );
    polygon_slider->setValue( polygon_opacity );
    polygon_slider->setRange( 0, 255 );
    polygon_slider->setMargin( 15 );
    polygon_slider->setCaption("Polygon Opacity");
    polygon_slider->setTextColor( kvs::RGBColor::White()  );
    polygon_slider->show();
    
    ::glut_timer->start( ::msec );
    ::glut_timer->stop();
    return app.run();
}
