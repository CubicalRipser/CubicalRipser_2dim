//ComputePairs.cpp
//#define PRINT_PERSISTENCE_PAIRS


#include <iostream>
#include <algorithm>
#include <queue>

#include "DenseCubicalGrids.h"
#include "BirthdayIndex.h"
#include "ColumnsToReduce.h"
#include "SimplexCoboundaryEnumerator.h"
#include "WritePairs.h"
#include "UnionFind.h"
#include "ComputePairs.h"

using namespace std;


	// DenseCubicalGrids* dcg;
	// ColumnsToReduce* ctr;
	// hash_map<int, int> pivot_column_index;
	// int ax, ay;
	// int dim;
	// const int mode = 1;
	// vector<Writepairs> *wp;

ComputePairs::ComputePairs(DenseCubicalGrids* _dcg, ColumnsToReduce* _ctr, vector<WritePairs> &_wp){
	dcg = _dcg;
	ctr = _ctr;
	dim = _ctr->dim;
	wp = &_wp;

	ax = _dcg->ax;
	ay = _dcg->ay;
}

void ComputePairs::compute_pairs_main(){
#ifdef PRINT_PERSISTENCE_PAIRS
	cout << "persistence intervals in dim " << dim << ":" << endl;
#endif
	int printlowerlimit=000;
	int printupperlimit=6000;
	bool lineStart=true;

	vector<BirthdayIndex> coface_entries;
	SimplexCoboundaryEnumerator cofaces;
	unordered_map<int, priority_queue<BirthdayIndex, vector<BirthdayIndex>, BirthdayIndexComparator>> recorded_wc;

	pivot_column_index = hash_map<int, int>();
	auto ctl_size = ctr->columns_to_reduce.size();
	pivot_column_index.reserve(ctl_size);
	recorded_wc.reserve(ctl_size);

	
	for(int i = 0; i < ctl_size; ++i){ 
		lineStart = true;
		auto column_to_reduce = ctr->columns_to_reduce[i]; 
		priority_queue<BirthdayIndex, vector<BirthdayIndex>, BirthdayIndexComparator> working_coboundary;
		double birth = column_to_reduce.getBirthday();
		//cout << "birth : " << birth <<endl;

		int j = i;
		BirthdayIndex pivot(0, -1, 0);
		bool might_be_apparent_pair = true;
		bool goto_found_persistence_pair = false;

		do {
			// if(printlowerlimit<=i && i<printupperlimit && i!=j){
			// 	if(lineStart){
			// 		cout << endl << i << "-";
			// 		lineStart=false; 
			// 	} 
			// 	cout << j << "," ;
			// }
			auto simplex = ctr->columns_to_reduce[j];
			coface_entries.clear();
			cofaces.setSimplexCoboundaryEnumerator(simplex, dcg);

			while (cofaces.hasNextCoface() && !goto_found_persistence_pair) {
				BirthdayIndex coface = cofaces.getNextCoface();
				coface_entries.push_back(coface);
				if (might_be_apparent_pair && (simplex.getBirthday() == coface.getBirthday())) {
					if (pivot_column_index.find(coface.getIndex()) == pivot_column_index.end()) {
						pivot.copyBirthdayIndex(coface);
						goto_found_persistence_pair = true;// goto (B)
						//break;
					} else {
						might_be_apparent_pair = false;// goto(A)
					}
				}
			}

			if (!goto_found_persistence_pair) {// (A) I haven't had a pivot
				auto findWc = recorded_wc.find(j);
				if(findWc != recorded_wc.end()){// if the pivot is old,
					auto wc = findWc->second;
					while (!wc.empty()){// we push the data of the old pivot's wc
						auto e = wc.top();
						working_coboundary.push(e);
						wc.pop();
					}
				} else {
					for (auto e : coface_entries) {// making wc here
						working_coboundary.push(e);
					}
				}
				pivot = get_pivot(working_coboundary);// getting a pivot from wc

				if (pivot.getIndex() != -1) {//When I have a pivot, ...
					auto pair = pivot_column_index.find(pivot.getIndex());
					if (pair != pivot_column_index.end()) {	// if the pivot already exists, go on the loop 
						j = pair->second;
						continue;
					} else {// if the pivot is new, 
						// I record this wc into recorded_wc, and 
						recorded_wc.insert(make_pair(i, working_coboundary));
						// I output PP as Writepairs
						double death = pivot.getBirthday();
						outputPP(dim,birth,death);
						pivot_column_index.insert(make_pair(pivot.getIndex(), i));
						break;
					}
				} else {// if wc is empty, I output a PP as [birth,) 
					outputPP(-1, birth, dcg->threshold);
					break;
				}
			} else {// (B) I have a pivot and output PP as Writepairs 
				double death = pivot.getBirthday();
				outputPP(dim,birth,death);
				pivot_column_index.insert(make_pair(pivot.getIndex(), i));
				break;
			}			

		} while (true);
	}
}

void ComputePairs::outputPP(int _dim, double _birth, double _death){
	if(_birth != _death){
		if(_death != dcg-> threshold){
#ifdef PRINT_PERSISTENCE_PAIRS
			cout << "[" <<_birth << "," << _death << ")" << endl;
#endif
			wp->push_back(WritePairs(_dim, _birth, _death));
		} else {
#ifdef PRINT_PERSISTENCE_PAIRS
			cout << "[" << _birth << ", )" << endl;
#endif
			wp->push_back(WritePairs(-1, _birth, dcg->threshold));
		}
	}
}

BirthdayIndex ComputePairs::pop_pivot(priority_queue<BirthdayIndex, vector<BirthdayIndex>, BirthdayIndexComparator>& column){
	if (column.empty()) {
		return BirthdayIndex(0, -1, 0);
	} else {
		auto pivot = column.top();
		column.pop();

		while (!column.empty() && column.top().index == pivot.getIndex()) {
			column.pop();
			if (column.empty())
				return BirthdayIndex(0, -1, 0);
			else {
				pivot = column.top();
				column.pop();
			}
		}
		return pivot;
	}
}

BirthdayIndex ComputePairs::get_pivot(priority_queue<BirthdayIndex, vector<BirthdayIndex>, BirthdayIndexComparator>&
	column) {
	BirthdayIndex result = pop_pivot(column);
	if (result.getIndex() != -1) {
		column.push(result);
	}
	return result;
}

void ComputePairs::assemble_columns_to_reduce() {
	++dim;
	ctr->dim = dim;
	const int typenum = 2;
	if (dim == 1) {
		ctr->columns_to_reduce.clear();
		for (int y = 1; y <= ay; ++y) {
			for (int x = 1; x <= ax; ++x) {
				for (int m = 0; m < typenum; ++m) {
					double index = x | (y << 11) | (m << 21);
					if (pivot_column_index.find(index) == pivot_column_index.end()) {
						double birthday = dcg->getBirthday(index, 1);
						if (birthday != dcg->threshold) {
							ctr->columns_to_reduce.push_back(BirthdayIndex(birthday, index, 1));
						}
					}
				}
			}
		}
	}
	sort(ctr->columns_to_reduce.begin(), ctr->columns_to_reduce.end(), BirthdayIndexComparator());
	cout << ctr->columns_to_reduce.size() << endl;
	/*for(int i = 0; i < ctr->size(); i++){
		cout << ctr->columns_to_reduce[i].getBirthday() << " : " << ctr->columns_to_reduce[i].getIndex() << endl;
	}*/
}

