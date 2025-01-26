\ Extensions to sod Forth kernel to make a complete Forth system.
\ Copyright 1994 L.C. Benschop Eindhoven, The Netherlands.
\ The program is released under the GNU General Public License version 2.
\ There is NO WARRANTY.

\ Changes 2025-01-18: Split generic and system dependent parts.

S" extend_g.4th" INCLUDED

: AT-XY ( x y --- )
\G Put screen cursor at location (x,y) (0,0) is upper left corner.
    31 EMIT SWAP EMIT EMIT ;

: PAGE 
\G Clear the screen.
  12 EMIT ;

VARIABLE TIMER
: TIMER-INT ( x ---) 
\G Timer interrupt handler.
  1 TIMER +! 49 SPECIAL
;

: MS ( n --- )
    \G Delay for n milliseconds.
    51 SPECIAL +
    BEGIN
	51 SPECIAL OVER U< 0=
    UNTIL DROP
;

SAVE-SYSTEM forth_a.img
BYE
