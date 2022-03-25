#include "pch.h"

#include "image_data.h"

#include <fstream>
#include <stdexcept>
#include <sstream>

/// <summary>
/// Function to read a big endian integer from a file stream.
/// </summary>
/// <returns>The integer with correct endianness corresponding to the system. </returns>
uint32_t readInteger(std::ifstream& fstream)
{
	uint8_t buffer[4] = { 0 };
	fstream.read((char*)buffer, sizeof(buffer));
	uint32_t result = buffer[0];
	result = (result << 8) | buffer[1];
	result = (result << 8) | buffer[2];
	result = (result << 8) | buffer[3];
	return result;
}

ImageData::ImageData(std::string labelFile, std::string imageFile)
	: labelFilePath_{labelFile}, imageFilePath_{imageFile}
{
	loadFileInfo();
	loadImages(imageCount_);
}

ImageData::ImageData(std::string labelFile, std::string imageFile, uint32_t maxImages)
{
	loadFileInfo();
	loadImages(std::min(imageCount_, maxImages));
}

ImageData::Image ImageData::getImage(size_t index)
{
	return ImageData::Image
	{
		images_[index].data(),
		labels_[index],
		height_,
		width_
	};
}

void ImageData::loadFileInfo()
{
	std::ifstream labelStream{ labelFilePath_ };
	std::ifstream imageStream{ imageFilePath_ };

	uint32_t numOfImagesLabelFile, numOfImagesImageFile;
	labelStream.seekg(4);
	numOfImagesLabelFile = readInteger(labelStream);
	imageStream.seekg(4);
	numOfImagesImageFile = readInteger(imageStream);

	if (numOfImagesImageFile != numOfImagesLabelFile)
	{
		throw std::runtime_error((std::stringstream("Number of items do not match in files: ") << labelFilePath_ << " and " << imageFilePath_).str());
	}

	imageCount_ = numOfImagesImageFile;

	imageStream.seekg(8);
	rows_ = readInteger(imageStream);
	imageStream.seekg(12);
	columns_ = readInteger(imageStream);
}

void ImageData::loadImages(uint64_t imagesToLoad)
{
	const uint64_t imageSize = width_ * height_;

	loadedImages_ = imagesToLoad;

	labels_.resize(imagesToLoad);
	images_.resize(imagesToLoad);


	std::ifstream labelStream{ labelFilePath_ };
	labelStream.seekg(8);
	labelStream.read((char*)labels_.data(), imagesToLoad);

	std::ifstream imageStream{ imageFilePath_, std::ifstream::binary };
	
	imageStream.seekg(16);
	for (auto& imageVec : images_)
	{
		imageVec.resize(imageSize);
		imageStream.read((char*)imageVec.data(), imageSize);
	}
}
