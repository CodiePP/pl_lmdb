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
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include <gprolog.h>
#include <lmdb.h>

/* declarations */
PlBool gp_lmdb_version(PlTerm *out_v);
PlBool gp_lmdb_env_create(PlLong *out_env);
PlBool gp_lmdb_env_close(PlLong in_env);
PlBool gp_lmdb_env_open(PlLong in_env, const char *p, long flags, long mode);
PlBool gp_lmdb_env_set_mapsize(PlLong in_env, long sz);
PlBool gp_lmdb_env_set_maxdbs(PlLong in_env, long n);

PlBool gp_lmdb_encode_int32_as_list(PlLong, PlTerm out_list);
PlBool gp_lmdb_encode_int32_from_list(PlLong*, PlTerm in_list);
PlBool gp_lmdb_encode_int64_as_list(PlLong, PlTerm out_list);
PlBool gp_lmdb_encode_int64_from_list(PlLong*, PlTerm in_list);
PlBool gp_lmdb_encode_double_as_list(double, PlTerm out_list);
PlBool gp_lmdb_encode_double_from_list(double*, PlTerm in_list);

PlBool gp_lmdb_env_stats(PlLong in_env, PlTerm out_list);
PlBool gp_lmdb_env_info(PlLong in_env, PlTerm out_list);

PlBool gp_lmdb_txn_begin(PlLong in_env, PlLong in_parent, PlLong in_flags, PlLong *out_txn);
PlBool gp_lmdb_txn_abort(PlLong in_txn);
PlBool gp_lmdb_txn_commit(PlLong in_txn);
PlBool gp_lmdb_dbi_open(PlLong in_txn, const char *in_name, PlLong in_flags, PlLong *out_dbi);
PlBool gp_lmdb_dbi_close(PlLong in_env, PlLong in_dbi);

PlBool gp_lmdb_get(PlLong in_txn, PlLong in_dbi, PlTerm in_key, PlTerm out_value);
PlBool gp_lmdb_put(PlLong in_txn, PlLong in_dbi, PlTerm in_key, PlTerm in_value, PlLong in_flags);
PlBool gp_lmdb_del(PlLong in_txn, PlLong in_dbi, PlTerm in_key, PlTerm in_value);

PlBool gp_lmdb_cursor_open(PlLong in_txn, PlLong in_dbi, PlLong *out_cursor);
PlBool gp_lmdb_cursor_op(PlLong in_cursor, const char *in_opname, PlLong *out_op);
PlBool gp_lmdb_cursor_get(PlLong in_cursor, PlTerm out_key, PlTerm out_value, PlLong in_op);
PlBool gp_lmdb_cursor_put(PlLong in_cursor, PlTerm in_key, PlTerm in_value, PlLong in_flags);
PlBool gp_lmdb_cursor_del(PlLong in_cursor, PlLong in_flags);
PlBool gp_lmdb_cursor_count(PlLong in_cursor, PlLong *out_count);
PlBool gp_lmdb_cursor_close(PlLong in_cursor);

/* utilities */
void* uint_to_ptr(unsigned long v) {
  if (v == 0) return NULL;
  return (void*)(v << 3);
}

unsigned long ptr_to_uint(const void *p) {
  if (p == NULL) return 0;
  return ((unsigned long)p >> 3);
}

/* definitions */

PlBool gp_lmdb_encode_int32_as_list(PlLong in_value, PlTerm out_list)
{
    PlTerm out[sizeof(int32_t)];
    for (int i = 0; i < sizeof(int32_t); i++) {
      out[i] = Pl_Mk_Code(((char*)&in_value)[i]);
    }
    return Pl_Un_Proper_List_Check(sizeof(int32_t), out, out_list);
}

PlBool gp_lmdb_encode_int64_as_list(PlLong in_value, PlTerm out_list)
{
    PlTerm out[sizeof(int64_t)];
    for (int i = 0; i < sizeof(int64_t); i++) {
      out[i] = Pl_Mk_Code(((char*)&in_value)[i]);
    }
    return Pl_Un_Proper_List_Check(sizeof(int64_t), out, out_list);
}

