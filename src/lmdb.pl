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
                % transaction
                , lmdb_txn_flags/2
                , lmdb_txn_begin/4
                , lmdb_txn_abort/1
                , lmdb_txn_commit/1
                % named database
                , lmdb_dbi_flags/2
                , lmdb_dbi_open/4
                % direct api
                , lmdb_get/4
                , lmdb_put_flags/2
                , lmdb_put/4
                , lmdb_put/5
                , lmdb_del/4
                % cursor
                , lmdb_cursor_open/3
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

% convention used:
% * all keys are atoms
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

% lmdb_env_flags(+List, -Flags)
lmdb_env_flags(List, Flags) :-
  ValidFlags = [('MDB_FIXEDMAP'-0x01), ('MDB_NOSUBDIR'-0x4000), ('MDB_RDONLY'-0x20000),
                ('MDB_NOSYNC'-0x10000), ('MDB_NOMETASYNC'-0x40000), ('MDB_WRITEMAP'-0x80000),
                ('MDB_MAPASYNC'-0x100000), ('MDB_NOTLS'-0x200000), ('MDB_NOLOCK'-0x400000),
                ('MDB_NORDAHEAD'-0x800000), ('MDB_NOMEMINIT'-0x1000000)],
  foldl([X,Acc,Y] >> (once(member((X-N),ValidFlags)), Y is Acc \/ N), List, 0, Flags).

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

% lmdb_txn_flags(+List, -Flags)
lmdb_txn_flags(List, Flags) :-
  ValidFlags = [('MDB_RDONLY'-0x20000)],
  foldl([X,Acc,Y] >> (once(member((X-N),ValidFlags)), Y is Acc \/ N), List, 0, Flags).

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

% lmdb_dbi_flags(+List, -Integer)
lmdb_dbi_flags(List, Flags) :-
  ValidFlags = [('MDB_REVERSEKEY'-0x02),('MDB_DUPSORT'-0x04),('MDB_INTEGERKEY'-0x08),
                ('MDB_DUPFIXED'-0x10),('MDB_INTEGERDUP'-0x20),('MDB_REVERSEDUP'-0x40),
                ('MDB_CREATE'-0x40000)],
  foldl([X,Acc,Y] >> (once(member((X-N),ValidFlags)), Y is Acc \/ N), List, 0, Flags).

% lmdb_dbi_open(+Txn, +String, +DbiFlags, -Dbi)
lmdb_dbi_open(Txn, DbName, DbiFlags, Dbi) :-
    nonvar(Txn),
    (checklist(DbName) ; string(DbName) ; atom(DbName)),
    nonvar(DbiFlags), var(Dbi),
    pl_lmdb_dbi_open(Txn, DbName, DbiFlags, Dbi).

% lmdb_get(+Txn, +Dbi, +Key, -Value)
lmdb_get(Txn, Dbi, Key, Value) :-
    nonvar(Txn), nonvar(Dbi),
    atom(Key),
    var(Value),
    pl_lmdb_get(Txn, Dbi, Key, Value).

% lmdb_put_flags(+List, -Integer)
lmdb_put_flags(List, Flags) :-
  ValidFlags = [('MDB_NOOVERWRITE'-0x10),('MDB_NODUPDATA'-0x20),('MDB_CURRENT'-0x40),
                ('MDB_APPEND'-0x20000),('MDB_APPENDDUP'-0x40000),('MDB_MULTIPLE'-0x80000)],
  foldl([X,Acc,Y] >> (once(member((X-N),ValidFlags)), Y is Acc \/ N), List, 0, Flags).

% lmdb_put(+Txn, +Dbi, +Key, +Value, +Flags)
lmdb_put(Txn, Dbi, Key, Value) :-
  lmdb_put(Txn, Dbi, Key, Value, 0).
lmdb_put(Txn, Dbi, Key, Value, Flags) :-
    nonvar(Txn), nonvar(Dbi),
    atom(Key),
    (checklist(Value) ; string(Value) ; atom(Value)),
    nonvar(Flags), !,
    pl_lmdb_put(Txn, Dbi, Key, Value, Flags).

% lmdb_del(+Txn, +Dbi, +Key, +Value)
lmdb_del(Txn, Dbi, Key, Value) :-
    nonvar(Txn), nonvar(Dbi),
    atom(Key),
    (checklist(Value) ; string(Value) ; atom(Value)),
    pl_lmdb_del(Txn, Dbi, Key, Value).

% lmdb_cursor_open(+Txn, +Dbi, -Cursor)
lmdb_cursor_open(Txn, Dbi, Cursor) :-
    nonvar(Txn), nonvar(Dbi), var(Cursor),
    pl_lmdb_cursor_open(Txn, Dbi, Cursor).

% lmdb_cursor_get(+Cursor, Key, Value, +Op)
lmdb_cursor_get(Cursor, Key, Value, Op) :-
    nonvar(Cursor), nonvar(Op),
    var(Key), var(Value),
    pl_lmdb_cursor_get(Cursor, Key, Value, Op).

% lmdb_cursor_put(+Cursor, +Key, +Value, +Flags)
lmdb_cursor_put(Cursor, Key, Value, Flags) :-
    nonvar(Cursor),
    atom(Key),
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

