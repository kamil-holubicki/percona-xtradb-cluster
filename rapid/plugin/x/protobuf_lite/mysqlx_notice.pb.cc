// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: mysqlx_notice.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "mysqlx_notice.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>
// @@protoc_insertion_point(includes)

namespace Mysqlx {
namespace Notice {

void protobuf_ShutdownFile_mysqlx_5fnotice_2eproto() {
  delete Frame::default_instance_;
  delete Warning::default_instance_;
  delete SessionVariableChanged::default_instance_;
  delete SessionStateChanged::default_instance_;
}

#ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
void protobuf_AddDesc_mysqlx_5fnotice_2eproto_impl() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

#else
void protobuf_AddDesc_mysqlx_5fnotice_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

#endif
  ::Mysqlx::Datatypes::protobuf_AddDesc_mysqlx_5fdatatypes_2eproto();
  Frame::default_instance_ = new Frame();
  Warning::default_instance_ = new Warning();
  SessionVariableChanged::default_instance_ = new SessionVariableChanged();
  SessionStateChanged::default_instance_ = new SessionStateChanged();
  Frame::default_instance_->InitAsDefaultInstance();
  Warning::default_instance_->InitAsDefaultInstance();
  SessionVariableChanged::default_instance_->InitAsDefaultInstance();
  SessionStateChanged::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_mysqlx_5fnotice_2eproto);
}

#ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AddDesc_mysqlx_5fnotice_2eproto_once_);
void protobuf_AddDesc_mysqlx_5fnotice_2eproto() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AddDesc_mysqlx_5fnotice_2eproto_once_,
                 &protobuf_AddDesc_mysqlx_5fnotice_2eproto_impl);
}
#else
// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_mysqlx_5fnotice_2eproto {
  StaticDescriptorInitializer_mysqlx_5fnotice_2eproto() {
    protobuf_AddDesc_mysqlx_5fnotice_2eproto();
  }
} static_descriptor_initializer_mysqlx_5fnotice_2eproto_;
#endif

// ===================================================================

bool Frame_Scope_IsValid(int value) {
  switch(value) {
    case 1:
    case 2:
      return true;
    default:
      return false;
  }
}

#ifndef _MSC_VER
const Frame_Scope Frame::GLOBAL;
const Frame_Scope Frame::LOCAL;
const Frame_Scope Frame::Scope_MIN;
const Frame_Scope Frame::Scope_MAX;
const int Frame::Scope_ARRAYSIZE;
#endif  // _MSC_VER
#ifndef _MSC_VER
const int Frame::kTypeFieldNumber;
const int Frame::kScopeFieldNumber;
const int Frame::kPayloadFieldNumber;
#endif  // !_MSC_VER

Frame::Frame()
  : ::google::protobuf::MessageLite() {
  SharedCtor();
  // @@protoc_insertion_point(constructor:Mysqlx.Notice.Frame)
}

void Frame::InitAsDefaultInstance() {
}

Frame::Frame(const Frame& from)
  : ::google::protobuf::MessageLite() {
  SharedCtor();
  MergeFrom(from);
  // @@protoc_insertion_point(copy_constructor:Mysqlx.Notice.Frame)
}

