//DenseCubicalGrids.h
#include <string>

enum file_format { DIPHA, PERSEUS };

class DenseCubicalGrids { // file_read

public:
	double threshold;
	int dim;
	int ax, ay;
	double dense2[2048][1024];
	file_format format;

	DenseCubicalGrids(const std::string& filename, double _threshold, file_format _format) ; 
	double getBirthday(int index, int dim);
};
