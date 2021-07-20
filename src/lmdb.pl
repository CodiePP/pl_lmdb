/*-------------------------------------------------------------------------*/
/* Prolog Interface to Lightning db (LMDB)                                 */
/*                                                                         */
/* File  : lmdb.pl                                                         */
/* Author: Alexander Diemand                                               */
/*                                                                         */
/* Copyright (C) 2021 Alexander Diemand                                    */
/*                                                                         */
/*   This program is free software: you can redistribute it and/or modify  */
/*   it under the terms of the GNU General Public License as published by  */
/*   the Free Software Foundation, either version 3 of the License, or     */
/*   (at your option) any later version.                                   */
/*                                                                         */
/*   This program is distributed in the hope that it will be useful,       */
/*   but WITHOUT ANY WARRANTY; without even the implied warranty of        */
/*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         */
/*   GNU General Public License for more details.                          */
/*                                                                         */
/*   You should have received a copy of the GNU General Public License     */
/*   along with this program.  If not, see <http://www.gnu.org/licenses/>. */
/*-------------------------------------------------------------------------*/

:- module(lmdb, [ lmdb_version/1
                % environment
                , lmdb_env_create/1
                , lmdb_env_set_maxdbs/2
                , lmdb_env_set_mapsize/2
                , lmdb_env_flags/2
                , lmdb_env_open/2
                , lmdb_env_open/3
                , lmdb_env_open/4
                , lmdb_env_close/1
                , lmdb_env_info/2
                , lmdb_env_stats/2
                % transaction
                , lmdb_txn_flags/2
                , lmdb_txn_begin/4
                , lmdb_txn_abort/1
                , lmdb_txn_commit/1
                % named database
                , lmdb_dbi_flags/2
                , lmdb_dbi_open/4
                , lmdb_dbi_close/2
                % direct api
                , lmdb_get/4
                , lmdb_put_flags/2
                , lmdb_put/4
                , lmdb_put/5
                , lmdb_del/4
                % cursor
                , lmdb_cursor_open/3
                , lmdb_cursor_op/3
                , lmdb_cursor_get/4
                , lmdb_cursor_put/4
                , lmdb_cursor_del/2
                , lmdb_cursor_count/2
                , lmdb_cursor_close/1
                % utilities
                , lmdb_encode_int32/2
                , lmdb_encode_int64/2
                , lmdb_encode_double/2
                ]).

:- use_foreign_library(sbcl('pllmdb')).
:- include('lmdb_flags.pl').

% convention used:
% * all keys are list of codes
% * values entering the db can be either string, atom or list of codes
% * values retrieved are list of codes


% test for a non-empty list of atomic values
checklist([]) :- !, fail.
checklist([A]) :- atomic(A), !.
checklist([A|R]) :- atomic(A), checklist(R).

% value encoding functions
lmdb_encode_int32(Int32, List) :-
  nonvar(Int32), var(List), !,
  pl_lmdb_encode_int32_as_list(Int32, List).
lmdb_encode_int32(Int32, List) :-
  var(Int32), nonvar(List), length(List, 4),
  pl_lmdb_encode_int32_from_list(Int32, List).

lmdb_encode_int64(Int64, List) :-
  nonvar(Int64), var(List), !,
  pl_lmdb_encode_int64_as_list(Int64, List).
lmdb_encode_int64(Int64, List) :-
  var(Int64), nonvar(List), length(List, 8),
  pl_lmdb_encode_int64_from_list(Int64, List).

lmdb_encode_double(Double, List) :-
  nonvar(Double), var(List), !,
  pl_lmdb_encode_double_as_list(Double, List).
lmdb_encode_double(Double, List) :-
  var(Double), nonvar(List), length(List, 8),
  pl_lmdb_encode_double_from_list(Double, List).

% lmdb_version(-String)
lmdb_version(V) :-
    var(V),
    pl_lmdb_version(V).

% lmdb_env_create(-Env)
lmdb_env_create(Env) :-
    var(Env),
    pl_lmdb_env_create(Env).

% lmdb_env_open(+Env, +Path, +Integer, +Integer)
% Mode may be passed in as an octal number: e.g. 0o0640
lmdb_env_open(Env, Path) :-
    lmdb_env_open(Env, Path, 0, 0o0640).
lmdb_env_open(Env, Path, Flags) :-
    lmdb_env_open(Env, Path, Flags, 0o0640).
lmdb_env_open(Env, Path, Flags, Mode) :-
    nonvar(Env),
    (checklist(Path) ; string(Path) ; atom(Path)),
    integer(Flags), integer(Mode),
    pl_lmdb_env_open(Env, Path, Flags, Mode).

% lmdb_env_info(+Env, +List)
lmdb_env_info(Env, Info) :-
    nonvar(Env), var(Info),
    pl_lmdb_env_info(Env, Info).

% lmdb_env_info(+Env, +List)
lmdb_env_stats(Env, Stats) :-
    nonvar(Env), var(Stats),
    pl_lmdb_env_stats(Env, Stats).

