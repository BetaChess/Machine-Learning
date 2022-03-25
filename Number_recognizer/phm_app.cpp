#include "pch.h"


#include "phm_app.h"

#include "simple_render_system.h"
#include "time.h"

#include "NEAT.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>
#include <iostream>

namespace phm
{
	Application::Application()
	{
		Genome testGenome(2, 1);
		
		current_image_ = testdata_.getImage(curr_image_index_);
		loadObjects();
	}

	Application::~Application()
	{
	}

	void Application::run()
	{
		SimpleRenderSystem simpleRenderSystem{ device_, renderer_.getSwapChainRenderPass() };

		Time time;
		float currIndex = 0;

		while (!window_.shouldClose())
		{
			glfwPollEvents();
			time.updateTime();
			currIndex += time.deltaTime() / 0.5f;
			curr_image_index_ = currIndex;
			current_image_ = trainingdata_.getImage(curr_image_index_);

			float aspect = renderer_.getAspectRatio();

			auto commandBuffer = renderer_.beginFrame();
			if (commandBuffer != nullptr)
			{
				numberRenderSquare.transform.scale.x = 1.0f / aspect;
				numberRenderSquare.transform.translation.x = -(1.0f - 1.0f / aspect);
				numberRenderSquare.color = { 1.0f, 1.0f, 1.0f };

				for (size_t i = 0; i < pixelSquares.size(); i++)
				{
					const float pixelHeight = 1.0f / trainingdata_.width();
					const float pixelWidth = pixelHeight * (1.0f / aspect);
					pixelSquares[i].transform.scale.x = pixelWidth;
					pixelSquares[i].transform.scale.y = pixelHeight;

					pixelSquares[i].transform.translation.y = -1 + pixelHeight + 2 * pixelHeight * ((i) / trainingdata_.width());
					pixelSquares[i].transform.translation.x = -1 + pixelWidth + 2 * pixelWidth * ((i) % trainingdata_.width()); // -(1.0f - 1.0f / aspect)

					pixelSquares[i].color = { current_image_.pixels[i] / 255.0f, current_image_.pixels[i] / 255.0f, current_image_.pixels[i] / 255.0f };
				}

				// Render
				renderer_.beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderObjects(commandBuffer, pixelSquares);
				simpleRenderSystem.renderObjects(commandBuffer, { numberRenderSquare });

				renderer_.endSwapChainRenderPass(commandBuffer);
				renderer_.endFrame();
			}
		}

		vkDeviceWaitIdle(device_.device());
	}


	void Application::loadObjects()
	{
		
		// Drawing object (background)
		{
			glm::vec3 position{};
			glm::vec3 color{};
			glm::vec3 normal{};
			glm::vec2 uv{};

			PhmModel::Builder builder;
			builder.vertices =
			{
				{
					{ -1, -1, 0 },	// position
					{ 0, 0, 0 },	// color
					{ 0, 0 ,0 },	// normal (not used)
					{ 0,0 }			// UV TODO
				},
				{
					{ 1, -1, 0 },	// position
					{ 0, 0, 0 },	// color
					{ 0, 0 ,0 },	// normal (not used)
					{ 0,0 }			// UV TODO
				},
				{
					{ 1, 1, 0 },	// position
					{ 0, 0, 0 },	// color
					{ 0, 0 ,0 },	// normal (not used)
					{ 0,0 }			// UV TODO
				},
				{
					{ -1, 1, 0 },	// position
					{ 0, 0, 0 },	// color
					{ 0, 0 ,0 },	// normal (not used)
					{ 0,0 }			// UV TODO
				}
			};
			builder.indices =
			{
				0, 3, 2,
				0, 2, 1
			};

			std::shared_ptr<PhmModel> model = std::make_shared<PhmModel>(device_, builder);
			
			numberRenderSquare.model = model;
		}

		// Pixels
		pixelSquares.resize(trainingdata_.height() * trainingdata_.width());
		for (size_t i = 0; i < pixelSquares.size(); i++)
		{
			glm::vec3 position{};
			glm::vec3 color{};
			glm::vec3 normal{};
			glm::vec2 uv{};

			PhmModel::Builder builder;
			builder.vertices =
			{
				{
					{ -1, -1, 0 },	// position
					{ 0, 0, 0 },	// color
					{ 0, 0 ,0 },	// normal (not used)
					{ 0,0 }			// UV TODO
				},
				{
					{ 1, -1, 0 },	// position
					{ 0, 0, 0 },	// color
					{ 0, 0 ,0 },	// normal (not used)
					{ 0,0 }			// UV TODO
				},
				{
					{ 1, 1, 0 },	// position
					{ 0, 0, 0 },	// color
					{ 0, 0 ,0 },	// normal (not used)
					{ 0,0 }			// UV TODO
				},
				{
					{ -1, 1, 0 },	// position
					{ 0, 0, 0 },	// color
					{ 0, 0 ,0 },	// normal (not used)
					{ 0,0 }			// UV TODO
				}
			};
			builder.indices =
			{
				0, 3, 2,
				0, 2, 1
			};

			std::shared_ptr<PhmModel> model = std::make_shared<PhmModel>(device_, builder);

			pixelSquares[i].model = model;
		}

	}
}


