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

  struct StateBuffer {
    struct SegInfo {
      int startIndex;
      int length;
      double startTime;
      double interval;
    }
    
    int nPoints;
    /* if uniformally spaced data cannot be provided it will be split into segments
       where the points of each segment are uniformally spaced */
    int nSegments;
    vector<SegInfo>
  };
};
