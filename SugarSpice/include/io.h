#pragma once

#include <string>
#include <vector>
#include <ghc/fs_std.hpp>

class SpkSegment {
  public:

    /**
     * Constructs a fully populated SpkSegment
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
    SpkSegment(std::string segmentComment,
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
    std::string getComment() const {
      return m_comment;
    }


    /**
     * Returns the NAIF body code for this segment
     *
     * @return int
     */
    int getBodyCode() const {
      return m_bodyCode;
    }


    /**
     * Return the NAIF center of motion body code for this segment
     *
     * @return int
     */
    int getCenterOfMotion() const {
      return m_centerOfMotion;
    }


    std::string getReferenceFrame() const {
      return m_referenceFrame;
    }


    std::string getSegmentId() const {
      return m_segmentId;
    }


    int getPolynomialDegree() const {
      return m_polyDegree;
    }


    std::vector<std::vector<double>> getStatePositions() const {
      return m_statePositions;
    }


    std::vector<std::vector<double>> getStateVelocities() const {
      return m_stateVelocities;
    }


    std::vector<double> getStateTimes() const {
      return m_stateTimes;
    }


    double getStartTime() const {
      return m_stateTimes[0];
    }


    double getEndTime() const {
      return m_stateTimes[getSize()-1];
    }


    int getSize() const {
      return m_stateTimes.size();
    }


    /**
     * Combine the state positions and velocities into a single vector
     *
     * @return Single vector with {X1, Y1, Z1, dX1, dY1, dZ1, X2, Y2, Z2, dX2, dY2, dZ2, ...}
     */
    std::vector<double> concatStates () const;


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
                   std::vector<SpkSegment> segments);


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



