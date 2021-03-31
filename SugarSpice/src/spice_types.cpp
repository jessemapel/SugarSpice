/**
  *
  *
  *
 **/

#include "spice_types.h"

using namespace std; 

/**
  * Used here to do reverse lookups of enum stringss
  *
 **/
template < typename T> pair<bool, int > findInVector(const std::vector<T>  & vecOfElements, const T  & element) {
    pair<bool, int > result;
    auto it = find(vecOfElements.begin(), vecOfElements.end(), element);
    if (it != vecOfElements.end()) {
        result.second = distance(vecOfElements.begin(), it);
        result.first = true;
    }
    else {
        result.first = false;
        result.second = -1;
    }
    return result;
}


const static std::vector<std::string> Kernel::Types =  {"ck", "spk", "tspk", 
                                                        "lsk", "mk", "sclk", 
                                                        "iak", "ik", "fk", 
                                                        "dsk", "pck", "ek"};

const static std::vector<std::string> Kernel::Qualities = {"predicted",
                                                           "nadir", 
                                                           "reconstructed", 
                                                           "smithed"};


string Kernel::translateType(Type type) { 
    return Kernel::Qualities[type];
}

Type Kernel::translateType(string type) {
    auto res = findInVector(Kernel::Types); 
};

string Kernel::translateQuality(Quality qa);
Quality Kernel::translateQuality(string qa);



Kernel::Kernel(fs::path path) { 
    this->path = path; 
    furnsh_c(path.c_str());
}


Kernel::~Kernel() {
    unload_c(this->path.c_str());
}