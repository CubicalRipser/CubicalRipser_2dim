//ColumnsToReduce.cpp


#include <algorithm>
#include <unordered_map>


// #include "Coeff.h"
// #include "Vertices.h"
#include "DenseCubicalGrids.h"
#include "BirthdayIndex.h"
#include "ColumnsToReduce.h"

using namespace std;


// vector<BirthdayIndex> columns_to_reduce;
// int dim;
// int max_of_index;

ColumnsToReduce::ColumnsToReduce(DenseCubicalGrids* _dcg) {
	dim = 0;
	int ax = _dcg->ax;
	int ay = _dcg->ay;
	max_of_index = 2048*(ay + 2);
	int index;
	double birthday;
	for (int y = ay; y > 0; y--) {
		for (int x = ax; x > 0; x--) {
			birthday = _dcg->dense2[x][y];
			index = x | (y << 11);
			if (birthday != _dcg->threshold) {
				columns_to_reduce.push_back(BirthdayIndex(birthday, index, 0));
			}
		}
	}
	sort(columns_to_reduce.rbegin(), columns_to_reduce.rend(), BirthdayIndexInverseComparator());
}

int ColumnsToReduce::size() {
	return columns_to_reduce.size();
}
