:- use_module(sbcl(lmdb)).

% run:
% $ swipl -l test/t4.pl -g test

% time used:
% 2021-06-23 23:40
%

test :-
  time(run_test),
  halt.

run_test :-
  lmdb_version(V),
  format("LMDB version: ~a~n",[V]),
  lmdb_env_create(Env),
  lmdb_env_set_maxdbs(Env, 42),
  MapSz is 2^30 * 1, % 1 GB
  lmdb_env_set_mapsize(Env, MapSz),
  lmdb_env_flags(['MDB_NOSYNC','MDB_RDONLY'], EnvFlags),
  lmdb_env_open(Env, '.', EnvFlags, 0o0640),
  lmdb_txn_flags(['MDB_RDONLY'], TxnFlags),
  lmdb_txn_begin(Env, 0, TxnFlags, Txn),
  lmdb_dbi_open(Txn, 'test1M', 0, Dbi),
  format("db opened~n"),

  read_items(1000000, Txn, Dbi),

  lmdb_txn_abort(Txn),
  lmdb_env_close(Env),

  format("all done.~n").

read_items(0, _, _) :- !.
read_items(N, Txn, Dbi) :-
  atom_number(Key, N),
  lmdb_get(Txn, Dbi, Key, Ans),
  lmdb_encode_int32(Num, Ans),
  (N = Num ; (format("failed to retrieve: ~p~n", [Key]), fail)),
  !,
  Ndown is N - 1,
  read_items(Ndown, Txn, Dbi).

