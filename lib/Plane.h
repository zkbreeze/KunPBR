//
//  Plane.h
//
//
//  Created by Kun Zhao on 2015-09-07 11:32:09.
//
//

#ifndef KUN__PLANE_H_INCLUDE
#define KUN__PLANE_H_INCLUDE

#include <kvs/Vector3>

namespace kun
{
	
class Plane
{

float m_a;
float m_b;
float m_c;
float m_d; // Plane: ax + by + cz + d = 0
kvs::Vec3 m_V0; // Base point on the plane

public:
	Plane();
	Plane( float a, float b, float c, float d ); // From 4 coefficients
	Plane( const kvs::Vec3 Normal, float d ); // From Normal and coefficient d
	Plane( const kvs::Vec3 Normal, const kvs::Vec3 P ); // From Normal and point P
	Plane( const kvs::Vec3 P1, const kvs::Vec3 P2, const kvs::Vec3 P3 ); //From 3 points

	static Plane* ConstructXPlane( float X );
	static Plane* ConstructYPlane( float Y );
	static Plane* ConstructZPlane( float Z );

	void calculateBasePoint();

	float pointDistance( kvs::Vec3 P ); // Return the distance between point and plane. Take (m_a, m_b, m_c) as the normal to calculate.
	float pointDistance( kvs::Vec3 P, kvs::Vec3 normal ); // Calculation with assigned normal of the face
	bool intersectSegLine( kvs::Vec3 P1, kvs::Vec3 P2, kvs::Vec3 &intersection );
	// Return true if the segment line is intersected by the plane. 
	//Intersection point is written into &intersection
	// If there is no intersection, ( 0.0, 0.0, 0.0 ) would be written into &intersection	

	bool intersectTriangle( kvs::Vec3 P1, kvs::Vec3 P2, kvs::Vec3 P3, kvs::Vec3 &intersection1, kvs::Vec3 &intersection2 );
	// Return true if the triangle is intersected by the plane.
	// Two intersection points are written into &intersection1 and &intersection2. 
	// If there are no intersections, ( 0.0, 0.0, 0.0 ) would be written into &intersection1 and &intersection2

	float a(){ return m_a; }
	float b(){ return m_b; }
	float c(){ return m_c; }
	float d(){ return m_d; }

	~Plane();
};


} // end of namespace kun
 
#endif // KUN__PLANE_H_INCLUDE