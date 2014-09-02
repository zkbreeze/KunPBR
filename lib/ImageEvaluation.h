#ifndef IMAGE_EVALUATION
#define IMAGE_EVALUATION

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

#endif