:- use_module(sbcl(lmdb)).

test :-
  run_test,
  halt.

run_test :-
  lmdb_version(V),
  format("LMDB version: ~a~n",[V]),
  lmdb_env_create(Env),
  lmdb_env_set_maxdbs(Env, 42),
  MapSz is 2^20 * 100, % 100 MB
  lmdb_env_set_mapsize(Env, MapSz),
  getenv('TMPDIR', P),
  lmdb_env_open(Env, P, 0, 0o0640),
  lmdb_txn_begin(Env, 0, 0, Txn),
  lmdb_dbi_flags(['MDB_CREATE'], DbiFlags),
  lmdb_dbi_open(Txn, 'test1', DbiFlags, Dbi),
  lmdb_put_flags(['MDB_NOOVERWRITE'], DbiPutFlags),
  lmdb_put(Txn, Dbi, 'hello', "world", DbiPutFlags),
  lmdb_encode_int32(42, Num1),
  lmdb_put(Txn, Dbi, 'num1', Num1),
  lmdb_encode_double(42.0, Num2),
  lmdb_put(Txn, Dbi, 'num2', Num2),
  lmdb_get(Txn, Dbi, 'hello', Codes1),
  atom_codes(Ans1, Codes1),
  (Ans1 = 'world' ; format(user_error, "expected: 'world', got: ~p~n", [Ans1])),
  lmdb_get(Txn, Dbi, 'num2', Ans2),
  lmdb_encode_double(Num2Ret, Ans2),
  (Num2Ret = 42.0 ; format(user_error, "expected: '42.0', got: ~p~n", [Num2Ret])),
  lmdb_get(Txn, Dbi, 'num1', Ans3),
  lmdb_encode_int32(Num1Ret, Ans3),
  (Num1Ret = 42 ; format(user_error, "expected: '42', got: ~p~n", [Num1Ret])),

  lmdb_txn_commit(Txn),
  lmdb_env_close(Env),

  format("all done.~n").

