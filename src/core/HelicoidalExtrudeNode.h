#pragma once

#include "node.h"
#include "Value.h"

class HelicoidalExtrudeNode : public AbstractPolyNode
{
public:
	VISITABLE();
	HelicoidalExtrudeNode(const ModuleInstantiation *mi) : AbstractPolyNode(mi) {
		convexity = 0;
		fn = fs = fa = 0;
		origin_x = origin_y = scale = 0;
        angle = 360;
		step = 0;
        delta = 0;
        scale = 1.0;
        xScalEnd =yScalEnd = zScalEnd = 1;
		nbRotation = 1;
        xOffset = 0.0;
        axeRotate = 0;
        zRotate = 0.0;
        xRotate = 0.0;
        xOffsetEnd = 0.0;
	}
	std::string toString() const override;
	std::string name() const override { return "helicoidal_extrude"; }

	int convexity;
	double fn, fs, fa;
	double origin_x, origin_y, scale, angle;
    double step;
    double delta;
    double xScalEnd, yScalEnd, zScalEnd;
    double nbRotation;
	std::string layername;
	
	double xOffset;
	double axeRotate;
	double zRotate;
	double xRotate;
	double xOffsetEnd;
};
