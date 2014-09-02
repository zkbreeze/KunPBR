#include "ImageEvaluation.h"
#include <kvs/RGBColor>

namespace kun
{

ImageEvaluation::ImageEvaluation( kvs::ColorImage* image_base, kvs::ColorImage* image_target )
{
	m_image_base = image_base;
	m_image_target = image_target;
	this->exec();
}

void ImageEvaluation::exec()
{
	size_t pixel_number = m_image_base->numberOfPixels();

	float diff = 0.0;
	size_t max_pixel = 256 * 256 * 3;
	for ( size_t i = 0; i < pixel_number; i++ )
	{
		kvs::RGBColor color_base = m_image_base->pixel( i );
		kvs::RGBColor color_target = m_image_target->pixel( i );

		float r_diff_power = ( color_target.r() - color_base.r() ) * ( color_target.r() - color_base.r() );
		float g_diff_power = ( color_target.g() - color_base.g() ) * ( color_target.g() - color_base.g() );
		float b_diff_power = ( color_target.b() - color_base.b() ) * ( color_target.b() - color_base.b() );
		diff += std::sqrt( r_diff_power + g_diff_power + b_diff_power );
	}	

	m_mse = diff / pixel_number;
	m_rmse = std::sqrt( m_mse );
	m_psnr = 10 * std::log10( max_pixel / m_mse );
}

float ImageEvaluation::MSE()
{
	return m_mse;
}

float ImageEvaluation::RMSE()
{
	return m_rmse;
}

float ImageEvaluation::PSNR()
{
	return m_psnr;
}

}// end of namespace kun