

: MODE ( m --- )
    22 EMIT EMIT ;

: FG ( c ---)
    17 EMIT EMIT ;
: BG ( c ---)
    17 EMIT 128 + EMIT ;
: PEN ( c ---)
    18 EMIT 0 EMIT EMIT ;

: 2EMIT ( n ---)
    DUP 255 AND EMIT 8 RSHIFT 255 AND EMIT ;
: PLOT ( x y mode ---)
    25 EMIT EMIT SWAP 2EMIT 2EMIT ;

: TEXTCOLOURS
    0 MODE
    8 0 DO
	8 0 DO
	    I J <> IF I FG J BG ." See these colours!  " THEN
	LOOP
    LOOP
    0 BG 7 FG ;

: XORLINES
    2 MODE
    1280 0 DO
	1280 0 DO
	    I 0 4 PLOT J 959 6 PLOT
	2  +LOOP
    2 +LOOP KEY DROP ;
