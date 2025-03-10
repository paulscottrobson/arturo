

: PAUSE
    100 0 DO
	KEY? IF
	    KEY $DF AND [CHAR] Q = IF
		CURON 0 MODE BYE
	    ELSE
		LEAVE
	    THEN
	THEN	
	100 MS
    LOOP ;

: COLOURED-TEXT
    0 MODE CUROFF
    2 0 DO
	8 0 DO
	    8 0 DO
		I J <> IF I FG J BG ." See these colours!  " THEN
	    LOOP
	LOOP
    LOOP
    0 BG 7 FG PAUSE ;

VARIABLE COL 1 COL !
: NEXTCOL
  COL @ PEN 1 COL +! COL @ 7 = IF 1 COL ! THEN ;


: COLOURED-LINES
  0 MODE CUROFF
  960 0 DO
    960 0 DO
     NEXTCOL 0 I MOVETO 1279 J LINETO
    32 +LOOP
  64 +LOOP PAUSE
 ;

: POLYGON ( sidelen nsides ---)
\G Draw a regular polygon
  DUP 0 DO
    OVER FD
    360 OVER / LT
  LOOP 2DROP ;

: RANDCOLOUR 7 RAND 1+  0 MONO-COLOURS ;

VARIABLE SIDELEN
VARIABLE NSIDES
: POLYGON-CIRCLE ( npols nsides --- )
  2 MODE CUROFF  HOME RANDCOLOUR
  4000 OVER / SIDELEN !
  SWAP DUP NSIDES ! 0 DO
   SIDELEN @ OVER POLYGON 
   360 NSIDES @ / LT
  LOOP DROP ;


CREATE DIVIDERS 3 , 4 , 5 , 6 , 8 , 9 , 10 , 12 , 15 , 20 , 24 , 36 ,
: DIVSEL
  12 RAND CELLS DIVIDERS + @ ;
: POLYGON-CIRCLE-DEMO
  5 0 DO
    DIVSEL DIVSEL POLYGON-CIRCLE PAUSE
  LOOP ;

VARIABLE SIDEMAX
: POLYGON-SPIRAL ( npols nsides )
  2 MODE CUROFF HOME RANDCOLOUR
  8 SIDELEN !
  4000 OVER / 400 + SIDEMAX !
  BEGIN
   SIDELEN @ OVER POLYGON
   8 SIDELEN +!
   360 2 PICK  / RT
  SIDELEN @ SIDEMAX @ < 0= UNTIL 2DROP ; 	

: POLYGON-SPIRAL-DEMO
  10 0 DO
    DIVSEL 2 RAND IF NEGATE THEN DIVSEL POLYGON-SPIRAL PAUSE
  LOOP ;


: LISSAJOUS ( f1 f2 --- )
  2 MODE CUROFF PU RANDCOLOUR 361 0 DO
   I 2 PICK * SIN 1600 M* SCALE14 DROP 
   I 2 PICK * COS 1600 M* SCALE14 DROP 
   DRAWTO PD 
  LOOP 2DROP PAUSE ;

: LISSAJOUS-DEMO
  5 0 DO
    0 0 
    BEGIN
      2DROP 8 RAND 1+ 4 RAND 2* 1+ 2DUP <>
    UNTIL  LISSAJOUS
  LOOP ;
 

32 VALUE THICKNESS
VARIABLE X1 VARIABLE Y1
VARIABLE X2 VARIABLE Y2
VARIABLE X3 VARIABLE Y3
VARIABLE X4 VARIABLE Y4 \ Coordinates of all 4 corners of the rectangle.
: THICK-LINE ( dist thickness ---)
\G Like FORWARD, but draw a thick line
   8 * TO THICKNESS PEN-STATE @ PEN-UP
   90 LT THICKNESS 2/ FD POSX @ POSY @ COORD-TRANSLATE Y1 ! X1 !
   THICKNESS BK          POSX @ POSY @ COORD-TRANSLATE Y2 ! X2 !
   90 RT SWAP FD         POSX @ POSY @ COORD-TRANSLATE Y3 ! X3 !
   90 LT THICKNESS FD    POSX @ POSY @ COORD-TRANSLATE Y4 ! X4 !
   THICKNESS 2/ BK 90 RT PEN-STATE !
   \ Use the 85 VDU PLOT code to draw two triangles that form the rectangle.
   X1 @ Y1 @ MOVETO X2 @ Y2 @ MOVETO X4 @ Y4 @ 85 PLOT
   X3 @ Y3 @ 85 PLOT POSX @ POSY @ COORD-TRANSLATE MOVETO 
  ;


\ Draw random tree, based on a program from Kees Moerman, published in the Dutch
\ FORTH magazine Vijgeblad #27, 1989


: SSPLIT ( length thick --- len thick angle)
    OVER 2* 3 / OVER 1-
    2DUP 40 RAND NEGATE ROT ROT 40 RAND ;