PlBool gp_lmdb_encode_double_as_list(double in_value, PlTerm out_list)
{
    PlTerm out[sizeof(double)];
    for (int i = 0; i < sizeof(double); i++) {
      out[i] = Pl_Mk_Code(((char*)&in_value)[i]);
    }
    return Pl_Un_Proper_List_Check(sizeof(double), out, out_list);
}

PlBool gp_lmdb_encode_int32_from_list(PlLong *out_value, PlTerm in_list)
{
    if (! Pl_Builtin_List(in_list)) { return PL_FALSE; }
    if (Pl_List_Length(in_list) != sizeof(int32_t)) { return PL_FALSE; }

    char buffer[sizeof(int32_t)];
    PlTerm lst[sizeof(int32_t)];
    int cnt = Pl_Rd_Proper_List_Check(in_list, lst);
    for (int i = 0; i < cnt; i++) {
      buffer[i] = Pl_Rd_Code(lst[i]) & 0xff;
    }
    *out_value = *(int32_t*)buffer;
    return PL_TRUE;
}

PlBool gp_lmdb_encode_int64_from_list(PlLong *out_value, PlTerm in_list)
{
    if (! Pl_Builtin_List(in_list)) { return PL_FALSE; }
    if (Pl_List_Length(in_list) != sizeof(int64_t)) { return PL_FALSE; }

    char buffer[sizeof(int64_t)];
    PlTerm lst[sizeof(int64_t)];
    int cnt = Pl_Rd_Proper_List_Check(in_list, lst);
    for (int i = 0; i < cnt; i++) {
      buffer[i] = Pl_Rd_Code(lst[i]) & 0xff;
    }
    *out_value = *(int64_t*)buffer;
    return PL_TRUE;
}

PlBool gp_lmdb_encode_double_from_list(double *out_value, PlTerm in_list)
{
    if (! Pl_Builtin_List(in_list)) { return PL_FALSE; }
    if (Pl_List_Length(in_list) != sizeof(double)) { return PL_FALSE; }

    char buffer[sizeof(double)];
    PlTerm *lst;
    int cnt = Pl_Rd_Proper_List_Check(in_list, lst);
    for (int i = 0; i < cnt; i++) {
      buffer[i] = Pl_Rd_Code(lst[i]) & 0xff;
    }
    *out_value = *(double*)buffer;
    return PL_TRUE;
}

PlBool gp_lmdb_version(PlTerm *out_v)
{
    const char *res = mdb_version(NULL,NULL,NULL);
/*    unsigned long v = ptr_to_uint(res);
    void *p = uint_to_ptr(v);
    fprintf(stderr, "lowest 3 bits: %d\n%ld == %ld\nres = %ld\n",
                    ((unsigned int)res & 0x07),
                    v, p, res); */
    *out_v = Pl_Mk_String(res);
    return PL_TRUE;
}

PlBool gp_lmdb_env_create(PlLong *out_env)
{
    MDB_env *env = NULL;
    int res = mdb_env_create(&env);
    if (res != 0) {
        mdb_env_close(env);
        return PL_FALSE;
    } else {
        *out_env = ptr_to_uint(env);
    }
    return PL_TRUE;
}

PlBool gp_lmdb_env_close(PlLong in_env)
{
    MDB_env *env = uint_to_ptr(in_env);
    mdb_env_close(env);
    return PL_TRUE;
}

PlBool gp_lmdb_env_set_mapsize(PlLong in_env, PlLong in_sz)
{
    MDB_env *env = uint_to_ptr(in_env);

    int res = mdb_env_set_mapsize(env, in_sz);
    return (res != 0) ? PL_FALSE : PL_TRUE;
}

PlBool gp_lmdb_env_set_maxdbs(PlLong in_env, PlLong in_ndbs)
{
    MDB_env *env = uint_to_ptr(in_env);

    int res = mdb_env_set_maxdbs(env, in_ndbs);
    return (res != 0) ? PL_FALSE : PL_TRUE;
}

