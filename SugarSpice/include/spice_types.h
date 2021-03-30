/** 
  *
  *
 **/



enum KernelType {
  CK=0, SPK, TSPK, 
  LSK, MK, SCLK,
  IAK, IK, FK, 
  DSK, PCK, EK 
}; 

// enum KernelType {
//     SPK = 0,
//     CK,
//     PCK,
//     DSK,
//     EK,
//     META,
//     TEXT // this can be LSK, IK, IAK, FK 
// }

std::vector<std::string> KernelTypes = {"ck", "spk", "tspk", 
                                        "lsk", "mk", "sclk", 
                                        "iak", "ik", "fk", 
                                        "dsk", "pck", "ek", 
                                        "text"};

enum KernelQuality  { 
    Predicted = 0, // Based on predicted location of instrument     
    Nadir,         // Assumes Nadir pointing   
    Reconstructed, // 
    Smithed       
};

std::vector<std::string> KernelQualities = {"predicted", "nadir", "reconstructed", "smithed"};

