/*
    Exemple simpliste de vis et ecrou avec forme triangulaire.
 */
 
module vis(hauteur, pas, diam) { 
    diam2 = pas*0.99/sin(60);
    union() {
        cylinder(d=diam, h=hauteur, $fn=100);
        helicoidal_extrude(angle =360*(hauteur-pas)/pas, convexity = 2, $fs=20, step=pas, $fn=100, xOffset=diam/2+diam2*cos(60)/2) {
                circle(d=diam2, $fn=3);
            }
        translate([0,0,-5]) cylinder(d=diam*1.8, h=5, $fn=6);
    }
}

module ecrou(hauteur, pas, diam) { 
    diam2 = pas*0.99/sin(60);
    union() {

        difference() {
            cylinder(d=diam*1.8, h=hauteur, $fn=6);
            translate([0,0,-0.5])cylinder(d=diam+2*diam2, h=hauteur+1, $fn=100);
}

        translate([0,0,pas/2]) helicoidal_extrude(angle =360*(hauteur-pas)/pas, convexity = 2, $fs=20, step=pas, $fn=100, xOffset=diam/2+pas*sin(60)) {
                rotate([0,0,180]) circle(d=diam2, $fn=3);
          }
    }
}

color("green") ecrou(21, 2, 20);

translate([25,25,0])color("red") vis(22, 2, 20);