#!/usr/bin/env python2

import sys
def gen_test(types):
    for a in range(len(types)):
        for b in range(len(types)):
            if a < b:
                print
                print "CREATE TABLE t (a %s, PRIMARY KEY(a));" % (types[a])
                print "--replace_regex /MariaDB/XYZ/ /MySQL/XYZ/"
                print "--error ER_UNSUPPORTED_EXTENSION"
                print "ALTER TABLE t CHANGE COLUMN a a %s;" % (types[b])
                print "DROP TABLE t;"

                print "CREATE TABLE t (a %s, KEY(a));" % (types[a])
                print "--replace_regex /MariaDB/XYZ/ /MySQL/XYZ/"
                print "--error ER_UNSUPPORTED_EXTENSION"
                print "ALTER TABLE t CHANGE COLUMN a a %s;" % (types[b])
                print "DROP TABLE t;"

                print "CREATE TABLE t (a %s, CLUSTERING KEY(a));" % (types[a])
                print "--replace_regex /MariaDB/XYZ/ /MySQL/XYZ/"
                print "--error ER_UNSUPPORTED_EXTENSION"
                print "ALTER TABLE t CHANGE COLUMN a a %s;" % (types[b])
                print "DROP TABLE t;"

def main():
    print "source include/have_tokudb.inc;"
    print "# this test is generated by change_int_key.py"
    print "# ensure that changing an int column that is part of a key is not hot"
    print "--disable_warnings"
    print "DROP TABLE IF EXISTS t;"
    print "--enable_warnings"
    print "SET SESSION DEFAULT_STORAGE_ENGINE=\"TokuDB\";"
    print "SET SESSION TOKUDB_DISABLE_SLOW_ALTER=1;"
    gen_test([ "TINYINT", "SMALLINT", "MEDIUMINT", "INT", "BIGINT" ])
    gen_test([ "TINYINT UNSIGNED", "SMALLINT UNSIGNED", "MEDIUMINT UNSIGNED", "INT UNSIGNED", "BIGINT UNSIGNED" ])
    return 0
sys.exit(main())
