//
//  SnapKey.cpp
//
//
//  Created by Kun Zhao on 2014-09-04 16:12:11.
//	
//

#include "SnapKey.h"
#include <kvs/glut/Screen>
#include <kvs/ColorImage>
#include <kvs/Camera>

namespace kun
{

void SnapKey::update( kvs::KeyEvent* event )
{
	switch( event->key() )
	{
		case kvs::Key::p:
		{
			kvs::glut::Screen* glut_screen = static_cast<kvs::glut::Screen*>( screen() );
			kvs::ColorImage image = glut_screen->scene()->camera()->snapshot();
			image.write( "snapshot.bmp");
			std::cout << "Finish writing the snapshot.bmp." << std::endl;
		}
	}
}

} // end of namespace kun

