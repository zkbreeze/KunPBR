//
//  ImageEvaluation.h
//
//
//  Created by Kun Zhao on 2014-09-04 16:09:30.
//
//

#ifndef KUN__IMAGEEVALUATION_H_INCLUDE
#define KUN__IMAGEEVALUATION_H_INCLUDE
 
#include <kvs/ColorImage>

namespace kun
{

class ImageEvaluation
{
	kvs::ColorImage* m_image_base;
	kvs::ColorImage* m_image_target;

	float m_mse;
	float m_rmse;
	float m_psnr;

public:
	ImageEvaluation( kvs::ColorImage* image_base, kvs::ColorImage* image_target );

	void exec();

	float MSE();
	float RMSE();
	float PSNR();
};

} // end of namespace kun
 
#endif // KUN__IMAGEEVALUATION_H_INCLUDE