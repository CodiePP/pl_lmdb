/*
 *  mapping of op name to op id
 *
 */

struct lmdb_cursor_op_itm {
  MDB_cursor_op _op;
  const char *_nm;
};
static
struct lmdb_cursor_op_itm lmdb_cursor_op_tbl[] = {
    { MDB_FIRST, "MDB_FIRST" },
    { MDB_FIRST_DUP, "MDB_FIRST_DUP" },
    { MDB_GET_BOTH, "MDB_GET_BOTH"},
    { MDB_GET_BOTH_RANGE, "MDB_GET_BOTH_RANGE"},
    { MDB_GET_CURRENT, "MDB_GET_CURRENT"},
    { MDB_GET_MULTIPLE, "MDB_GET_MULTIPLE"},
    { MDB_LAST, "MDB_LAST"},
    { MDB_LAST_DUP, "MDB_LAST_DUP"},
    { MDB_NEXT, "MDB_NEXT"},
    { MDB_NEXT_DUP, "MDB_NEXT_DUP"},
    { MDB_NEXT_MULTIPLE, "MDB_NEXT_MULTIPLE"},
    { MDB_NEXT_NODUP, "MDB_NEXT_NODUP"},
    { MDB_PREV, "MDB_PREV"},
    { MDB_PREV_DUP, "MDB_PREV_DUP"},
    { MDB_PREV_NODUP, "MDB_PREV_NODUP"},
    { MDB_SET, "MDB_SET"},
    { MDB_SET_KEY, "MDB_SET_KEY"},
    { MDB_SET_RANGE, "MDB_SET_RANGE"},
    { MDB_PREV_MULTIPLE, "MDB_PREV_MULTIPLE"},
  };

