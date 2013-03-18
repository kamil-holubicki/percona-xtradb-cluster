#!/usr/bin/perl -w
# Copyright (c) 2012, Oracle and/or its affiliates. All rights
# reserved.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; version 2 of
# the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
# 02110-1301  USA
# This module contains functions which will support the testing of mysqld options givven on command line
# or in the configuration file (my.cnf).
# Creator: Horst Hunger
# Date:    2012-09-17
#
package server_option_test;
use strict;

use base qw(Exporter);
our @EXPORT= qw(init_bootstrap fini_bootstrap check_bootstrap_log
                insert_option_my_cnf create_var_stmt my_cnf_bootstrap
	        commandline_bootstrap);

our $mysqld_bootstrap_cmd= "$ENV{'MYSQLD_BOOTSTRAP_CMD'} --datadir=$ENV{'MYSQLTEST_VARDIR'}/tmp/bootstrap/data/ --tmpdir=$ENV{'MYSQLTEST_VARDIR'}/tmp/bootstrap/tmp/ --log-error=$ENV{'MYSQLTEST_VARDIR'}/tmp/bootstrap/log/bootstrap.log --default-storage-engine=InnoDB --default-tmp-storage-engine=InnoDB";

our $mysqld_bootstrap= "$ENV{'MYSQLD'} --defaults-file=$ENV{'MYSQLTEST_VARDIR'}/tmp/bootstrap/my.cnf --bootstrap --core-file --datadir=$ENV{'MYSQLTEST_VARDIR'}/tmp/bootstrap/data/ --tmpdir=$ENV{'MYSQLTEST_VARDIR'}/tmp/bootstrap/tmp/";
#
# Creates the directories needed by bootstrap cmd.
#
sub init_bootstrap {
mkdir("$ENV{'MYSQLTEST_VARDIR'}/tmp/bootstrap",0744);
mkdir("$ENV{'MYSQLTEST_VARDIR'}/tmp/bootstrap/data",0744);
mkdir("$ENV{'MYSQLTEST_VARDIR'}/tmp/bootstrap/tmp",0744);
mkdir("$ENV{'MYSQLTEST_VARDIR'}/tmp/bootstrap/log",0744);
}

#
# Removes the directories needed by bootstrap cmd.
#
sub fini_bootstrap {
use File::Path 'rmtree';
rmtree("$ENV{'MYSQLTEST_VARDIR'}/tmp/bootstrap");
}

#
# Creates a SELECT variable into file to be executed by bootstrap cmd.
# Bootstrap will fail if not such an input file will be given.
# The result of the sql execution has no meaning in the option test, but may also cause a bug
# and is therefore interesting for testing.
#
sub create_var_stmt ($) {
my $test_var= shift;
# Input file for bootstrap cmd (mysql_install_db) containing a SELECT var otherwise it is failing.
# It can also be used for test purposes.
my $fname= "$ENV{'MYSQLTEST_VARDIR'}/tmp/bootstrap/tmp/bootstrap_in.sql";
my @wrlines;
if ($test_var) {
  $test_var=~ s/--//;
  $test_var=~ s/loose-//;
  $test_var=~ s/-/_/g;
  $test_var=~ s/=.*//;
  push(@wrlines,"SELECT \@\@GLOBAL.$test_var INTO OUTFILE \'$ENV{'MYSQLTEST_VARDIR'}/tmp/bootstrap/tmp/bootstrap.out\';\n");
  open(FILE, ">", $fname) or die "Error: Cannot open bootstrap_in.sql for writing.\n";
  print FILE @wrlines;
  close FILE;
}
}

#
# Checks the bootstrap.log if containing the pattern
#
sub check_bootstrap_log ($) {
my @patterns= @_;
my $found_pattern= 0;
my $fname= "$ENV{'MYSQLTEST_VARDIR'}/tmp/bootstrap/log/bootstrap.log";
open(FILE, "<", $fname) or die "\nError: Cannot read bootstrap.log.\nError: Nothing has been written to the log file, which might be suspicious in case of wrong/missing value for the option. It is recommended to file a bug about the weak handling of such values.\n\n";
my @lines= <FILE>;
# those must be in the file for the test to pass
foreach my $one_line (@lines)
{
  foreach my $one_pattern (@_)
  {
   # print pattern, not line, to get a stable output
   # For test purposes: 
   # print "$one_line\n" if ($one_line =~ /$one_pattern/);
   $found_pattern= 1  if ($one_line =~ /$one_pattern/);
  }
}
close FILE;
return $found_pattern;
}

