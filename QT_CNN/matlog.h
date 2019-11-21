#pragma once
#include "Matrix.h"
#include <vector>
#include <string>
#include <fstream>

class matlog
{
public:
	matlog(const std::string& filename):filename(filename){}
	void load(std::vector<Matrix<double>>& W,
		std::vector<Matrix<double>>& B)
	{
		file.open(filename, std::ios::in | std::ios::binary);
		for (auto& x : W)x.load(file);
		for (auto& x : B)x.load(file);
		file.close();
	}
	void save(std::vector<Matrix<double>>& W,
		std::vector<Matrix<double>>& B)
	{
		file.open(filename, std::ios::out);
		file.close();//clear
		file.open(filename, std::ios::app | std::ios::binary);
		for (auto x : W)x.save(file);
		for (auto x : B)x.save(file);
		file.close();
	}
	~matlog()
	{
		file.close();
	}
private:
	std::string filename;
	std::fstream file;
};
