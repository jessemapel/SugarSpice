#include <iostream>
#include <optional>

#include "SpiceUsr.h"

#include "io.h"

using namespace std; 

namespace SugarSpice {

  SpkSegment::SpkSegment (vector<vector<double>> statePositions,
                          vector<double> stateTimes,
                          int bodyCode,
                          int centerOfMotion,
                          string referenceFrame,
                          string id, int degree,
                          optional<vector<vector<double>>> stateVelocities,
                          optional<string> comment) {

    this->comment         = comment;
    this->bodyCode        = bodyCode;
    this->centerOfMotion  = centerOfMotion;
    this->referenceFrame  = referenceFrame;
    this->id              = id;
    this->polyDegree      = degree;
    this->statePositions  = statePositions;
    this->stateVelocities = stateVelocities;
    this->stateTimes      = stateTimes;
    return;
  }


  vector<vector<double>> concatStates (vector<vector<double>> statePositions, vector<vector<double>> stateVelocities) {

    if (statePositions.size() != stateVelocities.size()) {
      throw invalid_argument("Both statePositions and stateVelocities need to match in size.");
    }

    vector<vector<double>> states;
    for (int i=0; i<statePositions.size(); i++) {
      states.push_back({statePositions[i][0],
                        statePositions[i][1],
                        statePositions[i][2],
                        stateVelocities[i][0],
                        stateVelocities[i][1],
                        stateVelocities[i][2]});
    }
    return states;
  }


  CkSegment::CkSegment(vector<vector<double>> quats, 
                       vector<double> times, 
                       int bodyCode, 
                       string referenceFrame, 
                       string segmentId,
                       optional<vector<vector<double>>> anglularVelocities, 
                       optional<string> comment) {
      
    this->comment            = comment;
    this->bodyCode           = bodyCode;
    this->referenceFrame     = referenceFrame;
    this->id                 = segmentId;
    this->angularVelocities = anglularVelocities;
    this->comment            = comment;
  }


  void writeCk(fs::path path, 
               vector<vector<double>> quats, 
               vector<double> times, 
               int bodyCode, 
               string referenceFrame, 
               string segmentId, 
               optional<vector<vector<double>>> angularVelocities, 
               optional<string> comment) {

    SpiceInt handle; 

    ckopn_c(path.c_str(), "CK", comment.value_or("CK Kernel").size(), &handle);

    ckw03_c (handle, 
             times.at(0),
             times.at(times.size()-1),
             bodyCode,
             referenceFrame.c_str(),
             (bool)angularVelocities,
             segmentId.c_str(), 
             times.size(),
             times.data(),
             quats.data(),
             (angularVelocities) ? angularVelocities->data() : nullptr,
             times.size(),
             times.data());
  }


  void writeSpk(fs::path fileName, 
                 vector<vector<double>> statePositions,
                 vector<double> stateTimes,
                 int bodyCode,
                 int centerOfMotion,
                 string referenceFrame,
                 string segmentId, 
                 int polyDegree,
                 optional<vector<vector<double>>> stateVelocities,
                 optional<string> segmentComment) {

    vector<vector<double>> states;

    if (!stateVelocities) {
      // init a 0 velocity array 
      vector<vector<double>> velocities;
      for (int i = 0; i < statePositions.size(); i++) { 
        velocities.push_back({0,0,0});
      }
      stateVelocities = velocities;
    }
    
    states = concatStates(statePositions, *stateVelocities);

    vector<double*> ptrs;
    for (auto& vec : states) {
        ptrs.push_back(vec.data());
    }
    double **statePtrs = ptrs.data();

    SpiceInt handle;
    spkopn_c(fileName.string().c_str(), "SPK", 512, &handle);

    spkw13_c(handle,
             bodyCode,
             centerOfMotion,
             referenceFrame.c_str(),
             stateTimes[0],
             stateTimes.size() - 1,
             segmentId.c_str(),
             polyDegree,
             stateTimes.size(),
             statePtrs,
             stateTimes.data());

    spkcls_c(handle);

    return;
  }


  void writeSpk(fs::path fileName, vector<SpkSegment> segments) {

    // TODO:
    //   write all segments not just first
    //   trap naif errors and do ????
    //   if file exists do something (delete it, error out)
    //   Add convience function to SpkSegment to combine the pos and vel into a single array

    // Combine positions and velocities for spicelib call
    
    writeSpk(fileName,
             segments[0].statePositions,
             segments[0].stateTimes, 
             segments[0].bodyCode,
             segments[0].centerOfMotion,
             segments[0].referenceFrame,
             segments[0].id,
             segments[0].polyDegree,
             segments[0].stateVelocities,
             segments[0].comment);

    return;
  }

  void writeCk(fs::path fileName, vector<CkSegment> segments) {

    // TODO:
    //   write all segments not just first
    //   trap naif errors and do ????
    //   if file exists do something (delete it, error out)
    //   Add convience function to SpkSegment to combine the pos and vel into a single array

    // Combine positions and velocities for spicelib call
    
    writeCk(fileName,
            segments[0].quats,
            segments[0].times, 
            segments[0].bodyCode,
            segments[0].referenceFrame,
            segments[0].id,
            segments[0].angularVelocities,
            segments[0].comment);

  }


}
