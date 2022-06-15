#!/bin/bash

# Unit tests will be executed by worker 1, so do not assign galera suites, wich are executed with less parallelism
#WORKER_1_MTR_SUITES=innodb_undo,test_services,audit_null,service_sys_var_registration,connection_control,data_masking,binlog_57_decryption,service_udf_registration,service_status_var_registration,procfs,interactive_utilities,percona-pam-for-mysql
#WORKER_2_MTR_SUITES=galera,galera_nbo,galera_3nodes,galera_sr,galera_3nodes_nbo,galera_3nodes_sr,wsrep
#WORKER_3_MTR_SUITES=engines/funcs,innodb
#WORKER_4_MTR_SUITES=rpl,main
#WORKER_5_MTR_SUITES=rpl_nogtid,rpl_gtid
#WORKER_6_MTR_SUITES=parts,group_replication,clone,innodb_gis
#WORKER_7_MTR_SUITES=stress,perfschema,component_keyring_file,binlog,innodb_fts,sys_vars,innodb_zip,x,gcol,engines/iuds,encryption,federated,funcs_1,auth_sec,binlog_nogtid,binlog_gtid,funcs_2,jp,information_schema,rpl_encryption,sysschema,json,opt_trace,audit_log,collations,gis,query_rewrite_plugins,test_service_sql_api,secondary_engine
#WORKER_8_MTR_SUITES=
WORKER_1_MTR_SUITES=wsrep
WORKER_2_MTR_SUITES=galera_3nodes_sr
WORKER_3_MTR_SUITES=galera_3nodes_nbo
WORKER_4_MTR_SUITES=galera_sr
WORKER_5_MTR_SUITES=galera_3nodes
WORKER_6_MTR_SUITES=galera_nbo
WORKER_7_MTR_SUITES=galera
WORKER_8_MTR_SUITES=
INPUT=${2:-./mysql-test-run.pl}

check() {
  if [[ ! -f ${INPUT} ]]
    then
    echo "${INPUT} file does not exist on your filesystem."
    exit 1
  fi
  exit 0
  echo "Checking if suites list is consistent with the one specified in mysql-test-run.pl"
  echo
  
  local all_suites_1=,${WORKER_1_MTR_SUITES},${WORKER_2_MTR_SUITES},${WORKER_3_MTR_SUITES},${WORKER_4_MTR_SUITES},${WORKER_5_MTR_SUITES},${WORKER_6_MTR_SUITES},${WORKER_7_MTR_SUITES},${WORKER_8_MTR_SUITES},
  
  local all_suites_2=
  local capturing=0
  while read -r line
  do
    if [[ "${capturing}" == "1" ]]; then
      if [[ "${line}" == *");"* ]]; then
        capturing=0
        break
      fi
    fi

    if [[ "$capturing" == "1" ]]; then
      local all_suites_2=${all_suites_2}${line},
    fi
    
    if [[ "${line}" == *"DEFAULT_SUITES = qw"* ]]; then
      capturing=1
    fi
    
  done < "${INPUT}"

  echo "Suites for Jenkins: ${all_suites_1}"
  echo
  echo "Suites from mysql-test-run.pl: ${all_suites_2}"
  echo 
  
  local failure=0
  for suite in ${all_suites_2//,/ }
  do
    # check if the suite from pl scipt is assigned to any worker
    if [[ ${all_suites_1} != *",${suite},"* ]]; then
      echo "${suite} MISSING"
      failure=1
    fi
  done
 
  echo "************************"
  if [[ "${failure}" == "1" ]]; then
    echo "Inconsitencies detected"
  else
    echo "Everything is OK"
  fi
  echo "************************"
  
  exit ${failure}
}

case "$1" in
  'check')
    check
    ;;
esac
