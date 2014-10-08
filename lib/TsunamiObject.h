//
//  TsunamiObject.h
//
//
//  Created by Kun Zhao on 2014-10-01 17:46:49.
//
//

#ifndef KUN__TSUNAMIOBJECT_H_INCLUDE
#define KUN__TSUNAMIOBJECT_H_INCLUDE
 
#include <iostream>
#include <fstream>
#include "PointObject.h"
#include <kvs/PointObject>

namespace kun
{

class TsunamiObject
{
    size_t m_size;
    float* m_x;
    float* m_y;
    float* m_z;
    float* m_u;
    float* m_v;
    float* m_w;
    float* m_depth;
    float* m_eddy_x;
    float* m_eddy_y;
    float* m_eddy_z;

public:
    
    TsunamiObject( void );
    TsunamiObject( std::string filename );
    void read( std::string filename );
    size_t size( void );
    
    kun::PointObject* toKUNPointObject( size_t Parameter_ID );
    kvs::PointObject* toKVSPointObject( void );
};

} // end of namespece kun
 
#endif // KUN__TSUNAMIOBJECT_H_INCLUDE