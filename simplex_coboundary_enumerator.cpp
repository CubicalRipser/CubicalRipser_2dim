/* simplex_coboundary_enumerator.cpp

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
#include <string>

#include "dense_cubical_grids.h"
#include "birthday_index.h"
#include "columns_to_reduce.h"
#include "simplex_coboundary_enumerator.h"

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
	
	cx = (simplex.index) & 0x07ff;
	cy = (simplex.index >> 11) & 0x03ff;
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
			case 0: // y+
				index = (1 << 21) | ((cy) << 11) | (cx);
				birthday = max(birthtime, dcg->dense2[cx  ][cy+1]);
				break;
			case 1: // y-
				index = (1 << 21) | ((cy-1) << 11) | (cx);
				birthday = max(birthtime, dcg->dense2[cx  ][cy-1]);
				break;
			case 2: // x+
				index = (0 << 21) | ((cy) << 11) | (cx);
				birthday = max(birthtime, dcg->dense2[cx+1][cy  ]);
				break;
			case 3: // x-
				index = (0 << 21) | ((cy) << 11) | (cx-1);
				birthday = max(birthtime, dcg->dense2[cx-1][cy  ]);
				break;
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
			if(count == 0){ // upper
				count++;
				index = ((cy) << 11) | cx;
				birthday = max({birthtime, dcg->dense2[cx][cy + 1], dcg->dense2[cx + 1][cy + 1]});
				if (birthday != threshold) {
					nextCoface = BirthdayIndex(birthday, index, 2);
					return true;
				}
			} else 
			if(count == 1){ // lower
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
			if(count == 0){ // right
				count ++;
				index = ((cy) << 11) | cx;
				birthday = max({birthtime, dcg->dense2[cx + 1][cy], dcg->dense2[cx + 1][cy + 1]});
				if (birthday != threshold) {
					nextCoface = BirthdayIndex(birthday, index, 2);
					return true;
				}
			} else 
			if(count == 1){ //left
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
