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
                , lmdb_env_create/1
                , lmdb_env_open/2
                , lmdb_env_open/3
                , lmdb_env_open/4
                , lmdb_env_close/1
                ]).

:- use_foreign_library(sbcl('pllmdb')).

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
    (string(Path) ; atom(Path)),
    integer(Flags), integer(Mode),
    pl_lmdb_env_open(Env, Path, Flags, Mode).

% lmdb_env_close(+Env)
lmdb_env_close(Env) :-
    nonvar(Env),
    pl_lmdb_env_close(Env).
