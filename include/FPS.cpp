//
//  FPS.cpp
//
//
//  Created by Kun Zhao on 2014-09-06 14:01:21.
//
//

#include "FPS.h"
#include <kvs/glut/Screen>
#include <kvs/RendererBase>
#include <kvs/RendererManager>

namespace kun
{

void FPS::update()
{
	kvs::glut::Screen* glut_screen = static_cast<kvs::glut::Screen*>( screen() );
	std::cout << "\r" << "                           " << std::flush;
	std::cout << "\r" << "FPS: " <<  glut_screen->scene()->rendererManager()->renderer()->timer().fps() << std::flush;
}

} // end of namespace kun