void Frame::SharedCtor() {
  ::google::protobuf::internal::GetEmptyString();
  _cached_size_ = 0;
  type_ = 0u;
  scope_ = 1;
  payload_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

Frame::~Frame() {
  // @@protoc_insertion_point(destructor:Mysqlx.Notice.Frame)
  SharedDtor();
}

void Frame::SharedDtor() {
  if (payload_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete payload_;
  }
  #ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  if (this != &default_instance()) {
  #else
  if (this != default_instance_) {
  #endif
  }
}

void Frame::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const Frame& Frame::default_instance() {
#ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  protobuf_AddDesc_mysqlx_5fnotice_2eproto();
#else
  if (default_instance_ == NULL) protobuf_AddDesc_mysqlx_5fnotice_2eproto();
#endif
  return *default_instance_;
}

Frame* Frame::default_instance_ = NULL;

Frame* Frame::New() const {
  return new Frame;
}

void Frame::Clear() {
  if (_has_bits_[0 / 32] & 7) {
    type_ = 0u;
    scope_ = 1;
    if (has_payload()) {
      if (payload_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
        payload_->clear();
      }
    }
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->clear();
}

bool Frame::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  ::google::protobuf::io::StringOutputStream unknown_fields_string(
      mutable_unknown_fields());
  ::google::protobuf::io::CodedOutputStream unknown_fields_stream(
      &unknown_fields_string);
  // @@protoc_insertion_point(parse_start:Mysqlx.Notice.Frame)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoff(127);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required uint32 type = 1;
      case 1: {
        if (tag == 8) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::uint32, ::google::protobuf::internal::WireFormatLite::TYPE_UINT32>(
                 input, &type_)));
          set_has_type();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(16)) goto parse_scope;
        break;
      }

      // optional .Mysqlx.Notice.Frame.Scope scope = 2 [default = GLOBAL];
      case 2: {
        if (tag == 16) {
         parse_scope:
          int value;
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   int, ::google::protobuf::internal::WireFormatLite::TYPE_ENUM>(
                 input, &value)));
          if (::Mysqlx::Notice::Frame_Scope_IsValid(value)) {
            set_scope(static_cast< ::Mysqlx::Notice::Frame_Scope >(value));
          } else {
            unknown_fields_stream.WriteVarint32(tag);
            unknown_fields_stream.WriteVarint32(value);
          }
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(26)) goto parse_payload;
        break;
      }

      // optional bytes payload = 3;
      case 3: {
        if (tag == 26) {
         parse_payload:
          DO_(::google::protobuf::internal::WireFormatLite::ReadBytes(
                input, this->mutable_payload()));
        } else {
          goto handle_unusual;
        }
        if (input->ExpectAtEnd()) goto success;
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0 ||
            ::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          goto success;
        }
        DO_(::google::protobuf::internal::WireFormatLite::SkipField(
            input, tag, &unknown_fields_stream));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:Mysqlx.Notice.Frame)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:Mysqlx.Notice.Frame)
  return false;
#undef DO_
}

void Frame::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:Mysqlx.Notice.Frame)
  // required uint32 type = 1;
  if (has_type()) {
    ::google::protobuf::internal::WireFormatLite::WriteUInt32(1, this->type(), output);
  }

  // optional .Mysqlx.Notice.Frame.Scope scope = 2 [default = GLOBAL];
  if (has_scope()) {
    ::google::protobuf::internal::WireFormatLite::WriteEnum(
      2, this->scope(), output);
  }

  // optional bytes payload = 3;
  if (has_payload()) {
    ::google::protobuf::internal::WireFormatLite::WriteBytesMaybeAliased(
      3, this->payload(), output);
  }

  output->WriteRaw(unknown_fields().data(),
                   unknown_fields().size());
  // @@protoc_insertion_point(serialize_end:Mysqlx.Notice.Frame)
}

int Frame::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // required uint32 type = 1;
    if (has_type()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::UInt32Size(
          this->type());
    }

    // optional .Mysqlx.Notice.Frame.Scope scope = 2 [default = GLOBAL];
    if (has_scope()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::EnumSize(this->scope());
    }

    // optional bytes payload = 3;
    if (has_payload()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::BytesSize(
          this->payload());
    }

  }
  total_size += unknown_fields().size();

  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void Frame::CheckTypeAndMergeFrom(
    const ::google::protobuf::MessageLite& from) {
  MergeFrom(*::google::protobuf::down_cast<const Frame*>(&from));
}

void Frame::MergeFrom(const Frame& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_type()) {
      set_type(from.type());
    }
    if (from.has_scope()) {
      set_scope(from.scope());
    }
    if (from.has_payload()) {
      set_payload(from.payload());
    }
  }
  mutable_unknown_fields()->append(from.unknown_fields());
}

void Frame::CopyFrom(const Frame& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool Frame::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000001) != 0x00000001) return false;

  return true;
}

