:- use_module(sbcl(lmdb)).

% run:
% $ rm lock.mdb data.mdb
% $ swipl -l test/t5.pl -g test

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
  lmdb_env_flags(['MDB_WRITEMAP'], EnvFlags),
  lmdb_env_open(Env, '.', EnvFlags, 0o0640),
  lmdb_txn_begin(Env, 0, 0, Txn1),
  lmdb_dbi_flags(['MDB_CREATE'], DbiFlags),
  lmdb_dbi_open(Txn1, 'db1', DbiFlags, Dbi1),
  write_items(0, 299, Txn1, Dbi1),
  lmdb_txn_commit(Txn1),
  lmdb_dbi_close(Env, Dbi1),
  lmdb_txn_begin(Env, 0, 0, Txn2),
  lmdb_dbi_open(Txn2, 'db2', DbiFlags, Dbi2),
  write_items(1000, 1299, Txn2, Dbi2),
  lmdb_txn_commit(Txn2),
  lmdb_dbi_close(Env, Dbi2),
  lmdb_env_info(Env, Info),
  lmdb_env_stats(Env, Stats),
  lmdb_env_close(Env),
  format("all written: ~p~n~p~n", [Stats,Info]),
  verify('db1', 0, 299),
  verify('db2', 1000, 1299),
  traverse('db2').

write_items(Lim, Lim, _, _) :- !.
write_items(Lim, N, Txn, Dbi) :-
  N > Lim,
  atom_number(Key, N),
  lmdb_encode_int32(N, Num),
  lmdb_put_flags(['MDB_NOOVERWRITE'], DbiPutFlags),
  lmdb_put(Txn, Dbi, Key, Num, DbiPutFlags),
  !,
  Ndown is N - 1,
  write_items(Lim, Ndown, Txn, Dbi).

traverse(DbName) :-
  lmdb_env_create(Env),
  lmdb_env_set_maxdbs(Env, 42),
  %MapSz is 2^20 * 100, % 100 MB
  %lmdb_env_set_mapsize(Env, MapSz),
  lmdb_env_flags(['MDB_NOSYNC','MDB_RDONLY'], EnvFlags),
  lmdb_env_open(Env, '.', EnvFlags, 0o0640),
  lmdb_txn_flags(['MDB_RDONLY'], TxnFlags),
  lmdb_txn_begin(Env, 0, TxnFlags, Txn),
  lmdb_dbi_open(Txn, DbName, 0, Dbi),

  lmdb_cursor_open(Txn, Dbi, Cursor),
  lmdb_cursor_op(Cursor, 'MDB_FIRST', OpFirst),
  ( lmdb_cursor_get(Cursor, K, V0, OpFirst),
    lmdb_encode_int32(V, V0),
    format("first entry: ~s/~p~n",[K,V]),
    lmdb_cursor_op(Cursor, 'MDB_NEXT', OpNext),
    traverse_db(Cursor, OpNext)
  ; format("cannot set cursor to first entry~n")
  ),

  lmdb_txn_abort(Txn),
  lmdb_env_close(Env).

traverse_db(Cursor, Op) :-
  lmdb_cursor_get(Cursor, K, V0, Op),
  lmdb_encode_int32(V, V0),
  format("next entry: ~s/~p~n",[K,V]), !,
  traverse_db(Cursor, Op).
traverse_db(_, _) :-
  format("[the end]~n").

verify(DbName, Start, End) :-
  lmdb_env_create(Env),
  lmdb_env_set_maxdbs(Env, 42),
  %MapSz is 2^20 * 100, % 100 MB
  %lmdb_env_set_mapsize(Env, MapSz),
  lmdb_env_flags(['MDB_NOSYNC','MDB_RDONLY'], EnvFlags),
  lmdb_env_open(Env, '.', EnvFlags, 0o0640),
  lmdb_txn_flags(['MDB_RDONLY'], TxnFlags),
  lmdb_txn_begin(Env, 0, TxnFlags, Txn),
  lmdb_dbi_open(Txn, DbName, 0, Dbi),

  verify_items(Start, End, Txn, Dbi),

  lmdb_txn_abort(Txn),
  lmdb_env_info(Env, Info),
  lmdb_env_stats(Env, Stats),
  lmdb_env_close(Env),

  format("verified '~p': ~p~n~p~n", [DbName,Stats,Info]).

verify_items(End, End, _, _) :- !.
verify_items(End, N, Txn, Dbi) :-
  N > End,
  atom_number(Key, N),
  lmdb_get(Txn, Dbi, Key, Ans),
  lmdb_encode_int32(Num, Ans),
  (N = Num ; (format("failed to retrieve: ~p~n", [Key]), fail)),
  !,
  Ndown is N - 1,
  verify_items(End, Ndown, Txn, Dbi).