PlBool gp_lmdb_env_open(PlLong in_env, const char *in_path, PlLong in_flags, PlLong in_mode)
{
    MDB_env *env = uint_to_ptr(in_env);

    uint32_t u_flags = in_flags;
    int32_t i_mode = in_mode;

    int res = mdb_env_open(env, in_path, u_flags, i_mode);
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
        return PL_FALSE;
    } else {
        return PL_TRUE;
    }
}

#define MDB_Struct_Add_Int(Tgt, Nm, Val) { \
      int func = Pl_Create_Atom(Nm); \
      PlTerm args[1]; args[0] = Pl_Mk_Integer(Val); \
      Tgt = Pl_Mk_Compound(func, 1, args); \
    }

PlBool gp_lmdb_env_stats(PlLong in_env, PlTerm out_list)
{
    MDB_env *env = uint_to_ptr(in_env);

    MDB_stat envstat;
    if (mdb_env_stat(env, &envstat) != 0) {
      return PL_FALSE;
    }
    const int sz = 6;
    PlTerm ele[sz];
    MDB_Struct_Add_Int(ele[0], "ms_psize", envstat.ms_psize);
    MDB_Struct_Add_Int(ele[1], "ms_depth", envstat.ms_depth);
    MDB_Struct_Add_Int(ele[2], "ms_branch_pages", envstat.ms_branch_pages);
    MDB_Struct_Add_Int(ele[3], "ms_leaf_pages", envstat.ms_leaf_pages);
    MDB_Struct_Add_Int(ele[4], "ms_overflow_pages", envstat.ms_overflow_pages);
    MDB_Struct_Add_Int(ele[5], "ms_entries", envstat.ms_entries);

    return Pl_Un_Proper_List_Check(sz, ele, out_list);
}

PlBool gp_lmdb_env_info(PlLong in_env, PlTerm out_list)
{
    MDB_env *env = uint_to_ptr(in_env);

    MDB_envinfo envinfo;
    if (mdb_env_info(env, &envinfo) != 0) {
      return PL_FALSE;
    }
    const int sz = 6;
    PlTerm ele[sz];
    MDB_Struct_Add_Int(ele[0], "me_mapaddr", (long)envinfo.me_mapaddr);
    MDB_Struct_Add_Int(ele[1], "me_mapsize", envinfo.me_mapsize);
    MDB_Struct_Add_Int(ele[2], "me_last_pgno", envinfo.me_last_pgno);
    MDB_Struct_Add_Int(ele[3], "me_last_txnid", envinfo.me_last_txnid);
    MDB_Struct_Add_Int(ele[4], "me_maxreaders", envinfo.me_maxreaders);
    MDB_Struct_Add_Int(ele[5], "me_numreaders", envinfo.me_numreaders);

    return Pl_Un_Proper_List_Check(sz, ele, out_list);
}

PlBool gp_lmdb_txn_begin(PlLong in_env, PlLong in_parent, PlLong in_flags, PlLong *out_txn) {
    MDB_env *env = uint_to_ptr(in_env);

    MDB_txn *parent = uint_to_ptr(in_parent);

    uint32_t u_flags = in_flags;

    MDB_txn *txn;
    int res = mdb_txn_begin(env, parent, u_flags, &txn);
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
        return PL_FALSE;
    } else {
        *out_txn = ptr_to_uint(txn);
        return PL_TRUE;
    }
}

PlBool gp_lmdb_txn_abort(PlLong in_txn) {
    MDB_txn *txn = uint_to_ptr(in_txn);
    mdb_txn_abort(txn);
    return PL_TRUE;
}

PlBool gp_lmdb_txn_commit(PlLong in_txn) {
    MDB_txn *txn = uint_to_ptr(in_txn);
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
      return PL_FALSE;
    }
    return PL_TRUE;
}

PlBool gp_lmdb_dbi_open(PlLong in_txn, const char *in_name, PlLong in_flags, PlLong *out_dbi) {
    MDB_txn *txn = uint_to_ptr(in_txn);

    MDB_dbi dbi;
    int res = mdb_dbi_open(txn, in_name, in_flags, &dbi);
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
        return PL_FALSE;
    } else {
        *out_dbi = dbi;
    }
    return PL_TRUE;
}

