#include <iostream>
#include <optional>

#include "SpiceUsr.h"

#include "io.h"

using namespace std; 

namespace SugarSpice {

  SpkSegment::SpkSegment (string segmentComment,
                          int bodyCode,
                          int centerOfMotion,
                          string referenceFrame,
                          string segmentId, int polyDegree,
                          vector<vector<double>> statePositions,
                          vector<vector<double>> stateVelocities,
                          vector<double> stateTimes) {

    this->comment         = segmentComment;
    this->bodyCode        = bodyCode;
    this->centerOfMotion  = centerOfMotion;
    this->referenceFrame  = referenceFrame;
    this->segmentId       = segmentId;
    this->polyDegree      = polyDegree;
    this->statePositions  = statePositions;
    this->stateVelocities = stateVelocities;
    this->stateTimes      = stateTimes;
    return;
  }


  vector<double> SpkSegment::concatStates (vector<vector<double>> statePositions, vector<vector<double>> stateVelocities) const {

    vector<double> states;
    for (int i=0; i<getSize(); i++) {
      states.push_back(statePositions[i][0]);
      states.push_back(statePositions[i][1]);
      states.push_back(statePositions[i][2]);
      states.push_back(stateVelocities[i][0]);
      states.push_back(stateVelocities[i][1]);
      states.push_back(stateVelocities[i][2]);
    }

    return states;
  }


  CkSegment::CkSegment(vector<vector<double>> quats, 
                       vector<double> times, 
                       int bodyCode, 
                       string referenceFrame, 
                       string segmentId,
                       optional<vector<vector<double>>> anglularVelocity = nullopt, 
                       optional<string> comment = nullopt) {
      
    this->comment          = comment;
    this->bodyCode         = bodyCode;
    this->referenceFrame   = referenceFrame;
    this->segmentId        = segmentId;
    this->anglularVelocity = anglularVelocity;
    this->comment          = comment;
  }


  void writeCk(fs::path path, 
               vector<vector<double>> quats, 
               vector<double> times, 
               int bodyCode, 
               string referenceFrame, 
               string segmentId, 
               optional<vector<vector<double>>> anglularVelocity, 
               optional<string> comment) {
    SpiceInt handle; 
    SpiceBoolean avflag = (bool)anglularVelocity;
    SpiceDouble **quatPtrs;
    SpiceDouble **avPtrs;
    
    vector<double*> ptrs;
    for (auto& vec : quats) {
        ptrs.push_back(vec.data());
    }
    quatPtrs = ptrs.data();

    ptrs.clear();
    if (anglularVelocity) {
      for (auto& vec : quats) {
        ptrs.push_back(vec.data());
      }
      avPtrs = ptrs.data();
    }
    else {
      avPtrs = nullptr;
    }

    ckopn_c(path.c_str(), "CK", comment.value_or("CK Kernel").size(), &handle);

    ckw03_c (handle, 
             times.at(0),
             times.at(times.size()-1),
             bodyCode,
             referenceFrame.c_str(),
             avflag,
             segmentId.c_str(), 
             times.size(),
             times.data(),
             quatPtrs,
             avPtrs,
             times.size(),
             times.data());
  }


  void writeSpk(fs::path fileName, string comment, vector<SpkSegment> segments) {

    // TODO:
    //   write all segments not just first
    //   trap naif errors and do ????
    //   if file exists do something (delete it, error out)
    //   Add convience function to SpkSegment to combine the pos and vel into a single array

    // Combine positions and velocities for spicelib call
    vector<vector<double>> pos = segments[0].getStatePositions();
    vector<vector<double>> vel = segments[0].getStateVelocities();
    vector<double> times = segments[0].getStateTimes();

    writeSpk(fileName,
             segments[0].statePositions,
             segments[0].getStateTimes, 
             segments[0].getBodyCode,
             segments[0].getCenterOfMotion,
             segments[0].getReferenceFrame,
             segments[0].getSegmentId,
             segments[0].getPolynomialDegree,
             segments[0].getStateVelocities,
             times.data;

    return;
  }


  void writeSpk(fs::path fileName, 
                 vector<vector<double>> statePositions,
                 vector<double> stateTimes
                 int bodyCode,
                 int centerOfMotion,
                 string referenceFrame,
                 string segmentId, 
                 int polyDegree,
                 optional<vector<vector<double>>> stateVelocities,
                 optional<string> segmentComment) {

    vector<double> states = concatStates(statePositions, stateVelocities);

    SpiceInt handle;
    spkopn_c(fileName.string().c_str(), "SPK", 512, &handle);

    spkw13_c(handle,
             bodyCode,
             centerOfMotion,
             referenceFrame,
             stateTimes[0],
             stateTimes.getSize() - 1,
             segmentId.c_str(),
             polyDegree,
             stateTimes[0].getSize(),
             states.data(),
             times.data());

    spkcls_c(handle);

    return;
  }

}
