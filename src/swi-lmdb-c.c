/*   SWI-Prolog Interface to Kafka
 *   Copyright (C) 2021  Alexander Diemand
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include <SWI-Prolog.h>
#include <lmdb.h>

/* declarations */
foreign_t swi_lmdb_version(atom_t v);
foreign_t swi_lmdb_env_create(term_t out_env);
foreign_t swi_lmdb_env_close(term_t in_env);
foreign_t swi_lmdb_env_set_maxdbs(term_t in_env, term_t in_sz);
foreign_t swi_lmdb_env_set_mapsize(term_t in_env, term_t in_sz);
foreign_t swi_lmdb_env_open(term_t in_env, term_t in_path, term_t in_flags, term_t in_mode);
foreign_t swi_lmdb_env_stats(term_t in_env, term_t out_list);
foreign_t swi_lmdb_env_info(term_t in_env, term_t out_list);
foreign_t swi_lmdb_txn_begin(term_t in_env, term_t in_parent, term_t in_flags, term_t out_txn);
foreign_t swi_lmdb_txn_abort(term_t in_txn);
foreign_t swi_lmdb_txn_commit(term_t in_txn);
foreign_t swi_lmdb_dbi_open(term_t in_txn, term_t in_name, term_t in_flags, term_t out_dbi);
foreign_t swi_lmdb_dbi_close(term_t in_env, term_t in_dbi);
foreign_t swi_lmdb_get(term_t in_txn, term_t in_dbi, term_t in_key, term_t out_value);
foreign_t swi_lmdb_put(term_t in_txn, term_t in_dbi, term_t in_key, term_t in_value, term_t in_flags);
foreign_t swi_lmdb_del(term_t in_txn, term_t in_dbi, term_t in_key, term_t in_value);
foreign_t swi_lmdb_cursor_open(term_t in_txn, term_t in_dbi, term_t out_cursor);
foreign_t swi_lmdb_cursor_op(term_t in_cursor, term_t in_opname, term_t out_op);
foreign_t swi_lmdb_cursor_get(term_t in_cursor, term_t in_key, term_t out_value, term_t in_op);
foreign_t swi_lmdb_cursor_put(term_t in_cursor, term_t in_key, term_t in_value, term_t in_flags);
foreign_t swi_lmdb_cursor_del(term_t in_cursor, term_t in_flags);
foreign_t swi_lmdb_cursor_count(term_t in_cursor, term_t out_count);
foreign_t swi_lmdb_cursor_close(term_t in_cursor);
foreign_t swi_lmdb_encode_int32_as_list(term_t, term_t);
foreign_t swi_lmdb_encode_int32_from_list(term_t, term_t);
foreign_t swi_lmdb_encode_int64_as_list(term_t, term_t);
foreign_t swi_lmdb_encode_int64_from_list(term_t, term_t);
foreign_t swi_lmdb_encode_double_as_list(term_t, term_t);
foreign_t swi_lmdb_encode_double_from_list(term_t, term_t);

