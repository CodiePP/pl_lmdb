/*-------------------------------------------------------------------------*/
/* Prolog Interface to Lightning db (LMDB)                                 */
/*                                                                         */
/* File  : lmdb_flags.pl                                                   */
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

foldflags([], _List, Acc, Acc) :- !.
foldflags(_In, [], Acc, Acc) :- !.
foldflags([A|R], List, Acc, Out) :-
  (member(A-N, List) ; N = 0), !,
  NewAcc is Acc \/ N,
  foldflags(R, List, NewAcc, Out).

% lmdb_env_flags(+List, -Flags)
lmdb_env_flags(List, Flags) :-
  ValidFlags = [('MDB_FIXEDMAP'-0x01), ('MDB_NOSUBDIR'-0x4000), ('MDB_RDONLY'-0x20000),
                ('MDB_NOSYNC'-0x10000), ('MDB_NOMETASYNC'-0x40000), ('MDB_WRITEMAP'-0x80000),
                ('MDB_MAPASYNC'-0x100000), ('MDB_NOTLS'-0x200000), ('MDB_NOLOCK'-0x400000),
                ('MDB_NORDAHEAD'-0x800000), ('MDB_NOMEMINIT'-0x1000000)],
  foldflags(List, ValidFlags, 0, Flags).

% lmdb_txn_flags(+List, -Flags)
lmdb_txn_flags(List, Flags) :-
  ValidFlags = [('MDB_RDONLY'-0x20000)],
  foldflags(List, ValidFlags, 0, Flags).

% lmdb_dbi_flags(+List, -Integer)
lmdb_dbi_flags(List, Flags) :-
  ValidFlags = [('MDB_REVERSEKEY'-0x02),('MDB_DUPSORT'-0x04),('MDB_INTEGERKEY'-0x08),
                ('MDB_DUPFIXED'-0x10),('MDB_INTEGERDUP'-0x20),('MDB_REVERSEDUP'-0x40),
                ('MDB_CREATE'-0x40000)],
  foldflags(List, ValidFlags, 0, Flags).

% lmdb_put_flags(+List, -Integer)
lmdb_put_flags(List, Flags) :-
  ValidFlags = [('MDB_NOOVERWRITE'-0x10),('MDB_NODUPDATA'-0x20),('MDB_CURRENT'-0x40),
                ('MDB_APPEND'-0x20000),('MDB_APPENDDUP'-0x40000),('MDB_MULTIPLE'-0x80000)],
  foldflags(List, ValidFlags, 0, Flags).