void Frame::Swap(Frame* other) {
  if (other != this) {
    std::swap(type_, other->type_);
    std::swap(scope_, other->scope_);
    std::swap(payload_, other->payload_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.swap(other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::std::string Frame::GetTypeName() const {
  return "Mysqlx.Notice.Frame";
}


// ===================================================================

bool Warning_Level_IsValid(int value) {
  switch(value) {
    case 1:
    case 2:
    case 3:
      return true;
    default:
      return false;
  }
}

#ifndef _MSC_VER
const Warning_Level Warning::NOTE;
const Warning_Level Warning::WARNING;
const Warning_Level Warning::ERROR;
const Warning_Level Warning::Level_MIN;
const Warning_Level Warning::Level_MAX;
const int Warning::Level_ARRAYSIZE;
#endif  // _MSC_VER
#ifndef _MSC_VER
const int Warning::kLevelFieldNumber;
const int Warning::kCodeFieldNumber;
const int Warning::kMsgFieldNumber;
#endif  // !_MSC_VER

Warning::Warning()
  : ::google::protobuf::MessageLite() {
  SharedCtor();
  // @@protoc_insertion_point(constructor:Mysqlx.Notice.Warning)
}

void Warning::InitAsDefaultInstance() {
}

Warning::Warning(const Warning& from)
  : ::google::protobuf::MessageLite() {
  SharedCtor();
  MergeFrom(from);
  // @@protoc_insertion_point(copy_constructor:Mysqlx.Notice.Warning)
}

void Warning::SharedCtor() {
  ::google::protobuf::internal::GetEmptyString();
  _cached_size_ = 0;
  level_ = 2;
  code_ = 0u;
  msg_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

Warning::~Warning() {
  // @@protoc_insertion_point(destructor:Mysqlx.Notice.Warning)
  SharedDtor();
}

void Warning::SharedDtor() {
  if (msg_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete msg_;
  }
  #ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  if (this != &default_instance()) {
  #else
  if (this != default_instance_) {
  #endif
  }
}

void Warning::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const Warning& Warning::default_instance() {
#ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  protobuf_AddDesc_mysqlx_5fnotice_2eproto();
#else
  if (default_instance_ == NULL) protobuf_AddDesc_mysqlx_5fnotice_2eproto();
#endif
  return *default_instance_;
}

Warning* Warning::default_instance_ = NULL;

Warning* Warning::New() const {
  return new Warning;
}

void Warning::Clear() {
  if (_has_bits_[0 / 32] & 7) {
    level_ = 2;
    code_ = 0u;
    if (has_msg()) {
      if (msg_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
        msg_->clear();
      }
    }
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->clear();
}

bool Warning::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  ::google::protobuf::io::StringOutputStream unknown_fields_string(
      mutable_unknown_fields());
  ::google::protobuf::io::CodedOutputStream unknown_fields_stream(
      &unknown_fields_string);
  // @@protoc_insertion_point(parse_start:Mysqlx.Notice.Warning)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoff(127);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // optional .Mysqlx.Notice.Warning.Level level = 1 [default = WARNING];
      case 1: {
        if (tag == 8) {
          int value;
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   int, ::google::protobuf::internal::WireFormatLite::TYPE_ENUM>(
                 input, &value)));
          if (::Mysqlx::Notice::Warning_Level_IsValid(value)) {
            set_level(static_cast< ::Mysqlx::Notice::Warning_Level >(value));
          } else {
            unknown_fields_stream.WriteVarint32(tag);
            unknown_fields_stream.WriteVarint32(value);
          }
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(16)) goto parse_code;
        break;
      }

      // required uint32 code = 2;
      case 2: {
        if (tag == 16) {
         parse_code:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::uint32, ::google::protobuf::internal::WireFormatLite::TYPE_UINT32>(
                 input, &code_)));
          set_has_code();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(26)) goto parse_msg;
        break;
      }

      // required string msg = 3;
      case 3: {
        if (tag == 26) {
         parse_msg:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_msg()));
        } else {
          goto handle_unusual;
        }
        if (input->ExpectAtEnd()) goto success;
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0 ||
            ::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          goto success;
        }
        DO_(::google::protobuf::internal::WireFormatLite::SkipField(
            input, tag, &unknown_fields_stream));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:Mysqlx.Notice.Warning)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:Mysqlx.Notice.Warning)
  return false;