20 VALUE ANGLE         \ angle main branch to trunk
30 VALUE SUBANGLE      \ angle subbranch to trunk
100 VALUE DLENGTH      \ change of length at split
500 VALUE LENGTH       \ Initial length
10 VALUE THICK         \ Initial thickness of trunk
120 VALUE DTHICK       \ change of thickness at split
8 VALUE LEAF-SIZE      \ Size of leaf

\ Draw a tree leaf 
: LEAF
    LEAF-SIZE IF
	8 0 DO 45 RT LEAF-SIZE FD LOOP 
    THEN ;

: REDUCE ( length thick angle --- length thick angle)
    SWAP OVER ABS NEGATE           \ Compute new thickness and length
      DTHICK + DTHICK */ SWAP      \ by reducing dependent on angle
    ROT OVER ABS NEGATE            \ larger angle -> thinner branch
    DLENGTH + DLENGTH */ ROT ROT
;

: SPLIT ( length thick --- 2* (length thick) \ Compute branches
    2DUP                \ length thick length thick
          
    2DUP >R >R          \ length thick lenth thick 

    ANGLE 2* 1+ RAND ANGLE - DUP >R \ branch 1 (main -ANGLE..ANGLE
    DUP 0=                          \ Branch1 same direction as trunk?    
    IF DROP SUBANGLE 2* 1+ RAND     \ side branch -SUBANGLE..SUBANGLE
	SUBANGLE -
    ELSE DUP 0<
	IF SUBANGLE + ELSE SUBANGLE - THEN \ else side branch = +/- SUBANGLE
    THEN
    REDUCE                          \ Reduce length/thickness of branch 1

    R> R> R> ROT
    REDUCE                          \ Reduce length/thickness of branch 2
;

: (TREE) ( len thick --- )
    DUP 0>
    IF
	\ 90 RIGHT 2 FORWARD 90 LEFT
	2DUP THICK-LINE
	SPLIT                       \ stack: 2*(length' thick' angle')

	DUP >R LEFT                 \ Turn main angle
	RECURSE                     \ draw branch 1
	R> RIGHT                    \ and back

	DUP >R LEFT                  \ Turn other angle
	RECURSE                     \ draw branch 2
	R> RIGHT                    \ and back

	DROP
	180 LEFT FORWARD 180 RIGHT  \ Move back along trunk and reverse dir.
    ELSE
	2DROP LEAF                  \ End of branch (thickness=0) draw leaf
    THEN
 ;

: TREE ( length thick --- )
    2DUP TO THICK TO LENGTH 0 -1500 SETPOS 90 SETHEADING
    2DUP THICK-LINE (TREE)
;

: CHERRY \ Cherry tree
    20 TO ANGLE 40 TO SUBANGLE
    100 TO DLENGTH 500 TO LENGTH
    10 TO THICK 120 TO DTHICK
    10 TO LEAF-SIZE ;
: YOUNG-TREE \ Young deciduos tree
    10 TO ANGLE 40 TO SUBANGLE
    150 TO DLENGTH 300 TO LENGTH
    10 TO THICK 150 TO DTHICK
    25 TO LEAF-SIZE ;
: BIG-TREE \ Big older tree (like an oak)
    10 TO ANGLE 40 TO SUBANGLE
    150 TO DLENGTH 200 TO LENGTH
    20 TO THICK 110 TO DTHICK
    0 TO LEAF-SIZE ;
: BIG-TREE2  \ Other big older tree
    40 TO ANGLE 50 TO SUBANGLE
    150 TO DLENGTH 300 TO LENGTH
    20 TO THICK 150 TO DTHICK
    0 TO LEAF-SIZE ;

: FOREST ( n ---)
    0 DO
	-2000 4000 RAND + -1500 SETPOS 90 SETHEADING
	3 RAND
	CASE
	    0 OF  CHERRY ENDOF
	    1 OF  YOUNG-TREE ENDOF
	    2 OF  BIG-TREE2 ENDOF
	ENDCASE
	THICK 80 100 */ TO THICK
	LENGTH THICK 2DUP THICK-LINE (TREE)
    LOOP ;

: FOREST-DEMO
  5 0 DO
    2 MODE CUROFF 2 0 MONO-COLOURS 5 FOREST PAUSE
  LOOP ;


: XORLINES
    2 MODE CUROFF
    1280 0 DO
	1280 0 DO
	    I 0 4 PLOT J 959 6 PLOT
	2  +LOOP
	KEY? IF KEY DROP LEAVE THEN
    2 +LOOP PAUSE ;


: COLOUR-BAR
    3 MODE CUROFF
    64 0 DO
	I PEN
	5 0 DO
	    J 20 * I 4 * + DUP 0 MOVETO 959 LINETO
	LOOP
    LOOP
    PAUSE ;

: RUN-DEMO
  0 MODE CUROFF \ Tests require mode 0.
  BEGIN 
      COLOURED-TEXT
      POLYGON-CIRCLE-DEMO
      COLOURED-LINES
      XORLINES
      POLYGON-SPIRAL-DEMO
      COLOUR-BAR
      LISSAJOUS-DEMO
      FOREST-DEMO
  AGAIN
;
