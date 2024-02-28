#pragma once

#include <string>

#include <hpp/fcl/collision.h>

namespace mplib::collision_detection {

/// Result of the collision checking.
template <typename S>
struct WorldCollisionResultTpl {
  // TODO: Update with
  // https://moveit.picknik.ai/main/api/html/structcollision__detection_1_1CollisionResult.html

  ::hpp::fcl::CollisionResult res;  ///< the fcl CollisionResult
  std::string collision_type,     ///< type of the collision
      object_name1,               ///< name of the first object
      object_name2,               ///< name of the second object
      link_name1,                 ///< link name of the first object in collision
      link_name2;                 ///< link name of the second object in collision
};

}  // namespace mplib::collision_detection