#undef DO_
}

void Warning::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:Mysqlx.Notice.Warning)
  // optional .Mysqlx.Notice.Warning.Level level = 1 [default = WARNING];
  if (has_level()) {
    ::google::protobuf::internal::WireFormatLite::WriteEnum(
      1, this->level(), output);
  }

  // required uint32 code = 2;
  if (has_code()) {
    ::google::protobuf::internal::WireFormatLite::WriteUInt32(2, this->code(), output);
  }

  // required string msg = 3;
  if (has_msg()) {
    ::google::protobuf::internal::WireFormatLite::WriteStringMaybeAliased(
      3, this->msg(), output);
  }

  output->WriteRaw(unknown_fields().data(),
                   unknown_fields().size());
  // @@protoc_insertion_point(serialize_end:Mysqlx.Notice.Warning)
}

int Warning::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // optional .Mysqlx.Notice.Warning.Level level = 1 [default = WARNING];
    if (has_level()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::EnumSize(this->level());
    }

    // required uint32 code = 2;
    if (has_code()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::UInt32Size(
          this->code());
    }

    // required string msg = 3;
    if (has_msg()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->msg());
    }

  }
  total_size += unknown_fields().size();

  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void Warning::CheckTypeAndMergeFrom(
    const ::google::protobuf::MessageLite& from) {
  MergeFrom(*::google::protobuf::down_cast<const Warning*>(&from));
}

void Warning::MergeFrom(const Warning& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_level()) {
      set_level(from.level());
    }
    if (from.has_code()) {
      set_code(from.code());
    }
    if (from.has_msg()) {
      set_msg(from.msg());
    }
  }
  mutable_unknown_fields()->append(from.unknown_fields());
}

void Warning::CopyFrom(const Warning& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool Warning::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000006) != 0x00000006) return false;

  return true;
}

