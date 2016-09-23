// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: mysqlx.proto

#ifndef PROTOBUF_mysqlx_2eproto__INCLUDED
#define PROTOBUF_mysqlx_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2006000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2006000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/message_lite.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include "mysqlx_sql.pb.h"
#include "mysqlx_resultset.pb.h"
#include "mysqlx_crud.pb.h"
#include "mysqlx_session.pb.h"
#include "mysqlx_connection.pb.h"
#include "mysqlx_expect.pb.h"
#include "mysqlx_notice.pb.h"
// @@protoc_insertion_point(includes)

namespace Mysqlx {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_mysqlx_2eproto();
void protobuf_AssignDesc_mysqlx_2eproto();
void protobuf_ShutdownFile_mysqlx_2eproto();

class ClientMessages;
class ServerMessages;
class Ok;
class Error;

enum ClientMessages_Type {
  ClientMessages_Type_CON_CAPABILITIES_GET = 1,
  ClientMessages_Type_CON_CAPABILITIES_SET = 2,
  ClientMessages_Type_CON_CLOSE = 3,
  ClientMessages_Type_SESS_AUTHENTICATE_START = 4,
  ClientMessages_Type_SESS_AUTHENTICATE_CONTINUE = 5,
  ClientMessages_Type_SESS_RESET = 6,
  ClientMessages_Type_SESS_CLOSE = 7,
  ClientMessages_Type_SQL_STMT_EXECUTE = 12,
  ClientMessages_Type_CRUD_FIND = 17,
  ClientMessages_Type_CRUD_INSERT = 18,
  ClientMessages_Type_CRUD_UPDATE = 19,
  ClientMessages_Type_CRUD_DELETE = 20,
  ClientMessages_Type_EXPECT_OPEN = 24,
  ClientMessages_Type_EXPECT_CLOSE = 25
};
bool ClientMessages_Type_IsValid(int value);
const ClientMessages_Type ClientMessages_Type_Type_MIN = ClientMessages_Type_CON_CAPABILITIES_GET;
const ClientMessages_Type ClientMessages_Type_Type_MAX = ClientMessages_Type_EXPECT_CLOSE;
const int ClientMessages_Type_Type_ARRAYSIZE = ClientMessages_Type_Type_MAX + 1;

enum ServerMessages_Type {
  ServerMessages_Type_OK = 0,
  ServerMessages_Type_ERROR = 1,
  ServerMessages_Type_CONN_CAPABILITIES = 2,
  ServerMessages_Type_SESS_AUTHENTICATE_CONTINUE = 3,
  ServerMessages_Type_SESS_AUTHENTICATE_OK = 4,
  ServerMessages_Type_NOTICE = 11,
  ServerMessages_Type_RESULTSET_COLUMN_META_DATA = 12,
  ServerMessages_Type_RESULTSET_ROW = 13,
  ServerMessages_Type_RESULTSET_FETCH_DONE = 14,
  ServerMessages_Type_RESULTSET_FETCH_SUSPENDED = 15,
  ServerMessages_Type_RESULTSET_FETCH_DONE_MORE_RESULTSETS = 16,
  ServerMessages_Type_SQL_STMT_EXECUTE_OK = 17,
  ServerMessages_Type_RESULTSET_FETCH_DONE_MORE_OUT_PARAMS = 18
};
bool ServerMessages_Type_IsValid(int value);
const ServerMessages_Type ServerMessages_Type_Type_MIN = ServerMessages_Type_OK;
const ServerMessages_Type ServerMessages_Type_Type_MAX = ServerMessages_Type_RESULTSET_FETCH_DONE_MORE_OUT_PARAMS;
const int ServerMessages_Type_Type_ARRAYSIZE = ServerMessages_Type_Type_MAX + 1;

enum Error_Severity {
  Error_Severity_ERROR = 0,
  Error_Severity_FATAL = 1
};
bool Error_Severity_IsValid(int value);
const Error_Severity Error_Severity_Severity_MIN = Error_Severity_ERROR;
const Error_Severity Error_Severity_Severity_MAX = Error_Severity_FATAL;
const int Error_Severity_Severity_ARRAYSIZE = Error_Severity_Severity_MAX + 1;

// ===================================================================

class ClientMessages : public ::google::protobuf::MessageLite {
 public:
  ClientMessages();
  virtual ~ClientMessages();

