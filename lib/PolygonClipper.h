//
//  PolygonClipper.h
//
//
//  Created by Kun Zhao on 2015-09-08 16:29:19.
//
//

#ifndef KUN__POLYGONCLIPPER_H_INCLUDE
#define KUN__POLYGONCLIPPER_H_INCLUDE
 
#include "Plane.h"
#include <kvs/PolygonObject>

namespace kun
{
	
class PolygonClipper
{

public:
	// PolygonClipper();
	// PolygonClipper( kvs::PolygonObject* polygon, kvs::Vec3 min, kvs::Vec3 max );
	// ~PolygonClipper();

	enum PlaneDirection 
	{
		UP,
		DOWN,
	};

	static void ClipBox( kvs::PolygonObject* polygon, kvs::Vec3 min, kvs::Vec3 max );
	static void ClipBoxCourse( kvs::PolygonObject* polygon, kvs::Vec3 min, kvs::Vec3 max );
	static void ClipXPlane( kvs::PolygonObject* polygon, float X, PlaneDirection direction );
	static void ClipYPlane( kvs::PolygonObject* polygon, float Y, PlaneDirection direction );
	static void ClipZPlane( kvs::PolygonObject* polygon, float Z, PlaneDirection direction );
	static void ClipPlane( kvs::PolygonObject* polygon, kun::Plane* plane, kvs::Vec3 plane_normal);
};


} // end of namespace kun
 
#endif // KUN__POLYGONCLIPPER_H_INCLUDE