//
//  AdvObject.h
//
//
//  Created by Kun Zhao on 2015-11-17 17:32:08.
//
//

#ifndef KUN__ADVOBJECT_H_INCLUDE
#define KUN__ADVOBJECT_H_INCLUDE

#include <Adv/AdvDocument.h>
#include "PointObject.h"

namespace kun
{
	
class AdvObject
{
	int m_number;
	float* m_x;
	float* m_y;
	float* m_z;
	float* m_u;
	float* m_v;
	float* m_w;
	int* m_type;

public:
	AdvObject(){}
	AdvObject( std::string filename );
	~AdvObject();

	void read( std::string filename );
	size_t numberOfParticles();

	kun::PointObject* toKUNPointObject();

};

} // end of namespace kun
 
#endif // KUN__ADVOBJECT_H_INCLUDE