PlBool gp_lmdb_dbi_close(PlLong in_env, PlLong in_dbi)
{
    MDB_env *env = uint_to_ptr(in_env);
    MDB_dbi dbi = in_dbi;

    mdb_dbi_close(env, dbi);
    return PL_TRUE;
}

PlBool gp_lmdb_get(PlLong in_txn, PlLong in_dbi, PlTerm in_key, PlTerm out_value) {
    MDB_txn *txn = uint_to_ptr(in_txn);
    MDB_dbi dbi = in_dbi;

    PlTerm args[512];
    MDB_val key;
    key.mv_size = Pl_Rd_Proper_List_Check(in_key, args);
    key.mv_data = calloc(key.mv_size, 1);
    for (int i = 0; i < key.mv_size; i++) {
      ((char*)(key.mv_data))[i] = Pl_Rd_Byte(args[i]) & 0xff;
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
      return PL_FALSE;
    } else {
      PlTerm out[data.mv_size];
      for (int i = 0; i < data.mv_size; i++) {
        out[i] = Pl_Mk_Code(((char*)data.mv_data)[i] & 0xff);
      }
      return Pl_Un_Proper_List_Check(data.mv_size, out, out_value);
    }
}

PlBool gp_lmdb_put(PlLong in_txn, PlLong in_dbi, PlTerm in_key, PlTerm in_value, PlLong in_flags) {
    MDB_txn *txn = uint_to_ptr(in_txn);
    MDB_dbi dbi = in_dbi;

    PlTerm args[512];
    MDB_val key;
    key.mv_size = Pl_Rd_Proper_List_Check(in_key, args);
    key.mv_data = calloc(key.mv_size, 1);
    for (int i = 0; i < key.mv_size; i++) {
      ((char*)(key.mv_data))[i] = Pl_Rd_Byte(args[i]) & 0xff;
    }
    MDB_val data;
    data.mv_size = Pl_Rd_Proper_List_Check(in_value, args);
    data.mv_data = calloc(data.mv_size, 1);
    for (int i = 0; i < data.mv_size; i++) {
      ((char*)(data.mv_data))[i] = Pl_Rd_Byte(args[i]) & 0xff;
    }

    uint32_t u_flags = in_flags;

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
      return PL_FALSE;
    } else {
      return PL_TRUE;
    }
}

PlBool gp_lmdb_del(PlLong in_txn, PlLong in_dbi, PlTerm in_key, PlTerm in_value) {
    MDB_txn *txn = uint_to_ptr(in_txn);
    MDB_dbi dbi = in_dbi;

    PlTerm args[512];
    MDB_val key;
    key.mv_size = Pl_Rd_Proper_List_Check(in_key, args);
    key.mv_data = calloc(key.mv_size, 1);
    for (int i = 0; i < key.mv_size; i++) {
      ((char*)(key.mv_data))[i] = Pl_Rd_Byte(args[i]) & 0xff;
    }
    MDB_val data;
    data.mv_size = Pl_Rd_Proper_List_Check(in_value, args);
    data.mv_data = calloc(data.mv_size, 1);
    for (int i = 0; i < data.mv_size; i++) {
      ((char*)(data.mv_data))[i] = Pl_Rd_Byte(args[i]) & 0xff;
    }

    if (mdb_del(txn, dbi, &key, &data) != 0) {
      return PL_FALSE;
    } else {
      return PL_TRUE;
    }
}

PlBool gp_lmdb_cursor_open(PlLong in_txn, PlLong in_dbi, PlLong *out_cursor) {
    MDB_txn *txn = uint_to_ptr(in_txn);
    MDB_dbi dbi = in_dbi;

    MDB_cursor *cursor;
    if (mdb_cursor_open(txn, dbi, &cursor) != 0) {
        return PL_FALSE;
    } else {
        *out_cursor = ptr_to_uint(cursor);
        return PL_TRUE;
    }
}

#include "cursor_op.h"

