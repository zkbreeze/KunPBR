/*****************************************************************************/
/**
 *  @file   zooming.vert
 *  @author Naohisa Sakamoto
 */
/*----------------------------------------------------------------------------
 *
 *  Copyright 2007 Visualization Laboratory, Kyoto University.
 *  All rights reserved.
 *  See http://www.viz.media.kyoto-u.ac.jp/kvs/copyright/ for details.
 *
 *  $Id: zooming.vert 992 2011-10-15 00:24:45Z naohisa.sakamoto@gmail.com $
 */
/*****************************************************************************/
uniform float object_depth;
uniform vec2 screen_scale;
uniform sampler2D random_texture;
uniform sampler2D transfer_function_texture;
uniform float random_texture_size_inv;
uniform float scale;
uniform float max_alpha;
uniform float base_opacity;
uniform float x_min;
uniform float x_max;
uniform float y_min;
uniform float y_max;

attribute vec2 random_index;
/*ADD*/ attribute float value;
attribute float value2;

//const float CIRCLE_THRESHOLD = 3.0;
const float CIRCLE_THRESHOLD = 0.1;
const float CIRCLE_SCALE = 0.564189583547756; // 1.0 / sqrt(PI)

varying vec3 position;
varying vec3 normal;
varying vec2 center;
varying float radius;
//varying float depth;


/*===========================================================================*/
/**
 *  @brief  Return the footprint size of the particle in pixel.
 *  @param  p [in] particle position
 *  @return footprint size [pixel]
 */
/*===========================================================================*/
float zooming( in vec4 p )
{
    // Depth value. 
    float D = p.z;
    if ( D < 1.0 ) D = 1.0; // to avoid front-clip

    // Calculate the footprint size of the particle.
    float s = object_depth / D; // footprint size of the particle in pixel
// #if defined( ENABLE_PARTICLE_SIZE )
//     /*ADD*/ s *= size;
// #endif

    #if defined( ENABLE_VALID_RANGE )
    if( value < x_min || value > x_max || value2 < y_min || value2 > y_max )
        s = 0.0;
    else
    {
        scale_x = 1.0 / ( x_max - x_min );
        scale_y = 1.0 / ( y_max - y_min );
        value = ( value - x_min ) * scale_x;
        value2 = ( value2 - y_min ) * scale_y;
        float a = texture2D( transfer_function_texture, vec2( value, value2 ) ).a;
        if ( a < max_alpha )
            s *= sqrt( log( 1.0 - a ) / log( 1.0 - base_opacity ) );
        else
            s *= sqrt( log( 1.0 - max_alpha ) / log( 1.0 - base_opacity ) );   
    }
    #else
    float a = texture2D( transfer_function_texture, vec2( value, value2 ) ).a;
    if ( a < max_alpha )
        s *= sqrt( log( 1.0 - a ) / log( 1.0 - base_opacity ) );
    else
        s *= sqrt( log( 1.0 - max_alpha ) / log( 1.0 - base_opacity ) );    
    #endif



    s *= scale;
    
    float sf = floor( s );       // round-down value of s
    float sc = ceil( s );        // round-up value of s

    // Calculate a probability 'pc' that the footprint size is 'sc'.
    float fraction = fract( s );
    float pc = fraction * ( 2.0 * sf + fraction ) / ( 2.0 * sf + 1.0 );

    // Random number from the random number texture.
    vec2 random_texture_index = random_index * random_texture_size_inv;
    float R = texture2D( random_texture, random_texture_index ).x;

    if ( CIRCLE_THRESHOLD <= 0.0 || s <= CIRCLE_THRESHOLD )
    {
        // Draw the particle as square.
        s = ( ( R < pc ) ? sc : sf );
        radius = 0.0;
    }
    else
    {
        // Draw the particle as circle.
        // Convert position to screen coordinates.
        center = screen_scale + ( ( p.xy / p.w ) * screen_scale );
        radius = ( ( R < pc ) ? sc : sf ) * CIRCLE_SCALE;
        s = ceil( s * CIRCLE_SCALE * 2.0 ) + 1.0;
    }

    return( s );
}

/*===========================================================================*/
/**
 *  @brief  Calculates a size of the particle in pixel.
 */
/*===========================================================================*/
void main()
{
    gl_FrontColor = texture2D( transfer_function_texture, vec2( 0.5, 0.5 ) );
    
    gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
    gl_PointSize = zooming( gl_Position );

    normal = gl_Normal.xyz;
    position = vec3( gl_ModelViewMatrix * gl_Vertex );
//    depth = gl_Position.z / gl_Position.w;
}
