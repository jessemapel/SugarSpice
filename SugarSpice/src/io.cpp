#include <iostream>

#include "SpiceUsr.h"

#include "io.h"

namespace SugarSpice {

  void writeSpk (fs::path fileName, std::string comment, std::vector<SpkSegment> segments) {

    // TODO:
    //   write all segments not just first
    //   trap naif errors and do ????
    //   if file exists do something (delete it, error out)
    //   Add convience function to SpkSegment to combine the pos and vel into a single array

    // Combine positions and velocities for spicelib call
    std::vector<std::vector<double>> pos = segments[0].getStatePositions();
    std::vector<std::vector<double>> vel = segments[0].getStateVelocities();
    std::vector<double> times = segments[0].getStateTimes();

    std::vector<double> states(segments[0].concatStates());

    SpiceInt handle;
    spkopn_c(fileName.string().c_str(), "SPK", 512, &handle);

    spkw13_c(handle,
             segments[0].getBodyCode(),
             segments[0].getCenterOfMotion(),
             segments[0].getReferenceFrame().c_str(),
             times[0],
             segments[0].getSize() - 1,
             segments[0].getSegmentId().c_str(),
             segments[0].getPolynomialDegree(),
             segments[0].getSize(),
             states.data(),
             times.data());

    spkcls_c(handle);

    return;
  }


  SpkSegment::SpkSegment (std::string segmentComment,
                          int bodyCode,
                          int centerOfMotion,
                          std::string referenceFrame,
                          std::string segmentId, int polyDegree,
                          std::vector<std::vector<double>> statePositions,
                          std::vector<std::vector<double>> stateVelocities,
                          std::vector<double> stateTimes) {

    m_comment = segmentComment;
    m_bodyCode = bodyCode;
    m_centerOfMotion = centerOfMotion;
    m_referenceFrame = referenceFrame;
    m_segmentId = segmentId;
    m_polyDegree = polyDegree;
    m_statePositions = statePositions;
    m_stateVelocities = stateVelocities;
    m_stateTimes = stateTimes;

    return;

  }


  std::vector<double> SpkSegment::concatStates () const {

    std::vector<double> states;
    for (int i=0; i<getSize(); i++) {
      states.push_back(m_statePositions[i][0]);
      states.push_back(m_statePositions[i][1]);
      states.push_back(m_statePositions[i][2]);
      states.push_back(m_stateVelocities[i][0]);
      states.push_back(m_stateVelocities[i][1]);
      states.push_back(m_stateVelocities[i][2]);
    }

    return states;
  }
}
