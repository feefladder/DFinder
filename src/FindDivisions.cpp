#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <math.h>

namespace DivisionFinder{
/****
generate a vector of non-prime factors of total. these should be in the cycle.
****/

std::set<int> prime_factors(int n){
    std::set<int> pfact;
    while(n%2==0){pfact.insert(2);n=n/2;}
    for (int i = 3; i <= n; i = i+2){
        while (n%i == 0){
            pfact.insert(i);
            n = n/i;
        }
    }
    return pfact;
}

std::set<int> non_prime_factors(int n){
    std::set<int> pfact = prime_factors(n);
    std::set<int> nfact;
    for (int i=1;i<= n; i++){
        if(std::all_of(pfact.begin(),pfact.end(),[i](int p){return i%p!=0;})){
            nfact.insert(i);
            nfact.insert(n-i);
        }
    }
    return nfact;
}

/*****
Find all cycles for a given total. Will not get equivalent values: for example 1/5 and 4/5 are equivalent
******/
std::vector<std::vector<int>> find_cycles(int total){
    std::vector<std::vector<int>> cycles;
    std::set<int> to_check = non_prime_factors(total);
    
    int current = 1;

    while (!to_check.empty()){
        std::vector<int> cycle = {current};
        while (to_check.erase(current) && to_check.erase(total-current)){
            current = (current % 2 == 0) ? current/2 : (total+current)/2;
            cycle.push_back(current);
        }
        cycles.push_back(cycle);
        current = *to_check.begin();
    }
    std::cout << "Found " << cycles.size() << "cycles!\n";
    for(auto c:cycles){
        std::cout<< "Cycle of length " << c.size() << ": ";
        for(auto i:c){std::cout<<i<<" ,";}
        std::cout<<std::endl;
    }
    return cycles;
}

std::vector<int> find_cycle(int start, int total){
    int current = (start%2==0)?start/2:(total+start)/2;
    std::vector<int> cycle = {start};
    while (current!=start){
        cycle.push_back(current);
        current = (current%2==0)?current/2:(total+current)/2;
    }
    return cycle;
}

std::pair<int,int> find_fold(std::vector<int>& folded, std::set<int>& to_fold){
    for(size_t i=0;i<folded.size();i++){
        for(size_t j=folded.size();j>i;j--){
            if((folded[i]+folded[j])%2==0 && folded[i]!=folded[j]){
                if(to_fold.find((folded[i]+folded[j])/2) != to_fold.end()){
                    return std::pair<int,int>(folded[i],folded[j]);
                }
            } else {
            }
        }
    }
    //Should not reach here: that means there is no possible fold
    std::cerr<< "big oops!, this should not happen! Could not find a fold!";
    for(auto i:folded){std::cerr<<i<<", ";}
    throw;
}
} //namespace