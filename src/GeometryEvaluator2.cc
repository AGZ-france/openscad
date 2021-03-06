#include "GeometryEvaluator.h"
#include "Tree.h"
#include "GeometryCache.h"
#include "CGALCache.h"
#include "Polygon2d.h"
#include "module.h"
#include "ModuleInstantiation.h"
#include "state.h"
#include "roofnode.h"
#include "roof_ss.h"
#include "roof_vd.h"
#include "HelicoidalExtrudeNode.h"
#include "csgnode.h"
#include "cgaladvnode.h"
#include "projectionnode.h"
#include "csgops.h"
#include "textnode.h"
#include "cgalutils.h"
#include "clipper-utils.h"
#include "polyset-utils.h"
#include "polyset.h"
#include "calc.h"
#include "printutils.h"
#include "svg.h"
#include "calc.h"
#include "dxfdata.h"
#include "degree_trig.h"
#include <ciso646> // C alternative tokens (xor)
#include <algorithm>
#include "boost-utils.h"

#include <CGAL/convex_hull_2.h>
#include <CGAL/Point_2.h>



// A.G. Scaling object added
static void AGZ_fillRing(std::vector<Vector3d> &ring, const Outline2d &o, double a, bool flip, double step, double xStep=0, double xScale = 1, double yScale = 1, double xOffset = 0, double axeRotate=0, double zRotate = 0)
{
  if (flip) {
    unsigned int l = o.vertices.size() - 1;
    for (unsigned int i = 0; i < o.vertices.size(); ++i) {
				double x = o.vertices[l-i][0];
				double y = o.vertices[l-i][1];
			    double Y = y;
				if(zRotate != 0) {
					Y = x * sin_degrees(zRotate) + y * cos_degrees(zRotate);
					x = x * cos_degrees(zRotate) - y * sin_degrees(zRotate);
				}
			    double X = x  * xScale;
				ring[i][0] = (xOffset+xStep* xScale) * sin_degrees(a) + X * sin_degrees(a+axeRotate);
				ring[i][1] = (xOffset+xStep* xScale) * cos_degrees(a) + X * cos_degrees(a+axeRotate);
	            ring[i][2] = (Y + step) * yScale ;
    }
  } else {
    for (unsigned int i = 0; i < o.vertices.size(); ++i) {
				double x = o.vertices[i][0];
				double y = o.vertices[i][1];
			    double Y = y;
				if(zRotate != 0) {
					Y = x * sin_degrees(zRotate) + y * cos_degrees(zRotate);
					x = x * cos_degrees(zRotate) - y * sin_degrees(zRotate);
				}
			    double X = x  * xScale;

				ring[i][0] = (xOffset+xStep* xScale) * sin_degrees(a) + X * sin_degrees(a+axeRotate);
				ring[i][1] = (xOffset+xStep* xScale) * cos_degrees(a) + X * cos_degrees(a+axeRotate);
	            ring[i][2] = (Y + step) * yScale;
    }
  }
}

