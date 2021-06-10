:- use_module(sbcl(lmdb)).

test :-
  run_test,
  halt.

run_test :-
  lmdb_version(V),
  format("LMDB version: ~a~n",[V]),
  lmdb_env_create(Env),
  lmdb_env_open(Env, "/tmp/", 0, 0o0640),
  lmdb_env_close(Env).

