//
//  kun_PBR_zooming_point.vert
//
//
//  Created by Kun Zhao on 2015-07-08 11:59:07.
//
//

uniform float object_depth;
uniform vec2 screen_scale;
uniform sampler2D random_texture;
uniform sampler1D transfer_function_texture;
uniform float random_texture_size_inv;
uniform float scale;
uniform float max_alpha;
uniform sampler3D density_volume;
uniform int repetition_level;
uniform vec3 min;
uniform vec3 max;

attribute vec2 random_index;
/*ADD*/ attribute float value;

//const float CIRCLE_THRESHOLD = 3.0;
const float CIRCLE_THRESHOLD = 0.1;
const float CIRCLE_SCALE = 0.564189583547756; // 1.0 / sqrt(PI)
const float PI = 3.14159265359;
uniform float Rad;

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
float zooming( in vec4 p, in vec4 coord )
{
    // Depth value. 
    float D = p.z;
    if ( D < 1.0 ) D = 1.0; // to avoid front-clip

    // Calculate the footprint size of the particle.
    float s = object_depth / D; // footprint size of the particle in pixel
// #if defined( ENABLE_PARTICLE_SIZE )
//     /*ADD*/ s *= size;
// #endif

    vec3 index = ( vec3( coord.x, coord.y, coord.z ) - min ) / ( max - min );
    float density = texture3D( density_volume, index ).w / float(repetition_level);
    float base_opacity = 1.0 - exp( - 0.5 * PI * Rad * Rad * Rad * density );
    // float base_opacity = 0.3;

    float a = texture1D( transfer_function_texture, value ).a;
    if ( a < max_alpha )
        s *= sqrt( log( 1.0 - a ) / log( 1.0 - base_opacity ) );
    else
        s *= sqrt( log( 1.0 - max_alpha ) / log( 1.0 - base_opacity ) ); 

    float max_rad = Rad * 5.0;
    if ( s > max_rad ) s = max_rad;
    
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
    gl_FrontColor = texture1D( transfer_function_texture, value );
    
    gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
    gl_PointSize = zooming( gl_Position, gl_Vertex );

    normal = gl_Normal.xyz;
    position = vec3( gl_ModelViewMatrix * gl_Vertex );
//    depth = gl_Position.z / gl_Position.w;
}