/* install predicates */
install_t install()
{
  PL_register_foreign("pl_lmdb_version", 1, swi_lmdb_version, 0);
  PL_register_foreign("pl_lmdb_env_create", 1, swi_lmdb_env_create, 0);
  PL_register_foreign("pl_lmdb_env_open", 4, swi_lmdb_env_open, 0);
  PL_register_foreign("pl_lmdb_env_set_mapsize", 2, swi_lmdb_env_set_mapsize, 0);
  PL_register_foreign("pl_lmdb_env_set_maxdbs", 2, swi_lmdb_env_set_maxdbs, 0);
  PL_register_foreign("pl_lmdb_env_close", 1, swi_lmdb_env_close, 0);
  PL_register_foreign("pl_lmdb_env_stats", 2, swi_lmdb_env_stats, 0);
  PL_register_foreign("pl_lmdb_env_info", 2, swi_lmdb_env_info, 0);
  PL_register_foreign("pl_lmdb_txn_begin", 4, swi_lmdb_txn_begin, 0);
  PL_register_foreign("pl_lmdb_txn_abort", 1, swi_lmdb_txn_abort, 0);
  PL_register_foreign("pl_lmdb_txn_commit", 1, swi_lmdb_txn_commit, 0);
  PL_register_foreign("pl_lmdb_dbi_open", 4, swi_lmdb_dbi_open, 0);
  PL_register_foreign("pl_lmdb_dbi_close", 2, swi_lmdb_dbi_close, 0);
  PL_register_foreign("pl_lmdb_get", 4, swi_lmdb_get, 0);
  PL_register_foreign("pl_lmdb_put", 5, swi_lmdb_put, 0);
  PL_register_foreign("pl_lmdb_del", 4, swi_lmdb_del, 0);
  PL_register_foreign("pl_lmdb_cursor_open", 3, swi_lmdb_cursor_open, 0);
  PL_register_foreign("pl_lmdb_cursor_op", 3, swi_lmdb_cursor_op, 0);
  PL_register_foreign("pl_lmdb_cursor_get", 4, swi_lmdb_cursor_get, 0);
  PL_register_foreign("pl_lmdb_cursor_put", 4, swi_lmdb_cursor_put, 0);
  PL_register_foreign("pl_lmdb_cursor_del", 2, swi_lmdb_cursor_del, 0);
  PL_register_foreign("pl_lmdb_cursor_count", 2, swi_lmdb_cursor_count, 0);
  PL_register_foreign("pl_lmdb_cursor_close", 1, swi_lmdb_cursor_close, 0);
  PL_register_foreign("pl_lmdb_encode_int32_as_list", 2, swi_lmdb_encode_int32_as_list, 0);
  PL_register_foreign("pl_lmdb_encode_int32_from_list", 2, swi_lmdb_encode_int32_from_list, 0);
  PL_register_foreign("pl_lmdb_encode_int64_as_list", 2, swi_lmdb_encode_int64_as_list, 0);
  PL_register_foreign("pl_lmdb_encode_int64_from_list", 2, swi_lmdb_encode_int64_from_list, 0);
  PL_register_foreign("pl_lmdb_encode_double_as_list", 2, swi_lmdb_encode_double_as_list, 0);
  PL_register_foreign("pl_lmdb_encode_double_from_list", 2, swi_lmdb_encode_double_from_list, 0);
}

/* definitions */

foreign_t swi_lmdb_encode_int32_as_list(term_t in_value, term_t out_list)
{
    if (PL_is_variable(in_value)) { PL_fail; }
    if (!PL_is_variable(out_list)) { PL_fail; }
    int32_t value;
    if (!PL_get_integer(in_value, &value)) { PL_fail; }
    return PL_unify_list_ncodes(out_list, sizeof(int32_t), (char*)&value);
}

foreign_t swi_lmdb_encode_int32_from_list(term_t out_value, term_t in_list)
{
    if (PL_is_variable(in_list)) { PL_fail; }
    if (!PL_is_variable(out_value)) { PL_fail; }
    char *buffer;
    if (!PL_get_list_nchars(in_list, NULL, &buffer, 0)) {
      PL_fail;
    } else {
      return PL_unify_integer(out_value, *(int32_t*)buffer);
    }
}

foreign_t swi_lmdb_encode_int64_as_list(term_t in_value, term_t out_list)
{
    if (PL_is_variable(in_value)) { PL_fail; }
    if (!PL_is_variable(out_list)) { PL_fail; }
    int64_t value;
    if (!PL_get_int64(in_value, &value)) { PL_fail; }
    return PL_unify_list_ncodes(out_list, sizeof(int64_t), (char*)&value);
}

foreign_t swi_lmdb_encode_int64_from_list(term_t out_value, term_t in_list)
{
    if (PL_is_variable(in_list)) { PL_fail; }
    if (!PL_is_variable(out_value)) { PL_fail; }
    char *buffer;
    if (!PL_get_list_nchars(in_list, NULL, &buffer, 0)) {
      PL_fail;
    } else {
      return PL_unify_integer(out_value, *(int64_t*)buffer);
    }
}

foreign_t swi_lmdb_encode_double_as_list(term_t in_value, term_t out_list)
{
    if (PL_is_variable(in_value)) { PL_fail; }
    if (!PL_is_variable(out_list)) { PL_fail; }
    double value;
    if (!PL_get_float(in_value, &value)) { PL_fail; }
    return PL_unify_list_ncodes(out_list, sizeof(double), (char*)&value);
}

