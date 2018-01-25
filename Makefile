TARGET = CRR
SRCS = CubicalRipser_2dim.cpp DenseCubicalGrids.cpp BirthdayIndex.cpp ColumnsToReduce.cpp SimplexCoboundaryEnumerator.cpp UnionFind.cpp WritePairs.cpp ComputePairs.cpp
OBJS = CubicalRipser_2dim.o DenseCubicalGrids.o BirthdayIndex.o ColumnsToReduce.o SimplexCoboundaryEnumerator.o UnionFind.o WritePairs.o ComputePairs.o
 
all: $(TARGET)

$(TARGET): $(OBJS) $(SRCS)
	c++ -std=c++11 -o $@ $(OBJS) 

CubicalRipser_2dim.o: CubicalRipser_2dim.cpp
	c++ -std=c++11  -c -o $@ $< -Ofast

DenseCubicalGrids.o: DenseCubicalGrids.cpp
	c++ -std=c++11  -c -o $@ $< -Ofast

BirthdayIndex.o: BirthdayIndex.cpp
	c++ -std=c++11  -c -o $@ $< -Ofast

ColumnsToReduce.o: ColumnsToReduce.cpp
	c++ -std=c++11  -c -o $@ $< -Ofast

SimplexCoboundaryEnumerator.o: SimplexCoboundaryEnumerator.cpp
	c++ -std=c++11  -c -o $@ $< -Ofast

UnionFind.o: UnionFind.cpp
	c++ -std=c++11  -c -o $@ $< -Ofast

WritePairs.o: WritePairs.cpp
	c++ -std=c++11  -c -o $@ $< -Ofast

ComputePairs.o: ComputePairs.cpp
	c++ -std=c++11  -c -o $@ $< -Ofast
