/*
 *  OpenSCAD (www.openscad.org)
 *  Copyright (C) 2009-2011 Clifford Wolf <clifford@clifford.at> and
 *                          Marius Kintel <marius@kintel.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  As a special exception, you have permission to link this program
 *  with the CGAL library and distribute executables, as long as you
 *  follow the requirements of the GNU GPL in regard to all of the
 *  software in the executable aside from CGAL.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "HelicoidalExtrudeNode.h"
#include "module.h"
#include "ModuleInstantiation.h"
#include "children.h"
#include "parameters.h"
#include "printutils.h"
#include "fileutils.h"
#include "builtin.h"
#include "polyset.h"
#include "handle_dep.h"

#include <sstream>
#include <boost/assign/std/vector.hpp>
using namespace boost::assign; // bring 'operator+=()' into scope

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;
/*
 * Distinction xScale pour un tour (360 °) du xScale de la form en fin de rotation (xScalEnd).
 * idem pour yScale
 */
static std::shared_ptr<AbstractNode> builtin_helicoidal_extrude(const ModuleInstantiation *inst, Arguments arguments, Children children)
{
	auto node = std::make_shared<HelicoidalExtrudeNode>(inst);

        Parameters parameters = Parameters::parse(std::move(arguments), inst->location(),
                {"layer", "origin", "scale"},
                {"convexity", "angle", "step", "delta", "xScale", "yScale", "xOffset", "axeRotate", "xScalEnd", "yScalEnd", "zRotate" }
        );

	node->fn = parameters["$fn"].toDouble();
	node->fs = parameters["$fs"].toDouble();
	node->fa = parameters["$fa"].toDouble();
    
	node->layername = parameters["layer"].isUndefined() ? "" : parameters["layer"].toString();
	node->convexity = static_cast<int>(parameters["convexity"].toDouble());
	bool originOk = parameters["origin"].getVec2(node->origin_x, node->origin_y);
	originOk &= std::isfinite(node->origin_x) && std::isfinite(node->origin_y);
	if (parameters["origin"].isDefined() && !originOk){
			LOG(message_group::Warning,inst->location(),parameters.documentRoot(),"helicoidal_extrude(..., origin=%1$s) could not be converted",parameters["origin"].toEchoString());
	}


	node->scale = parameters["scale"].toDouble();
	if (node->scale <= 0) node->scale = 1;

	node->zRotate = 0;
	parameters["zRotate"].getFiniteDouble(node->zRotate);

	node->step = 0;
	parameters["step"].getFiniteDouble(node->step);
	node->angle = 360;
    parameters["angle"].getFiniteDouble(node->angle);

    node->delta = 0;
    parameters["delta"].getFiniteDouble(node->delta);

    if((node->step == 0) && (node->delta == 0) ) {
		// Classic Rotate_extrude ...
		if ((node->angle <= -360) || (node->angle > 360))
			node->angle = 360;
		if( (node->angle == 360) && (node->zRotate != 0)) {
			// Limitation temporaire.
			node->zRotate = (node->zRotate >= 180) ? 360 : 0;
		}
    }

    if(node->angle == 0)
		node->angle = 360;	// Zero interdit !
	node->nbRotation = fabs(node->angle / 360);
    
    node->xScalEnd = 1;
    parameters["xScalEnd"].getFiniteDouble(node->xScalEnd);
    node->yScalEnd = 1;
    parameters["yScalEnd"].getFiniteDouble(node->yScalEnd);
    if(node->xScalEnd <=0) node->xScalEnd = 1;
    if(node->yScalEnd <=0) node->yScalEnd = 1;

	double xScale, yScale;
	if(parameters["xScale"].getFiniteDouble(xScale)) {
		if(xScale <0) xScale  *= -1;
		if(xScale <= 0.1) xScale = 0.1; // Limitation de reduction !
		if(xScale != 1)
			node->xScalEnd = xScale * node->nbRotation;
	}
	if(parameters["yScale"].getFiniteDouble(yScale)) {
		if(yScale <0) yScale  *= -1;
		if(yScale <= 0.1) yScale = 0.1; // Limitation de reduction !
		if(yScale != 1)
			node->yScalEnd = yScale * node->nbRotation;
	}
	
    if (node->convexity <= 0)
		node->convexity = 2;



    node->xOffset = 0;
    parameters["xOffset"].getFiniteDouble(node->xOffset);

    node->axeRotate = 0;
    parameters["axeRotate"].getFiniteDouble(node->axeRotate);
 
		
    
	children.instantiate(node);
	return node;
}

std::string HelicoidalExtrudeNode::toString() const
{
	std::ostringstream stream;

	stream << this->name() << "(";

	stream <<
		"angle = " << this->angle << ", "
		"convexity = " << this->convexity << ", "
        "$fn = " << this->fn << ", $fa = " << this->fa << ", $fs = " << this->fs << ", step = " << this->step << ", delta = " << this->delta
        << ", xScalEnd = " << this->xScalEnd << ", yScalEnd = " << this->yScalEnd << ", xOffset = " << this->xOffset <<", axeRotate = " << this->axeRotate
        << ", zRotate = " << this->zRotate << ")";

	return stream.str();
}

void register_builtin_helicoidal_extrude()
{
	Builtins::init("helicoidal_extrude", new BuiltinModule(builtin_helicoidal_extrude),
				{
					"helicoidal_extrude(angle = 360, convexity = 2)",
				});
}
