#pragma once

#include <string>
#include <vector>
#include <ghc/fs_std.hpp>

namespace SugarSpice {

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
       * Combine the state positions and velocities into a single vector
       *
       * @return Single vector with {X1, Y1, Z1, dX1, dY1, dZ1, X2, Y2, Z2, dX2, dY2, dZ2, ...}
       */
      std::vector<std::vector<double>> concatStates (std::vector<std::vector<double>> statePositions, std::vector<std::vector<double>> stateVelocities);

      std::string comment;
      int bodyCode;
      int centerOfMotion;
      std::string referenceFrame;
      std::string segmentId;
      int polyDegree;
      std::vector<std::vector<double>> statePositions;
      std::vector<std::vector<double>> stateVelocities;
      std::vector<double> stateTimes;
  };


  class CkSegment {
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
         * @param pointing Time ordered vector of state positions X,Y,Z
         * @param anglularVelocity Time ordered vector of state velocities dX, dY, dZ
         * @param stateTimes Time ordered vector of state ephemeris times (TDB)
         */
        CkSegment(std::vector<std::vector<double>> quats,std::vector<double> times,  int bodyCode, 
                 std::string referenceFrame, std::string segmentId, 
                 std::optional<std::vector<std::vector<double>>> anglularVelocity = std::nullopt, 
                 std::optional<std::string> comment = std::nullopt);
  
  
        std::vector<double> times; 
        std::vector<std::vector<double>> quats; 
        int bodyCode; 
        std::string referenceFrame; 
        std::string segmentId;
        std::optional<std::vector<std::vector<double>>> anglularVelocity = std::nullopt; 
        std::optional<std::string> comment = std::nullopt;
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
                   std::vector<SpkSegment> segments);
  
  
    /**
      * @brief Write CK segments to a file
      *
      * Given orientations, angular velocities, times, target and reference frames, write data as a segment in a CK kernel. 
      *
      * @param path path to file to write the segment to 
      * @param quats nx4 vector of orientations as quaternions 
      * @param times nx1 vector of times matching the number of quats
      * @param bodyCode NAIF body code identifying the orientations belong to 
      * @param referenceFrame NAIF string for the reference frame the orientations are in 
      * @param segmentId Some ID to give the segment 
      * @param angularVelocity optional, nx3 array of angular velocities 
      * @param comment optional, comment to be associated with the segment
      */
    void writeCk(fs::path path, 
                 std::vector<std::vector<double>> quats, 
                 std::vector<double> times, 
                 int bodyCode, 
                 std::string referenceFrame, 
                 std::string segmentId, 
                 std::optional<std::vector<std::vector<double>>> anglularVelocity = std::nullopt, 
                 std::optional<std::string> comment= std::nullopt);
  
  }
