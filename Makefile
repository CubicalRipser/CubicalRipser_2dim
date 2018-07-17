TARGET = CR2
SRCS = cubicalripser_2dim.cpp dense_cubical_grids.cpp birthday_index.cpp columns_to_reduce.cpp simplex_coboundary_enumerator.cpp union_find.cpp write_pairs.cpp compute_pairs.cpp
OBJS = cubicalripser_2dim.o dense_cubical_grids.o birthday_index.o columns_to_reduce.o simplex_coboundary_enumerator.o union_find.o write_pairs.o compute_pairs.o
 
all: $(TARGET)

$(TARGET): $(OBJS) $(SRCS)
	c++ -std=c++11 -o $@ $(OBJS) 

cubicalripser_2dim.o: cubicalripser_2dim.cpp
	c++ -std=c++11  -c -o $@ $< -Ofast

dense_cubical_grids.o: dense_cubical_grids.cpp
	c++ -std=c++11  -c -o $@ $< -Ofast

birthday_index.o: birthday_index.cpp
	c++ -std=c++11  -c -o $@ $< -Ofast

columns_to_reduce.o: columns_to_reduce.cpp
	c++ -std=c++11  -c -o $@ $< -Ofast

simplex_coboundary_enumerator.o: simplex_coboundary_enumerator.cpp
	c++ -std=c++11  -c -o $@ $< -Ofast

union_find.o: union_find.cpp
	c++ -std=c++11  -c -o $@ $< -Ofast

write_pairs.o: write_pairs.cpp
	c++ -std=c++11  -c -o $@ $< -Ofast

compute_pairs.o: compute_pairs.cpp
	c++ -std=c++11  -c -o $@ $< -Ofast
