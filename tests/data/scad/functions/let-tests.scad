$a = 1;
b = 1;
echo(let() $a*b);
echo(let($a=2,b=3) $a*b);
//trailing comma test
echo(let(b=2,$a=3,) $a*b);
echo(let($a=2,) $a*b);
echo(let(b=3) $a*b);
echo(let($a=2,b=3,$a=4) $a*b);
echo(let($a=2,b=3,b=5) $a*b);
echo(let($a=2,b=3,$a=4,b=5) $a*b);
echo(let($a=2,b=$a*3) $a*b);
echo(let($a=2*b,b=3) $a*b);
echo(let($a=2*b,b=$a*3,) $a*b);
echo(let(b=$a*3,$a=2*b) $a*b);
//trailing comma test
echo(let(b=$a,$a=b,) [$a, b]);
echo(let(b=2*$a,$a=b) [for (i=[1:b]) [i,$a]]);

//functions and modules can have unnamend parameters
//but this does not make sense for let
//trailing comma test
let(42,);
let(1,2,3,4,);
