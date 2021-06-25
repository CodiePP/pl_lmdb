:- use_module(sbcl(lmdb)).

test :-
  run_test,
  halt.

run_test :-
  lmdb_version(V),
  format("LMDB version: ~a~n",[V]),
  lmdb_env_create(Env),
  lmdb_env_set_maxdbs(Env, 42),
  MapSz is 2^30 * 100, % 100 GB
  lmdb_env_set_mapsize(Env, MapSz),
  getenv('TMPDIR', P),
  lmdb_env_open(Env, P, 0, 0o0640),
  lmdb_env_close(Env).

