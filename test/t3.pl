:- use_module(sbcl(lmdb)).

% run:
% $ swipl -l test/t3.pl -g test

% time used:
% 2021-06-23 23:40
%% LMDB version: LMDB 0.9.29: (March 16, 2021)
%% mdb_dbi_open: test1M (6)
%% all done.
%% 31,077,371 inferences, 13.187 CPU in 13.353 seconds (99% CPU, 2356634 Lips)
%
% 2021-07-20 21:28
%% LMDB version: LMDB 0.9.29: (March 16, 2021)
%% all done: [ms_psize(4096),ms_depth(1),ms_branch_pages(0),ms_leaf_pages(1),ms_overflow_pages(0),ms_entries(1)]
%% [me_mapaddr(0),me_mapsize(104857600),me_last_pgno(8894),me_last_txnid(1),me_maxreaders(126),me_numreaders(0)]
%% 21,910,225 inferences, 5.814 CPU in 5.947 seconds (98% CPU, 3768210 Lips)

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
  lmdb_txn_begin(Env, 0, 0, Txn),
  lmdb_dbi_flags(['MDB_CREATE'], DbiFlags),
  lmdb_dbi_open(Txn, 'test1M', DbiFlags, Dbi),

  write_items(1000000, Txn, Dbi),

  lmdb_txn_commit(Txn),
  lmdb_env_info(Env, Info),
  lmdb_env_stats(Env, Stats),
  lmdb_env_close(Env),

  format("all done: ~p~n~p~n", [Stats,Info]).

write_items(0, _, _) :- !.
write_items(N, Txn, Dbi) :-
  number_codes(N, Key),
  lmdb_encode_int32(N, Num),
  lmdb_put_flags(['MDB_NOOVERWRITE'], DbiPutFlags),
  lmdb_put(Txn, Dbi, Key, Num, DbiPutFlags),
  !,
  Ndown is N - 1,
  write_items(Ndown, Txn, Dbi).