foreign_t swi_lmdb_encode_double_from_list(term_t out_value, term_t in_list)
{
    if (PL_is_variable(in_list)) { PL_fail; }
    if (!PL_is_variable(out_value)) { PL_fail; }
    char *buffer;
    if (!PL_get_list_nchars(in_list, NULL, &buffer, 0)) {
      PL_fail;
    } else {
      return PL_unify_float(out_value, *(double*)buffer);
    }
}

foreign_t swi_lmdb_version(atom_t lmdb_version)
{
  const char *mdbv = mdb_version(NULL,NULL,NULL);
  atom_t a_mdbv = PL_new_atom(mdbv);
  return PL_unify_atom(lmdb_version, a_mdbv);
}

foreign_t swi_lmdb_env_create(term_t out_env)
{
    if (!PL_is_variable(out_env)) { PL_fail; }
    MDB_env *env = NULL;
    int res = mdb_env_create(&env);
    if (res != 0) {
        mdb_env_close(env);
        PL_fail;
    } else {
        return PL_unify_pointer(out_env, env);
    }
}

foreign_t swi_lmdb_env_close(term_t in_env)
{
    if (PL_is_variable(in_env)) { PL_fail; }
    MDB_env *env = NULL;
    if (!PL_get_pointer(in_env, (void**)&env)) { PL_fail; }
    mdb_env_close(env);
    PL_succeed;
}

foreign_t swi_lmdb_env_set_mapsize(term_t in_env, term_t in_sz)
{
    if (PL_is_variable(in_env)) { PL_fail; }
    MDB_env *env = NULL;
    if (!PL_get_pointer(in_env, (void**)&env)) { PL_fail; }

    long i_sz;
    if (!PL_get_long(in_sz, &i_sz)) { PL_fail; }

    int res = mdb_env_set_mapsize(env, i_sz);
    if (res != 0) {
      PL_fail;
    } else {
      PL_succeed;
    }
}

foreign_t swi_lmdb_env_set_maxdbs(term_t in_env, term_t in_ndbs)
{
    if (PL_is_variable(in_env)) { PL_fail; }
    MDB_env *env = NULL;
    if (!PL_get_pointer(in_env, (void**)&env)) { PL_fail; }

    int32_t i_ndbs;
    if (!PL_get_integer(in_ndbs, &i_ndbs)) { PL_fail; }

    int res = mdb_env_set_maxdbs(env, i_ndbs);
    if (res != 0) {
      PL_fail;
    } else {
      PL_succeed;
    }
}

foreign_t swi_lmdb_env_open(term_t in_env, term_t in_path, term_t in_flags, term_t in_mode)
{
    if (PL_is_variable(in_env)) { PL_fail; }
    MDB_env *env = NULL;
    if (!PL_get_pointer(in_env, (void**)&env)) { PL_fail; }

    if (PL_is_variable(in_path)) { PL_fail; }
    char *s_path;
    if (!PL_get_chars(in_path, &s_path, CVT_ATOM|CVT_STRING|CVT_LIST)) { PL_fail; }

    uint32_t u_flags;
    if (!PL_get_integer(in_flags, (int*)&u_flags)) { PL_fail; }

    int32_t i_mode;
    if (!PL_get_integer(in_mode, &i_mode)) { PL_fail; }

    int res = mdb_env_open(env, s_path, u_flags, i_mode);
    if (res != 0) {
        switch (res) {
            case MDB_VERSION_MISMATCH:
                fprintf(stderr, "mdb_env_open: the version of the LMDB library doesn't match the version that created the database environment.\n");
                break;
            case MDB_INVALID:
                fprintf(stderr, "mdb_env_open: the environment file headers are corrupted.\n");
                break;
            case ENOENT:
                fprintf(stderr, "mdb_env_open: the directory specified by the path parameter doesn't exist.\n");
                break;
            case EACCES:
                fprintf(stderr, "mdb_env_open: the user didn't have permission to access the environment files.\n");
                break;
            case EAGAIN:
                fprintf(stderr, "mdb_env_open: the environment was locked by another process.\n");
                break;
            default:
                fprintf(stderr, "mdb_env_open: other error with code = %d\n", res);
        }
        PL_fail;
    } else {
        PL_succeed;
    }
}

