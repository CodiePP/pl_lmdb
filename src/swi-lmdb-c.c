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
foreign_t swi_lmdb_env_open(term_t in_env, term_t in_path, term_t in_flags, term_t in_mode);

/* install predicates */
install_t install()
{
  PL_register_foreign("pl_lmdb_version", 1, swi_lmdb_version, 0);
  PL_register_foreign("pl_lmdb_env_create", 1, swi_lmdb_env_create, 0);
  PL_register_foreign("pl_lmdb_env_open", 4, swi_lmdb_env_open, 0);
  PL_register_foreign("pl_lmdb_env_close", 1, swi_lmdb_env_close, 0);
}

/* definitions */

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

foreign_t swi_lmdb_env_open(term_t in_env, term_t in_path, term_t in_flags, term_t in_mode)
{
    if (PL_is_variable(in_env)) { PL_fail; }
    MDB_env *env = NULL;
    if (!PL_get_pointer(in_env, (void**)&env)) { PL_fail; }

    if (PL_is_variable(in_path)) { PL_fail; }
    char *s_path;
    if (!PL_get_chars(in_path, &s_path, CVT_ATOM|CVT_STRING)) { PL_fail; }

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
