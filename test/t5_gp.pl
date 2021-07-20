
time(A) :- call(A).

test_gp :- run_test, statistics, halt.

:- initialization(test_gp).
