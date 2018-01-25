//ColumnsToReduce.h

#include <vector>
using namespace std;

class ColumnsToReduce
{

public:
	vector<BirthdayIndex> columns_to_reduce;
	int dim;
	int max_of_index;

	ColumnsToReduce(DenseCubicalGrids* _dcg);
	int size() ;

};