#pragma once
#include "linalg.h"	// Vector3d

#include <memory>

#include "Geometry.h"

class Polygon2d;
class PolySet;

namespace PolySetUtils {

std::unique_ptr<Polygon2d> project(const PolySet& ps);
std::unique_ptr<PolySet> tessellate_faces(const PolySet& inps);
bool is_approximately_convex(const PolySet& ps);

void translate(PolySet& ps, const Vector3d& translation);
void scale(PolySet &ps, const Vector3d &scale);
std::shared_ptr<const PolySet> getGeometryAsPolySet(const std::shared_ptr<const class Geometry>&);

std::string polySetToPolyhedronSource(const PolySet& ps);

}
