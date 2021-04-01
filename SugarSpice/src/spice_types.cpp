/**
  * @file
  *
  *
 **/

#include <fmt/format.h>
#include <SpiceUsr.h>

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


const std::vector<std::string> Kernel::TYPES =  {"ck", "spk", "tspk", 
                                           "lsk", "mk", "sclk", 
                                           "iak", "ik", "fk", 
                                           "dsk", "pck", "ek"};

const std::vector<std::string> Kernel::QUALITIES = {"predicted",
                                                     "nadir", 
                                                     "reconstructed", 
                                                     "smithed"};


string Kernel::translateType(Kernel::Type type) { 
    return Kernel::TYPES[type];
}


Kernel::Type Kernel::translateType(string type) {
    auto res = findInVector<string>(Kernel::TYPES, type);
    if (res.first) {
        return static_cast<Kernel::Type>(res.second); 
    }

    throw invalid_argument(fmt::format("{} is not a valid kernel type", type));
};


string Kernel::translateQuality(Kernel::Quality qa) {
    return Kernel::QUALITIES[qa];
}


Kernel::Quality Kernel::translateQuality(string qa) {
    auto res = findInVector<string>(Kernel::QUALITIES, qa);
    if (res.first) {
        return static_cast<Kernel::Quality>(res.second); 
    }

    throw invalid_argument(fmt::format("{} is not a valid kernel type", qa)); 
}


Kernel::Kernel(fs::path path) { 
    this->path = path; 
    furnsh_c(path.c_str());
}


Kernel::~Kernel() {
    unload_c(this->path.c_str());
}