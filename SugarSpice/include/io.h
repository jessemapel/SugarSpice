#pragma once

#include <string>
#include <vector>
#include <ghc/fs_std.hpp>

class spkSegment {
  public:

    /**
     * Constructs a fully populated spkSegment
     *
     * @param segmentComment The comment string for the new segment
     * @param bodyCode Naif body code of an object whose state is described by the segments
     * @param centerOfMotion Naif body code of an object which is the center of motion for
     *                      bodyCode
     * @param referenceFrame Naif name of the reference system relative to which the state is
     * @param segmentId SPK segment identifier (max size 40)
     * @param degree Degree of the Hermite polynomials used to interpolate the states
     * @param statePositions Time ordered vector of state positions X,Y,Z
     * @param stateVelocities Time ordered vector of state velocities dX, dY, dZ
     * @param stateTimes Time ordered vector of state ephemeris times (TDB)
     */
    spkSegment(std::string segmentComment,
               int bodyCode,
               int centerOfMotion,
               std::string referenceFrame,
               std::string id, int degree,
               std::vector<std::vector<double>> statePositions,
               std::vector<std::vector<double>> stateVelocities,
               std::vector<double> stateTimes);


    /**
     *  Returns the comment string for this segment
     *
     * @return std::string
     */
    std::string getComment() {
      return m_comment;
    }


    /**
     * Returns the NAIF body code for this segment
     *
     * @return int
     */
    int getBodyCode() {
      return m_bodyCode;
    }


    /**
     * Return the NAIF center of motion body code for this segment
     *
     * @return int
     */
    int getCenterOfMotion() {
      return m_centerOfMotion;
    }


    std::string getReferenceFrame() {
      return m_referenceFrame;
    }


    std::string getSegmentId() {
      return m_segmentId;
    }


    int getPolynomialDegree() {
      return m_polyDegree;
    }


    std::vector<std::vector<double>> getStatePositions() {
      return m_statePositions;
    }


    std::vector<std::vector<double>> getStateVelocities() {
      return m_stateVelocities;
    }


    std::vector<double> getStateTimes() {
      return m_stateTimes;
    }


    double getStartTime() {
      return m_stateTimes[0];
    }


    double getEndTime() {
      return m_stateTimes[getSize()-1];
    }


    int getSize() {
      return m_stateTimes.size();
    }


  private:
    std::string m_comment;
    int m_bodyCode;
    int m_centerOfMotion;
    std::string m_referenceFrame;
    std::string m_segmentId;
    int m_polyDegree;
    std::vector<std::vector<double>> m_statePositions;
    std::vector<std::vector<double>> m_stateVelocities;
    std::vector<double> m_stateTimes;
  };


/**
  * @brief Write SPK segments to a file
  *
  * Given a vector of SPK segments, write them to the requested SPK file.
  *
  * @param Full file specification to have the SPK segments written to
  * @param Vector of spkSegments to be written
  */
void writeSpk (fs::path fileName,
               std::string comment,
               std::vector<spkSegment> segments);

