//
//  FPS.h
//
//
//  Created by Kun Zhao on 2014-09-06 13:59:47.
//
//

#ifndef KUN__FPS_H_INCLUDE
#define KUN__FPS_H_INCLUDE
 
#include <iostream>
#include <kvs/PaintEventListener>
#include <kvs/Timer>

namespace kun
{

class FPS : public kvs::PaintEventListener
{
	kvs::Timer time;

public:
	void update();
};

} // end of namespace of kun
 
#endif // KUN__FPS_H_INCLUDE