#!/usr/bin/env python

import sys
def gen_test(types, values):
    for a in range(len(types)):
        for b in range(len(types)):
            print
            print "CREATE TABLE t (a %s);" % (types[a])
            if a != b: # a > b:
                print "--replace_regex /MariaDB/XYZ/ /MySQL/XYZ/"
                print "--error ER_UNSUPPORTED_EXTENSION"
            else:
                for x in values[a]:
                    print "INSERT INTO t VALUES (", x, ");"
                print "CREATE TABLE ti LIKE t;"
                print "ALTER TABLE ti ENGINE=myisam;"
                print "INSERT INTO ti SELECT * from t;"
                print "ALTER TABLE ti CHANGE COLUMN a aa %s;" % (types[b])
            print "ALTER TABLE t CHANGE COLUMN a aa %s;" % (types[b])
            if a != b: # a > b:
                pass
            else:
                print "ALTER TABLE t CHANGE COLUMN aa a %s;" % (types[b])
                print "ALTER TABLE ti CHANGE COLUMN aa a %s;" % (types[b])
                print "let $diff_tables = test.t, test.ti;"
                print "source include/diff_tables.inc;"
                print "DROP TABLE ti;"
            print "DROP TABLE t;"
def main():
    print "source include/have_tokudb.inc;"
    print "# this test is generated by change_int_rename.py"
    print "--disable_warnings"
    print "DROP TABLE IF EXISTS t, ti;"
    print "--enable_warnings"
    print "SET SESSION DEFAULT_STORAGE_ENGINE=\"TokuDB\";"
    print "SET SESSION TOKUDB_DISABLE_SLOW_ALTER=1;"
    gen_test(
        [ "TINYINT", "SMALLINT", "MEDIUMINT", "INT", "BIGINT" ], 
        [ [ -128, -1, 0, 1, 127 ], [ -32768, -1, 0, 1, 32767], [-8388608, -1, 0, 1, 8388607], [-2147483648, 0, 1, 2147483647], [-9223372036854775808, 0, 1, 9223372036854775807] ]
    )
    gen_test(
        [ "TINYINT UNSIGNED", "SMALLINT UNSIGNED", "MEDIUMINT UNSIGNED", "INT UNSIGNED", "BIGINT UNSIGNED" ],
        [ [ 0, 1, 255 ], [ 0, 1, 65535], [0, 1, 16777215], [0, 1, 4294967295], [0, 1, 18446744073709551615] ]
    )
    return 0
sys.exit(main())
