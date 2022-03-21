#ifndef PHM_APP_H
#define PHM_APP_H

#include <memory>
#include <vector>

#include "phm_window.h"
#include "phm_renderer.h"
#include "phm_object.h"
#include "image_data.h"


namespace phm
{
	class Application
	{

	public:
		static constexpr size_t WIDTH = 800;
		static constexpr size_t HEIGHT = 600;

		Application();
		~Application();

		Application(const Application&) = delete;
		Application& operator=(const Application&) = delete;

		void run();

	private:
		PhmWindow window_{ WIDTH, HEIGHT, "Number recoqnizer" };
		PhmDevice device_{ window_ };
		PhmRenderer renderer_{ window_, device_ };

		ImageData trainingdata_{ "training_data/train-labels.idx1-ubyte", "training_data/train-images.idx3-ubyte" };
		ImageData testdata_{ "test_data/t10k-labels.idx1-ubyte", "test_data/t10k-images.idx3-ubyte" };

		size_t curr_image_index_ = 0;
		ImageData::Image current_image_;

		PhmObject numberRenderSquare;
		std::vector<PhmObject> pixelSquares;

		void loadObjects();
	};
}



#endif /* PHM_APP_H */
