//
//  Plane.cpp
//
//
//  Created by Kun Zhao on 2015-09-07 11:32:28.
//
//

#include "Plane.h"

namespace
{

float DotProduct( kvs::Vec3 V1, kvs::Vec3 V2 )
{
	return V1.x() * V2.x() + V1.y() * V2.y() + V1.z() * V2.z();
}

kvs::Vec3 CrossProduct( kvs::Vec3 V1, kvs::Vec3 V2 )
{
	float a = V1.y() * V2.z() - V1.z() * V2.y();
	float b = V1.z() * V2.x() - V1.x() * V2.z();
	float c = V1.x() * V2.y() - V1.y() * V2.x();
	return kvs::Vec3( a, b, c );
}

}

namespace kun
{

Plane::Plane( float a, float b, float c, float d )
{
	m_a = a;
	m_b = b;
	m_c = c;
	m_d = d;
	this->calculateBasePoint();
}

Plane::Plane( const kvs::Vec3 Normal, float d )
{
	m_a = Normal.normalized().x();
	m_b = Normal.normalized().y();
	m_c = Normal.normalized().z();
	m_d = d;
	this->calculateBasePoint();
}


Plane::Plane( const kvs::Vec3 Normal, const kvs::Vec3 P )
{
	m_a = Normal.normalized().x();
	m_b = Normal.normalized().y();
	m_c = Normal.normalized().z();
	m_d = DotProduct( Normal.normalized(), P );
	this->calculateBasePoint();
}

Plane::Plane( const kvs::Vec3 P1, const kvs::Vec3 P2, const kvs::Vec3 P3 )
{
	kvs::Vec3 Normal = CrossProduct( P1 - P2, P3 - P2 );
	m_a = Normal.normalized().x();
	m_b = Normal.normalized().y();
	m_c = Normal.normalized().z();
	m_d = DotProduct( Normal.normalized(), P1 );
	this->calculateBasePoint();
}

Plane Plane::ConstructXPlane( float X )
{
	float a = 1;
	float b = 0;
	float c = 0;
	float d = - X;
	Plane plane( a, b, c, d );
	return plane;
}

Plane Plane::ConstructYPlane( float Y )
{
	float a = 0;
	float b = 1;
	float c = 0;
	float d = - Y;
	Plane plane( a, b, c, d );
	return plane;
}

Plane Plane::ConstructZPlane( float Z )
{
	float a = 0;
	float b = 0;
	float c = 1;
	float d = - Z;
	Plane plane( a, b, c, d );
	return plane;
}

void Plane::calculateBasePoint( void )
{
	if( m_a ) m_V0 = kvs::Vec3( - m_d / m_a, 0.0, 0.0 );
	else if( m_b ) m_V0 = kvs::Vec3( 0.0, - m_d / m_b, 0.0 );
	else if( m_c ) m_V0 = kvs::Vec3( 0.0, 0.0, - m_d / m_c );
}

float Plane::pointDistance( kvs::Vec3 P )
{
	kvs::Vec3 vector = P - m_V0;
	kvs::Vec3 normal( m_a, m_b, m_c );
	return DotProduct( vector, normal.normalized() );
}

bool Plane::intersectSegLine( kvs::Vec3 P1, kvs::Vec3 P2, kvs::Vec3 &intersection )
{
	float distanceP1 = this->pointDistance( P1 );
	float distanceP2 = this->pointDistance( P2 );
	if( distanceP1 && distanceP2 ) return false;
	else
	{
		float u = - distanceP1 / ( distanceP2 - distanceP1 );
		intersection = P1 + u * ( P2 - P1 );
		return true;
	}
}

} // end of namespace kun
