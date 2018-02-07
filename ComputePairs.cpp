/* ComputePairs.cpp

Copyright 2017-2018 Takeki Sudo and Kazushi Ahara.

This file is part of CubicalRipser_2dim.

CubicalRipser: C++ system for computation of Cubical persistence pairs
Copyright 2017-2018 Takeki Sudo and Kazushi Ahara.
CubicalRipser is free software: you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the
Free Software Foundation, either version 3 of the License, or (at your option)
any later version.

CubicalRipser is deeply depending on 'Ripser', software for Vietoris-Rips 
persitence pairs by Ulrich Bauer, 2015-2016.  We appreciate Ulrich very much.
We rearrange his codes of Ripser and add some new ideas for optimization on it 
and modify it for calculation of a Cubical filtration.

This part of CubicalRiper is a calculator of cubical persistence pairs for 
2 dimensional pixel data. The input data format conforms to that of DIPHA.
 See more descriptions in README.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
You should have received a copy of the GNU Lesser General Public License along
with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


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

ComputePairs::ComputePairs(DenseCubicalGrids* _dcg, ColumnsToReduce* _ctr, vector<WritePairs> &_wp, const bool _print){
	dcg = _dcg;
	ctr = _ctr;
	dim = _ctr -> dim;
	wp = &_wp;
	print = _print;

	ax = _dcg -> ax;
	ay = _dcg -> ay;
}

void ComputePairs::compute_pairs_main(){
	if(print == true){
		cout << "persistence intervals in dim " << dim << ":" << endl;
	}
  
	vector<BirthdayIndex> coface_entries;
	SimplexCoboundaryEnumerator cofaces;
	unordered_map<int, priority_queue<BirthdayIndex, vector<BirthdayIndex>, BirthdayIndexComparator>> recorded_wc;

	pivot_column_index = hash_map<int, int>();
	auto ctl_size = ctr -> columns_to_reduce.size();
	pivot_column_index.reserve(ctl_size);
	recorded_wc.reserve(ctl_size);

	
	for(int i = 0; i < ctl_size; ++i){ 
		auto column_to_reduce = ctr -> columns_to_reduce[i]; 
		priority_queue<BirthdayIndex, vector<BirthdayIndex>, BirthdayIndexComparator> working_coboundary;
		double birth = column_to_reduce.getBirthday();

		int j = i;
		BirthdayIndex pivot(0, -1, 0);
		bool might_be_apparent_pair = true;
		bool goto_found_persistence_pair = false;

		do {
			auto simplex = ctr -> columns_to_reduce[j];// get CTR[i]
			coface_entries.clear();
			cofaces.setSimplexCoboundaryEnumerator(simplex, dcg);// make cofaces data

			while (cofaces.hasNextCoface() && !goto_found_persistence_pair) {// repeat there remains a coface
				BirthdayIndex coface = cofaces.getNextCoface();
				coface_entries.push_back(coface);
				if (might_be_apparent_pair && (simplex.getBirthday() == coface.getBirthday())) {// if bt is the same, go thru
					if (pivot_column_index.find(coface.getIndex()) == pivot_column_index.end()) {// if coface is not in pivot list
						pivot.copyBirthdayIndex(coface);// I have a new pivot
						goto_found_persistence_pair = true;// goto (B)
					} else {// if pivot list contains this coface,
						might_be_apparent_pair = false;// goto(A)
					}
				}
			}

			if (!goto_found_persistence_pair) {// (A) if pivot list contains this coface
				auto findWc = recorded_wc.find(j); // we seek wc list by 'j'
				if(findWc != recorded_wc.end()){// if the pivot is old,
					auto wc = findWc -> second;
					while (!wc.empty()){// we push the data of the old pivot's wc
						auto e = wc.top();
						working_coboundary.push(e);
						wc.pop();
					}
				} else {// if the pivot is new, 
					for (auto e : coface_entries) {// making wc here
						working_coboundary.push(e);
					}
				}
				pivot = get_pivot(working_coboundary);// getting a pivot from wc

				if (pivot.getIndex() != -1) {//When I have a pivot, ...
					auto pair = pivot_column_index.find(pivot.getIndex());
					if (pair != pivot_column_index.end()) {	// if the pivot already exists, go on the loop 
						j = pair -> second;
						continue;
					} else {// if the pivot is new, 
						// I record this wc into recorded_wc, and 
						recorded_wc.insert(make_pair(i, working_coboundary));
						// I output PP as Writepairs
						double death = pivot.getBirthday();
						outputPP(dim, birth, death);
						pivot_column_index.insert(make_pair(pivot.getIndex(), i));
						break;
					}
				} else {// if wc is empty, I output a PP as [birth,) 
					outputPP(-1, birth, dcg -> threshold);
					break;
				}
			} else {// (B) I have a new pivot and output PP as Writepairs 
				double death = pivot.getBirthday();
				outputPP(dim, birth, death);
				pivot_column_index.insert(make_pair(pivot.getIndex(), i));
				break;
			}			

		} while (true);
	}
}

void ComputePairs::outputPP(int _dim, double _birth, double _death){
	if(_birth != _death){
		if(_death != dcg-> threshold){
			if(print == true){
				cout << "[" <<_birth << "," << _death << ")" << endl;
			}
			
			wp->push_back(WritePairs(_dim, _birth, _death));
		} else {
			if(print == true){
				cout << "[" << _birth << ", )" << endl;
			}

			wp -> push_back(WritePairs(-1, _birth, dcg -> threshold));
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
	ctr -> dim = dim;
	const int typenum = 2;
	if (dim == 1) {
		ctr -> columns_to_reduce.clear();
		for (int y = 1; y <= ay; ++y) {
			for (int x = 1; x <= ax; ++x) {
				for (int m = 0; m < typenum; ++m) {
					double index = x | (y << 11) | (m << 21);
					if (pivot_column_index.find(index) == pivot_column_index.end()) {
						double birthday = dcg -> getBirthday(index, 1);
						if (birthday != dcg -> threshold) {
							ctr -> columns_to_reduce.push_back(BirthdayIndex(birthday, index, 1));
						}
					}
				}
			}
		}
	}
	sort(ctr -> columns_to_reduce.begin(), ctr -> columns_to_reduce.end(), BirthdayIndexComparator());
}