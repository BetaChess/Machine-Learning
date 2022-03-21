#ifndef IMAGE_DATA_H
#define IMAGE_DATA_H

#include <string_view>
#include <string>
#include <vector>
#include <memory>


class ImageData
{
public:
	// Loads all images
	ImageData(std::string labelFile, std::string imageFile);
	// Loads images up to the limit
	ImageData(std::string labelFile, std::string imageFile, uint32_t maxImages);

	struct Image
	{
		uint8_t* pixels;
		uint8_t label;
		union
		{
			uint32_t rows;
			uint32_t height;
		};
		union
		{
			uint32_t columns;
			uint32_t width;
		};
	};

	Image getImage(size_t index);
	uint32_t height() const { return height_; };
	uint32_t width() const { return width_; };

private:
	std::string labelFilePath_;
	std::string imageFilePath_;

	uint32_t imageCount_;	// The number of images in the file(s)
	uint32_t loadedImages_;	// The number of images loaded from the file(s)

	union
	{
		uint32_t rows_;
		uint32_t height_;
	};
	union
	{
		uint32_t columns_;
		uint32_t width_;
	};

	std::vector<uint8_t> labels_;
	std::vector<std::vector<uint8_t>> images_;

	void loadFileInfo();
	void loadImages(uint64_t imagesToLoad);
};

#endif /* IMAGE_DATA_H */
