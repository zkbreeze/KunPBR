//
//  PRTObject.h
//
//
//  Created by Kun Zhao on 2014-09-07 21:03:58.
//
//

#ifndef KUN__PRTOBJECT_H_INCLUDE
#define KUN__PRTOBJECT_H_INCLUDE
 
#include <iostream>
#include <iostream>
#include <fstream>
#include "PointObject.h"

namespace kun
{

class PRTObject
{
    size_t m_size;
    float* m_x;
    float* m_y;
    float* m_z;
    float* m_nutri;
    float* m_depth;
    int* m_i;
    int* m_j;
    int* m_k;
    int* m_ID;
    char* m_fishID;
    
    float m_min_nutrition;
    float m_max_nutrition;
    
public:
    
    PRTObject( void );
    PRTObject( std::string filename );
    void read( std::string filename );
    size_t size( void );
    float* x( void );
    float* y( void );
    float* z( void );
    float* nutrition( void );
    
    void setMinNutrition( float min );
    void setMaxNutrition( float max );
    
    kun::PointObject* toPointObject( void );
};

} // end of namespece kun
 
#endif // KUN__PRTOBJECT_H_INCLUDE