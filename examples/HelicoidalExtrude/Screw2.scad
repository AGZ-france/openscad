/*
    Exemple simpliste de vis et ecrou avec hexagonales.
 */
 
 module tige(hauteur, pas, diam) { 
    diam2 = pas*2/3;
    union() {
        cylinder(d=diam, h=hauteur, $fn=100);
        helicoidal_extrude(angle =360*(hauteur-pas)/pas, convexity = 2, $fs=20, step=pas, $fn=100, xOffset=diam/2) {
                rotate([0,0,30])circle(d=diam2, $fn=6);
            }
        translate([0,0,-5]) cylinder(d=diam*1.8, h=5, $fn=6);
    }
}

module vis(hauteur, pas, diam) {
     
    diam2 = pas/sin(60);
    union() {
        tige(hauteur, pas, diam);
        translate([0,0,-5]) cylinder(d=diam*1.8, h=5, $fn=6);
    }
}

module ecrou(hauteur, pas, diam) { 
    diam2 = pas/sin(60);

        difference() {
            cylinder(d=diam*1.8, h=hauteur, $fn=6);
            translate([0,0,-pas])tige(hauteur+2*pas, pas, diam);
        }
}

color("green") ecrou(21, 2, 20);

translate([25,25,0])color("red") vis(22, 2, 20);