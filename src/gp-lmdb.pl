%-------------------------------------------------------------------------
% GNU Prolog Interface to Lightning db (LMDB)
%
% File  : gp-lmdb.pl
% Author: Alexander Diemand
%
% Copyright (C) 2021 Alexander Diemand
%
%   This program is free software: you can redistribute it and/or modify
%   it under the terms of the GNU General Public License as published by
%   the Free Software Foundation, either version 3 of the License, or
%   (at your option) any later version.
%
%   This program is distributed in the hope that it will be useful,
%   but WITHOUT ANY WARRANTY; without even the implied warranty of
%   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
%   GNU General Public License for more details.
%
%   You should have received a copy of the GNU General Public License
%   along with this program.  If not, see <http://www.gnu.org/licenses/>.
%-------------------------------------------------------------------------

:- include('lmdb_flags.pl').

% lmdb_version(-version_string)
:- foreign(lmdb_version(-term), [fct_name('gp_lmdb_version')]).

% lmdb_env_create(-env)
:- foreign(lmdb_env_create(-positive), [fct_name('gp_lmdb_env_create')]).
% lmdb_env_close(+env)
:- foreign(lmdb_env_close(+positive), [fct_name('gp_lmdb_env_close')]).
% lmdb_env_set_maxdbs(+env, +num)
:- foreign(lmdb_env_set_maxdbs(+positive, +positive),
          [fct_name('gp_lmdb_env_set_maxdbs')]).
% lmdb_env_set_mapsize(+env, +num)
:- foreign(lmdb_env_set_mapsize(+positive, +positive),
          [fct_name('gp_lmdb_env_set_mapsize')]).
% lmdb_env_open(+env, +path, +flags, +mode)
:- foreign(lmdb_env_open(+positive, +string, +positive, +positive),
          [fct_name('gp_lmdb_env_open')]).
% lmdb_env_stats(+env, -list)
:- foreign(lmdb_env_stats(+positive, term),
          [fct_name('gp_lmdb_env_stats')]).
% lmdb_env_info(+env, -list)
:- foreign(lmdb_env_info(+positive, term),
          [fct_name('gp_lmdb_env_info')]).

% lmdb_txn_begin(+env, +parent, +flags, -txn)
:- foreign(lmdb_txn_begin(+positive, +positive, +positive, -positive),
          [fct_name('gp_lmdb_txn_begin')]).
% lmdb_txn_abort(+txn)
:- foreign(lmdb_txn_abort(+positive),
          [fct_name('gp_lmdb_txn_abort')]).
% lmdb_txn_commit(+txn)
:- foreign(lmdb_txn_commit(+positive),
          [fct_name('gp_lmdb_txn_commit')]).

% lmdb_dbi_open(+txn, +name, +flags, -dbi)
:- foreign(lmdb_dbi_open(+positive, +string, +positive, -positive),
          [fct_name('gp_lmdb_dbi_open')]).
% lmdb_dbi_close(+env, +dbi)
:- foreign(lmdb_dbi_close(+positive, +positive),
          [fct_name('gp_lmdb_dbi_close')]).

% lmdb_get(+txn, +dbi, +key, -value)
:- foreign(lmdb_get(+positive, +positive, +term, term),
          [fct_name('gp_lmdb_get')]).
% lmdb_put(+txn, +dbi, +key, +value, +flags)
:- foreign(lmdb_put(+positive, +positive, +term, +term, +positive),
          [fct_name('gp_lmdb_put')]).
% lmdb_del(+txn, +dbi, +key, +value)
:- foreign(lmdb_del(+positive, +positive, +term, +term),
          [fct_name('gp_lmdb_del')]).

% lmdb_cursor_open(+txn, +dbi, -cursor)
:- foreign(lmdb_cursor_open(+positive, +positive, -positive),
          [fct_name('gp_lmdb_cursor_open')]).
% lmdb_cursor_op(+cursor, +opname, -op)
:- foreign(lmdb_cursor_op(+positive, +string, -positive),
          [fct_name('gp_lmdb_cursor_op')]).
% lmdb_cursor_get(+cursor, -key, -value, +op)
:- foreign(lmdb_cursor_get(+positive, term, term, +positive),
          [fct_name('gp_lmdb_cursor_get')]).
% lmdb_cursor_put(+cursor, +key, +value, +flags)
:- foreign(lmdb_cursor_put(+positive, +term, +term, +positive),
          [fct_name('gp_lmdb_cursor_put')]).
% lmdb_cursor_del(+cursor, +flags)
:- foreign(lmdb_cursor_del(+positive, +positive),
          [fct_name('gp_lmdb_cursor_del')]).
% lmdb_cursor_count(+cursor, -count)
:- foreign(lmdb_cursor_count(+positive, -positive),
          [fct_name('gp_lmdb_cursor_count')]).
% lmdb_cursor_close(+cursor)
:- foreign(lmdb_cursor_close(+positive),
          [fct_name('gp_lmdb_cursor_close')]).

:- foreign(lmdb_encode_int32_as_list(+integer, term),
          [fct_name('gp_lmdb_encode_int32_as_list')]).
:- foreign(lmdb_encode_int32_from_list(-integer, term),
          [fct_name('gp_lmdb_encode_int32_from_list')]).
:- foreign(lmdb_encode_int64_as_list(+integer, term),
          [fct_name('gp_lmdb_encode_int64_as_list')]).
:- foreign(lmdb_encode_int64_from_list(-integer, +term),
          [fct_name('gp_lmdb_encode_int64_from_list')]).
:- foreign(lmdb_encode_double_as_list(+float, term),
          [fct_name('gp_lmdb_encode_double_as_list')]).
:- foreign(lmdb_encode_double_from_list(-float, +term),
          [fct_name('gp_lmdb_encode_double_from_list')]).

% value encoding functions
lmdb_encode_int32(Int32, List) :-
  nonvar(Int32), var(List), !,
  lmdb_encode_int32_as_list(Int32, List).
lmdb_encode_int32(Int32, List) :-
  var(Int32), nonvar(List), length(List, 4),
  lmdb_encode_int32_from_list(Int32, List).

lmdb_encode_int64(Int64, List) :-
  nonvar(Int64), var(List), !,
  lmdb_encode_int64_as_list(Int64, List).
lmdb_encode_int64(Int64, List) :-
  var(Int64), nonvar(List), length(List, 8),
  lmdb_encode_int64_from_list(Int64, List).

lmdb_encode_double(Double, List) :-
  nonvar(Double), var(List), !,
  lmdb_encode_double_as_list(Double, List).
lmdb_encode_double(Double, List) :-
  var(Double), nonvar(List), length(List, 8),
  lmdb_encode_double_from_list(Double, List).

