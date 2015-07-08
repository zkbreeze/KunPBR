//
//  RadiusCalculator.h
//
//
//  Created by Kun Zhao on 2015-07-08 15:43:05.
//
//

#ifndef KUN__RADIUSCALCULATOR_H_INCLUDE
#define KUN__RADIUSCALCULATOR_H_INCLUDE
 

#include <kvs/OpenGL>
#include <kvs/VolumeObjectBase>
#include <kvs/OpacityMap>
#include <kvs/Vector3>
#include <kvs/Math>
#include <kvs/Camera>


namespace kun
{

namespace detail
{

template <typename T>
inline kvs::Matrix44<T> ScalingMatrix( T x, T y, T z )
{
    const T elements[ 16 ] =
    {
        x, 0, 0, 0,
        0, y, 0, 0,
        0, 0, z, 0,
        0, 0, 0, 1
    };

    return kvs::Matrix44<T>( elements );
}

template <typename T>
inline kvs::Matrix44<T> TranslationMatrix( T x, T y, T z )
{
    const T elements[ 16 ] =
    {
        1, 0, 0, x,
        0, 1, 0, y,
        0, 0, 1, z,
        0, 0, 0, 1
    };

    return kvs::Matrix44<T>( elements );
}

inline void GetViewport( const kvs::Camera* camera, GLint (*viewport)[4] )
{
    (*viewport)[0] = 0;
    (*viewport)[1] = 0;
    (*viewport)[2] = camera->windowWidth();
    (*viewport)[3] = camera->windowHeight();
}

inline void GetProjectionMatrix( const kvs::Camera* camera, GLdouble (*projection)[16] )
{
    const kvs::Matrix44f P = camera->projectionMatrix();
    (*projection)[ 0] = P[0][0];
    (*projection)[ 1] = P[1][0];
    (*projection)[ 2] = P[2][0];
    (*projection)[ 3] = P[3][0];
    (*projection)[ 4] = P[0][1];
    (*projection)[ 5] = P[1][1];
    (*projection)[ 6] = P[2][1];
    (*projection)[ 7] = P[3][1];
    (*projection)[ 8] = P[0][2];
    (*projection)[ 9] = P[1][2];
    (*projection)[10] = P[2][2];
    (*projection)[11] = P[3][2];
    (*projection)[12] = P[0][3];
    (*projection)[13] = P[1][3];
    (*projection)[14] = P[2][3];
    (*projection)[15] = P[3][3];
}

inline void GetModelviewMatrix( const kvs::Camera* camera, const kvs::ObjectBase* object, GLdouble (*modelview)[16] )
{
    const kvs::Vector3f  min_external = object->minExternalCoord();
    const kvs::Vector3f  max_external = object->maxExternalCoord();
    const kvs::Vector3f  center       = ( max_external + min_external ) * 0.5f;
    const kvs::Vector3f  diff         = max_external - min_external;
    const float          normalize    = 6.0f / kvs::Math::Max( diff.x(), diff.y(), diff.z() );

    const kvs::Matrix44f T = TranslationMatrix<float>( -center.x(), -center.y(), -center.z() );
    const kvs::Matrix44f S = ScalingMatrix<float>( normalize, normalize, normalize );
    const kvs::Matrix44f L = camera->viewingMatrix();
    const kvs::Matrix44f M = L * S * T;

    (*modelview)[ 0] = M[0][0];
    (*modelview)[ 1] = M[1][0];
    (*modelview)[ 2] = M[2][0];
    (*modelview)[ 3] = M[3][0];
    (*modelview)[ 4] = M[0][1];
    (*modelview)[ 5] = M[1][1];
    (*modelview)[ 6] = M[2][1];
    (*modelview)[ 7] = M[3][1];
    (*modelview)[ 8] = M[0][2];
    (*modelview)[ 9] = M[1][2];
    (*modelview)[10] = M[2][2];
    (*modelview)[11] = M[3][2];
    (*modelview)[12] = M[0][3];
    (*modelview)[13] = M[1][3];
    (*modelview)[14] = M[2][3];
    (*modelview)[15] = M[3][3];
}

inline void Project(
    const GLdouble obj_x,
    const GLdouble obj_y,
    const GLdouble obj_z,
    const GLdouble modelview[16],
    const GLdouble projection[16],
    const GLint    viewport[4],
    GLdouble*      win_x,
    GLdouble*      win_y,
    GLdouble*      win_z )
{
    const kvs::Vector4d I( obj_x, obj_y, obj_z, 1.0 );

    const kvs::Matrix44d P(
        projection[0], projection[4], projection[8],  projection[12],
        projection[1], projection[5], projection[9],  projection[13],
        projection[2], projection[6], projection[10], projection[14],
        projection[3], projection[7], projection[11], projection[15] );

    const kvs::Matrix44d M(
        modelview[0], modelview[4], modelview[8],  modelview[12],
        modelview[1], modelview[5], modelview[9],  modelview[13],
        modelview[2], modelview[6], modelview[10], modelview[14],
        modelview[3], modelview[7], modelview[11], modelview[15] );

    const kvs::Vector4d O = P * M * I;

    const double w =  ( O.w() == 0.0f ) ? 1.0f : O.w();

    *win_x = viewport[0] + ( 1.0 + O.x() / w ) * viewport[2] / 2.0;
    *win_y = viewport[1] + ( 1.0 + O.y() / w ) * viewport[3] / 2.0;
    *win_z = ( 1.0 + O.z() / w ) / 2.0;
}

inline void UnProject(
    const GLdouble win_x,
    const GLdouble win_y,
    const GLdouble win_z,
    const GLdouble modelview[16],
    const GLdouble projection[16],
    const GLint    viewport[4],
    GLdouble*      obj_x,
    GLdouble*      obj_y,
    GLdouble*      obj_z )
{
    const kvs::Vector4d I(
        ( win_x - viewport[0] ) * 2.0 / viewport[2] - 1.0,
        ( win_y - viewport[1] ) * 2.0 / viewport[3] - 1.0,
        2.0 * win_z - 1.0,
        1.0 );

    const kvs::Matrix44d P(
        projection[0], projection[4], projection[8],  projection[12],
        projection[1], projection[5], projection[9],  projection[13],
        projection[2], projection[6], projection[10], projection[14],
        projection[3], projection[7], projection[11], projection[15] );

    const kvs::Matrix44d M(
        modelview[0], modelview[4], modelview[8],  modelview[12],
        modelview[1], modelview[5], modelview[9],  modelview[13],
        modelview[2], modelview[6], modelview[10], modelview[14],
        modelview[3], modelview[7], modelview[11], modelview[15] );

    const kvs::Vector4d O = ( P * M ).inverted() * I;

    const double w =  ( O.w() == 0.0 ) ? 1.0 : O.w();

    *obj_x = O.x() / w;
    *obj_y = O.y() / w;
    *obj_z = O.z() / w;
}

inline float CalculateObjectDepth(
    const kvs::ObjectBase* object,
    const GLdouble         modelview[16],
    const GLdouble         projection[16],
    const GLint            viewport[4] )
{
    // calculate suitable depth.
    GLdouble x, y, z;

    Project(
        object->objectCenter().x(),
        object->objectCenter().y(),
        object->objectCenter().z(),
        modelview, projection, viewport,
        &x, &y, &z );

    const float object_depth = static_cast<float>( z );

    return object_depth;
}

inline float CalculateSubpixelLength(
    const float    length,
    const float    object_depth,
    const GLdouble modelview[16],
    const GLdouble projection[16],
    const GLint    viewport[4] )
{
    GLdouble wx_min, wy_min, wz_min;
    GLdouble wx_max, wy_max, wz_max;

    UnProject(
        0.0, 0.0, GLdouble( object_depth ),
        modelview, projection, viewport,
        &wx_min, &wy_min, &wz_min);

    UnProject(
        1.0, 1.0 , GLdouble( object_depth ),
        modelview, projection, viewport,
        &wx_max, &wy_max, &wz_max);

    const float subpixel_length = static_cast<float>( ( wx_max - wx_min ) / length );

    return subpixel_length;
}

} // end of namespace


namespace RadiusCalculator
{

inline float CalculateObjectDepth( 
    const kvs::Camera& camera, 
    const kvs::ObjectBase& object )
{
    // Calculate a transform matrix.
    GLdouble modelview[16];  kun::detail::GetModelviewMatrix( &camera, &object, &modelview );
    GLdouble projection[16]; kun::detail::GetProjectionMatrix( &camera, &projection );
    GLint viewport[4];       kun::detail::GetViewport( &camera, &viewport );

    // Calculate a depth of the center of gravity of the object.
    const float object_depth = kun::detail::CalculateObjectDepth(
            &object, modelview, projection, viewport );

    return object_depth;
}

inline float CalculateObjectLength(
    const float length, 
    const kvs::Camera& camera, 
    const kvs::ObjectBase& object )
{
    // Calculate a transform matrix.
    GLdouble modelview[16];  kun::detail::GetModelviewMatrix( &camera, &object, &modelview );
    GLdouble projection[16]; kun::detail::GetProjectionMatrix( &camera, &projection );
    GLint viewport[4];       kun::detail::GetViewport( &camera, &viewport );

    // Calculate a depth of the center of gravity of the object.
    const float object_depth = kun::detail::CalculateObjectDepth(
            &object, modelview, projection, viewport );

    // Calculate suitable subpixel length.
    const float subpixel_length = kun::detail::CalculateSubpixelLength(
            length, object_depth, modelview, projection, viewport );

    return subpixel_length;
}

} // end of namespace RadiusCalculator

} // end of namespace kun

 
#endif // KUN__RADIUSCALCULATOR_H_INCLUDE