// A.G. Helicoidal Extrude from Rotate Extrude !
/*!
    Input to extrude should be clean. This means non-intersecting, correct winding order
    etc., the input coming from a library like Clipper.

    FIXME: We should handle some common corner cases better:
    o 2D polygon having an edge being on the Y axis:
        In this case, we don't need to generate geometry involving this edge as it
        will be an internal edge.
    o 2D polygon having a vertex touching the Y axis:
        This is more complex as the resulting geometry will (may?) be nonmanifold.
        In any case, the previous case is a specialization of this, so the following
        should be handled for both cases:
        Since the ring associated with this vertex will have a radius of zero, it will
        collapse to one vertex. Any quad using this ring will be collapsed to a triangle.

    Currently, we generate a lot of zero-area triangles

*/
static Geometry *helicoidalPolygon(const HelicoidalExtrudeNode &node, const Polygon2d &poly)
{
    if (node.angle == 0) return nullptr;

    bool b360 = (node.angle == 360);
    if(node.delta > 0) b360 = false;
    if(node.step != 0) b360 = false;

    PolySet *ps = new PolySet(3);
    ps->setConvexity(node.convexity);

    double min_x = 0;
    double max_x = 0;
    double min_y = 0;
    double max_y = 0;
    unsigned int fragCircle = 0;
    unsigned int fragments = 0;
    for(const auto &o : poly.outlines()) {
        for(const auto &v : o.vertices) {
            min_x = fmin(min_x, v[0]);
            max_x = fmax(max_x, v[0]);
            min_y = fmin(min_y, v[1]);
            max_y = fmax(max_y, v[1]);

            if ((max_x - min_x) > (max_x+node.xOffset) && (max_x - min_x) > fabs(min_x+node.xOffset)) {
			    LOG(message_group::Error,Location::NONE,"", "all points for helicoidal_extrude() must have the same X coordinate sign (range is %1$.2f -> %2$.2f)", min_x , max_x);
                delete ps;
                return nullptr;
            }
        }
    }
    if((node.axeRotate != 0.0)  && (min_x < 0)){
		Polygon2d poly2;
		LOG(message_group::Warning,Location::NONE,"", "Translate Polygone %1$.2f", -min_x);
 		for(auto &o : poly.outlines()) {
			Outline2d outline;
			for( auto &v : o.vertices) {
				outline.vertices.emplace_back(v[0] - min_x, v[1]);
			}
			poly2.addOutline(outline);
		}
		return helicoidalPolygon(node, poly2);
	}
		Transform2d mat2;
		mat2.matrix() <<
		  cos_degrees(node.zRotate), -sin_degrees(node.zRotate), 0,
		  sin_degrees(node.zRotate), cos_degrees(node.zRotate), 0,
		  0, 0, 1;
    //fragments = (unsigned int)fmax(Calc::get_fragments_from_r(max_x - min_x, node.fn, node.fs, node.fa) * std::abs(node.angle) / 360, 1);
    // Calcul provenant de rotate extrude non fonctionnel !
    fragCircle = Calc::get_fragments_from_r(fmax(std::abs(max_x+node.xOffset), std::abs(min_x+node.xOffset)) , node.fn, node.fs, node.fa);
    fragments = (unsigned int)fmax(fragCircle * std::abs(node.angle) / 360, 1);
    double delta_x = max_x - min_x;
    double delta_y = max_y - min_y;
    bool flip_faces = ((min_x+node.xOffset) >= 0 && node.angle > 0 && !b360) || ((min_x+node.xOffset) < 0 && (node.angle < 0 || b360));

    double step  = node.step;
    double xStep = node.delta;
    xStep  /= fragments;

 	//step fait r??f??rence ?? un tour complet
	double  ratio = (node.angle < 360) ? 360 / std::abs(node.angle) : 1.0;
	int  frag360 =  (unsigned int)fmax(fragCircle * ratio, 1);
    double pas = step  / frag360;
    
    Transform3d rotS(angle_axis_degrees(-node.axeRotate, Vector3d::UnitY()));
    if(!b360) {
        PolySet *ps_start = poly.tessellate(); // starting face
        ps_start->transform(rotS);
        PolysetUtils::translate(*ps_start, Vector3d(node.xOffset,0,0));
        
        Transform3d rot(angle_axis_degrees(90, Vector3d::UnitX()));
        ps_start->transform(rot);
        // Flip vertex ordering
        if (!flip_faces) {
            for(auto &p : ps_start->polygons) {
                std::reverse(p.begin(), p.end());
            }
        }
        ps->append(*ps_start);
        delete ps_start;

        PolySet *ps_end = poly.tessellate();
        
        if(node.zRotate != 0) {
			Polygon2d *poly2 = (Polygon2d *)poly.copy();
            poly2->transform(mat2);
            ps_end = poly2->tessellate();
		}
            
        Transform3d rotS2(angle_axis_degrees(-node.axeRotate, Vector3d::UnitY()));
               
        PolysetUtils::scale(*ps_end, Vector3d(node.xScalEnd, node.yScalEnd, 1));
        ps_end->transform(rotS2);
        PolysetUtils::translate(*ps_end, Vector3d(xStep*fragments*node.xScalEnd, pas*fragments*node.yScalEnd, 0));
        
        PolysetUtils::translate(*ps_end, Vector3d(node.xOffset,0,0));
        Transform3d rot2(angle_axis_degrees(node.angle, Vector3d::UnitZ()) * angle_axis_degrees(90, Vector3d::UnitX()));
        ps_end->transform(rot2);
        if (flip_faces) {
            for(auto &p : ps_end->polygons) {
                std::reverse(p.begin(), p.end());
            }
        }
        ps->append(*ps_end);
        delete ps_end;
    }

	
    for(const auto &o : poly.outlines()) {
        std::vector<Vector3d> rings[2];
        rings[0].resize(o.vertices.size());
        rings[1].resize(o.vertices.size());

        AGZ_fillRing(rings[0], o, (b360) ? -90 : 90, flip_faces, 0, 0, 1,1, node.xOffset, node.axeRotate, node.zRotate/fragments); // first ring
        for (unsigned int j = 0; j < fragments; j++) {
            double a;
            if (b360)
                a = -90 + ((j+1)%fragments) * 360.0 / fragments; // start on the -X axis, for legacy support
            else
                a = 90 - (j+1)* node.angle / fragments; // start on the X axis
                
            AGZ_fillRing(rings[(j+1)%2], o, a, flip_faces, (j+1)*pas, (j+1)*xStep, 1+(j+1)*(node.xScalEnd-1)/fragments, 1+(j+1)*(node.yScalEnd-1)/fragments, node.xOffset, node.axeRotate, (j+1)*node.zRotate/fragments);
            for (size_t i=0;i<o.vertices.size();i++) {
                ps->append_poly();
                ps->insert_vertex(rings[j%2][i]);
                ps->insert_vertex(rings[(j+1)%2][(i+1)%o.vertices.size()]);
                ps->insert_vertex(rings[j%2][(i+1)%o.vertices.size()]);
                ps->append_poly();
                ps->insert_vertex(rings[j%2][i]);
                ps->insert_vertex(rings[(j+1)%2][i]);
                ps->insert_vertex(rings[(j+1)%2][(i+1)%o.vertices.size()]);
            }
        }
    }

    return ps;
}

/*!
    input: List of 2D objects
    output: 3D PolySet
    operation:
        o Union all children
        o Perform extrude
 */
Response GeometryEvaluator::visit(State &state, const HelicoidalExtrudeNode &node)
{
    if (state.isPrefix() && isSmartCached(node)) return Response::PruneTraversal;
    if (state.isPostfix()) {
        shared_ptr<const Geometry> geom;
        if (!isSmartCached(node)) {
            const Geometry *geometry = nullptr;
            geometry = applyToChildren2D(node, OpenSCADOperator::UNION);

            if (geometry) {
                const Polygon2d *polygons = dynamic_cast<const Polygon2d*>(geometry);
                Geometry *rotated = helicoidalPolygon(node, *polygons);
                geom.reset(rotated);
                delete geometry;
            }
        }
        else {
            geom = smartCacheGet(node, false);
        }
        addToParent(state, node, geom);
        node.progress_report();
    }
    return Response::ContinueTraversal;
}

