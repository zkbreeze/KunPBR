//
//  DensityCalculator.h
//
//
//  Created by Kun Zhao on 2015-06-21 22:17:10.
//
//

#ifndef KUN__DENSITYCALCULATOR_H_INCLUDE
#define KUN__DENSITYCALCULATOR_H_INCLUDE
 
#include "PointObject.h"
#include <kvs/StructuredVolumeObject>

namespace kun
{

class DensityCalculator
{

	kun::PointObject* m_point;
	kvs::StructuredVolumeObject* m_volume;
	int m_x;
	int m_y;
	int m_z;
	int m_max_grid;
	bool m_use_given_grid;
	int* m_table;
	float m_cube_volume;

public:
	DensityCalculator( void );
	DensityCalculator( kun::PointObject* point );
	~DensityCalculator();

	void setPointObject( kun::PointObject* point ){ m_point = point; }
	void setGrid( int x, int y, int z ) { m_x = x; m_y = y; m_z = z; m_use_given_grid = true; }
	void setMaxGrid( int max ){ m_max_grid = max; }
	void exec();
	kvs::StructuredVolumeObject* outputDensityVolume();
};

} // end of namespace kun
 
#endif // KUN__DENSITYCALCULATOR_H_INCLUDE