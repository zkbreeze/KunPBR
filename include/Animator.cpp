//
//  Animator.cpp
//
//
//  Created by Kun Zhao on 2016-08-19 13:44:46.
//
//

#include "Animator.h"
#include <kvs/TimerEventListener>
#include <kvs/ObjectManager>
#include <kvs/RendererManager>
#include <kvs/glut/Label>
#include <kvs/glut/Slider>
#include <kvs/KeyPressEventListener>

namespace kun
{

class Animator::time_slider : public kvs::glut::Slider
{
public:
    
    time_slider( kvs::glut::Screen* screen ):
    kvs::glut::Slider( screen ){};
    
    void valueChanged( void )
    {
        kvs::glut::Screen* glut_screen = static_cast<kvs::glut::Screen*>( screen() );

        m_timer->stop();
        m_time_step = int( this->value() );
        
        glut_screen->scene()->objectManager()->change( m_object_name, m_objects[m_time_step], false );
        glut_screen->redraw();
    }
};

class Animator::time_label : public kvs::glut::Label
{
public:
	time_label( kvs::ScreenBase* screen ):
	kvs::glut::Label( screen )
	{
		setTextColor( kvs::RGBColor( 0, 0, 0 ) );
		setMargin( 10 );
	}

	void screenUpdated( void )
	{
		char* buf = new char[256];
		sprintf( buf, "Time step : %03d", m_time_step );
		setText( std::string( buf ).c_str() );
	}
};

class Animator::timer_event : public kvs::TimerEventListener
{
	Animator::time_slider* m_slider;

public:
	void setSlider( Animator::time_slider* slider ){ m_slider = slider; }
    void update( kvs::TimeEvent* event )
    {
        kvs::glut::Screen* glut_screen = static_cast<kvs::glut::Screen*>( screen() );

        glut_screen->scene()->objectManager()->change( m_objects[0]->name(), m_objects[m_time_step++], false );
        m_slider->setValue( (float)m_time_step );
        std::cout << "\r" << m_time_step <<std::flush;
        glut_screen->redraw();
        if( m_time_step == m_steps )
        {
            m_time_step = 0;
            m_timer->stop();
        }
    }
};

class Animator::key_event : public kvs::KeyPressEventListener
{
    void update( kvs::KeyEvent* event )
    {
        switch ( event->key() )
        {
            case kvs::Key::s:
            {
                if ( m_timer-> isStopped() )
                {
                    m_timer->start();
                    screen()->redraw();
                }
                else
                {
                    m_timer->stop();
                    screen()->redraw();
                }
                break;
                
            }
        }
    }
};

template <typename T>
std::vector<kvs::ObjectBase*> Animator::caster( std::vector<T*> objects )
{
    std::vector<kvs::ObjectBase*> outputs;
    auto itr = objects.begin();
    for( unsigned int i = 0; i < objects.size(); i++ )
    {
        outputs.push_back( *(itr++) );
    }
    return outputs;
}


Animator::Animator( kvs::glut::Screen* screen, kvs::glut::Timer* timer, std::vector<kvs::ObjectBase*> objects )
{
    m_screen = screen;
    m_timer = timer;
    m_objects = objects;
}


void Animator::setGlutScreen( kvs::glut::Screen* screen )
{
	m_screen = screen;
}

void Animator::setGlutTimer( kvs::glut::Timer* timer )
{
    m_timer = timer;
}

void Animator::setObjects( std::vector<kvs::PointObject*> objects )
{
	m_objects = this->caster<kvs::PointObject>( objects );
}

void Animator::exec()
{
	m_steps = m_objects.size();
	m_object_name = m_objects[0]->name(); 

	time_label* label = new time_label( m_screen );
	label->setX( m_screen->width() * 0.25 );
	label->setY( m_screen->height() - 80 );
	label->show();

	time_slider* slider = new time_slider( m_screen );
	slider->setSliderColor( kvs::RGBColor( 0, 0, 0 ) );
	slider->setX( m_screen->width() * 0.25 );
	slider->setY( m_screen->height() - 80 );
	slider->setWidth( m_screen->width() / 2 );
	slider->setValue( 0.0 );
	slider->setRange( 0.0, m_steps );
	slider->setMargin( 15 );
	slider->setCaption("");
	slider->setTextColor( kvs::RGBColor( 0, 0, 0 ) );
	slider->show();

	timer_event t_event;
	t_event.setSlider( slider );

	key_event key;

	m_screen->addTimerEvent( &t_event, m_timer );
	m_screen->addEvent( &key );
}

} // end of namespace kun