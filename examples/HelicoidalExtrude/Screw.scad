/*
    Exemple de vis et ecrou avec forme triangulaire, avec réduction filet aux extrémités.
 */
module triangle(h) {
    polygon([[0,h],[h,0],[0,-h]]);
}

module vis(hauteur, pas, diam) { 
    diam2 = 0.90*pas/2;
    union() {
        cylinder(d=diam, h=hauteur, $fs=0.1);
        helicoidal_extrude(angle =360*(hauteur-pas/2)/pas, convexity = 2, $fs=20, step=pas, $fn=80, xOffset=diam/2) {
                triangle(diam2); //circle(d=diam2, $fn=3);
            }
        translate([0,0,-5]) cylinder(d=diam*1.8, h=5, $fn=6);
        
        rotate([0,0,360*(hauteur-pas/2)/pas]) translate([0,0,hauteur-pas/2]) color("green")helicoidal_extrude(angle = 120, convexity = 2,  step=pas/3, $fs=0.01, xOffset=diam/2, xScalEnd=0.1, yScalEnd=0.1) {
                triangle(diam2); //circle(d=diam2, $fn=3);
            }
            
    }
}

module ecrou(hauteur, pas, diam) { 
    diam2 = 0.90* pas/2;
    union() {
        /*difference() {
            cylinder(d=diam*1.8, h=hauteur, $fn=6);
            translate([0,0,-0.5])cylinder(d=diam, h=hauteur+1, $fn=100);
        }  */
         translate([0,0,pas/2]) helicoidal_extrude(angle =360*(hauteur-pas)/pas, convexity = 2, step=pas, $fs=0.1, $fa=4,/*$fn=100,*/ xOffset=diam/2) {
                triangle(-diam2);
          }/* */
          color("blue")rotate([0,0,360*(hauteur-pas)/pas]) translate([0,0,hauteur-pas/2])
          helicoidal_extrude(angle = 90, convexity = 2, $fs=20, step=pas/4, $fn=100, xOffset=diam/2, xScalEnd=0.1, yScalEnd=0.1) {
                triangle(-diam2); 
            }/* */
          color("red")translate([0,0,pas/2])
          helicoidal_extrude(angle = -90, convexity = 2,  step=-pas/4, $fs=0.1, xOffset=diam/2, xScalEnd=0.1, yScalEnd=0.1) {
                triangle(-diam2); 
            }/**/

    }
}
module ecrou2(hauteur, pas, diam) { 
    diam2 = 0.90* pas/2;
        /*difference() {
            cylinder(d=diam*1.8, h=hauteur, $fn=6);
            translate([0,0,-0.5])cylinder(d=diam, h=hauteur+1, $fn=100);
        }  */
    difference() {
         helicoidal_extrude(angle =360*(hauteur)/pas, convexity = 2, $fs=20, step=pas, $fn=100, xOffset=diam/2) {
                triangle(-diam2);
          }/* */
          translate([0,0,-pas])cylinder(h=2*pas, d2=diam-pas, d1 = diam+pas,$fn=100);
    }
}
ecrou(21, 2, 20);
color("red")translate([20,20,0])ecrou2(21, 2, 20);
//translate([25,25,5])  vis(22, 2, 18);