void Warning::Swap(Warning* other) {
  if (other != this) {
    std::swap(level_, other->level_);
    std::swap(code_, other->code_);
    std::swap(msg_, other->msg_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.swap(other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::std::string Warning::GetTypeName() const {
  return "Mysqlx.Notice.Warning";
}


// ===================================================================

#ifndef _MSC_VER
const int SessionVariableChanged::kParamFieldNumber;
const int SessionVariableChanged::kValueFieldNumber;
#endif  // !_MSC_VER

SessionVariableChanged::SessionVariableChanged()
  : ::google::protobuf::MessageLite() {
  SharedCtor();
  // @@protoc_insertion_point(constructor:Mysqlx.Notice.SessionVariableChanged)
}

void SessionVariableChanged::InitAsDefaultInstance() {
#ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  value_ = const_cast< ::Mysqlx::Datatypes::Scalar*>(
      ::Mysqlx::Datatypes::Scalar::internal_default_instance());
#else
  value_ = const_cast< ::Mysqlx::Datatypes::Scalar*>(&::Mysqlx::Datatypes::Scalar::default_instance());
#endif
}

SessionVariableChanged::SessionVariableChanged(const SessionVariableChanged& from)
  : ::google::protobuf::MessageLite() {
  SharedCtor();
  MergeFrom(from);
  // @@protoc_insertion_point(copy_constructor:Mysqlx.Notice.SessionVariableChanged)
}

void SessionVariableChanged::SharedCtor() {
  ::google::protobuf::internal::GetEmptyString();
  _cached_size_ = 0;
  param_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  value_ = NULL;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

SessionVariableChanged::~SessionVariableChanged() {
  // @@protoc_insertion_point(destructor:Mysqlx.Notice.SessionVariableChanged)
  SharedDtor();
}

void SessionVariableChanged::SharedDtor() {
  if (param_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete param_;
  }
  #ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  if (this != &default_instance()) {
  #else
  if (this != default_instance_) {
  #endif
    delete value_;
  }
}

void SessionVariableChanged::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const SessionVariableChanged& SessionVariableChanged::default_instance() {
#ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  protobuf_AddDesc_mysqlx_5fnotice_2eproto();
#else
  if (default_instance_ == NULL) protobuf_AddDesc_mysqlx_5fnotice_2eproto();
#endif
  return *default_instance_;
}

SessionVariableChanged* SessionVariableChanged::default_instance_ = NULL;

SessionVariableChanged* SessionVariableChanged::New() const {
  return new SessionVariableChanged;
}

void SessionVariableChanged::Clear() {
  if (_has_bits_[0 / 32] & 3) {
    if (has_param()) {
      if (param_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
        param_->clear();
      }
    }
    if (has_value()) {
      if (value_ != NULL) value_->::Mysqlx::Datatypes::Scalar::Clear();
    }
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->clear();
}

bool SessionVariableChanged::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  ::google::protobuf::io::StringOutputStream unknown_fields_string(
      mutable_unknown_fields());
  ::google::protobuf::io::CodedOutputStream unknown_fields_stream(
      &unknown_fields_string);
  // @@protoc_insertion_point(parse_start:Mysqlx.Notice.SessionVariableChanged)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoff(127);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required string param = 1;
      case 1: {
        if (tag == 10) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_param()));
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(18)) goto parse_value;
        break;
      }

      // optional .Mysqlx.Datatypes.Scalar value = 2;
      case 2: {
        if (tag == 18) {
         parse_value:
          DO_(::google::protobuf::internal::WireFormatLite::ReadMessageNoVirtual(
               input, mutable_value()));
        } else {
          goto handle_unusual;
        }
        if (input->ExpectAtEnd()) goto success;
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0 ||
            ::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          goto success;
        }
        DO_(::google::protobuf::internal::WireFormatLite::SkipField(
            input, tag, &unknown_fields_stream));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:Mysqlx.Notice.SessionVariableChanged)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:Mysqlx.Notice.SessionVariableChanged)
  return false;
#undef DO_
}

void SessionVariableChanged::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:Mysqlx.Notice.SessionVariableChanged)
  // required string param = 1;
  if (has_param()) {
    ::google::protobuf::internal::WireFormatLite::WriteStringMaybeAliased(
      1, this->param(), output);
  }

  // optional .Mysqlx.Datatypes.Scalar value = 2;
  if (has_value()) {
    ::google::protobuf::internal::WireFormatLite::WriteMessage(
      2, this->value(), output);
  }

  output->WriteRaw(unknown_fields().data(),
                   unknown_fields().size());
  // @@protoc_insertion_point(serialize_end:Mysqlx.Notice.SessionVariableChanged)
}

int SessionVariableChanged::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // required string param = 1;
    if (has_param()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->param());
    }

    // optional .Mysqlx.Datatypes.Scalar value = 2;
    if (has_value()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::MessageSizeNoVirtual(
          this->value());
    }

  }
  total_size += unknown_fields().size();

  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void SessionVariableChanged::CheckTypeAndMergeFrom(
    const ::google::protobuf::MessageLite& from) {
  MergeFrom(*::google::protobuf::down_cast<const SessionVariableChanged*>(&from));
}

void SessionVariableChanged::MergeFrom(const SessionVariableChanged& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_param()) {
      set_param(from.param());
    }
    if (from.has_value()) {
      mutable_value()->::Mysqlx::Datatypes::Scalar::MergeFrom(from.value());
    }
  }
  mutable_unknown_fields()->append(from.unknown_fields());
}

void SessionVariableChanged::CopyFrom(const SessionVariableChanged& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool SessionVariableChanged::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000001) != 0x00000001) return false;

  if (has_value()) {
    if (!this->value().IsInitialized()) return false;
  }
  return true;
}

