/*
 Copyright (c) 2012, Oracle and/or its affiliates. All rights
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

"use strict";

var assert = require("assert");
var commonDBTableHandler = require("./DBTableHandler.js");

var apiSession = require("../../api/Session.js");

/** Create a function to manage the context of a user's asynchronous call.
 * All asynchronous user functions make a callback passing
 * the user's extra parameters from the original call as extra parameters
 * beyond the specified parameters of the call. For example, the persist function
 * is specified to take two parameters: the data object itself and the callback.
 * The result of persist is to call back with parameters of an error object, 
 * and the same data object which was passed. 
 * If extra parameters are passed to the persist function, the user's function
 * will be called with the specified parameters plus all extra parameters from
 * the original call. 
 * The constructor remembers the original user callback function and the original
 * parameters of the function.
 * The user callback function is always the last required parameter of the function call.
 * Additional context is added as the function progresses.
 * @param user_arguments the original arguments as supplied by the user
 * @param required_parameter_count the number of required parameters 
 * NOTE: the user callback function must be the last of the required parameters
 * @param returned_parameter_count the number of required parameters returned to the callback
 * @param session the Session which may be null for SessionFactory functions
 * @param session_factory the SessionFactory which may be null for Session functions
 */
exports.UserContext = function(user_arguments, required_parameter_count, returned_parameter_count,
    session, session_factory) {
  if (arguments.length !== 5) {
    throw new Error(
        'Fatal internal exception: wrong parameter count ' + arguments.length + ' for UserContext constructor' +
        '; expected ' + this.returned_parameter_count);
  }
  this.user_arguments = user_arguments;
  this.user_callback = user_arguments[required_parameter_count - 1];
  this.required_parameter_count = required_parameter_count;
  this.extra_arguments_count = user_arguments.length - required_parameter_count;
  this.returned_parameter_count = returned_parameter_count;
  this.session = session;
  this.session_factory = session_factory;
};


/** Get table metadata.
 * Delegate to DBConnectionPool.getTableMetadata.
 */
exports.UserContext.prototype.getTableMetadata = function() {
  var userContext = this;
  var getTableMetadataOnTableMetadata = function(err, tableMetadata) {
    userContext.applyCallback(err, tableMetadata);
  };
  
  var databaseName = this.user_arguments[0];
  var tableName = this.user_arguments[1];
  var dbSession = (this.session)?this.session.dbSession:null;
  this.session_factory.dbConnectionPool.getTableMetadata(
      databaseName, tableName, dbSession, getTableMetadataOnTableMetadata);
};


/** List all tables in the default database.
 * Delegate to DBConnectionPool.listTables.
 */
exports.UserContext.prototype.listTables = function() {
  var userContext = this;
  var listTablesOnTableList = function(err, tableList) {
    userContext.applyCallback(err, tableList);
  };

  var databaseName = this.user_arguments[0];
  var dbSession = (this.session)?this.session.dbSession:null;
  this.session_factory.dbConnectionPool.listTables(databaseName, dbSession, listTablesOnTableList);
};

/** Get the table handler for a table name or prototype.
 */
var getTableHandler = function(tableNameOrPrototype, session, onTableHandler) {

  var TableHandlerFactory = function(tableName, sessionFactory, dbSession, mapping, onTableHandler) {
    this.tableName = tableName;
    this.sessionFactory = sessionFactory;
    this.dbSession = dbSession;
    this.onTableHandler = onTableHandler;
    this.mapping = mapping;
    
    this.createTableHandler = function() {
      var tableHandlerFactory = this;
      var tableHandler;
      var dbName;
      
      var onTableMetadata = function(err, tableMetadata) {
        if (err) {
          tableHandlerFactory.onTableHandler(err, null);
        } else {
          udebug.log('UserContext.TableHandlerFactory.onTableMetadata for ' + dbName + '.' + tableName);
          // we have the table metadata; now create the table handler
          var tableHandler = new commonDBTableHandler.DBTableHandler(tableMetadata, mapping);
          if (mapping == null) {
            // put the default table handler into the session factory
            tableHandlerFactory.sessionFactory.tableHandlers[tableHandlerFactory.tableName] = tableHandler;
          }
        }
        tableHandlerFactory.onTableHandler(null, tableHandler);
      };
      
      // start of createTableHandler
      
      // get the table metadata from the db connection pool
      // getTableMetadata(dbSession, databaseName, tableName, callback(error, DBTable));
      dbName = this.sessionFactory.properties.database;
      udebug.log('UserContext.TableHandlerFactory.createTableHandler for ' + dbName + '.' + tableName);
      this.sessionFactory.dbConnectionPool.getTableMetadata(dbName, tableName, session.dbSession,
          onTableMetadata);
    };
  };
    
  // start of getTableHandler 
  
  if (typeof(tableNameOrPrototype) === 'string') {
    // parameter is a table name; look up in table name to table handler hash
    var tableHandler = session.sessionFactory.tableHandlers[tableNameOrPrototype];
    if (typeof(tableHandler) === 'undefined') {
      // create a new table handler for a table name with no mapping
      // create a closure to create the table handler
      var tableHandlerFactory = new TableHandlerFactory(
          tableNameOrPrototype, session.sessionFactory, session.dbSession, null, onTableHandler);
      tableHandlerFactory.createTableHandler(null);
    } else {
      // send back the tableHandler
      onTableHandler(null, tableHandler);
    }
  } else {
    // parameter is a prototype; it must have been annotated already
    if (typeof(tableNameOrPrototype.mynode) === 'undefined') {
      var err = new Error('User exception: prototype must have been annotated.');
      onTableHandler(err, null);
    } else {
      tableHandler = tableNameOrPrototype.mynode.tableHandler;
      if (typeof(tableHandler) === 'undefined') {
        // create the tableHandler
        // getTableMetadata(dbSession, databaseName, tableName, callback(error, DBTable));
        var tableHandlerFactory = new TableHandlerFactory(
            tableNameOrPrototype.mynode.mapping.table, session.sessionFactory, session.dbSession, tableNameOrPrototype.mynode.mapping, onTableHandler);
        tableHandlerFactory.createTableHandler();
      } else {
      // prototype has been annotated; return the table handler
      onTableHandler(null, tableHandler);
      }
    }
  }
};


