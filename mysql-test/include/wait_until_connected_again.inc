#
# Include this script to wait until the connection to the
# server has been restored or timeout occurs
#
# Parameters:
# $wait_counter - can be set before calling the script, will be reset at the end
#
# $explicit_default_counter - can be set before calling the script,
#                             reset only at the end of the test, overrides $wait_counter

#
# We require two consequtive connection successes in order to
# work around a race condition on Galera startup where the server
# can temporarily accept queries before starting to reject them again
#

--disable_result_log
--disable_query_log
# Wsrep change: The native $counter initial value was 500, but this
# may not be enough for all tests which restart a node which joins
# back with SST.
--let $counter= 5000
if ($explicit_default_counter)
{
  --let $wait_counter= $explicit_default_counter
}
if ($VALGRIND_TEST) {
  --let $wait_counter= 30000
}
if ($wait_counter) {
  --let $counter= $wait_counter
}
let $mysql_errno= 9999;
let $successes= 2;

while ($successes) {
  while ($mysql_errno) {
    # Strangely enough, the server might return "Too many connections"
    # while being shutdown, thus 1040 is an "allowed" error
    # See BUG#36228
    --error 0,1040,1047,1053,1205,2002,2003,2006,2013,1045,ER_SECURE_TRANSPORT_REQUIRED,ER_SECURE_TRANSPORT_REQUIRED
    show session status;
    if ($mysql_errno == 1045){
      let mysql_errno=0;
    }
    if ($mysql_errname == ER_SECURE_TRANSPORT_REQUIRED){
      let mysql_errno=0;
    }

    --dec $counter
    if (!$counter) {
      --die Server failed to restart
    }
     --sleep 0.1
  }
  --dec $successes
  --sleep 0.1
}
# Reset $wait_counter so that its value won't be used on subsequent
# calls, and default will be used instead.
if (!$explicit_default_counter)
{
  --let $wait_counter= 0
}
--enable_query_log
--enable_result_log
