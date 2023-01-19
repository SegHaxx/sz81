( example from the manual )
hex 16 co 5 + c!
0 17 1f 17 screen st st rev
: scl 0 c n>c st .w ;
: std # 5 w> st .w ;
: st4 4 0 do 4 i - pick std loop ;
: ste scl st4 fa7e sp@ - 2/ " sp = " st .w std ;
task stk ste
every 1 ts stk decimal
