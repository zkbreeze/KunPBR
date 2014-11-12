//
//  CellByCellUniformSampling2D.cpp
//
//
//  Created by Kun Zhao on 2014-10-24 12:53:46.
//
//

#include "CellByCellUniformSampling2D.h"
#include <vector>
#include <kvs/DebugNew>
#include <kvs/Camera>
#include <kvs/TrilinearInterpolator>
#include <kvs/Value>
#include <kvs/CellBase>
#include <kvs/TetrahedralCell>
#include <kvs/QuadraticTetrahedralCell>
#include <kvs/HexahedralCell>
#include <kvs/QuadraticHexahedralCell>
#include <kvs/PyramidalCell>
#include <kvs/PrismaticCell>
#include <kvs/Xorshift128>

namespace Generator = kvs::CellByCellParticleGenerator;

namespace
{

template <int Dim, typename T>
kvs::ValueArray<T> ShuffleArray( const kvs::ValueArray<T>& values, kvs::UInt32 seed )
{
    KVS_ASSERT( Dim > 0 );
    KVS_ASSERT( values.size() % Dim == 0 );

    kvs::Xorshift128 rng; rng.setSeed( seed );
    kvs::ValueArray<T> ret;
    if ( values.unique() ) { ret = values; }
    else { ret = values.clone(); }

    T* p = ret.data();
    size_t size = ret.size() / Dim;

    for ( size_t i = 0; i < size; ++i )
    {
        size_t j = rng.randInteger() % ( i + 1 );
        for ( int k = 0; k < Dim; ++k )
        {
            std::swap( p[ i * Dim + k ], p[ j * Dim + k ] );
        }
    }
    return ret;
}

}

