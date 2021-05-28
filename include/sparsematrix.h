#ifndef SPARSEMATRIX_H
#define SPARSEMATRIX_H
#include <map>
#include <vector>
#include <cmath>
#include <iterator>
#include <iostream>

/*

  Template class to store a sparse matrix as a list

  e.g

  [1 2
   0 3]

  ==

  [1,1] = 1
  [1,2] = 2
  [2,2] = 3

  or

  [1,2] = 1
  [3,5] = 4.56
  [10,11] = 2

  all unlisted values are 0

*/
template <class T>
class SparseMatrix
{
public:
  SparseMatrix(int & n, int & m)
  : n(n), m(m)
  {}

  inline bool set_value(std::pair<int,int> index, T value)
  {
    if (index.first < 0 || index.second < 0 || index.first >= this->n || index.second >= this->m)
    {
      return false;
    }
    return this->entries.insert(std::pair<std::pair<int,int>,T>(index,value)).second;
  }

  inline bool set_value(int x, int y, T value)
  {
    return this->set_value(std::pair<int,int>(x,y),value);
  }

  inline T operator[] (std::pair<int,int> index)
  {
    if (index.first < 0 || index.second < 0 || index.first >= this->n || index.second >= this->m)
    {
      return std::nan("1");
    }
    typename std::map<std::pair<int,int>,T>::iterator iter = this->entries.find(index);
    if (iter == this->entries.end()){
      return 0.0;
    }
    else{
      return iter->second;
    }
  }

  inline T operator() (int x, int y)
  {
    return SparseMatrix::operator[](std::pair<int,int>(x,y));
  }

  inline int get_n(){
    return n;
  }

  inline int get_m(){
    return m;
  }

  inline void clear(){
    entries.clear();
  }

  inline std::map<std::pair<int,int>,T> get_non_zero() {return entries;}

  inline int length(){return entries.size();}

private:
  int n;
  int m;
  std::map<std::pair<int,int>,T> entries;
};
#endif
