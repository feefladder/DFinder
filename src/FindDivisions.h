
#ifndef _FIND_DIVISIONS_H_
#define _FIND_DIVSIIONS_H_

#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <math.h>

namespace DivisionFinder{
/****
generate a vector of non-prime factors of total. these should be in the cycle.
****/

std::set<int> prime_factors(int n);

std::set<int> non_prime_factors(int n);
/*****
Find all cycles for a given total. Will not get equivalent values: for example 1/5 and 4/5 are equivalent
******/
std::vector<std::vector<int>> find_cycles(int total);

std::vector<int> find_cycle(int start, int total);

std::pair<int,int> find_fold(std::vector<int>& cycle, std::set<int>& to_fold);


}
#endif //_FIND_DIVISIONS_H_