#pragma once
/** 
 * @file
 * 
 * Functions for reading and writing Kernels
 *
 **/

#include <string>
#include <vector>
#include <ghc/fs_std.hpp>

#include <nlohmann/json.hpp>

namespace SugarSpice {

  class SpkSegment {
    public:

      /**
       * Constructs a fully populated SpkSegment
       *
       * @param statePositions Time ordered vector of state positions X,Y,Z
       * @param stateTimes Time ordered vector of state ephemeris times (TDB)
       * @param bodyCode Naif body code of an object whose state is described by the segments
       * @param centerOfMotion Naif body code of an object which is the center of motion for
       *                      bodyCode
       * @param referenceFrame Naif name of the reference system relative to which the state is
       * @param id SPK segment identifier (max size 40)
       * @param degree Degree of the Hermite polynomials used to interpolate the states
       * @param stateVelocities Time ordered vector of state velocities dX, dY, dZ
       * @param segmentComment The comment string for the new segment
      **/
      SpkSegment(std::vector<std::vector<double>> statePositions,
                 std::vector<double> stateTimes,
                 int bodyCode,
                 int centerOfMotion,
                 std::string referenceFrame,
                 std::string id, int degree,
                 std::optional<std::vector<std::vector<double>>> stateVelocities,
                 std::optional<std::string> segmentComment);

      /**
       * Combine the state positions and velocities into a single vector
       *
       * @return Single vector with {X1, Y1, Z1, dX1, dY1, dZ1, X2, Y2, Z2, dX2, dY2, dZ2, ...}
       */
      static std::vector<std::vector<double>> concatStates (std::vector<std::vector<double>> statePositions, std::vector<std::vector<double>> stateVelocities);

      std::vector<double> stateTimes;
      int bodyCode;
      int centerOfMotion;
      std::string referenceFrame;
      std::string id;
      int polyDegree;
      std::vector<std::vector<double>> statePositions;
      std::optional<std::vector<std::vector<double>>> stateVelocities;
      std::optional<std::string> comment;
  };


  class CkSegment {
      public:
  
        /**
         * Constructs a fully populated SpkSegment
         * @param quats Time ordered vector of orientations as quaternions
         * @param times times for the CK segment in ascending order
         * @param bodyCode Naif body code of an object whose state is described by the segments
         * @param referenceFrame Naif name of the reference system relative to which the state is
         * @param id SPK segment identifier (max size 40)
         * @param anglularVelocities Time ordered vector of state velocities dX, dY, dZ
         * @param comment The comment string for the new segment
         */
        CkSegment(std::vector<std::vector<double>> quats, std::vector<double> times,  int bodyCode, 
                  std::string referenceFrame, std::string id, 
                  std::optional<std::vector<std::vector<double>>> anglularVelocities = std::nullopt, 
                  std::optional<std::string> comment = std::nullopt);
  
        std::vector<double> times; 
        std::vector<std::vector<double>> quats; 
        int bodyCode; 
        std::string referenceFrame; 
        std::string id; 
        std::optional<std::vector<std::vector<double>>> angularVelocities = std::nullopt; 
        std::optional<std::string> comment = std::nullopt; 
    };
  
  
    /**
      * @brief Write SPK segments to a file
      *
      * Given a vector of SPK segments, write them to the requested SPK file.
      *
      * @param fileName file specification to have the SPK segments written to
      * @param segments spkSegments to be written
      */
    void writeSpk (fs::path fileName,
                   std::vector<SpkSegment> segments);


    /**
     * @brief Write SPK to path
     * 
     * @param fileName full path to file to write the segment to  
     * @param statePositions Nx3 array of positions in X,Y,Z order
     * @param stateTimes Nx1 array of times
     * @param bodyCode NAIF integer code for the body the states belong to
     * @param centerOfMotion is the NAIF integer code for the center of motion of the object identified by body.
     * @param referenceFrame The NAIF code the states are relative to
     * @param segmentId ID for the segment
     * @param polyDegree degree of the hermite polynomials used for interpolation
     * @param stateVelocities Nx3 array of state velocities in VX, VY, VZ order, optional
     * @param segmentComment Comment associated with the segment, optional
     */
    void writeSpk(fs::path fileName, 
                  std::vector<std::vector<double>> statePositions,
                  std::vector<double> stateTimes,
                  int bodyCode,
                  int centerOfMotion,
                  std::string referenceFrame,
                  std::string segmentId, 
                  int polyDegree,
                  std::optional<std::vector<std::vector<double>>> stateVelocities = std::nullopt,
                  std::optional<std::string> segmentComment = std::nullopt);


    /**
      * @brief Write CK segments to a file
      *
      * Given orientations, angular velocities, times, target and reference frames, write data as a segment in a CK kernel. 
      *
      * @param fileName path to file to write the segment to 
      * @param quats nx4 vector of orientations as quaternions 
      * @param times nx1 vector of times matching the number of quats
      * @param bodyCode NAIF body code identifying the orientations belong to 
      * @param referenceFrame NAIF string for the reference frame the orientations are in 
      * @param segmentId Some ID to give the segment 
      * @param angularVelocity optional, nx3 array of angular velocities 
      * @param comment optional, comment to be associated with the segment
      */
    void writeCk(fs::path fileName, 
                 std::vector<std::vector<double>> quats, 
                 std::vector<double> times, 
                 int bodyCode, 
                 std::string referenceFrame, 
                 std::string segmentId, 
                 fs::path sclk, 
                 fs::path lsk,
                 std::optional<std::vector<std::vector<double>>> angularVelocity = std::nullopt, 
                 std::optional<std::string> comment= std::nullopt);
  
  
    /**
     * @brief Write CK segments to a file
     *
     * Given orientations, angular velocities, times, target and reference frames, write data as a segment in a CK kernel. 
     *
     * @param fileName path to file to write the segment to 
     * @param sclk path to SCLK kernel matching the segments' frame code
     * @param lsk path to LSK kernel 
     * @param segments spkSegments to be writte
     */
  void writeCk(fs::path fileName, 
               fs::path sclk, 
               fs::path lsk,
               std::vector<CkSegment> segments);


  /**
   * @brief Write json key value pairs into a NAIF text kernel
   *          
   * @param fileName pull path to the text kernel
   * @param type kernel type string, valid text kernel types: FK, IK, LSK, MK, PCK, SCLK
   * @param comment the comment to add to the top of the kernel 
   * @param keywords json object containing key/value pairs to write to the text kernel
   */
  void writeTextKernel(fs::path fileName, std::string type, nlohmann::json &keywords, std::optional<std::string> comment = std::nullopt);
  
  }
