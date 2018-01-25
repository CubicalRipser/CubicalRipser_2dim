//SimplexCoboundaryEnumerator.cpp

#include <iostream>
#include <algorithm>
#include <string>

// #include "Coeff.h"
// #include "Vertices.h"
#include "DenseCubicalGrids.h"
#include "BirthdayIndex.h"
#include "ColumnsToReduce.h"
#include "SimplexCoboundaryEnumerator.h"

// BirthdayIndex* simplex;
// DenseCubicalGrids* dcg;
// int dim;
// double birthtime;
// int ax, ay;
// int cx, cy;
// int count;
// BirthdayIndex* nextCoface;
// double threshold;
// const int mode = 1; // 0 -> eight neighbourhoods 、 1 -> four neighbourhoods 
// //
// // 0 4 2
// // 7 * 6
// // 3 5 1
// //
// int dx[8] = { -1, 1, 1, -1, 0, 0, 1, -1 };
// int dy[8] = { 1, -1, 1, -1, 1, -1, 0, 0 };
// int dcx[8] = { -1, 0, 0, -1, 0, 0, 0, -1 };
// int dcy[8] = { 0, -1, 0, -1, 0, -1, 0, 0 };
// int cm1[8] = { 3, 3, 2, 2, 1, 1, 0, 0 };

SimplexCoboundaryEnumerator::SimplexCoboundaryEnumerator(){
	nextCoface = BirthdayIndex(0, -1, 1);
}


void SimplexCoboundaryEnumerator::setSimplexCoboundaryEnumerator(BirthdayIndex _s, DenseCubicalGrids* _dcg) {
	simplex = _s;
	dcg = _dcg;
	dim = simplex.dim;
	birthtime = simplex.birthday;
	ax = _dcg->ax;
	ay = _dcg->ay;
	
	cx = (simplex.index) & 0x07ff;//
	cy = (simplex.index >> 11) & 0x03ff;//
	cm = (simplex.index >> 21) & 0xff;

	threshold = _dcg->threshold;
	count = 0;
}


bool SimplexCoboundaryEnumerator::hasNextCoface() {
	int index = 0;
	double birthday = 0;
	switch (dim) {
	case 0:
		for (int i = count; i < 4; i++) {
			switch(i){
			case 0:/// y+
				index = (1 << 21) | ((cy) << 11) | (cx);
				birthday = max(birthtime, dcg->dense2[cx  ][cy+1]);
				break;
			case 1:/// y-
				index = (1 << 21) | ((cy-1) << 11) | (cx);
				birthday = max(birthtime, dcg->dense2[cx  ][cy-1]);
				break;
			case 2:/// x+
				index = (0 << 21) | ((cy) << 11) | (cx);
				birthday = max(birthtime, dcg->dense2[cx+1][cy  ]);
				break;
			case 3:/// x-
				index = (0 << 21) | ((cy) << 11) | (cx-1);
				birthday = max(birthtime, dcg->dense2[cx-1][cy  ]);
				break;
			// default:
			// 	index = (cm1[i + 4] << 21) | ((cy + dcy[i + 4]) << 11) | (cx + dcx[i + 4]);
			// 	birthday = max(birthtime, dcg->dense2[cx + dx[i + 4]][cy + dy[i + 4]]);
			}
			if (birthday != threshold) {
				count = i + 1;
				nextCoface = BirthdayIndex(birthday, index, 1);
				return true;
			}
		}
		return false;
	case 1:
		switch (cm) {
		case 0:
			if(count == 0){ // upper(4)
				count++;
				index = ((cy) << 11) | cx;
				birthday = max({birthtime, dcg->dense2[cx][cy + 1], dcg->dense2[cx + 1][cy + 1]});
				if (birthday != threshold) {
					nextCoface = BirthdayIndex(birthday, index, 2);
					return true;
				}
			} else 
			if(count == 1){ // lower(5)
				count++;
				index = ((cy - 1) << 11) | cx;
				birthday = max({birthtime, dcg->dense2[cx][cy - 1], dcg->dense2[cx + 1][cy - 1]});
				if (birthday != threshold) {
					nextCoface = BirthdayIndex(birthday, index, 2);
					return true;
				}
			}
			return false;
		case 1:
			if(count == 0){ // right(6)
				count ++;
				index = ((cy) << 11) | cx;
				birthday = max({birthtime, dcg->dense2[cx + 1][cy], dcg->dense2[cx + 1][cy + 1]});
				if (birthday != threshold) {
					nextCoface = BirthdayIndex(birthday, index, 2);
					return true;
				}
			} else 
			if(count == 1){ //left(7)
				count++;
				index = ((cy) << 11) | (cx - 1);
				birthday = max({birthtime, dcg->dense2[cx - 1][cy], dcg->dense2[cx - 1][cy + 1]});
				if (birthday != threshold) {
					nextCoface = BirthdayIndex(birthday, index, 2);
					return true;
				}
			}
			return false;
		}
	}
	return false;
}

BirthdayIndex SimplexCoboundaryEnumerator::getNextCoface() {
	return nextCoface;
}
