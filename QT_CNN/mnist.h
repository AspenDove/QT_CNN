#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <random>
#include <algorithm>
#include <numeric>
#include <opencv2/opencv.hpp>

const int MAGICNUMBEROFIMAGE = 2051;
const int MAGICNUMBEROFLABEL = 2049;

class mnist
{
public:
	mnist(const std::string& image, const std::string& Label, const size_t& size = 60000):read(0)
	{
		allImage = ReadImages(image, size);
		allLabel = ReadLabels(Label, size);
		shuffle_index.resize(60000);
		iota(shuffle_index.begin(), shuffle_index.end(), 0);
	}
	void next_batch(const size_t& size)
	{
		if (read + size > 60000)read = 0;
		Label = cv::Mat(allLabel, cv::Range(read, read + size)).clone();
		Image = cv::Mat(allImage, cv::Range(read, read + size)).clone();
		read += size;
	}
	void shuffle_batch(const size_t& size)
	{
		if (size > 60000)return;
		cv::Mat rLabel, rImage;
		
		random_shuffle(shuffle_index.begin(), shuffle_index.end());

		for (size_t i = 0; i != size; ++i)
		{
			rImage.push_back(allImage.row(shuffle_index[i]));
			rLabel.push_back(allLabel.row(shuffle_index[i]));
		}
		Label = rLabel.clone();
		Image = rImage.clone();
	}
	int ConvertCharArrayToInt(unsigned char* array, int LengthOfArray)
	{
		if (LengthOfArray < 0)
		{
			return -1;
		}
		int result = static_cast<signed int>(array[0]);
		for (int i = 1; i < LengthOfArray; i++)
		{
			result = (result << 8) + array[i];
		}
		return result;
	}
	bool IsImageDataFile(unsigned char* MagicNumber, int LengthOfArray)
	{
		int MagicNumberOfImage = ConvertCharArrayToInt(MagicNumber, LengthOfArray);
		if (MagicNumberOfImage == MAGICNUMBEROFIMAGE)
		{
			return true;
		}
		return false;
	}
	bool IsLabelDataFile(unsigned char *MagicNumber, int LengthOfArray)
	{
		int MagicNumberOfLabel = ConvertCharArrayToInt(MagicNumber, LengthOfArray);
		if (MagicNumberOfLabel == MAGICNUMBEROFLABEL)
		{
			return true;
		}
		return false;
	}
	cv::Mat ReadData(std::fstream& DataFile, int NumberOfData, int DataSizeInBytes)
	{
		cv::Mat DataMat;
		// read the data if the file is opened.
		if (DataFile.is_open())
		{
			int AllDataSizeInBytes = DataSizeInBytes * NumberOfData;
			unsigned char* TmpData = new unsigned char[AllDataSizeInBytes];
			DataFile.read((char *)TmpData, AllDataSizeInBytes);
			DataMat = cv::Mat(NumberOfData, DataSizeInBytes, CV_8UC1,
				TmpData).clone();
			delete[] TmpData;
			DataFile.close();
		}
		return DataMat;
	}
	cv::Mat ReadImageData(std::fstream& ImageDataFile, int NumberOfImages)
	{
		int ImageSizeInBytes = 28 * 28;
		return ReadData(ImageDataFile, NumberOfImages, ImageSizeInBytes);
	}
	cv::Mat ReadLabelData(std::fstream& LabelDataFile, int NumberOfLabel)
	{
		int LabelSizeInBytes = 1;
		return ReadData(LabelDataFile, NumberOfLabel, LabelSizeInBytes);
	}
	cv::Mat ReadImages(const std::string& FileName, const size_t& size)
	{
		std::fstream File(FileName.c_str(), std::ios_base::in | std::ios_base::binary);
		if (!File.is_open())
		{
			return cv::Mat();
		}
		MNISTImageFileHeader FileHeader;
		File.read((char *)(&FileHeader), sizeof(FileHeader));
		if (!IsImageDataFile(FileHeader.MagicNumber, 4))
		{
			return cv::Mat();
		}
		int NumberOfImage = ConvertCharArrayToInt(FileHeader.NumberOfImages, 4);
		return ReadImageData(File, size);
	}
	cv::Mat ReadLabels(const std::string& FileName, const size_t& size)
	{
		std::fstream File(FileName.c_str(), std::ios_base::in | std::ios_base::binary);
		if (!File.is_open())
		{
			return cv::Mat();
		}
		MNISTLabelFileHeader FileHeader;
		File.read((char *)(&FileHeader), sizeof(FileHeader));
		if (!IsLabelDataFile(FileHeader.MagicNumber, 4))
		{
			return cv::Mat();
		}
		int NumberOfImage = ConvertCharArrayToInt(FileHeader.NumberOfLabels, 4);
		return ReadLabelData(File, size);
	}
	cv::Mat MergecvMat(bool isCorrect[], const size_t& row, const size_t& col)
	{
		cv::Mat result(row * 28, col * 28, CV_8UC1);
		cv::cvtColor(result, result, cv::COLOR_GRAY2RGB);
		for (int i = 0; i != row; ++i)
			for (int j = 0; j != col; ++j)
			{
				//cv::Mat m = cv::Mat(cv::Size(28, 28), CV_8UC1, this->Image.ptr<uchar[28 * 28]>(i*col + j)).clone();
				cv::Mat imgRGB;
				cv::cvtColor(cv::Mat(cv::Size(28, 28), CV_8UC1, 
					this->Image.ptr<uchar[28 * 28]>(i*col + j)),
					imgRGB, cv::COLOR_GRAY2RGB);
				if (!isCorrect[i*col + j])
					cv::rectangle(imgRGB, cv::Rect(1, 1, 27, 27), cv::Scalar(0, 0, 255));
				else
					cv::rectangle(imgRGB, cv::Rect(1, 1, 27, 27), cv::Scalar(0, 255, 0));

				cv::Mat imageROI = result(cv::Rect(j * 28, (row - i - 1) * 28, 28, 28));
				imgRGB.copyTo(imageROI);
			}
		return result.clone();
	}

	std::vector<int> shuffle_index;
	cv::Mat Label, Image;
	size_t read;

private:
	cv::Mat allLabel, allImage;

	struct MNISTImageFileHeader
	{
		unsigned char MagicNumber[4];
		unsigned char NumberOfImages[4];
		unsigned char NumberOfRows[4];
		unsigned char NumberOfColums[4];
	};

	struct MNISTLabelFileHeader
	{
		unsigned char MagicNumber[4];
		unsigned char NumberOfLabels[4];
	};
};
