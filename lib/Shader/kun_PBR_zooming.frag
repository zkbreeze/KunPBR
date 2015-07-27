//
//  PBR_kun_zooming.frag
//
//
//  Created by Kun Zhao on 2014/08/30.
//
//

//#define ENABLE_TWO_SIDE_LIGHTING

struct ShadingParameter
{
    float Ka; // ambient
    float Kd; // diffuse
    float Ks; // specular
    float S;  // shininess
};

vec3 ShadingNone( in ShadingParameter shading, in vec3 color )
{
    return( color );
}

vec3 ShadingLambert( in ShadingParameter shading, in vec3 color, in vec3 L, in vec3 N )
{
#if defined( ENABLE_TWO_SIDE_LIGHTING )
    float dd = abs( dot( N, L ) );
#else
    float dd = max( dot( N, L ), 0.0 );
#endif

    float Ia = shading.Ka;
    float Id = shading.Kd * dd;

    return( color * ( Ia + Id ) );
}

vec3 ShadingPhong( in ShadingParameter shading, in vec3 color, in vec3 L, in vec3 N, in vec3 V )
{
    vec3 R = reflect( -L, N );
#if defined( ENABLE_TWO_SIDE_LIGHTING )
    float dd = abs( dot( N, L ) );
    float ds = pow( abs( dot( R, V ) ), shading.S );
#else
    float dd = max( dot( N, L ), 0.0 );
    float ds = pow( max( dot( R, V ), 0.0 ), shading.S );
#endif
    if ( dd <= 0.0 ) ds = 0.0;

    float Ia = shading.Ka;
    float Id = shading.Kd * dd;
    float Is = shading.Ks * ds;

    return( color * ( Ia + Id ) + Is );
}

vec3 ShadingBlinnPhong( in ShadingParameter shading, in vec3 color, in vec3 L, in vec3 N, in vec3 V )
{
    vec3 H = normalize( L + V );
#if defined( ENABLE_TWO_SIDE_LIGHTING )
    float dd = abs( dot( N, L ) );
    float ds = pow( abs( dot( H, N ) ), shading.S );
#else
    float dd = max( dot( N, L ), 0.0 );
    float ds = pow( max( dot( H, N ), 0.0 ), shading.S );
#endif
    if ( dd <= 0.0 ) ds = 0.0;

    float Ia = shading.Ka;
    float Id = shading.Kd * dd;
    float Is = shading.Ks * ds;

    return( color * ( Ia + Id ) + Is );
}


varying vec3  position;
varying vec3  normal;
varying vec2  center;
varying float radius;
//varying float depth;

uniform ShadingParameter shading;

/*===========================================================================*/
/**
 *  @brief  Calculates a shaded color of the particle with Lambert shading.
 */
/*===========================================================================*/
void main( void )
{
    if ( radius == 0.0 ) { discard; return; }

    // Discard a pixel outside circle.
    if ( radius > 0.0 )
    {
        if ( distance( gl_FragCoord.xy, center ) > radius ) { discard; return; }
    }

    // if ( radius > 3.0 ) { discard; return; }

    // Light position.
    vec3 light_position = gl_LightSource[0].position.xyz;

    // Light vector (L) and Normal vector (N)
    vec3 L = normalize( light_position - position );
    vec3 N = normalize( gl_NormalMatrix * normal );

#if   defined( ENABLE_LAMBERT_SHADING )
    vec3 shaded_color = ShadingLambert( shading, gl_Color.xyz, L, N );

#elif defined( ENABLE_PHONG_SHADING )
    vec3 V = normalize( -position );
    vec3 shaded_color = ShadingPhong( shading, gl_Color.xyz, L, N, V );

#elif defined( ENABLE_BLINN_PHONG_SHADING )
    vec3 V = normalize( -position );
    vec3 shaded_color = ShadingBlinnPhong( shading, gl_Color.xyz, L, N, V );

#else // DISABLE SHADING
    vec3 shaded_color = ShadingNone( shading, gl_Color.xyz );
#endif

    gl_FragColor = vec4( shaded_color, 1.0 );
//    gl_FragDepth = depth;
}