#define MDB_Struct_Acc(Nm, Fld, Cast, Acc) { \
      functor_t fct = PL_new_functor(PL_new_atom(Nm), 1); \
      if (!PL_unify_list(lst, ele, lst)) { PL_fail; }; \
      if (!PL_unify_term(ele, PL_FUNCTOR, fct, Acc, Cast Fld )) { PL_fail; } \
    }

foreign_t swi_lmdb_env_stats(term_t in_env, term_t out_list)
{
    if (PL_is_variable(in_env)) { PL_fail; }
    MDB_env *env = NULL;
    if (!PL_get_pointer(in_env, (void**)&env)) { PL_fail; }

    MDB_stat envstat;
    if (mdb_env_stat(env, &envstat) != 0) {
      PL_fail;
    }

    term_t ele = PL_new_term_ref();
    term_t lst = PL_copy_term_ref(out_list);
    MDB_Struct_Acc("ms_psize", envstat.ms_psize, (int), PL_INT);
    MDB_Struct_Acc("ms_depth", envstat.ms_depth, (int), PL_INT);
    MDB_Struct_Acc("ms_branch_pages", envstat.ms_branch_pages, (size_t), PL_LONG);
    MDB_Struct_Acc("ms_leaf_pages", envstat.ms_leaf_pages, (size_t), PL_LONG);
    MDB_Struct_Acc("ms_overflow_pages", envstat.ms_overflow_pages, (size_t), PL_LONG);
    MDB_Struct_Acc("ms_entries", envstat.ms_entries, (size_t), PL_LONG);
    return PL_unify_nil(lst);
}

foreign_t swi_lmdb_env_info(term_t in_env, term_t out_list)
{
    if (PL_is_variable(in_env)) { PL_fail; }
    MDB_env *env = NULL;
    if (!PL_get_pointer(in_env, (void**)&env)) { PL_fail; }

    MDB_envinfo envinfo;
    if (mdb_env_info(env, &envinfo) != 0) {
      PL_fail;
    }

    term_t ele = PL_new_term_ref();
    term_t lst = PL_copy_term_ref(out_list);
    MDB_Struct_Acc("me_mapaddr", envinfo.me_mapaddr, (void*), PL_LONG);
    MDB_Struct_Acc("me_mapsize", envinfo.me_mapsize, (size_t), PL_LONG);
    MDB_Struct_Acc("me_last_pgno", envinfo.me_last_pgno, (size_t), PL_LONG);
    MDB_Struct_Acc("me_last_txnid", envinfo.me_last_txnid, (size_t), PL_LONG);
    MDB_Struct_Acc("me_maxreaders", envinfo.me_maxreaders, (int), PL_INT);
    MDB_Struct_Acc("me_numreaders", envinfo.me_numreaders, (int), PL_INT);
    return PL_unify_nil(lst);
}

foreign_t swi_lmdb_txn_begin(term_t in_env, term_t in_parent, term_t in_flags, term_t out_txn) {
    if (PL_is_variable(in_env)) { PL_fail; }
    MDB_env *env = NULL;
    if (!PL_get_pointer(in_env, (void**)&env)) { PL_fail; }

    if (PL_is_variable(in_parent)) { PL_fail; }
    MDB_txn *parent = NULL;
    if (!PL_get_pointer(in_parent, (void**)&parent)) { PL_fail; }

    uint32_t u_flags;
    if (!PL_get_integer(in_flags, (int*)&u_flags)) { PL_fail; }

    MDB_txn *txn;
    int res = mdb_txn_begin(env, parent, u_flags, &txn);
    if (res != 0) {
        fprintf(stderr, "mdb_txn_begin: failed with %d\n", res);
        PL_fail;
    } else {
        return PL_unify_pointer(out_txn, txn);
    }
}

foreign_t swi_lmdb_txn_abort(term_t in_txn) {
    if (PL_is_variable(in_txn)) { PL_fail; }
    MDB_txn *txn = NULL;
    if (!PL_get_pointer(in_txn, (void**)&txn)) { PL_fail; }
    mdb_txn_abort(txn);
    PL_succeed;
}