/** Find the object by key.
 * 
 */
exports.UserContext.prototype.find = function() {
  var userContext = this;
  var tableHandler;

  function findOnTableHandler(err, dbTableHandler) {
    var keys, index, op;
    if (err) {
      userContext.applyCallback(err, null);
    } else {
      keys = userContext.user_arguments[1];
      index = dbTableHandler.getIndexHandler(keys);
      if (index === null) {
        var err = new Error('UserContext.find unable to get an index to use for ' + JSON.stringify(keys));
        userContext.applyCallback(err, null);
      } else {
        var err = new Error('UserContext.find using index ' + index.dbIndex.name + ' to be continued...');
        userContext.applyCallback(err, 'this is the user Session object');      
      }
    }
  }

  // find starts here
  // session.find(prototypeOrTableName, key, callback)
  // get DBTableHandler for prototype/tableName
  getTableHandler(userContext.user_arguments[0], userContext.session, findOnTableHandler);
};


/** Persist the object.
 * 
 */
exports.UserContext.prototype.persist = function() {
  var userContext = this;
  var tableHandler;
  function persistOnTableHandler(err, dbTableHandler) {
    var op;
    if (err) {
      userContext.applyCallback(err, null);
    } else {
    op = userContext.session.dbSession.buildInsertOperation(dbTableHandler, userContext.user_arguments[1]);
    var err = new Error('UserContext.persist will continue after a brief intermission...');
    this.applyCallback(err, null);
    // now what
    }
  }

  // persist starts here
  // persist(object, callback)
  // get DBTableHandler for prototype/tableName
  getTableHandler(userContext.user_arguments[0], userContext.session, persistOnTableHandler);
};

/** Open a session. Allocate a slot in the session factory sessions array.
 * Call the DBConnectionPool to create a new DBSession.
 * Wrap the DBSession in a new Session and return it to the user. 
 */
exports.UserContext.prototype.openSession = function() {
  var userContext = this;
  var openSessionOnSessionCreated = function(err, dbSession) {
    if (err) {
      userContext.applyCallback(err, null);
    } else {
      userContext.session = new apiSession.Session(userContext.session_index, userContext.session_factory, dbSession);
      userContext.session_factory.sessions[userContext.session_index] = userContext.session;
      userContext.applyCallback(err, userContext.session);
    }
  };

  var i;
  // allocate a new session slot in sessions
  for (i = 0; i < this.session_factory.sessions.length; ++i) {
    if (this.session_factory.sessions[i] === null) {
      break;
    }
  }
  this.session_factory.sessions[i] = {'placeholder':true, 'index':i};
  // remember the session index
  this.session_index = i;
  // get a new DBSession from the DBConnectionPool
  this.session_factory.dbConnectionPool.getDBSession(i, openSessionOnSessionCreated);
};

/** Complete the user function by calling back the user with the results of the function.
 * Apply the user callback using the current arguments and the extra parameters from the original function.
 * Create the args for the callback by copying the current arguments to this function. Then, copy
 * the extra parameters from the original function. Finally, call the user callback.
 */
exports.UserContext.prototype.applyCallback = function() {
  if (arguments.length !== this.returned_parameter_count) {
    throw new Error(
        'Fatal internal exception: wrong parameter count ' + arguments.length +' for UserContext applyCallback' + 
        '; expected ' + this.returned_parameter_count);
  }
  var args = [];
  var i, j;
  for (i = 0; i < arguments.length; ++i) {
    args.push(arguments[i]);
  }
  for (j = this.required_parameter_count; j < this.user_arguments.length; ++j) {
    args.push(this.user_arguments[j]);
  }
  this.user_callback.apply(null, args);
};
