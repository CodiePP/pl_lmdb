:- use_module(sbcl(lmdb)).

% run:
% $ swipl -l test/t4.pl -g test

% time used:
% 2021-06-23 23:40
% 2021-07-04 % 8,077,411 inferences, 2.296 CPU in 2.373 seconds (97% CPU, 3517589 Lips)
% 2021-07-20 % 13,910,223 inferences, 3.504 CPU in 3.548 seconds (99% CPU, 3970175 Lips)

test :-
  time(run_test),
  halt.

run_test :-
  lmdb_version(V),
  format("LMDB version: ~a~n",[V]),
  lmdb_env_create(Env),
  lmdb_env_set_maxdbs(Env, 42),
  MapSz is 2^20 * 100, % 100 MB
  lmdb_env_set_mapsize(Env, MapSz),
  lmdb_env_flags(['MDB_NOSYNC','MDB_RDONLY'], EnvFlags),
  lmdb_env_open(Env, '.', EnvFlags, 0o0640),
  lmdb_txn_flags(['MDB_RDONLY'], TxnFlags),
  lmdb_txn_begin(Env, 0, TxnFlags, Txn),
  lmdb_dbi_open(Txn, 'test1M', 0, Dbi),

  read_items(1000000, Txn, Dbi),

  lmdb_txn_abort(Txn),
  lmdb_env_info(Env, Info),
  lmdb_env_stats(Env, Stats),
  lmdb_env_close(Env),

  format("all done: ~p~n~p~n", [Stats,Info]).

read_items(0, _, _) :- !.
read_items(N, Txn, Dbi) :-
  number_codes(N, Key),
  lmdb_get(Txn, Dbi, Key, Ans),
  lmdb_encode_int32(Num, Ans),
  (N = Num ; (format("failed to retrieve: ~p~n", [Key]), fail)),
  !,
  Ndown is N - 1,
  read_items(Ndown, Txn, Dbi).

