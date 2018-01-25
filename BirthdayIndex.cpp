//BirthdayIndex.cpp

#include <iostream>

#include "BirthdayIndex.h"


// double birthday;
// int index;
// int dim;

BirthdayIndex::BirthdayIndex(){
	birthday = 0;
	index = -1;
	dim = 1;
}

BirthdayIndex::BirthdayIndex(const BirthdayIndex& b){
	birthday = b.birthday;
	index = b.index;
	dim = b.dim;
}

BirthdayIndex::BirthdayIndex(double _b, int _i, int _d){
	birthday = _b;
	index = _i;
	dim = _d;
}

void BirthdayIndex::copyBirthdayIndex(BirthdayIndex v){
	birthday = v.birthday;
	index = v.index;
	dim = v.dim;
}

double BirthdayIndex::getBirthday(){
	return birthday;
}

long BirthdayIndex::getIndex(){
	return index;
}

int BirthdayIndex::getDimension(){
	return dim;
}

void BirthdayIndex::print(){
	std::cout << "(dob:" << birthday << "," << index << ")" << std::endl;
}

bool BirthdayIndexComparator::operator()(const BirthdayIndex& o1, const BirthdayIndex& o2) const{
	if(o1.birthday == o2.birthday){
		if(o1.index < o2.index){
			//return 1;
			return true;
		} else{
			return false;
		}
	} else {
		if(o1.birthday > o2.birthday){
			return true;
		} else {
			return false;
		}
	}
}

bool BirthdayIndexInverseComparator::operator()(const BirthdayIndex& o1, const BirthdayIndex& o2) const{
	if(o1.birthday == o2.birthday){
		if(o1.index < o2.index){
			return false;
		} else {
			return true;
		}
	} else {
		if(o1.birthday > o2.birthday){
			return false;
		} else {
			return true;
		}
	}
}