  ClientMessages(const ClientMessages& from);

  inline ClientMessages& operator=(const ClientMessages& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::std::string& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::std::string* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ClientMessages& default_instance();

  #ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  // Returns the internal default instance pointer. This function can
  // return NULL thus should not be used by the user. This is intended
  // for Protobuf internal code. Please use default_instance() declared
  // above instead.
  static inline const ClientMessages* internal_default_instance() {
    return default_instance_;
  }
  #endif

  void Swap(ClientMessages* other);

  // implements Message ----------------------------------------------

  ClientMessages* New() const;
  void CheckTypeAndMergeFrom(const ::google::protobuf::MessageLite& from);
  void CopyFrom(const ClientMessages& from);
  void MergeFrom(const ClientMessages& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  void DiscardUnknownFields();
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  ::std::string GetTypeName() const;

  // nested types ----------------------------------------------------

  typedef ClientMessages_Type Type;
  static const Type CON_CAPABILITIES_GET = ClientMessages_Type_CON_CAPABILITIES_GET;
  static const Type CON_CAPABILITIES_SET = ClientMessages_Type_CON_CAPABILITIES_SET;
  static const Type CON_CLOSE = ClientMessages_Type_CON_CLOSE;
  static const Type SESS_AUTHENTICATE_START = ClientMessages_Type_SESS_AUTHENTICATE_START;
  static const Type SESS_AUTHENTICATE_CONTINUE = ClientMessages_Type_SESS_AUTHENTICATE_CONTINUE;
  static const Type SESS_RESET = ClientMessages_Type_SESS_RESET;
  static const Type SESS_CLOSE = ClientMessages_Type_SESS_CLOSE;
  static const Type SQL_STMT_EXECUTE = ClientMessages_Type_SQL_STMT_EXECUTE;
  static const Type CRUD_FIND = ClientMessages_Type_CRUD_FIND;
  static const Type CRUD_INSERT = ClientMessages_Type_CRUD_INSERT;
  static const Type CRUD_UPDATE = ClientMessages_Type_CRUD_UPDATE;
  static const Type CRUD_DELETE = ClientMessages_Type_CRUD_DELETE;
  static const Type EXPECT_OPEN = ClientMessages_Type_EXPECT_OPEN;
  static const Type EXPECT_CLOSE = ClientMessages_Type_EXPECT_CLOSE;
  static inline bool Type_IsValid(int value) {
    return ClientMessages_Type_IsValid(value);
  }
  static const Type Type_MIN =
    ClientMessages_Type_Type_MIN;
  static const Type Type_MAX =
    ClientMessages_Type_Type_MAX;
  static const int Type_ARRAYSIZE =
    ClientMessages_Type_Type_ARRAYSIZE;

  // accessors -------------------------------------------------------

  // @@protoc_insertion_point(class_scope:Mysqlx.ClientMessages)
 private:

  ::std::string _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  #ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  friend void  protobuf_AddDesc_mysqlx_2eproto_impl();
  #else
  friend void  protobuf_AddDesc_mysqlx_2eproto();
  #endif
  friend void protobuf_AssignDesc_mysqlx_2eproto();
  friend void protobuf_ShutdownFile_mysqlx_2eproto();

  void InitAsDefaultInstance();
  static ClientMessages* default_instance_;
};
// -------------------------------------------------------------------

class ServerMessages : public ::google::protobuf::MessageLite {
 public:
  ServerMessages();
  virtual ~ServerMessages();

  ServerMessages(const ServerMessages& from);

  inline ServerMessages& operator=(const ServerMessages& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::std::string& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::std::string* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ServerMessages& default_instance();

  #ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  // Returns the internal default instance pointer. This function can
  // return NULL thus should not be used by the user. This is intended
  // for Protobuf internal code. Please use default_instance() declared
  // above instead.
  static inline const ServerMessages* internal_default_instance() {
    return default_instance_;
  }
  #endif

  void Swap(ServerMessages* other);

  // implements Message ----------------------------------------------

  ServerMessages* New() const;
  void CheckTypeAndMergeFrom(const ::google::protobuf::MessageLite& from);
  void CopyFrom(const ServerMessages& from);
  void MergeFrom(const ServerMessages& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  void DiscardUnknownFields();
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  ::std::string GetTypeName() const;

  // nested types ----------------------------------------------------

  typedef ServerMessages_Type Type;
  static const Type OK = ServerMessages_Type_OK;
  static const Type ERROR = ServerMessages_Type_ERROR;
  static const Type CONN_CAPABILITIES = ServerMessages_Type_CONN_CAPABILITIES;
  static const Type SESS_AUTHENTICATE_CONTINUE = ServerMessages_Type_SESS_AUTHENTICATE_CONTINUE;
  static const Type SESS_AUTHENTICATE_OK = ServerMessages_Type_SESS_AUTHENTICATE_OK;
  static const Type NOTICE = ServerMessages_Type_NOTICE;
  static const Type RESULTSET_COLUMN_META_DATA = ServerMessages_Type_RESULTSET_COLUMN_META_DATA;
  static const Type RESULTSET_ROW = ServerMessages_Type_RESULTSET_ROW;
  static const Type RESULTSET_FETCH_DONE = ServerMessages_Type_RESULTSET_FETCH_DONE;
  static const Type RESULTSET_FETCH_SUSPENDED = ServerMessages_Type_RESULTSET_FETCH_SUSPENDED;
  static const Type RESULTSET_FETCH_DONE_MORE_RESULTSETS = ServerMessages_Type_RESULTSET_FETCH_DONE_MORE_RESULTSETS;
  static const Type SQL_STMT_EXECUTE_OK = ServerMessages_Type_SQL_STMT_EXECUTE_OK;
  static const Type RESULTSET_FETCH_DONE_MORE_OUT_PARAMS = ServerMessages_Type_RESULTSET_FETCH_DONE_MORE_OUT_PARAMS;
  static inline bool Type_IsValid(int value) {
    return ServerMessages_Type_IsValid(value);
  }
  static const Type Type_MIN =
    ServerMessages_Type_Type_MIN;
  static const Type Type_MAX =
    ServerMessages_Type_Type_MAX;
  static const int Type_ARRAYSIZE =
    ServerMessages_Type_Type_ARRAYSIZE;

  // accessors -------------------------------------------------------

  // @@protoc_insertion_point(class_scope:Mysqlx.ServerMessages)
 private:

  ::std::string _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  #ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  friend void  protobuf_AddDesc_mysqlx_2eproto_impl();
  #else
  friend void  protobuf_AddDesc_mysqlx_2eproto();
  #endif
  friend void protobuf_AssignDesc_mysqlx_2eproto();
  friend void protobuf_ShutdownFile_mysqlx_2eproto();

  void InitAsDefaultInstance();
  static ServerMessages* default_instance_;
};
// -------------------------------------------------------------------

class Ok : public ::google::protobuf::MessageLite {
 public:
  Ok();
  virtual ~Ok();

  Ok(const Ok& from);

  inline Ok& operator=(const Ok& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::std::string& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::std::string* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const Ok& default_instance();

  #ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  // Returns the internal default instance pointer. This function can
  // return NULL thus should not be used by the user. This is intended
  // for Protobuf internal code. Please use default_instance() declared
  // above instead.
  static inline const Ok* internal_default_instance() {
    return default_instance_;
  }
  #endif

  void Swap(Ok* other);

  // implements Message ----------------------------------------------

  Ok* New() const;
  void CheckTypeAndMergeFrom(const ::google::protobuf::MessageLite& from);
  void CopyFrom(const Ok& from);
  void MergeFrom(const Ok& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  void DiscardUnknownFields();
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  ::std::string GetTypeName() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // optional string msg = 1;
  inline bool has_msg() const;
  inline void clear_msg();
  static const int kMsgFieldNumber = 1;
  inline const ::std::string& msg() const;
  inline void set_msg(const ::std::string& value);
  inline void set_msg(const char* value);
  inline void set_msg(const char* value, size_t size);
  inline ::std::string* mutable_msg();
  inline ::std::string* release_msg();
  inline void set_allocated_msg(::std::string* msg);

  // @@protoc_insertion_point(class_scope:Mysqlx.Ok)
 private:
  inline void set_has_msg();
  inline void clear_has_msg();

  ::std::string _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::std::string* msg_;
  #ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  friend void  protobuf_AddDesc_mysqlx_2eproto_impl();
  #else
  friend void  protobuf_AddDesc_mysqlx_2eproto();
  #endif
  friend void protobuf_AssignDesc_mysqlx_2eproto();
  friend void protobuf_ShutdownFile_mysqlx_2eproto();

  void InitAsDefaultInstance();
  static Ok* default_instance_;
};
// -------------------------------------------------------------------

class Error : public ::google::protobuf::MessageLite {
 public:
  Error();
  virtual ~Error();

  Error(const Error& from);

  inline Error& operator=(const Error& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::std::string& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::std::string* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const Error& default_instance();

  #ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  // Returns the internal default instance pointer. This function can
  // return NULL thus should not be used by the user. This is intended
  // for Protobuf internal code. Please use default_instance() declared
  // above instead.
  static inline const Error* internal_default_instance() {
    return default_instance_;
  }
  #endif

  void Swap(Error* other);

  // implements Message ----------------------------------------------

  Error* New() const;
  void CheckTypeAndMergeFrom(const ::google::protobuf::MessageLite& from);
  void CopyFrom(const Error& from);
  void MergeFrom(const Error& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  void DiscardUnknownFields();
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  ::std::string GetTypeName() const;

  // nested types ----------------------------------------------------

  typedef Error_Severity Severity;
  static const Severity ERROR = Error_Severity_ERROR;
  static const Severity FATAL = Error_Severity_FATAL;
  static inline bool Severity_IsValid(int value) {
    return Error_Severity_IsValid(value);
  }
  static const Severity Severity_MIN =
    Error_Severity_Severity_MIN;
  static const Severity Severity_MAX =
    Error_Severity_Severity_MAX;
  static const int Severity_ARRAYSIZE =
    Error_Severity_Severity_ARRAYSIZE;

  // accessors -------------------------------------------------------

  // optional .Mysqlx.Error.Severity severity = 1 [default = ERROR];
  inline bool has_severity() const;
  inline void clear_severity();
  static const int kSeverityFieldNumber = 1;
  inline ::Mysqlx::Error_Severity severity() const;
  inline void set_severity(::Mysqlx::Error_Severity value);

  // required uint32 code = 2;
  inline bool has_code() const;
  inline void clear_code();
  static const int kCodeFieldNumber = 2;
  inline ::google::protobuf::uint32 code() const;
  inline void set_code(::google::protobuf::uint32 value);

  // required string sql_state = 4;
  inline bool has_sql_state() const;
  inline void clear_sql_state();
  static const int kSqlStateFieldNumber = 4;
  inline const ::std::string& sql_state() const;
  inline void set_sql_state(const ::std::string& value);
  inline void set_sql_state(const char* value);
  inline void set_sql_state(const char* value, size_t size);
  inline ::std::string* mutable_sql_state();
  inline ::std::string* release_sql_state();
  inline void set_allocated_sql_state(::std::string* sql_state);

  // required string msg = 3;
  inline bool has_msg() const;
  inline void clear_msg();
  static const int kMsgFieldNumber = 3;
  inline const ::std::string& msg() const;
  inline void set_msg(const ::std::string& value);
  inline void set_msg(const char* value);
  inline void set_msg(const char* value, size_t size);
  inline ::std::string* mutable_msg();
  inline ::std::string* release_msg();
  inline void set_allocated_msg(::std::string* msg);

  // @@protoc_insertion_point(class_scope:Mysqlx.Error)
 private:
  inline void set_has_severity();
  inline void clear_has_severity();
  inline void set_has_code();
  inline void clear_has_code();
  inline void set_has_sql_state();
  inline void clear_has_sql_state();
  inline void set_has_msg();
  inline void clear_has_msg();

  ::std::string _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  int severity_;
  ::google::protobuf::uint32 code_;
  ::std::string* sql_state_;
  ::std::string* msg_;
  #ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  friend void  protobuf_AddDesc_mysqlx_2eproto_impl();
  #else
  friend void  protobuf_AddDesc_mysqlx_2eproto();
  #endif
  friend void protobuf_AssignDesc_mysqlx_2eproto();
  friend void protobuf_ShutdownFile_mysqlx_2eproto();

  void InitAsDefaultInstance();
  static Error* default_instance_;
};
// ===================================================================


// ===================================================================

// ClientMessages

// -------------------------------------------------------------------

// ServerMessages

// -------------------------------------------------------------------

// Ok

// optional string msg = 1;
inline bool Ok::has_msg() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void Ok::set_has_msg() {
  _has_bits_[0] |= 0x00000001u;
}
inline void Ok::clear_has_msg() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void Ok::clear_msg() {
  if (msg_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    msg_->clear();
  }
  clear_has_msg();
}
inline const ::std::string& Ok::msg() const {
  // @@protoc_insertion_point(field_get:Mysqlx.Ok.msg)
  return *msg_;
}
inline void Ok::set_msg(const ::std::string& value) {
  set_has_msg();
  if (msg_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    msg_ = new ::std::string;
  }
  msg_->assign(value);
  // @@protoc_insertion_point(field_set:Mysqlx.Ok.msg)
}
inline void Ok::set_msg(const char* value) {
  set_has_msg();
  if (msg_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    msg_ = new ::std::string;
  }
  msg_->assign(value);
  // @@protoc_insertion_point(field_set_char:Mysqlx.Ok.msg)
}
inline void Ok::set_msg(const char* value, size_t size) {
  set_has_msg();
  if (msg_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    msg_ = new ::std::string;
  }
  msg_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:Mysqlx.Ok.msg)
}
inline ::std::string* Ok::mutable_msg() {
  set_has_msg();
  if (msg_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    msg_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:Mysqlx.Ok.msg)
  return msg_;
}
inline ::std::string* Ok::release_msg() {
  clear_has_msg();
  if (msg_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = msg_;
    msg_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void Ok::set_allocated_msg(::std::string* msg) {
  if (msg_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete msg_;
  }
  if (msg) {
    set_has_msg();
    msg_ = msg;
  } else {
    clear_has_msg();
    msg_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:Mysqlx.Ok.msg)
}

// -------------------------------------------------------------------

// Error

// optional .Mysqlx.Error.Severity severity = 1 [default = ERROR];
inline bool Error::has_severity() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void Error::set_has_severity() {
  _has_bits_[0] |= 0x00000001u;
}
inline void Error::clear_has_severity() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void Error::clear_severity() {
  severity_ = 0;
  clear_has_severity();
}
inline ::Mysqlx::Error_Severity Error::severity() const {
  // @@protoc_insertion_point(field_get:Mysqlx.Error.severity)
  return static_cast< ::Mysqlx::Error_Severity >(severity_);
}
inline void Error::set_severity(::Mysqlx::Error_Severity value) {
  assert(::Mysqlx::Error_Severity_IsValid(value));
  set_has_severity();
  severity_ = value;
  // @@protoc_insertion_point(field_set:Mysqlx.Error.severity)
}

// required uint32 code = 2;
inline bool Error::has_code() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void Error::set_has_code() {
  _has_bits_[0] |= 0x00000002u;
}
inline void Error::clear_has_code() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void Error::clear_code() {
  code_ = 0u;
  clear_has_code();
}
inline ::google::protobuf::uint32 Error::code() const {
  // @@protoc_insertion_point(field_get:Mysqlx.Error.code)
  return code_;
}
inline void Error::set_code(::google::protobuf::uint32 value) {
  set_has_code();
  code_ = value;
  // @@protoc_insertion_point(field_set:Mysqlx.Error.code)
}

// required string sql_state = 4;
inline bool Error::has_sql_state() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void Error::set_has_sql_state() {
  _has_bits_[0] |= 0x00000004u;
}
inline void Error::clear_has_sql_state() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void Error::clear_sql_state() {
  if (sql_state_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    sql_state_->clear();
  }
  clear_has_sql_state();
}
inline const ::std::string& Error::sql_state() const {
  // @@protoc_insertion_point(field_get:Mysqlx.Error.sql_state)
  return *sql_state_;
}
inline void Error::set_sql_state(const ::std::string& value) {
  set_has_sql_state();
  if (sql_state_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    sql_state_ = new ::std::string;
  }
  sql_state_->assign(value);
  // @@protoc_insertion_point(field_set:Mysqlx.Error.sql_state)
}
inline void Error::set_sql_state(const char* value) {
  set_has_sql_state();
  if (sql_state_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    sql_state_ = new ::std::string;
  }
  sql_state_->assign(value);
  // @@protoc_insertion_point(field_set_char:Mysqlx.Error.sql_state)
}
inline void Error::set_sql_state(const char* value, size_t size) {
  set_has_sql_state();
  if (sql_state_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    sql_state_ = new ::std::string;
  }
  sql_state_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:Mysqlx.Error.sql_state)
}
inline ::std::string* Error::mutable_sql_state() {
  set_has_sql_state();
  if (sql_state_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    sql_state_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:Mysqlx.Error.sql_state)
  return sql_state_;
}
inline ::std::string* Error::release_sql_state() {
  clear_has_sql_state();
  if (sql_state_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = sql_state_;
    sql_state_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void Error::set_allocated_sql_state(::std::string* sql_state) {
  if (sql_state_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete sql_state_;
  }
  if (sql_state) {
    set_has_sql_state();
    sql_state_ = sql_state;
  } else {
    clear_has_sql_state();
    sql_state_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:Mysqlx.Error.sql_state)
}

// required string msg = 3;
inline bool Error::has_msg() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void Error::set_has_msg() {
  _has_bits_[0] |= 0x00000008u;
}
inline void Error::clear_has_msg() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void Error::clear_msg() {
  if (msg_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    msg_->clear();
  }
  clear_has_msg();
}
inline const ::std::string& Error::msg() const {
  // @@protoc_insertion_point(field_get:Mysqlx.Error.msg)
  return *msg_;
}
inline void Error::set_msg(const ::std::string& value) {
  set_has_msg();
  if (msg_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    msg_ = new ::std::string;
  }
  msg_->assign(value);
  // @@protoc_insertion_point(field_set:Mysqlx.Error.msg)
}
inline void Error::set_msg(const char* value) {
  set_has_msg();
  if (msg_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    msg_ = new ::std::string;
  }
  msg_->assign(value);
  // @@protoc_insertion_point(field_set_char:Mysqlx.Error.msg)
}
inline void Error::set_msg(const char* value, size_t size) {
  set_has_msg();
  if (msg_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    msg_ = new ::std::string;
  }
  msg_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:Mysqlx.Error.msg)
}
inline ::std::string* Error::mutable_msg() {
  set_has_msg();
  if (msg_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    msg_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:Mysqlx.Error.msg)
  return msg_;
}
inline ::std::string* Error::release_msg() {
  clear_has_msg();
  if (msg_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = msg_;
    msg_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void Error::set_allocated_msg(::std::string* msg) {
  if (msg_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete msg_;
  }
  if (msg) {
    set_has_msg();
    msg_ = msg;
  } else {
    clear_has_msg();
    msg_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:Mysqlx.Error.msg)
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace Mysqlx

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_mysqlx_2eproto__INCLUDED
