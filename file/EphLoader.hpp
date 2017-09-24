// ROOT/file/EphLoader.hpp
#pragma once
#include <string>

#include <glm/vec3.hpp>

#include "sim/State.hpp"

namespace file {

/* Abstract class for loading ephemeris (position and velocity) data
   from a file into an array that is passed to the program.
   NOTE: the actual points loaded into the buffer may not match the points
   requested with load_state. It is up to the calling program to see
   which points were actually loaded
*/
class EphLoader {
public:
  /* load nPoints position and velocity coords at intervals of timeStep
     IN: 
     OUT: stateArr - the state array filled with pos and vel points
          realStart - the actual time for the first point (since it may not be possible to get the start time
                      exactly
          realInterval - the actual interval of the points
     RETURNS: true if (start == realStart and timeStep == realInterval)
   */
  virtual bool load_state(std::string target, std::string rel, double start, double timeStep, int nPoints,
                   sim::State* stateArr, double* realStart, double* realInterval) = 0;

  /* load nPoints position and velocity coords, where the time interval
     is (end-start)/nPoints
     IN: 
     OUT: stateArr - the state array filled with pos and vel points
          realStart - the actual time for the first point (since it may not be possible to get the start time
                      exactly
          realInterval - the actual interval of the points
     RETURNS: true if (start == realStart and timeStep == realInterval)
   */
  virtual bool load_state_range(std::string target, std::string rel, double start, double end, int nPoints,
                   sim::State* stateArr, double* realStart, double* realInterval) = 0;
};

};
