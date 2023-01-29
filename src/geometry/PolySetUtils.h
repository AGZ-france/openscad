#pragma once
#include "linalg.h"	// Vector3d

class Polygon2d;
class PolySet;

namespace PolySetUtils {

Polygon2d *project(const PolySet& ps);
void tessellate_faces(const PolySet& inps, PolySet& outps);
bool is_approximately_convex(const PolySet& ps);

void translate(PolySet& ps, const Vector3d& translation);
void scale(PolySet &ps, const Vector3d &scale);
}