% lmdb_env_set_maxdbs(+Env, +Integer)
lmdb_env_set_maxdbs(Env, MaxSz) :-
    nonvar(Env), integer(MaxSz),
    pl_lmdb_env_set_maxdbs(Env, MaxSz).

% lmdb_env_set_mapsize(+Env, +Integer)
lmdb_env_set_mapsize(Env, MapSz) :-
    nonvar(Env), integer(MapSz),
    pl_lmdb_env_set_mapsize(Env, MapSz).

% lmdb_env_close(+Env)
lmdb_env_close(Env) :-
    nonvar(Env),
    pl_lmdb_env_close(Env).

% lmdb_txn_begin(+Env, +Txn, +TxnFlags, -Txn)
lmdb_txn_begin(Env, ParentTxn, TxnFlags, Txn) :-
    nonvar(Env), nonvar(ParentTxn),
    nonvar(TxnFlags), var(Txn),
    pl_lmdb_txn_begin(Env, ParentTxn, TxnFlags, Txn).

% lmdb_txn_abort(+Txn)
lmdb_txn_abort(Txn) :-
    nonvar(Txn),
    pl_lmdb_txn_abort(Txn).

% lmdb_txn_commit(+Txn)
lmdb_txn_commit(Txn) :-
    nonvar(Txn),
    pl_lmdb_txn_commit(Txn).

% lmdb_dbi_open(+Txn, +String, +DbiFlags, -Dbi)
lmdb_dbi_open(Txn, DbName, DbiFlags, Dbi) :-
    nonvar(Txn),
    (checklist(DbName) ; string(DbName) ; atom(DbName)),
    nonvar(DbiFlags), var(Dbi),
    pl_lmdb_dbi_open(Txn, DbName, DbiFlags, Dbi).

% lmdb_dbi_close(+Env, +Dbi)
lmdb_dbi_close(Env, Dbi) :-
    nonvar(Env), nonvar(Dbi),
    pl_lmdb_dbi_close(Env, Dbi).

% lmdb_get(+Txn, +Dbi, +Key, -Value)
lmdb_get(Txn, Dbi, Key, Value) :-
    nonvar(Txn), nonvar(Dbi),
    checklist(Key),
    var(Value),
    pl_lmdb_get(Txn, Dbi, Key, Value).

% lmdb_put(+Txn, +Dbi, +Key, +Value, +Flags)
lmdb_put(Txn, Dbi, Key, Value) :-
  lmdb_put(Txn, Dbi, Key, Value, 0).
lmdb_put(Txn, Dbi, Key, Value, Flags) :-
    nonvar(Txn), nonvar(Dbi),
    checklist(Key),
    (checklist(Value) ; string(Value) ; atom(Value)),
    nonvar(Flags), !,
    pl_lmdb_put(Txn, Dbi, Key, Value, Flags).

% lmdb_del(+Txn, +Dbi, +Key, +Value)
lmdb_del(Txn, Dbi, Key, Value) :-
    nonvar(Txn), nonvar(Dbi),
    checklist(Key),
    (checklist(Value) ; string(Value) ; atom(Value)),
    pl_lmdb_del(Txn, Dbi, Key, Value).

% lmdb_cursor_open(+Txn, +Dbi, -Cursor)
lmdb_cursor_open(Txn, Dbi, Cursor) :-
    nonvar(Txn), nonvar(Dbi), var(Cursor),
    pl_lmdb_cursor_open(Txn, Dbi, Cursor).

% lmdb_cursor_op(+Cursor, +String, -Op)
lmdb_cursor_op(Cursor, OpName, Op) :-
    nonvar(Cursor), nonvar(OpName),
    var(Op),
    pl_lmdb_cursor_op(Cursor, OpName, Op).

% lmdb_cursor_get(+Cursor, Key, Value, +Op)
lmdb_cursor_get(Cursor, Key, Value, Op) :-
    nonvar(Cursor), nonvar(Op),
    var(Key), var(Value),
    pl_lmdb_cursor_get(Cursor, Key, Value, Op).

% lmdb_cursor_put(+Cursor, +Key, +Value, +Flags)
lmdb_cursor_put(Cursor, Key, Value, Flags) :-
    nonvar(Cursor),
    checklist(Key),
    (checklist(Value) ; string(Value) ; atom(Value)),
    nonvar(Flags),
    pl_lmdb_cursor_put(Cursor, Key, Value, Flags).

% lmdb_cursor_del(+Cursor, +Flags)
lmdb_cursor_del(Cursor, Flags) :-
    nonvar(Cursor), nonvar(Flags),
    pl_lmdb_cursor_del(Cursor, Flags).

% lmdb_cursor_count(+Cursor, -Integer)
lmdb_cursor_count(Cursor, Count) :-
    nonvar(Cursor), var(Count),
    pl_lmdb_cursor_count(Cursor, Count).

% lmdb_cursor_close(+Cursor)
lmdb_cursor_close(Cursor) :-
    nonvar(Cursor),
    pl_lmdb_cursor_close(Cursor).

