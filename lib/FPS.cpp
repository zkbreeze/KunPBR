//
//  FPS.cpp
//
//
//  Created by Kun Zhao on 2014-09-06 14:01:21.
//
//

#include "FPS.h"

namespace kun
{

void FPS::update()
{
	time.stop();
	std::cout << "\r" << "                           " << std::flush;
	std::cout << "\r" << "FPS: " << time.fps() << std::flush;
	time.start();
}

} // end of namespace kun