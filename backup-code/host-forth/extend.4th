\ Extensions to sod Forth kernel to make a complete Forth system.
\ Copyright 1994 L.C. Benschop Eindhoven, The Netherlands.
\ The program is released under the GNU General Public License version 2.
\ There is NO WARRANTY.

\ Changes 2025-01-18: Split generic and system dependent parts.

S" extend_g.4th" INCLUDED

: AT-XY ( x y --- )
\G Put screen cursor at location (x,y) (0,0) is upper left corner.
  27 EMIT [CHAR] [ EMIT 1+  0 .R [CHAR] ; EMIT 
   1+ 0 .R [CHAR] H EMIT ;

: PAGE 
\G Clear the screen.
  27 EMIT ." [2J" 0 0 AT-XY ;

VARIABLE TIMER
: TIMER-INT ( x ---) 
\G Timer interrupt handler.
  1 TIMER +! 49 SPECIAL
;

: MS ( n --- )
\G Delay for n milliseconds.
  ['] TIMER-INT 16 ! 1000 * 50 SPECIAL TIMER @
  BEGIN TIMER @ OVER - UNTIL DROP ;


SAVE-SYSTEM forth.img
BYE