#
# Inserts a startup option into my.cnf.
#
sub insert_option_my_cnf ($) {
use strict;
my $test_option= shift;
my $paragraph= "mysqld";
my $fname= "$ENV{'MYSQLTEST_VARDIR'}/tmp/bootstrap/my.cnf";
my @wrlines;
# those must be in the file for the test to pass
my $nb_rm_files= unlink("$ENV{'MYSQLTEST_VARDIR'}/tmp/bootstrap/log/bootstrap.log");
$test_option=~ s/--//;
#print "$test_option\n";
# Create a mini config file including the option to be tested.
# This config file is relying on "small.cnf" (see user manual).
push(@wrlines,"[mysqld]\n");
push(@wrlines,"open-files-limit=1024\n");
push(@wrlines,"local-infile\n");
push(@wrlines,"character-set-server=latin1\n");
push(@wrlines,"connect-timeout=60\n");
push(@wrlines,"socket=$ENV{'MYSQLTEST_VARDIR'}/tmp/bootstrap/tmp/mysqld.sock\n");
push(@wrlines,"skip-external-locking\n");
push(@wrlines,"key_buffer_size=16K\n");
push(@wrlines,"max_allowed_packet=1M\n");
push(@wrlines,"table_open_cache=4\n");
push(@wrlines,"sort_buffer_size=64K\n");
push(@wrlines,"read_buffer_size=256K\n");
push(@wrlines,"read_rnd_buffer_size=256K\n");
push(@wrlines,"net_buffer_length=2K\n");
push(@wrlines,"thread_stack=128K\n");
push(@wrlines,"server-id=1\n");

push(@wrlines,$test_option."\n");

push(@wrlines,"\n");
push(@wrlines,"[client]\n");
push(@wrlines,"password=\n");
push(@wrlines,"host=localhost\n");
push(@wrlines,"user=root\n");
push(@wrlines,"socket=$ENV{'MYSQLTEST_VARDIR'}/tmp/bootstrap/tmp/mysqld.sock\n");

open(FILE, ">", $fname) or die "Error: Cannot open my.cnf for writing.\n";
print FILE @wrlines;
close FILE;
}

#
# Startup option on commandline and exec bootstrap with SELECT variable.
#
sub commandline_bootstrap ($) {
my $test_option= shift;
create_var_stmt($test_option);
my $nb_rm_files= unlink("$ENV{'MYSQLTEST_VARDIR'}/tmp/bootstrap/log/bootstrap.log");
$nb_rm_files= unlink("$ENV{'MYSQLTEST_VARDIR'}/tmp/bootstrap/tmp/bootstrap.out");
my $ret_code= qx($mysqld_bootstrap_cmd $test_option < $ENV{'MYSQLTEST_VARDIR'}/tmp/bootstrap/tmp/bootstrap_in.sql 2>&1);
#print "$mysqld_bootstrap_cmd\n";
#print "retcode: $ret_code\n";
}

#
# Inserts a startup option into my.cnf and exec bootstrap with SELECT variable.
#
sub my_cnf_bootstrap ($) {
my $test_option= shift;
insert_option_my_cnf($test_option);
create_var_stmt($test_option);
my $nb_rm_files= unlink("$ENV{'MYSQLTEST_VARDIR'}/tmp/bootstrap/log/bootstrap.log");
$nb_rm_files= unlink("$ENV{'MYSQLTEST_VARDIR'}/tmp/bootstrap/tmp/bootstrap.out");
my $ret_code= qx($mysqld_bootstrap < $ENV{'MYSQLTEST_VARDIR'}/tmp/bootstrap/tmp/bootstrap_in.sql > $ENV{'MYSQLTEST_VARDIR'}/tmp/bootstrap/log/bootstrap.log 2>&1);
#print "$mysqld_bootstrap\n";
#print "retcode: $ret_code\n";
}

1;