foreign_t swi_lmdb_txn_commit(term_t in_txn) {
    if (PL_is_variable(in_txn)) { PL_fail; }
    MDB_txn *txn = NULL;
    if (!PL_get_pointer(in_txn, (void**)&txn)) { PL_fail; }
    int res = mdb_txn_commit(txn);
    if (res != 0) {
      switch(res) {
        case EINVAL:
          fprintf(stderr, "mdb_txn_commit: an invalid parameter was specified.\n");
          break;
        case ENOSPC:
          fprintf(stderr, "mdb_txn_commit: no more disk space.\n");
          break;
        case EIO:
          fprintf(stderr, "mdb_txn_commit: a low-level I/O error occurred while writing.\n");
          break;
        case ENOMEM:
          fprintf(stderr, "mdb_txn_commit: out of memory.\n");
          break;
        default:
          fprintf(stderr, "mdb_txn_commit: another error %d\n", res);
      };
      PL_fail;
    }
    PL_succeed;
}

foreign_t swi_lmdb_dbi_open(term_t in_txn, term_t in_name, term_t in_flags, term_t out_dbi) {
    if (PL_is_variable(in_txn)) { PL_fail; }
    MDB_txn *txn = NULL;
    if (!PL_get_pointer(in_txn, (void**)&txn)) { PL_fail; }

    if (PL_is_variable(in_name)) { PL_fail; }
    char *s_name;
    size_t s_len;
    if (!PL_get_nchars(in_name, &s_len, &s_name, CVT_ATOM|CVT_STRING|CVT_LIST)) { PL_fail; }

    long i_flags;
    if (!PL_get_long(in_flags, &i_flags)) { PL_fail; }

    MDB_dbi dbi;
    int res = mdb_dbi_open(txn, s_name, i_flags, &dbi);
    if (res != 0) {
        switch (res) {
          case MDB_NOTFOUND:
              fprintf(stderr, "mdb_dbi_open: the specified database doesn't exist in the environment and #MDB_CREATE was not specified.\n");
              break;
          case MDB_DBS_FULL:
              fprintf(stderr, "mdb_dbi_open: too many databases have been opened.\n");
              break;
          default:
              fprintf(stderr, "mdb_dbi_open: other error with code = %d\n", res);
        }
        PL_fail;
    } else {
        return PL_unify_integer(out_dbi, dbi);
    }
}

foreign_t swi_lmdb_dbi_close(term_t in_env, term_t in_dbi)
{
    if (PL_is_variable(in_env)) { PL_fail; }
    MDB_env *env = NULL;
    if (!PL_get_pointer(in_env, (void**)&env)) { PL_fail; }

    if (PL_is_variable(in_dbi)) { PL_fail; }
    MDB_dbi dbi = 0;
    if (!PL_get_integer(in_dbi, (int*)&dbi)) { PL_fail; }

    mdb_dbi_close(env, dbi);
    PL_succeed;
}

foreign_t swi_lmdb_get(term_t in_txn, term_t in_dbi, term_t in_key, term_t out_value) {
    if (PL_is_variable(in_txn)) { PL_fail; }
    MDB_txn *txn = NULL;
    if (!PL_get_pointer(in_txn, (void**)&txn)) { PL_fail; }

    if (PL_is_variable(in_dbi)) { PL_fail; }
    MDB_dbi dbi = 0;
    if (!PL_get_integer(in_dbi, (int*)&dbi)) { PL_fail; }

    MDB_val key;
    if (!PL_get_nchars(in_key, &key.mv_size, (char**)&key.mv_data,
          CVT_ATOM|CVT_STRING|CVT_LIST)) {
      fprintf(stderr, "get key; not a string\n");
      PL_fail;
    }
    MDB_val data;
    int res = mdb_get(txn, dbi, &key, &data);
    if (res != 0) {
      switch (res) {
        case MDB_NOTFOUND:
          fprintf(stderr, "lmdb_get: the key was not in the database.\n");
          break;
        case EINVAL:
          fprintf(stderr, "lmdb_get: an invalid parameter was specified.\n");
          break;
        default:
          fprintf(stderr, "lmdb_get: another error %d\n", res);
      }
      PL_fail;
    } else {
      return PL_unify_list_ncodes(out_value, data.mv_size, data.mv_data);
    }
}

