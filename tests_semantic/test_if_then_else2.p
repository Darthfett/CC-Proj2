program testIfThenElse;

class testIfThenElse

BEGIN
   VAR aa, bb, cc: integer;

FUNCTION testIfThenElse;
BEGIN   
   aa := 0;
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

