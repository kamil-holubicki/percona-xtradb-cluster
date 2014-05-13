/*
 Copyright (c) 2014, Oracle and/or its affiliates. All rights
 reserved.
 
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; version 2 of
 the License.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 02110-1301  USA
*/

#include <NdbApi.hpp>

#include "adapter_global.h"
#include "AsyncNdbContext.h"
#include "DBSessionImpl.h"
#include "NdbWrappers.h"
#include "DBTransactionContext.h"
#include "DBOperationSet.h"

extern void setJsWrapper(DBTransactionContext *);
extern Persistent<Value> getWrappedObject(DBOperationSet *set);

const char * modes[4] = { "Prepare ","NoCommit","Commit  ","Rollback" };

DBTransactionContext::DBTransactionContext(DBSessionImpl *impl) :
  token(0),
  parent(impl),
  next(0),
  ndbTransaction(0),
  definedScan(0),
  tcNodeId(0)
{
  setJsWrapper(this);
  emptyOpSet = new DBOperationSet(this, 0);
  emptyOpSetWrapper = getWrappedObject(emptyOpSet);
}

DBTransactionContext::~DBTransactionContext() {
  DEBUG_MARKER(UDEB_DEBUG);
  jsWrapper.Dispose();
}


void DBTransactionContext::defineScan(ScanOperation *scanHelper) {
  assert(definedScan == 0);
  definedScan = scanHelper;
}


const NdbError & DBTransactionContext::getNdbError() {
  return ndbTransaction ? ndbTransaction->getNdbError() : parent->getNdbError();
}

bool DBTransactionContext::tryImmediateStartTransaction(KeyOperation * op) {
  token = parent->registerIntentToOpen();
  if(token == -1) {
    startTransaction(op);
    return true;
  }
  return false;
}

void DBTransactionContext::startTransaction(KeyOperation * op) {
  assert(ndbTransaction == 0);
  bool startWithHint = (op && op->key_buffer && (! definedScan)); 

  if(startWithHint) {
    char hash_buffer[512];        
    ndbTransaction = 
      parent->ndb->startTransaction(op->key_record->getNdbRecord(), 
                                    op->key_buffer, hash_buffer, 512);
  } else {
    ndbTransaction = parent->ndb->startTransaction();
  }

  tcNodeId = ndbTransaction ? ndbTransaction->getConnectedNodeId() : 0;
  DEBUG_PRINT("START TRANSACTION %s TC Node %d", 
              startWithHint ? "[with hint]" : "[ no hint ]", tcNodeId);
}

NdbScanOperation * DBTransactionContext::prepareAndExecuteScan() {
  NdbScanOperation * scanop;
  if(! ndbTransaction) {
    ndbTransaction = parent->ndb->startTransaction();
    tcNodeId = ndbTransaction ? ndbTransaction->getConnectedNodeId() : 0;
  }
  scanop = definedScan->prepareScan();
  ndbTransaction->execute(NdbTransaction::NoCommit, NdbOperation::AO_IgnoreError, 1);
  return scanop;
}

void DBTransactionContext::closeTransaction() {
  ndbTransaction->close();
}

void DBTransactionContext::registerClose() {
  ndbTransaction = 0;
  parent->registerTxClosed(token, tcNodeId);
}

int DBTransactionContext::execute(DBOperationSet *operations, 
                                  int _execType, int _abortOption, int force) {
  int rval;
  int opListSize = operations->size;
  NdbTransaction::ExecType execType = static_cast<NdbTransaction::ExecType>(_execType);
  NdbOperation::AbortOption abortOption = static_cast<NdbOperation::AbortOption>(_abortOption);
  bool didClose = false;
  
  if(! ndbTransaction) {
    startTransaction(operations->getKeyOperation(0));
  }
  operations->prepare(ndbTransaction);
  rval = ndbTransaction->execute(execType, abortOption, force);
  if(execType != NdbTransaction::NoCommit) {
    closeTransaction();
    didClose = true;
  }
  DEBUG_PRINT("EXECUTE sync : %s %d operation%s %s => return: %d",
              modes[execType], 
              opListSize, 
              (opListSize == 1 ? "" : "s"), 
              (didClose ? " & close transaction" : ""),
              rval);
  return rval;
}

int DBTransactionContext::executeAsynch(DBOperationSet *operations,  
                                        int execType, int abortOption, int forceSend, 
                                        v8::Persistent<v8::Function> callback) {
  assert(ndbTransaction);
  operations->prepare(ndbTransaction);
  int opListSize = operations->size;
  DEBUG_PRINT("EXECUTE async: %s %d operation%s", modes[execType], 
              opListSize, (opListSize == 1 ? "" : "s"));
  return parent->asyncContext->executeAsynch(this, ndbTransaction, 
                                             execType, abortOption, forceSend, 
                                             callback);
}                    


bool DBTransactionContext::clear() {
  /* Cannot clear if NdbTransaction is still open */
  if(ndbTransaction) return false;
  definedScan = 0;
  return true;
}

