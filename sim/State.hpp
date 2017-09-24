// ROOT/sim/State.hpp
#pragma once

#include <vector>

#include <glm/vec3.hpp>

/* represent position and velocity of objects
*/
namespace sim {
  struct State {
    glm::dvec3 pos;
    glm::dvec3 vel;
  };

  /* Transparent data structure containing an array of points along an objects
     trajectory which will be used to calculate its position
  */
  struct StateBuffer {
    struct SegInfo {
      int startIndex;
      int length;
      double startTime;
      double interval;
    };
    
    int nPoints;
    /* if uniformally spaced data cannot be provided it will be split into segments
       where the points of each segment are uniformally spaced */
    int nSegments;
    /* describe each segment */
    std::vector<SegInfo> segments;
    State* points;

    StateBuffer() {}
    StateBuffer(int n); // initialise with nPoint = n
  };
};