foreign_t swi_lmdb_put(term_t in_txn, term_t in_dbi, term_t in_key, term_t in_value, term_t in_flags) {
    if (PL_is_variable(in_txn)) { PL_fail; }
    MDB_txn *txn = NULL;
    if (!PL_get_pointer(in_txn, (void**)&txn)) { PL_fail; }

    if (PL_is_variable(in_dbi)) { PL_fail; }
    MDB_dbi dbi = 0;
    if (!PL_get_integer(in_dbi, (int*)&dbi)) { PL_fail; }

    MDB_val key;
    if (!PL_get_nchars(in_key, &key.mv_size, (char**)&key.mv_data,
          CVT_ATOM|CVT_STRING|CVT_LIST)) {
      fprintf(stderr, "mdb_put: error while reading key\n");
      PL_fail;
    }
    MDB_val data;
    if (!PL_get_nchars(in_value, &data.mv_size, (char**)&data.mv_data,
          CVT_ATOM|CVT_STRING|CVT_LIST)) {
      fprintf(stderr, "mdb_put: error while reading value\n");
      PL_fail;
    }

    uint32_t u_flags;
    if (!PL_get_integer(in_flags, (int*)&u_flags)) { PL_fail; }

    int res = mdb_put(txn, dbi, &key, &data, u_flags);
    if (res != 0) {
      switch (res) {
        case MDB_MAP_FULL:
          fprintf(stderr, "mdb_put: the database is full.\n");
          break;
        case MDB_TXN_FULL:
          fprintf(stderr, "mdb_put: the transaction has too many dirty pages.\n");
          break;
        case MDB_KEYEXIST:
          fprintf(stderr, "mdb_put: key/data pair already exists.\n");
          break;
        case EACCES:
          fprintf(stderr, "mdb_put: an attempt was made to write in a read-only transaction.\n");
          break;
        case EINVAL:
          fprintf(stderr, "mdb_put: an invalid parameter was specified.\n");
        default:
          fprintf(stderr, "mdb_put: other error with code = %d.\n", res);
      }
      PL_fail;
    } else {
      PL_succeed;
    }
}

foreign_t swi_lmdb_del(term_t in_txn, term_t in_dbi, term_t in_key, term_t in_value) {
    if (PL_is_variable(in_txn)) { PL_fail; }
    MDB_txn *txn = NULL;
    if (!PL_get_pointer(in_txn, (void**)&txn)) { PL_fail; }

    if (PL_is_variable(in_dbi)) { PL_fail; }
    MDB_dbi dbi = 0;
    if (!PL_get_integer(in_dbi, (int*)&dbi)) { PL_fail; }

    MDB_val key;
    if (!PL_get_nchars(in_key, &key.mv_size, (char**)&key.mv_data,
          CVT_ATOM|CVT_STRING|CVT_LIST)) {
      PL_fail;
    }
    MDB_val data;
    if (!PL_get_nchars(in_value, &data.mv_size, (char**)&data.mv_data,
          CVT_ATOM|CVT_STRING|CVT_LIST)) {
      PL_fail;
    }

    if (mdb_del(txn, dbi, &key, &data) != 0) {
      PL_fail;
    } else {
      PL_succeed;
    }
}

foreign_t swi_lmdb_cursor_open(term_t in_txn, term_t in_dbi, term_t out_cursor) {
    if (PL_is_variable(in_txn)) { PL_fail; }
    MDB_txn *txn = NULL;
    if (!PL_get_pointer(in_txn, (void**)&txn)) { PL_fail; }

    if (PL_is_variable(in_dbi)) { PL_fail; }
    MDB_dbi dbi = 0;
    if (!PL_get_integer(in_dbi, (int*)&dbi)) { PL_fail; }

    MDB_cursor *cursor;
    if (mdb_cursor_open(txn, dbi, &cursor) != 0) {
        PL_fail;
    } else {
        return PL_unify_pointer(out_cursor, cursor);
    }
}

