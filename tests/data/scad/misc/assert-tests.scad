a = 3;
b = 6;

assert(true);
//trailing comma test
assert("t1",);

assert(a*b,);

assert(a*b);
//trailing comma test
assert(condition = a*b,);

assert(true) cube(8, center = true);

c = 2;
translate([0, 20, 0])
assert(condition = 2)
sphere(5);

d = c + 9;
//trailing comma test
assert(condition = d + 5 > 15, message = str("value: ", d + 5),)
translate([15, 0, 0])
cylinder(8, 5, center = true);

echo("assert-tests");
