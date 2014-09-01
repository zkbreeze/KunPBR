#include "ImageEvaluation.h"
#include <kvs/ColorImage>

int main ( int agrc, char** argv )
{
	kvs::ColorImage* image_base = new kvs::ColorImage( argv[1] );
	kvs::ColorImage* image_target = new kvs::ColorImage( argv[2] );

	ImageEvaluation evaluator( image_base, image_target );
	std::cout << "MSE: " << evaluator.MSE() << std::endl;
	std::cout << "RMSE: " << evaluator.RMSE() << std::endl;
	std::cout << "PSNR: " << evaluator.PSNR() << std::endl;
}