#include "cursor_op.h"

foreign_t swi_lmdb_cursor_op(term_t in_cursor, term_t in_opname, term_t out_op) {
    if (PL_is_variable(in_cursor)) { PL_fail; }
    MDB_cursor *cursor = NULL;
    if (!PL_get_pointer(in_cursor, (void**)&cursor)) { PL_fail; }

    MDB_val opname;
    if (!PL_get_nchars(in_opname, &opname.mv_size, (char**)&opname.mv_data,
          CVT_ATOM)) {
      PL_fail;
    }

    int opid = -1;
    int maxsz = sizeof(lmdb_cursor_op_tbl) / sizeof(struct lmdb_cursor_op_itm);
    for (int i = 0; i < maxsz; i++) {
      if (strncmp(opname.mv_data, lmdb_cursor_op_tbl[i]._nm, opname.mv_size) == 0) {
        opid = lmdb_cursor_op_tbl[i]._op;
        break;
      }
    }
    if (opid < 0) {
      PL_fail;
    } else {
      return PL_unify_integer(out_op, opid);
    }
}

foreign_t swi_lmdb_cursor_get(term_t in_cursor, term_t out_key, term_t out_value, term_t in_op) {
    if (PL_is_variable(in_cursor)) { PL_fail; }
    MDB_cursor *cursor = NULL;
    if (!PL_get_pointer(in_cursor, (void**)&cursor)) { PL_fail; }

    if (PL_is_variable(in_op)) { PL_fail; }
    MDB_cursor_op op = 0;
    if (!PL_get_integer(in_op, (int*)&op)) { PL_fail; }

    MDB_val key;
    MDB_val data;
    int res = mdb_cursor_get(cursor, &key, &data, op);
    if (res != 0) {
      switch (res) {
        case MDB_NOTFOUND:
          fprintf(stderr, "mdb_cursor_get: no matching key found.\n");
          break;
        case EINVAL:
          fprintf(stderr, "mdb_cursor_get: an invalid parameter was specified.\n");
          break;
        default:
          fprintf(stderr, "mdb_cursor_get: other error %d.\n", res);
      }
      PL_fail;
    } else {
      if (!PL_unify_list_ncodes(out_key, key.mv_size, key.mv_data)) {
        PL_fail; }
      if (!PL_unify_list_ncodes(out_value, data.mv_size, data.mv_data)) {
        PL_fail; }
      PL_succeed;
    }
}

foreign_t swi_lmdb_cursor_put(term_t in_cursor, term_t in_key, term_t in_value, term_t in_flags) {
    if (PL_is_variable(in_cursor)) { PL_fail; }
    MDB_cursor *cursor = NULL;
    if (!PL_get_pointer(in_cursor, (void**)&cursor)) { PL_fail; }
    // TODO
    PL_fail;
}

foreign_t swi_lmdb_cursor_del(term_t in_cursor, term_t in_flags) {
    if (PL_is_variable(in_cursor)) { PL_fail; }
    MDB_cursor *cursor = NULL;
    if (!PL_get_pointer(in_cursor, (void**)&cursor)) { PL_fail; }

    uint32_t u_flags;
    if (!PL_get_integer(in_flags, (int*)&u_flags)) { PL_fail; }

    if (mdb_cursor_del(cursor, u_flags) != 0) {
      PL_fail;
    } else {
      PL_succeed;
    }
}

foreign_t swi_lmdb_cursor_count(term_t in_cursor, term_t out_count) {
    if (PL_is_variable(in_cursor)) { PL_fail; }
    MDB_cursor *cursor = NULL;
    if (!PL_get_pointer(in_cursor, (void**)&cursor)) { PL_fail; }

    size_t count = 0;
    if (mdb_cursor_count(cursor, &count) != 0) {
      PL_fail;
    }
    return PL_unify_integer(out_count, count);
}

foreign_t swi_lmdb_cursor_close(term_t in_cursor) {
    if (PL_is_variable(in_cursor)) { PL_fail; }
    MDB_cursor *cursor = NULL;
    if (!PL_get_pointer(in_cursor, (void**)&cursor)) { PL_fail; }

    mdb_cursor_close(cursor);
    PL_succeed;
}

