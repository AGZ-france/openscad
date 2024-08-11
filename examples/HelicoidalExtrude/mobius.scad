 /* Construction d'un noeud de mobius en 3D */
 module mobius(r1, r2, nb) {
        nb = (nb<3) ? 3 : nb;
        helicoidal_extrude(angle =360, convexity = 2, $fs=10,  xOffset=r1, zRotate=360/nb) {
                        circle(r=r2, $fn=nb);
        }
 }
   mobius(40, 3, 2, $fn=200);
   translate([0,0,20])mobius(30, 4, 6, $fn=100);
   