// Portions of this file are Copyright 2021 Google LLC, and licensed under GPL2+. See COPYING.
#include "cgalutils.h"

#include <CGAL/Polygon_mesh_processing/corefinement.h>
#include <CGAL/Surface_mesh.h>

namespace CGALUtils {

#if FAST_CSG_KERNEL_IS_LAZY

/*! Visitor that forces exact numbers for the vertices of all the faces created during corefinement.
 */
template <typename TriangleMesh>
struct ExactLazyNumbersVisitor
  : public CGAL::Polygon_mesh_processing::Corefinement::Default_visitor<TriangleMesh> {
  typedef boost::graph_traits<TriangleMesh> GT;
  typedef typename GT::face_descriptor face_descriptor;
  typedef typename GT::halfedge_descriptor halfedge_descriptor;
  typedef typename GT::vertex_descriptor vertex_descriptor;

#if CGAL_VERSION_NR >= CGAL_VERSION_NUMBER(5, 4, 0)
  void new_vertex_added(std::size_t i_id, vertex_descriptor v, const TriangleMesh& tm) {
    auto& pt = tm.point(v);
    CGAL::exact(pt.x());
    CGAL::exact(pt.y());
    CGAL::exact(pt.z());
  }
#else
  face_descriptor split_face;
  std::vector<face_descriptor> created_faces;

  void before_subface_creations(face_descriptor f_split, TriangleMesh& tm)
  {
    split_face = f_split;
    created_faces.clear();
  }

  void after_subface_creations(TriangleMesh& mesh)
  {
    for (auto& fi : created_faces) {
      CGAL::Vertex_around_face_iterator<TriangleMesh> vbegin, vend;
      for (boost::tie(vbegin, vend) = vertices_around_face(mesh.halfedge(fi), mesh); vbegin != vend;
           ++vbegin) {
        auto& v = mesh.point(*vbegin);
        CGAL::exact(v.x());
        CGAL::exact(v.y());
        CGAL::exact(v.z());
      }
    }
    created_faces.clear();
  }
  void after_subface_created(face_descriptor fi, TriangleMesh& tm) { created_faces.push_back(fi); }
#endif // if CGAL_VERSION_NR >= CGAL_VERSION_NUMBER(5, 4, 0)
};

#endif// FAST_CSG_KERNEL_IS_LAZY

template <class TriangleMesh>
bool corefineAndComputeUnion(TriangleMesh& lhs, TriangleMesh& rhs, TriangleMesh& out)
{
#if FAST_CSG_KERNEL_IS_LAZY
  if (Feature::ExperimentalFastCsgExactCorefinementCallback.is_enabled()) {
    return CGAL::Polygon_mesh_processing::corefine_and_compute_union(
      lhs, rhs, out,
      CGAL::Polygon_mesh_processing::parameters::visitor(
        ExactLazyNumbersVisitor<TriangleMesh>()),
      CGAL::Polygon_mesh_processing::parameters::visitor(
        ExactLazyNumbersVisitor<TriangleMesh>()));
  } else
#endif// FAST_CSG_KERNEL_IS_LAZY
  {
    return CGAL::Polygon_mesh_processing::corefine_and_compute_union(lhs, rhs, out);
  }
}

template <class TriangleMesh>
bool corefineAndComputeIntersection(TriangleMesh& lhs, TriangleMesh& rhs, TriangleMesh& out)
{
#if FAST_CSG_KERNEL_IS_LAZY
  if (Feature::ExperimentalFastCsgExactCorefinementCallback.is_enabled()) {
    return CGAL::Polygon_mesh_processing::corefine_and_compute_intersection(
      lhs, rhs, out,
      CGAL::Polygon_mesh_processing::parameters::visitor(
        ExactLazyNumbersVisitor<TriangleMesh>()),
      CGAL::Polygon_mesh_processing::parameters::visitor(
        ExactLazyNumbersVisitor<TriangleMesh>()));
  } else
#endif // FAST_CSG_KERNEL_IS_LAZY
  {
    return CGAL::Polygon_mesh_processing::corefine_and_compute_intersection(lhs, rhs, out);
  }
}

template <class TriangleMesh>
bool corefineAndComputeDifference(TriangleMesh& lhs, TriangleMesh& rhs, TriangleMesh& out)
{
#if FAST_CSG_KERNEL_IS_LAZY
  if (Feature::ExperimentalFastCsgExactCorefinementCallback.is_enabled()) {
    return CGAL::Polygon_mesh_processing::corefine_and_compute_difference(
      lhs, rhs, out,
      CGAL::Polygon_mesh_processing::parameters::visitor(
        ExactLazyNumbersVisitor<TriangleMesh>()),
      CGAL::Polygon_mesh_processing::parameters::visitor(
        ExactLazyNumbersVisitor<TriangleMesh>()));
  } else
#endif // FAST_CSG_KERNEL_IS_LAZY
  {
    return CGAL::Polygon_mesh_processing::corefine_and_compute_difference(lhs, rhs, out);
  }
}

template bool corefineAndComputeUnion(
  CGAL_HybridMesh& lhs, CGAL_HybridMesh& rhs, CGAL_HybridMesh& out);
template bool corefineAndComputeIntersection(
  CGAL_HybridMesh& lhs, CGAL_HybridMesh& rhs, CGAL_HybridMesh& out);
template bool corefineAndComputeDifference(
  CGAL_HybridMesh& lhs, CGAL_HybridMesh& rhs, CGAL_HybridMesh& out);

} // namespace CGALUtils