void SessionVariableChanged::Swap(SessionVariableChanged* other) {
  if (other != this) {
    std::swap(param_, other->param_);
    std::swap(value_, other->value_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.swap(other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::std::string SessionVariableChanged::GetTypeName() const {
  return "Mysqlx.Notice.SessionVariableChanged";
}


// ===================================================================

bool SessionStateChanged_Parameter_IsValid(int value) {
  switch(value) {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 9:
    case 10:
    case 11:
      return true;
    default:
      return false;
  }
}

#ifndef _MSC_VER
const SessionStateChanged_Parameter SessionStateChanged::CURRENT_SCHEMA;
const SessionStateChanged_Parameter SessionStateChanged::ACCOUNT_EXPIRED;
const SessionStateChanged_Parameter SessionStateChanged::GENERATED_INSERT_ID;
const SessionStateChanged_Parameter SessionStateChanged::ROWS_AFFECTED;
const SessionStateChanged_Parameter SessionStateChanged::ROWS_FOUND;
const SessionStateChanged_Parameter SessionStateChanged::ROWS_MATCHED;
const SessionStateChanged_Parameter SessionStateChanged::TRX_COMMITTED;
const SessionStateChanged_Parameter SessionStateChanged::TRX_ROLLEDBACK;
const SessionStateChanged_Parameter SessionStateChanged::PRODUCED_MESSAGE;
const SessionStateChanged_Parameter SessionStateChanged::CLIENT_ID_ASSIGNED;
const SessionStateChanged_Parameter SessionStateChanged::Parameter_MIN;
const SessionStateChanged_Parameter SessionStateChanged::Parameter_MAX;
const int SessionStateChanged::Parameter_ARRAYSIZE;
#endif  // _MSC_VER
#ifndef _MSC_VER
const int SessionStateChanged::kParamFieldNumber;
const int SessionStateChanged::kValueFieldNumber;
#endif  // !_MSC_VER

SessionStateChanged::SessionStateChanged()
  : ::google::protobuf::MessageLite() {
  SharedCtor();
  // @@protoc_insertion_point(constructor:Mysqlx.Notice.SessionStateChanged)
}

void SessionStateChanged::InitAsDefaultInstance() {
#ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  value_ = const_cast< ::Mysqlx::Datatypes::Scalar*>(
      ::Mysqlx::Datatypes::Scalar::internal_default_instance());
#else
  value_ = const_cast< ::Mysqlx::Datatypes::Scalar*>(&::Mysqlx::Datatypes::Scalar::default_instance());
#endif
}

SessionStateChanged::SessionStateChanged(const SessionStateChanged& from)
  : ::google::protobuf::MessageLite() {
  SharedCtor();
  MergeFrom(from);
  // @@protoc_insertion_point(copy_constructor:Mysqlx.Notice.SessionStateChanged)
}

void SessionStateChanged::SharedCtor() {
  _cached_size_ = 0;
  param_ = 1;
  value_ = NULL;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

SessionStateChanged::~SessionStateChanged() {
  // @@protoc_insertion_point(destructor:Mysqlx.Notice.SessionStateChanged)
  SharedDtor();
}

void SessionStateChanged::SharedDtor() {
  #ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  if (this != &default_instance()) {
  #else
  if (this != default_instance_) {
  #endif
    delete value_;
  }
}

void SessionStateChanged::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const SessionStateChanged& SessionStateChanged::default_instance() {
#ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  protobuf_AddDesc_mysqlx_5fnotice_2eproto();
#else
  if (default_instance_ == NULL) protobuf_AddDesc_mysqlx_5fnotice_2eproto();
#endif
  return *default_instance_;
}

SessionStateChanged* SessionStateChanged::default_instance_ = NULL;

SessionStateChanged* SessionStateChanged::New() const {
  return new SessionStateChanged;
}

void SessionStateChanged::Clear() {
  if (_has_bits_[0 / 32] & 3) {
    param_ = 1;
    if (has_value()) {
      if (value_ != NULL) value_->::Mysqlx::Datatypes::Scalar::Clear();
    }
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->clear();
}

bool SessionStateChanged::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  ::google::protobuf::io::StringOutputStream unknown_fields_string(
      mutable_unknown_fields());
  ::google::protobuf::io::CodedOutputStream unknown_fields_stream(
      &unknown_fields_string);
  // @@protoc_insertion_point(parse_start:Mysqlx.Notice.SessionStateChanged)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoff(127);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required .Mysqlx.Notice.SessionStateChanged.Parameter param = 1;
      case 1: {
        if (tag == 8) {
          int value;
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   int, ::google::protobuf::internal::WireFormatLite::TYPE_ENUM>(
                 input, &value)));
          if (::Mysqlx::Notice::SessionStateChanged_Parameter_IsValid(value)) {
            set_param(static_cast< ::Mysqlx::Notice::SessionStateChanged_Parameter >(value));
          } else {
            unknown_fields_stream.WriteVarint32(tag);
            unknown_fields_stream.WriteVarint32(value);
          }
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(18)) goto parse_value;
        break;
      }

      // optional .Mysqlx.Datatypes.Scalar value = 2;
      case 2: {
        if (tag == 18) {
         parse_value:
          DO_(::google::protobuf::internal::WireFormatLite::ReadMessageNoVirtual(
               input, mutable_value()));
        } else {
          goto handle_unusual;
        }
        if (input->ExpectAtEnd()) goto success;
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0 ||
            ::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          goto success;
        }
        DO_(::google::protobuf::internal::WireFormatLite::SkipField(
            input, tag, &unknown_fields_stream));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:Mysqlx.Notice.SessionStateChanged)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:Mysqlx.Notice.SessionStateChanged)
  return false;
#undef DO_
}