PlBool gp_lmdb_cursor_op(PlLong in_cursor, const char *in_opname, PlLong *out_op) {
    MDB_cursor *cursor = uint_to_ptr(in_cursor);

    MDB_val opname;
    opname.mv_data = (void*)in_opname;
    opname.mv_size = strlen(in_opname);

    int opid = -1;
    int maxsz = sizeof(lmdb_cursor_op_tbl) / sizeof(struct lmdb_cursor_op_itm);
    for (int i = 0; i < maxsz; i++) {
      if (strncmp(opname.mv_data, lmdb_cursor_op_tbl[i]._nm, opname.mv_size) == 0) {
        opid = lmdb_cursor_op_tbl[i]._op;
        break;
      }
    }
    if (opid < 0) {
      return PL_FALSE;
    } else {
      *out_op = opid;
      return PL_TRUE;
    }
}

PlBool gp_lmdb_cursor_get(PlLong in_cursor, PlTerm out_key, PlTerm out_value, PlLong in_op) {
    MDB_cursor *cursor = uint_to_ptr(in_cursor);

    MDB_cursor_op op = in_op;

    MDB_val key;
    key.mv_size = 0;
    key.mv_data = calloc(512,1);
    MDB_val data;
    data.mv_size = 0;
    data.mv_data = calloc(512,1);
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
      return PL_FALSE;
    } else {
      PlTerm outd[data.mv_size];
      for (int i = 0; i < data.mv_size; i++) {
        outd[i] = Pl_Mk_Code(((char*)data.mv_data)[i] & 0xff);
      }
      if (!Pl_Un_Proper_List_Check(data.mv_size, outd, out_value)) {
        return PL_FALSE;
      }
      PlTerm outk[key.mv_size];
      for (int i = 0; i < key.mv_size; i++) {
        outk[i] = Pl_Mk_Code(((char*)key.mv_data)[i] & 0xff);
      }
      if (!Pl_Un_Proper_List_Check(key.mv_size, outk, out_key)) {
        return PL_FALSE;
      }
      return PL_TRUE;
    }
}

PlBool gp_lmdb_cursor_put(PlLong in_cursor, PlTerm in_key, PlTerm in_value, PlLong in_flags) {
    MDB_cursor *cursor = uint_to_ptr(in_cursor);

    PlTerm args[512];
    MDB_val key;
    key.mv_size = Pl_Rd_Proper_List_Check(in_key, args);
    key.mv_data = calloc(key.mv_size, 1);
    for (int i = 0; i < key.mv_size; i++) {
      ((char*)(key.mv_data))[i] = Pl_Rd_Byte(args[i]) & 0xff;
    }
    MDB_val data;
    data.mv_size = Pl_Rd_Proper_List_Check(in_value, args);
    data.mv_data = calloc(data.mv_size, 1);
    for (int i = 0; i < data.mv_size; i++) {
      ((char*)(data.mv_data))[i] = Pl_Rd_Byte(args[i]) & 0xff;
    }

    int res = mdb_cursor_put(cursor, &key, &data, in_flags);
    if (res != 0) {
      switch (res) {
        case EACCES:
          fprintf(stderr, "mdb_cursor_put: an attempt was made to write in a read-only transaction.\n");
          break;
        case EINVAL:
          fprintf(stderr, "mdb_cursor_put: an invalid parameter was specified.\n");
          break;
        default:
          fprintf(stderr, "mdb_cursor_put: other error %d.\n", res);
      }
      return PL_FALSE;
    } else {
      return PL_TRUE;
    }
}

PlBool gp_lmdb_cursor_del(PlLong in_cursor, PlLong in_flags) {
    MDB_cursor *cursor = uint_to_ptr(in_cursor);

    uint32_t u_flags = in_flags;

    if (mdb_cursor_del(cursor, u_flags) != 0) {
      return PL_FALSE;
    } else {
      return PL_TRUE;
    }
}

PlBool gp_lmdb_cursor_count(PlLong in_cursor, PlLong *out_count) {
    MDB_cursor *cursor = uint_to_ptr(in_cursor);

    size_t count = 0;
    if (mdb_cursor_count(cursor, &count) != 0) {
      return PL_FALSE;
    }
    *out_count = count;
    return PL_TRUE;
}

PlBool gp_lmdb_cursor_close(PlLong in_cursor) {
    MDB_cursor *cursor = uint_to_ptr(in_cursor);

    mdb_cursor_close(cursor);
    return PL_TRUE;
}

