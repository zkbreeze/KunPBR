//
//  Animator.h
//
//
//  Created by Kun Zhao on 2016-08-19 13:36:14.
//
//

#ifndef KUN__ANIMATOR_H_INCLUDE
#define KUN__ANIMATOR_H_INCLUDE

#include <kvs/glut/Application> 
#include <kvs/glut/Screen>
#include <kvs/glut/Timer>
#include <kvs/ObjectBase>
#include <kvs/glut/Label>
#include <kvs/glut/Slider>
#include <kvs/PointObject>

namespace kun
{

class Animator
{
	static kvs::glut::Screen* m_screen;
	static kvs::glut::Timer* m_timer;
	static std::vector<kvs::ObjectBase*> m_objects;
	static int m_time_step;
	static int m_steps;
	static std::string m_object_name;
	// std::string m_renderer_name;

protected:
	class time_slider;
	class time_label;
	class timer_event;
	class key_event;

	template <typename T>
	std::vector<kvs::ObjectBase*> caster( std::vector<T*> points );

public:

	Animator(){}
	Animator( kvs::glut::Screen* screen, kvs::glut::Timer* timer, std::vector<kvs::ObjectBase*> objects );

	void setGlutScreen( kvs::glut::Screen* screen );
	void setGlutTimer( kvs::glut::Timer* timer );
	void setObjects( std::vector<kvs::PointObject*> objects );
	// void setRendererName( std::string name );

	void exec();
};

} // end of namespace kun
 
#endif // KUN__ANIMATOR_H_INCLUDE