//
//  CellByCellUniformSampling.h
//
//
//  Created by Kun Zhao on 2014-09-04 16:08:14.
//
//

#ifndef KUN__CELLBYCELLUNIFORMSAMPLING_H_INCLUDE
#define KUN__CELLBYCELLUNIFORMSAMPLING_H_INCLUDE

#include <kvs/MapperBase>
#include <kvs/Camera>
#include "PointObject.h"
#include <kvs/VolumeObjectBase>
#include <kvs/StructuredVolumeObject>
#include <kvs/UnstructuredVolumeObject>
#include <kvs/Module>
#include "CellByCellParticleGenerator.h"

namespace kun
{

/*===========================================================================*/
/**
 *  @brief  Cell-by-cell particle generation class.
 */
/*===========================================================================*/
class CellByCellUniformSampling : public kvs::MapperBase, public kun::PointObject
{
    kvsModuleName( kvs::CellByCellUniformSampling );
    kvsModuleCategory( Mapper );
    kvsModuleBaseClass( kvs::MapperBase );
    kvsModuleSuperClass( kun::PointObject );

private:

    const kvs::Camera* m_camera; ///< camera (reference)
    size_t m_subpixel_level; ///< subpixel level
    float m_sampling_step; ///< sampling step in the object coordinate
    float m_object_depth; ///< object depth
    kvs::ValueArray<float> m_density_map; ///< density map

    // ADD
    bool m_shuffle;
    bool m_kun_sampling_step;

public:

    CellByCellUniformSampling();
    CellByCellUniformSampling(
        const kvs::VolumeObjectBase* volume,
        const size_t subpixel_level,
        const float sampling_step,
        const kvs::TransferFunction& transfer_function,
        const float object_depth = 0.0f );
    CellByCellUniformSampling(
        const kvs::Camera* camera,
        const kvs::VolumeObjectBase* volume,
        const size_t subpixel_level,
        const float sampling_step,
        const kvs::TransferFunction& transfer_function,
        const float object_depth = 0.0f );
    virtual ~CellByCellUniformSampling();

    SuperClass* exec( const kvs::ObjectBase* object );

    size_t subpixelLevel() const;
    float samplingStep() const;
    float objectDepth() const;

    void attachCamera( const kvs::Camera* camera );
    void setSubpixelLevel( const size_t subpixel_level );
    void setSamplingStep( const float sampling_step );
    void setObjectDepth( const float object_depth );

    // ADD
    void EnableShuffleParticles(){ m_shuffle = true; }
    void EnableKunSamplingStep(){ m_kun_sampling_step = true; }

private:

    void mapping( const kvs::Camera* camera, const kvs::StructuredVolumeObject* volume );
    void mapping( const kvs::Camera* camera, const kvs::UnstructuredVolumeObject* volume );
    template <typename T> void generate_particles( const kvs::StructuredVolumeObject* volume );
    void generate_particles( const kvs::UnstructuredVolumeObject* volume );
};

} // end of namespace kun

#endif // KUN__CELLBYCELLUNIFORMSAMPLING_H_INCLUDE