void SessionStateChanged::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:Mysqlx.Notice.SessionStateChanged)
  // required .Mysqlx.Notice.SessionStateChanged.Parameter param = 1;
  if (has_param()) {
    ::google::protobuf::internal::WireFormatLite::WriteEnum(
      1, this->param(), output);
  }

  // optional .Mysqlx.Datatypes.Scalar value = 2;
  if (has_value()) {
    ::google::protobuf::internal::WireFormatLite::WriteMessage(
      2, this->value(), output);
  }

  output->WriteRaw(unknown_fields().data(),
                   unknown_fields().size());
  // @@protoc_insertion_point(serialize_end:Mysqlx.Notice.SessionStateChanged)
}

int SessionStateChanged::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // required .Mysqlx.Notice.SessionStateChanged.Parameter param = 1;
    if (has_param()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::EnumSize(this->param());
    }

    // optional .Mysqlx.Datatypes.Scalar value = 2;
    if (has_value()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::MessageSizeNoVirtual(
          this->value());
    }

  }
  total_size += unknown_fields().size();

  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void SessionStateChanged::CheckTypeAndMergeFrom(
    const ::google::protobuf::MessageLite& from) {
  MergeFrom(*::google::protobuf::down_cast<const SessionStateChanged*>(&from));
}

void SessionStateChanged::MergeFrom(const SessionStateChanged& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_param()) {
      set_param(from.param());
    }
    if (from.has_value()) {
      mutable_value()->::Mysqlx::Datatypes::Scalar::MergeFrom(from.value());
    }
  }
  mutable_unknown_fields()->append(from.unknown_fields());
}

void SessionStateChanged::CopyFrom(const SessionStateChanged& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool SessionStateChanged::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000001) != 0x00000001) return false;

  if (has_value()) {
    if (!this->value().IsInitialized()) return false;
  }
  return true;
}

void SessionStateChanged::Swap(SessionStateChanged* other) {
  if (other != this) {
    std::swap(param_, other->param_);
    std::swap(value_, other->value_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.swap(other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::std::string SessionStateChanged::GetTypeName() const {
  return "Mysqlx.Notice.SessionStateChanged";
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace Notice
}  // namespace Mysqlx

// @@protoc_insertion_point(global_scope)
