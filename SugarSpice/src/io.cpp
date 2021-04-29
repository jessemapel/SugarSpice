#include <iostream>

#include "SpiceUsr.h"

#include "io.h"


void writeSpk (fs::path fileName, std::string comment, std::vector<spkSegment> segments) {

  // TODO:
  //   write all segments not just first
  //   trap naif errors and do ????
  //   if file exists do something (delete it, error out)
  //   Add convience function to SpkSegment to combine the pos and vel into a single array

  // Combine positions and velocities for spicelib call
  std::vector<std::vector<double>> pos = segments[0].getStatePositions();
  std::vector<std::vector<double>> vel = segments[0].getStateVelocities();
  std::vector<double> times = segments[0].getStateTimes();

  std::vector<double> states;
  for (int i=0; i<segments[0].getSize(); i++) {
    states.push_back(pos[i][0]);
    states.push_back(pos[i][1]);
    states.push_back(pos[i][2]);
    states.push_back(vel[i][0]);
    states.push_back(vel[i][1]);
    states.push_back(vel[i][2]);
  }

  SpiceInt handle;
  spkopn_c(fileName.string().c_str(), "USGS_SPK", 512, &handle);
  std::cout << "w4" << " " << times[0] << " " << times[times.size()-1] << " " <<
segments[0].getSize() << std::endl;

  spkw13_c(handle,
           segments[0].getBodyCode(),
           segments[0].getCenterOfMotion(),
           segments[0].getReferenceFrame().c_str(),
           times[0],
           segments[0].getSize() - 1,
           segments[0].getSegmentId().c_str(),
           segments[0].getPolynomialDegree(),
           times.size(),
           states.data(),
           times.data());

  spkcls_c(handle);

  return;
}


/**
 * Create an SPK segment
 *
 * @param segmentComment
 * @param bodyCode
 * @param centerOfMotion
 * @param referenceFrame
 * @param segmentId
 * @param polyDegree
 * @param statePositions
 * @param stateVelocities
 * @param stateTimes
 */
  spkSegment::spkSegment (std::string segmentComment,
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
