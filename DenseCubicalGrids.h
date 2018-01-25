//DenseCubicalGrids.h
#include <string>

class DenseCubicalGrids { // file_read

public:
	double threshold;
	int dim;
	int ax, ay;
	double dense2[2048][1024];
	int flag;

	DenseCubicalGrids(const std::string& filename, double _threshold, int _flag) ; 
	double getBirthday(int index, int dim);
};
