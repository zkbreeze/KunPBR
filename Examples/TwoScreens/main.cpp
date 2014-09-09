#include <kvs/HydrogenVolumeData>
#include <kvs/RayCastingRenderer>
#include <kvs/glut/Screen>
#include <kvs/glut/Application>
#include <kvs/PaintEventListener>
#include <kvs/ObjectManager>
#include <kvs/Xform>
#include <kvs/TimerEventListener>
#include <kvs/glut/Timer>
#include <kvs/RendererManager>

namespace
{
	kvs::Xform xform;

}

class sync1 : public kvs::PaintEventListener
{

public:

	void update()
	{
		kvs::glut::Screen* glut_screen = static_cast<kvs::glut::Screen*>( screen() );
		xform = glut_screen->scene()->objectManager()->object()->xform();
		screen()->redraw();
	}
};

class sync2 : public kvs::TimerEventListener
{

public:
	void update( kvs::TimeEvent* event )
	{
		kvs::glut::Screen* glut_screen = static_cast<kvs::glut::Screen*>( screen() );
		kvs::glsl::RayCastingRenderer* renderer = new kvs::glsl::RayCastingRenderer();

		glut_screen->scene()->objectManager()->object()->setXform( xform );
		screen()->redraw();
	}
};

int main( int argc, char** argv )
{
	kvs::glut::Application app( argc, argv );
	kvs::glut::Screen screen1( &app );
	kvs::glut::Screen screen2( &app );

	kvs::StructuredVolumeObject* object1 = new kvs::HydrogenVolumeData( kvs::Vec3ui( 32, 32, 32 ) );
	kvs::StructuredVolumeObject* object2 = new kvs::HydrogenVolumeData( kvs::Vec3ui( 32, 32, 32 ) );
	kvs::glsl::RayCastingRenderer* renderer1 = new kvs::glsl::RayCastingRenderer();
	kvs::glsl::RayCastingRenderer* renderer2 = new kvs::glsl::RayCastingRenderer();

	screen1.registerObject( object1, renderer1 );
	screen1.setTitle( "screen1" );

	screen2.registerObject( object2, renderer2 );
	screen2.setTitle( "screen2" );

	sync1 master;
	screen1.addEvent( &master );

	sync2 slave;
	kvs::glut::Timer timer;
	screen2.addTimerEvent( &slave, &timer );

	screen2.show();
	screen1.show();

	return app.run();
}