namespace kun
{

/*===========================================================================*/
/**
 *  @brief  Constructs a new CellByCellUniformSampling2D class.
 */
/*===========================================================================*/
CellByCellUniformSampling2D::CellByCellUniformSampling2D():
    kvs::MapperBase(),
    kun::PointObject(),
    m_camera( 0 ),
    m_shuffle( false )
{
}

/*===========================================================================*/
/**
 *  @brief  Constructs a new CellByCellUniformSampling2D class.
 *  @param  volume [in] pointer to the volume object
 *  @param  subpixel_level [in] sub-pixel level
 *  @param  sampling_step [in] sapling step
 *  @param  transfer_function [in] transfer function
 *  @param  object_depth [in] depth value of the input volume at the CoG
 */
/*===========================================================================*/
CellByCellUniformSampling2D::CellByCellUniformSampling2D(
    const kvs::VolumeObjectBase* volume,
    const size_t                 subpixel_level,
    const float                  sampling_step,
    const kvs::TransferFunction& transfer_function,
    const float                  object_depth ):
    kvs::MapperBase( transfer_function ),
    kun::PointObject(),
    m_camera( 0 ),
    m_shuffle( false )
{
    this->setSubpixelLevel( subpixel_level );
    this->setSamplingStep( sampling_step );
    this->setObjectDepth( object_depth );
    this->exec( volume );
}

/*===========================================================================*/
/**
 *  @brief  Constructs a new CellByCellUniformSampling2D class.
 *  @param  camera [in] pointer to the camera
 *  @param  volume [in] pointer to the volume object
 *  @param  subpixel_level [in] sub-pixel level
 *  @param  sampling_step [in] sapling step
 *  @param  transfer_function [in] transfer function
 *  @param  object_depth [in] depth value of the input volume at the CoG
 */
/*===========================================================================*/
CellByCellUniformSampling2D::CellByCellUniformSampling2D(
    const kvs::Camera*           camera,
    const kvs::VolumeObjectBase* volume,
    const size_t                 subpixel_level,
    const float                  sampling_step,
    const kvs::TransferFunction& transfer_function,
    const float                  object_depth ):
    kvs::MapperBase( transfer_function ),
    kun::PointObject(),
    m_shuffle( false )
{
    this->attachCamera( camera ),
    this->setSubpixelLevel( subpixel_level );
    this->setSamplingStep( sampling_step );
    this->setObjectDepth( object_depth );
    this->exec( volume );
}

/*===========================================================================*/
/**
 *  @brief  Destroys the CellByCellMetropolisSampling class.
 */
/*===========================================================================*/
CellByCellUniformSampling2D::~CellByCellUniformSampling2D()
{
    m_density_map.release();
}

/*===========================================================================*/
/**
 *  @brief  Retruns the sub-pixel level.
 *  @return sub-pixel level
 */
/*===========================================================================*/
size_t CellByCellUniformSampling2D::subpixelLevel() const
{
    return m_subpixel_level;
}

/*===========================================================================*/
/**
 *  @brief  Returns the sampling step.
 *  @return sampling step
 */
/*===========================================================================*/
float CellByCellUniformSampling2D::samplingStep() const
{
    return m_sampling_step;
}

/*===========================================================================*/
/**
 *  @brief  Returns the depth of the object at the center of the gravity.
 *  @return depth
 */
/*===========================================================================*/
float CellByCellUniformSampling2D::objectDepth() const
{
    return m_object_depth;
}

/*===========================================================================*/
/**
 *  @brief  Attaches a camera.
 *  @param  camera [in] pointer to the camera
 */
/*===========================================================================*/
void CellByCellUniformSampling2D::attachCamera( const kvs::Camera* camera )
{
    m_camera = camera;
}

/*===========================================================================*/
/**
 *  @brief  Sets a sub-pixel level.
 *  @param  subpixel_level [in] sub-pixel level
 */
/*===========================================================================*/
void CellByCellUniformSampling2D::setSubpixelLevel( const size_t subpixel_level )
{
    m_subpixel_level = subpixel_level;
}

/*===========================================================================*/
/**
 *  @brief  Sets a sampling step.
 *  @param  sampling_step [in] sampling step
 */
/*===========================================================================*/
void CellByCellUniformSampling2D::setSamplingStep( const float sampling_step )
{
    m_sampling_step = sampling_step;
}

/*===========================================================================*/
/**
 *  @brief  Sets a depth of the object at the center of the gravity.
 *  @param  object_depth [in] depth
 */
/*===========================================================================*/
void CellByCellUniformSampling2D::setObjectDepth( const float object_depth )
{
    m_object_depth = object_depth;
}

// ADD

void CellByCellUniformSampling2D::setShuffleParticles()
{
    m_shuffle = true;
}

// ADD
// Use kun sampling step, which equals to 0.5 pixel.

void CellByCellUniformSampling2D::setKunSamplingStep()
{
    #define USE_KUN_SAMPLING_STEP
}

/*===========================================================================*/
/**
 *  @brief  Executes the mapper process.
 *  @param  object [in] pointer to the volume object
 *  @return pointer to the point object
 */
/*===========================================================================*/
CellByCellUniformSampling2D::SuperClass* CellByCellUniformSampling2D::exec( const kvs::ObjectBase* object )
{
    if ( !object )
    {
        BaseClass::setSuccess( false );
        kvsMessageError("Input object is NULL.");
        return NULL;
    }

    const kvs::VolumeObjectBase* volume = kvs::VolumeObjectBase::DownCast( object );
    if ( !volume )
    {
        BaseClass::setSuccess( false );
        kvsMessageError("Input object is not volume dat.");
        return NULL;
    }

    const kvs::VolumeObjectBase::VolumeType volume_type = volume->volumeType();
    if ( volume_type == kvs::VolumeObjectBase::Structured )
    {
        if ( m_camera )
        {
            this->mapping( m_camera, reinterpret_cast<const kvs::StructuredVolumeObject*>( object ) );
        }
        else
        {
            // Generate particles by using default camera parameters.
            kvs::Camera* camera = new kvs::Camera();

            this->mapping( camera, reinterpret_cast<const kvs::StructuredVolumeObject*>( object ) );
            delete camera;
        }
    }
    else // volume_type == kvs::VolumeObjectBase::Unstructured
    {
        if ( m_camera )
        {
            this->mapping( m_camera, reinterpret_cast<const kvs::UnstructuredVolumeObject*>( object ) );
        }
        else
        {
            // Generate particles by using default camera parameters.
            kvs::Camera* camera = new kvs::Camera();
            this->mapping( camera, reinterpret_cast<const kvs::UnstructuredVolumeObject*>( object ) );
            delete camera;
        }
    }

    return this;
}

/*===========================================================================*/
/**
 *  @brief  Mapping for the structured volume object.
 *  @param  camera [in] pointer to the camera
 *  @param  volume [in] pointer to the input volume object
 */
/*===========================================================================*/
void CellByCellUniformSampling2D::mapping( const kvs::Camera* camera, const kvs::StructuredVolumeObject* volume )
{
    // Attach the pointer to the volume object and set the min/max coordinates.
    BaseClass::attachVolume( volume );
    BaseClass::setRange( volume );
    BaseClass::setMinMaxCoords( volume, this );

    // Calculate the density map.
    m_density_map = Generator::CalculateDensityMap(
        camera,
        BaseClass::volume(),
        static_cast<float>( m_subpixel_level ),
        m_sampling_step,
        BaseClass::transferFunction().opacityMap() );

    // Generate the particles.
    const std::type_info& type = volume->values().typeInfo()->type();
    if (      type == typeid( kvs::Int8   ) ) this->generate_particles<kvs::Int8>( volume );
    else if ( type == typeid( kvs::Int16  ) ) this->generate_particles<kvs::Int16>( volume );
    else if ( type == typeid( kvs::Int32  ) ) this->generate_particles<kvs::Int32>( volume );
    else if ( type == typeid( kvs::UInt8  ) ) this->generate_particles<kvs::UInt8>( volume );
    else if ( type == typeid( kvs::UInt16 ) ) this->generate_particles<kvs::UInt16>( volume );
    else if ( type == typeid( kvs::UInt32 ) ) this->generate_particles<kvs::UInt32>( volume );
    else if ( type == typeid( kvs::Real32 ) ) this->generate_particles<kvs::Real32>( volume );
    else if ( type == typeid( kvs::Real64 ) ) this->generate_particles<kvs::Real64>( volume );
    else
    {
        BaseClass::setSuccess( false );
        kvsMessageError("Unsupported data type '%s'.", volume->values().typeInfo()->typeName() );
    }
}

/*===========================================================================*/
/**
 *  @brief  Mapping for the unstructured volume object.
 *  @param  camera [in] pointer to the camera
 *  @param  volume [in] pointer to the input volume object
 */
/*===========================================================================*/
void CellByCellUniformSampling2D::mapping( const kvs::Camera* camera, const kvs::UnstructuredVolumeObject* volume )
{
    // Attach the pointer to the volume object and set the min/max coordinates.
    BaseClass::attachVolume( volume );
    BaseClass::setRange( volume );
    BaseClass::setMinMaxCoords( volume, this );

    // Calculate the density map.
    m_density_map = Generator::CalculateDensityMap(
        camera,
        BaseClass::volume(),
        static_cast<float>( m_subpixel_level ),
        m_sampling_step,
        BaseClass::transferFunction().opacityMap() );

    // Generate the particles.
    this->generate_particles( volume );    
}

/*===========================================================================*/
/**
 *  @brief  Generates particles for the structured volume object.
 *  @param  volume [in] pointer to the input volume object
 */
/*===========================================================================*/
template <typename T>
void CellByCellUniformSampling2D::generate_particles( const kvs::StructuredVolumeObject* volume )
{
    // Vertex data arrays. (output)
    std::vector<kvs::Real32> vertex_coords;
    std::vector<kvs::Real32> vertex_values;
    std::vector<kvs::Real32> vertex_normals;

    // Set a trilinear interpolator.
    kvs::TrilinearInterpolator interpolator( volume );
    kvs::TrilinearInterpolator interpolator2( m_volume_2 );

    // Set parameters for normalization of the node values.
    const float min_value = BaseClass::transferFunction().colorMap().minValue();
    const float max_value = BaseClass::transferFunction().colorMap().maxValue();

    const float normalize_factor = BaseClass::transferFunction().resolution() / ( max_value - min_value );

    const float* const  density_map = m_density_map.data();
    const kvs::ColorMap color_map( BaseClass::transferFunction().colorMap() );

    // Generate particles for each cell.
    const kvs::Vector3ui ncells( volume->resolution() - kvs::Vector3ui::All(1) );
    for ( kvs::UInt32 z = 0; z < ncells.z(); ++z )
    {
        for ( kvs::UInt32 y = 0; y < ncells.y(); ++y )
        {
            for ( kvs::UInt32 x = 0; x < ncells.x(); ++x )
            {
                // Calculate a volume of cell.
                const float volume_of_cell = 1.0f;

                // Interpolate at the center of gravity of this cell.
                const kvs::Vector3f cog( x + 0.5f, y + 0.5f, z + 0.5f );
                interpolator.attachPoint( cog );

                // Calculate a density.
                const float  average_scalar = interpolator.scalar<T>();
                const size_t average_degree = static_cast<size_t>( ( average_scalar - min_value ) * normalize_factor );
                const float  density = density_map[ average_degree ];

                // Calculate a number of particles in this cell.
                const float p = density * volume_of_cell;
                size_t nparticles_in_cell = static_cast<size_t>( p );
                if ( p - nparticles_in_cell > Generator::GetRandomNumber() ) { ++nparticles_in_cell; }

                const kvs::Vector3f v( static_cast<float>(x), static_cast<float>(y), static_cast<float>(z) );
                for ( size_t particle = 0; particle < nparticles_in_cell; ++particle )
                {
                    // Calculate a coord.
                    const kvs::Vector3f coord( Generator::RandomSamplingInCube( v ) );

                    // Calculate a scalar value.
                    interpolator.attachPoint( coord );
                    interpolator2.attachPoint( coord );
                    float scalar = interpolator.scalar<T>();
                    float scalar2 = interpolator2.scalar<T>();

                    // Calculate a normal.
                    kvs::Vector3f normal( interpolator.gradient<T>() );
                    // if ( scalar == 0.0 )
                    //     normal = Vector3f( 0.0, 0.0, 1.0 );

                    // set coord, color, and normal to point object( this ).
                    vertex_coords.push_back( coord.x() );
                    vertex_coords.push_back( coord.y() );
                    vertex_coords.push_back( coord.z() );

                    vertex_values.push_back( scalar );
                    vertex_values.push_back( scalar2 );

                    vertex_normals.push_back( normal.x() );
                    vertex_normals.push_back( normal.y() );
                    vertex_normals.push_back( normal.z() );
                } // end of 'paricle' for-loop
            } // end of 'x' loop
        } // end of 'y' loop
    } // end of 'z' loop

    kvs::ValueArray<kvs::Real32> coords( vertex_coords );
    kvs::ValueArray<kvs::Real32> normals( vertex_normals );
    kvs::ValueArray<kvs::Real32> values( vertex_values );

    if ( m_shuffle )
    {
        kvs::UInt32 seed = 12345678;
        coords = ::ShuffleArray<3>( coords, seed );
        normals = ::ShuffleArray<3>( normals, seed );
        values = ::ShuffleArray<2>( values, seed );
    }

    SuperClass::setVeclen( 2 );
    SuperClass::setCoords( coords );
    SuperClass::setValues( values );
    SuperClass::setNormals( normals );
    SuperClass::updateMinMaxValues();
    SuperClass::updateMinMaxCoords();

}

/*===========================================================================*/
/**
 *  @brief  Generates particles for the unstructured volume object.
 *  @param  volume [in] pointer to the input volume object
 */
/*===========================================================================*/
void CellByCellUniformSampling2D::generate_particles( const kvs::UnstructuredVolumeObject* volume )
{
    // Vertex data arrays. (output)
    std::vector<kvs::Real32> vertex_coords;
    std::vector<kvs::Real32> vertex_values;
    std::vector<kvs::Real32> vertex_normals;

    // Set a tetrahedral cell interpolator.
    kvs::CellBase* cell = NULL;
    kvs::CellBase* cell2 = NULL;
    switch ( volume->cellType() )
    {
    case kvs::UnstructuredVolumeObject::Tetrahedra:
    {
        cell = new kvs::TetrahedralCell( volume );
        cell2 = new kvs::TetrahedralCell( m_volume_2 );
        break;
    }
    case kvs::UnstructuredVolumeObject::QuadraticTetrahedra:
    {
        cell = new kvs::QuadraticTetrahedralCell( volume );
        cell2 = new kvs::QuadraticTetrahedralCell( m_volume_2 );        
        break;
    }
    case kvs::UnstructuredVolumeObject::Hexahedra:
    {
        cell = new kvs::HexahedralCell( volume );
        cell2 = new kvs::HexahedralCell( m_volume_2 );
        break;
    }
    case kvs::UnstructuredVolumeObject::QuadraticHexahedra:
    {
        cell = new kvs::QuadraticHexahedralCell( volume );
        cell2 = new kvs::QuadraticHexahedralCell( m_volume_2 );
        break;
    }
    case kvs::UnstructuredVolumeObject::Pyramid:
    {
        cell = new kvs::PyramidalCell( volume );
        cell2 = new kvs::PyramidalCell( m_volume_2 );
        break;
    }
    case kvs::UnstructuredVolumeObject::Prism:
    {
        cell = new kvs::PrismaticCell( volume );
        cell2 = new kvs::PrismaticCell( m_volume_2 );
        break;
    }
    default:
    {
        BaseClass::setSuccess( false );
        kvsMessageError("Unsupported cell type.");
        return;
    }
    }
//    const float min_value = ( typeid(T) == typeid( kvs::UInt8 ) ) ? 0.0f : static_cast<float>( volume->minValue() );
//    const float max_value = ( typeid(T) == typeid( kvs::UInt8 ) ) ? 255.0f : static_cast<float>( volume->maxValue() );
    const float min_value = BaseClass::transferFunction().colorMap().minValue();
    const float max_value = BaseClass::transferFunction().colorMap().maxValue();
    const float max_range = static_cast<float>( BaseClass::transferFunction().resolution() - 1 );
    const float normalize_factor = max_range / ( max_value - min_value );

    const float* const  density_map = m_density_map.data();
    const kvs::ColorMap color_map( BaseClass::transferFunction().colorMap() );
    // Generate particles for each cell.
    const size_t ncells = volume->numberOfCells();
    for ( size_t index = 0; index < ncells; ++index )
    {
        // Bind the cell which is indicated by 'index'.
        cell->bindCell( index );
        cell2->bindCell( index );

        // Calculate a density.
        const float  average_scalar = cell->averagedScalar();
        const size_t average_degree = static_cast<size_t>( ( average_scalar - min_value ) * normalize_factor );
        const float  density = density_map[ average_degree ];

        // Calculate a number of particles in this cell.
        const float volume_of_cell = cell->volume();
        const float p = density * volume_of_cell;
        size_t nparticles_in_cell = static_cast<size_t>( p );

        if ( p - nparticles_in_cell > Generator::GetRandomNumber() ) { ++nparticles_in_cell; }

        // Generate a set of particles in this cell represented by v0,...,v3 and s0,...,s3.
        for ( size_t particle = 0; particle < nparticles_in_cell; ++particle )
        {
            // Calculate a coord.
            const kvs::Vector3f coord = cell->randomSampling();
            cell2->randomSampling();

            // Calculate a value.
            float scalar = cell->scalar();
            float scalar2 = cell2->scalar();

            // Calculate a normal.
            /* NOTE: The gradient vector of the cell is reversed for shading on the rendering process.
             */
            kvs::Vector3f normal( -cell->gradient() );
            // if ( scalar == 0.0 )
            //     normal = Vector3f( 0.0, 0.0, 1.0 );

            // set coord, color, and normal to point object( this ).
            vertex_coords.push_back( coord.x() );
            vertex_coords.push_back( coord.y() );
            vertex_coords.push_back( coord.z() );

            vertex_values.push_back( scalar );
            vertex_values.push_back( scalar2 );

            vertex_normals.push_back( normal.x() );
            vertex_normals.push_back( normal.y() );
            vertex_normals.push_back( normal.z() );
        } // end of 'paricle' for-loop
    } // end of 'cell' for-loop

    kvs::ValueArray<kvs::Real32> coords( vertex_coords );
    kvs::ValueArray<kvs::Real32> normals( vertex_normals );
    kvs::ValueArray<kvs::Real32> values( vertex_values );

    if ( m_shuffle )
    {
        kvs::UInt32 seed = 12345678;
        coords = ::ShuffleArray<3>( coords, seed );
        normals = ::ShuffleArray<3>( normals, seed );
        values = ::ShuffleArray<2>( values, seed );
    }

    SuperClass::setVeclen( 2 );
    SuperClass::setCoords( coords );
    SuperClass::setValues( values );
    SuperClass::setNormals( normals );
    SuperClass::updateMinMaxValues();
    SuperClass::updateMinMaxCoords();

    delete cell;
}

} // end of namespace kun
