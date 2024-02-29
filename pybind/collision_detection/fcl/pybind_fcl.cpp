#include <memory>
#include <vector>

#include <hpp/fcl/collision.h>
#include <hpp/fcl/distance.h>
#include <hpp/fcl/BVH/BVH_model.h>
#include <pybind11/eigen.h>
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "docstring/collision_detection/fcl/fcl.h"
#include "mplib/types.h"
#include "mplib/utils/conversion.h"
#include "pybind_macros.hpp"

namespace py = pybind11;

namespace mplib::collision_detection::fcl {

// Namespace alias
namespace fcl = ::hpp::fcl;

using Triangle = fcl::Triangle;
using CollisionGeometry = fcl::CollisionGeometry;
using Box = fcl::Box;
using Capsule = fcl::Capsule;
using Cone = fcl::Cone;
using Convex = fcl::Convex<fcl::Triangle>;
using Cylinder = fcl::Cylinder;
using Plane = fcl::Plane;
using Sphere = fcl::Sphere;
using BVHModel_OBBRSS = fcl::BVHModel<fcl::OBBRSS>;
using OcTree = fcl::OcTree;

using CollisionObject = fcl::CollisionObject;
using CollisionRequest = fcl::CollisionRequest;
using CollisionResult = fcl::CollisionResult;
using DistanceRequest = fcl::DistanceRequest;
using DistanceResult = fcl::DistanceResult;
using Contact = fcl::Contact;
using CollisionRequestFlag = fcl::CollisionRequestFlag;
// using ContactPoint = fcl::ContactPoint;
// using CostSource = fcl::CostSource;

void build_pyfcl(py::module &m) {
  // Data type
  auto PyTriangle = py::class_<Triangle, std::shared_ptr<Triangle>>(m, "Triangle",
                                                                    DOC(fcl, Triangle));
  PyTriangle.def(py::init())
      .def(py::init<size_t, size_t, size_t>(), py::arg("p1"), py::arg("p2"),
           py::arg("p3"))
      .def("set",
           [](Triangle &a, size_t p1, size_t p2, size_t p3) { a.set(p1, p2, p3); })
      .def("get", [](const Triangle &a, int i) { return a[i]; })
      .def("__getitem__", [](const Triangle &a, int i) { return a[i]; });

  // Collision Geometry type
  auto PyCollisionGeometry =
      py::class_<CollisionGeometry, std::shared_ptr<CollisionGeometry>>(
          m, "CollisionGeometry", DOC(fcl, CollisionGeometry));
  PyCollisionGeometry.def("computeLocalAABB", &CollisionGeometry::computeLocalAABB)
      .def("isOccupied", &CollisionGeometry::isOccupied)
      .def("isFree", &CollisionGeometry::isFree)
      .def("isUncertain", &CollisionGeometry::isUncertain)
      .def("computeCOM", &CollisionGeometry::computeCOM)
      .def("computeMomentofInertia", &CollisionGeometry::computeMomentofInertia)
      .def("computeVolume", &CollisionGeometry::computeVolume)
      .def("computeMomentofInertiaRelatedToCOM",
           &CollisionGeometry::computeMomentofInertiaRelatedToCOM)
      .def_readwrite("aabb_center", &CollisionGeometry::aabb_center)
      .def_readwrite("aabb_radius", &CollisionGeometry::aabb_radius)
      .def_readwrite("cost_density", &CollisionGeometry::cost_density);

  // collision geometries
  auto PyBox = py::class_<Box, std::shared_ptr<Box>>(m, "Box", PyCollisionGeometry,
                                                     DOC(fcl, Box));
  PyBox.def(py::init<const Vector3<S> &>(), py::arg("side"), DOC(fcl, Box, Box))
      .def(py::init<S, S, S>(), py::arg("x"), py::arg("y"), py::arg("z"),
           DOC(fcl, Box, Box, 2))
      .def_readwrite("half_side", &Box::halfSide);

  auto PyCapsule = py::class_<Capsule, std::shared_ptr<Capsule>>(
      m, "Capsule", PyCollisionGeometry, DOC(fcl, Capsule));
  PyCapsule
      .def(py::init<S, S>(), py::arg("radius"), py::arg("lz"),
           DOC(fcl, Capsule, Capsule))
      .def_readwrite("radius", &Capsule::radius)
      .def_readwrite("lz_2", &Capsule::halfLength);

  auto PyCone = py::class_<Cone, std::shared_ptr<Cone>>(m, "Cone", PyCollisionGeometry,
                                                        DOC(fcl, Cone));
  PyCone.def(py::init<S, S>(), py::arg("radius"), py::arg("lz"), DOC(fcl, Cone, Cone))
      .def_readwrite("radius", &Cone::radius)
      .def_readwrite("lz_2", &Cone::halfLength);

//   auto PyConvex = py::class_<Convex, std::shared_ptr<Convex>>(
//       m, "Convex", PyCollisionGeometry, DOC(fcl, Convex));
//   PyConvex
//       .def(py::init<const std::shared_ptr<const std::vector<Vector3<S>>> &, int,
//                     const std::shared_ptr<const std::vector<int>> &, bool>(),
//            py::arg("vertices"), py::arg("num_faces"), py::arg("faces"),
//            py::arg("throw_if_invalid") = true, DOC(fcl, Convex, Convex))
//       .def(py::init([](const MatrixX3<S> &vertices, const MatrixX3i &faces,
//                        bool throw_if_invalid) {
//              auto vertices_new {std::make_shared<std::vector<Vector3<S>>>()};
//              auto faces_new {std::make_shared<std::vector<int>>()};
//              for (const Vector3<S> &row : vertices.rowwise())
//                vertices_new->push_back(row);
//              for (auto i = 0; i < faces.rows(); i++) {
//                faces_new->push_back(3);
//                for (size_t j = 0; j < 3; j++) faces_new->push_back(faces(i, j));
//              }
//              return Convex(vertices_new, faces.rows(), faces_new, throw_if_invalid);
//            }),
//            py::arg("vertices"), py::arg("faces"), py::arg("throw_if_invalid") = true,
//            DOC(fcl, Convex, Convex, 2))
//       .def("get_face_count", &Convex::getFaceCount, DOC(fcl, Convex, getFaceCount))
//       .def("get_faces", &Convex::getFaces, DOC(fcl, Convex, getFaces))
//       .def("get_vertices", &Convex::getVertices, DOC(fcl, Convex, getVertices))
//       .def("compute_volume", &Convex::computeVolume, DOC(fcl, Convex, computeVolume))
//       .def("get_interior_point", &Convex::getInteriorPoint,
//            DOC(fcl, Convex, getInteriorPoint));

  auto PyCylinder = py::class_<Cylinder, std::shared_ptr<Cylinder>>(
      m, "Cylinder", PyCollisionGeometry, DOC(fcl, Cylinder));
  PyCylinder
      .def(py::init<S, S>(), py::arg("radius"), py::arg("lz"),
           DOC(fcl, Cylinder, Cylinder))
      .def_readwrite("radius", &Cylinder::radius)
      .def_readwrite("lz_2", &Cylinder::halfLength);

  auto PyPlane = py::class_<Plane, std::shared_ptr<Plane>>(
      m, "Plane", PyCollisionGeometry, DOC(fcl, Plane));
  PyPlane
      .def(py::init<const Vector3<S> &, S>(), py::arg("n"), py::arg("d"),
           DOC(fcl, Plane, Plane))
      .def(py::init<S, S, S, S>(), py::arg("a"), py::arg("b"), py::arg("c"),
           py::arg("d"), DOC(fcl, Plane, Plane, 2))
      .def_readwrite("n", &Plane::n)
      .def_readwrite("d", &Plane::d);

  auto PySphere = py::class_<Sphere, std::shared_ptr<Sphere>>(
      m, "Sphere", PyCollisionGeometry, DOC(fcl, Sphere));
  PySphere.def(py::init<S>(), py::arg("radius"), DOC(fcl, Sphere, Sphere))
      .def_readwrite("radius", &Sphere::radius);

  auto PyBVHModel_OBBRSS =
      py::class_<BVHModel_OBBRSS, std::shared_ptr<BVHModel_OBBRSS>>(
          m, "BVHModel", PyCollisionGeometry, DOC(fcl, BVHModel_OBBRSS));
  PyBVHModel_OBBRSS.def(py::init<>())
      .def("beginModel", &BVHModel_OBBRSS::beginModel, py::arg("num_faces") = 0,
           py::arg("num_vertices") = 0, DOC(fcl, BVHModel_OBBRSS, beginModel))
      .def("endModel", &BVHModel_OBBRSS::endModel, DOC(fcl, BVHModel_OBBRSS, endModel))
      .def("addSubModel",
           py::overload_cast<const std::vector<Vector3<S>> &>(
               &BVHModel_OBBRSS::addSubModel),
           py::arg("vertices"), DOC(fcl, BVHModel_OBBRSS, addSubModel))
      .def("addSubModel",
           py::overload_cast<const std::vector<Vector3<S>> &,
                             const std::vector<Triangle> &>(
               &BVHModel_OBBRSS::addSubModel),
           py::arg("vertices"), py::arg("faces"),
           DOC(fcl, BVHModel_OBBRSS, addSubModel, 2))
      .def(
          "addSubModel",
          [](BVHModel_OBBRSS &a, const std::vector<Vector3<S>> &vertices,
             const std::vector<Vector3i> &faces) {
            std::vector<Triangle> face_list;
            for (const auto &face : faces)
              face_list.push_back(Triangle(static_cast<size_t>(face[0]),
                                           static_cast<size_t>(face[1]),
                                           static_cast<size_t>(face[2])));
            a.addSubModel(vertices, face_list);
          },
          py::arg("vertices"), py::arg("faces"),
          DOC(fcl, BVHModel_OBBRSS, addSubModel, 3))
      .def(
          "get_vertices",
          [](const BVHModel_OBBRSS &a) {
            std::vector<Vector3<S>> ret;
            for (size_t i = 0; i < a.num_vertices; i++) ret.push_back(*(a.vertices + i));
            return ret;
          },
          DOC(fcl, BVHModel_OBBRSS, getVertices))
      .def(
          "get_faces",
          [](const BVHModel_OBBRSS &a) {
            std::vector<Triangle> ret;
            for (size_t i = 0; i < a.num_tris; i++) ret.push_back(*(a.tri_indices + i));
            return ret;
          },
          DOC(fcl, BVHModel_OBBRSS, getFaces))
      .def_readonly("num_faces", &BVHModel_OBBRSS::num_tris)
      .def_readonly("num_vertices", &BVHModel_OBBRSS::num_vertices);

//   auto PyOcTree = py::class_<OcTree, std::shared_ptr<OcTree>>(
//       m, "OcTree", PyCollisionGeometry, DOC(fcl, OcTree));
//   PyOcTree.def(py::init<S>(), py::arg("resolution") = 0.01, DOC(fcl, OcTree, OcTree))
//       .def(py::init([](const MatrixX3<S> &vertices, double resolution) {
//              auto tree = std::make_shared<octomap::OcTree>(resolution);
//              for (const auto &row : vertices.rowwise())
//                tree->updateNode(octomap::point3d {static_cast<float>(row(0)),
//                                                   static_cast<float>(row(1)),
//                                                   static_cast<float>(row(2))},
//                                 true);
//              return std::make_shared<OcTree>(tree);
//            }),
//            py::arg("vertices"), py::arg("resolution") = 0.01,
//            DOC(fcl, OcTree, OcTree, 2));

  // Collision Object = Geometry + Transformation
//   auto PyCollisionObject =
//       py::class_<CollisionObject, std::shared_ptr<CollisionObject>>(
//           m, "CollisionObject", DOC(fcl, CollisionObject));
//   PyCollisionObject
//       .def(py::init([](const std::shared_ptr<CollisionGeometry> &cgeom,
//                        const Vector3<S> &p, const Vector4<S> &q) {
//              auto rot_mat = Quaternion<S> {q(0), q(1), q(2), q(3)}.matrix();
//              return CollisionObject(cgeom, rot_mat, p);
//            }),
//            py::arg("collision_geometry"), py::arg("position") = Vector3<S> {0, 0, 0},
//            py::arg("quaternion") = Vector4<S> {1, 0, 0, 0},
//            DOC(fcl, CollisionObject, CollisionObject))
//       .def("get_collision_geometry", &CollisionObject::collisionGeometry)
//       .def("get_translation", &CollisionObject::getTranslation)
//       .def("get_rotation", &CollisionObject::getRotation)
//       .def("set_transformation", [](CollisionObject &a, const Vector7<S> &pose) {
//         a.setTransform(toIsometry<S>(pose));
//       });

//   /**********    narrowphase    *******/
//   auto PyGJKSolverType = py::enum_<GJKSolverType>(m, "GJKSolverType");
//   PyGJKSolverType.value("GST_LIBCCD", GJKSolverType::GST_LIBCCD)
//       .value("GST_INDEP", GJKSolverType::GST_INDEP)
//       .export_values();

  auto PyCollisionRequestFlag = py::enum_<CollisionRequestFlag>(m, "CollisionRequestFlag");
  PyCollisionRequestFlag.value("CONTACT", CollisionRequestFlag::CONTACT)
        .value("DISTANCE_LOWER_BOUND", CollisionRequestFlag::DISTANCE_LOWER_BOUND)
        .value("NO_REQUEST", CollisionRequestFlag::NO_REQUEST)
        .export_values();

  // CollisionRequest
  auto PyCollisionRequest =
      py::class_<CollisionRequest, std::shared_ptr<CollisionRequest>>(
          m, "CollisionRequest");

  PyCollisionRequest
      .def(py::init<CollisionRequestFlag, size_t>(),
           py::arg("flag") = CollisionRequestFlag::CONTACT,
           py::arg("num_max_contacts") = 1)
      .def("isSatisfied", &CollisionRequest::isSatisfied, py::arg("result"));

  // CollisionResult
  auto PyCollisionResult =
      py::class_<CollisionResult, std::shared_ptr<CollisionResult>>(m,
                                                                    "CollisionResult");
  PyCollisionResult.def(py::init<>())
      .def("add_contact", &CollisionResult::addContact, py::arg("c"))
      .def("is_collision", &CollisionResult::isCollision)
      .def("num_contacts", &CollisionResult::numContacts)
      .def("get_contact", &CollisionResult::getContact, py::arg("i"))
      .def("get_contacts",
           [](CollisionResult &result) {
             std::vector<Contact> contacts;
             result.getContacts(contacts);
             return contacts;
           })
      .def("clear", &CollisionResult::clear);

//   // DistanceRequest
//   auto PyDistanceRequest =
//       py::class_<DistanceRequest, std::shared_ptr<DistanceRequest>>(m,
//                                                                     "DistanceRequest");
//   PyDistanceRequest
//       .def(py::init<bool, bool, S, S, S, GJKSolverType>(),
//            py::arg("enable_nearest_points") = false,
//            py::arg("enable_signed_distance") = false, py::arg("rel_err") = 0.0,
//            py::arg("abs_err") = 0.0, py::arg("distance_tolerance") = 1e-6,
//            py::arg("gjk_solver_type") = GJKSolverType::GST_LIBCCD)
//       .def("isSatisfied", &DistanceRequest::isSatisfied, py::arg("result"));

//   // DistanceResult
//   auto PyDistanceResult =
//       py::class_<DistanceResult, std::shared_ptr<DistanceResult>>(m, "DistanceResult");
//   PyDistanceResult
//       .def(py::init<S>(), py::arg("min_distance") = std::numeric_limits<S>::max())
//       .def_readonly("min_distance", &DistanceResult::min_distance)
//       .def_readonly("nearest_points", &DistanceResult::nearest_points)
//       .def("clear", &DistanceResult::clear);

  // Contact
  auto PyContact = py::class_<Contact, std::shared_ptr<Contact>>(m, "Contact");
  PyContact.def(py::init<>())
      .def(py::init<const CollisionGeometry *, const CollisionGeometry *, int, int>(),
           py::arg("o1"), py::arg("o2"), py::arg("b1"), py::arg("b2"))
      .def(py::init<const CollisionGeometry *, const CollisionGeometry *, int, int,
                    const Vector3<S> &, const Vector3<S> &, S>(),
           py::arg("o1"), py::arg("o2"), py::arg("b1"), py::arg("b2"), py::arg("pos"),
           py::arg("normal"), py::arg("depth"))
      .def_readonly("normal", &Contact::normal)
      .def_readonly("pos", &Contact::pos)
      .def_readonly("penetration_depth", &Contact::penetration_depth);

//   // ContactPoint
//   auto PyContactPoint =
//       py::class_<ContactPoint, std::shared_ptr<ContactPoint>>(m, "ContactPoint");
//   PyContactPoint.def(py::init<>())
//       .def(py::init<const Vector3<S> &, const Vector3<S> &, S>(), py::arg("normal"),
//            py::arg("pos"), py::arg("penetration_depth"))
//       .def_readonly("normal", &ContactPoint::normal)
//       .def_readonly("pos", &ContactPoint::pos)
//       .def_readonly("penetration_depth", &ContactPoint::penetration_depth);

//   // CostSource
//   auto PyCostSource =
//       py::class_<CostSource, std::shared_ptr<CostSource>>(m, "CostSource");
//   PyCostSource.def(py::init<>())
//       .def(py::init<const Vector3<S> &, const Vector3<S> &, S>(), py::arg("aabb_min"),
//            py::arg("aabb_max"), py::arg("cost_density"))
//       .def_readonly("aabb_min", &CostSource::aabb_min)
//       .def_readonly("aabb_max", &CostSource::aabb_max)
//       .def_readonly("cost_density", &CostSource::cost_density)
//       .def_readonly("total_cost", &CostSource::total_cost);

  // collide / distance functions
  m.def("collide",
        [](const CollisionObject *o1, const CollisionObject *o2,
           const CollisionRequest &request) {
          CollisionResult result;
          fcl::collide(o1, o2, request, result);
          return result;
        })
      .def("distance", [](const CollisionObject *o1, const CollisionObject *o2,
                          const DistanceRequest &request) {
        DistanceResult result;
        fcl::distance(o1, o2, request, result);
        return result;
      });
}

}  // namespace mplib::collision_detection::fcl
