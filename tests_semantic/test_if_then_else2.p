program testIfThenElse;

class testIfThenElse

BEGIN
   VAR aa, bb, cc: integer;

FUNCTION testIfThenElse;
BEGIN   
   aa := 0;
   bb := aa + cc * 2;
   bb := aa + cc * 2;
   aa := aa + cc * 2;
   aa := 3;
   cc := 2;

   if aa = 0 THEN
      bb := 1
   ELSE
      bb := 0
   ;

   if bb = 1 THEN
      cc := aa
   ELSE
      cc := (bb * cc) + aa

END
END   
.

