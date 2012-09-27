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

#pragma once

#include "JsWrapper.h"
#include "v8_binder.h"

using namespace v8;
typedef Local<Value> jsvalue;
typedef Handle<Object> jsobject;
 

/*****************************************************************
 JsValueConverter 
 Value conversion from JavaScript to C
******************************************************************/

/* Generic (for pointers).
   If you get an "invalid static cast from type void *", then the compiler
   may be erroneously falling back on this implementation. 
*/
template <typename T> class JsValueConverter {
public:  
  JsValueConverter(jsvalue v) {
    DEBUG_ASSERT(v->IsObject());
    Local<Object> obj = v->ToObject();
    DEBUG_ASSERT(obj->InternalFieldCount() == 2);
    native_object = unwrapPointer<T>(obj);
  }

  virtual ~JsValueConverter() {}

  virtual T toC() { 
    return native_object;
  }

protected:
  T native_object;  
};


/**** Specializations *****/

template <>
class JsValueConverter <int> {
public:
  jsvalue jsval;
  
  JsValueConverter(jsvalue v) : jsval(v) {};
  int toC() { return jsval->Int32Value(); };
};


template <>
class JsValueConverter <uint32_t> {
public:
  jsvalue jsval;
  
  JsValueConverter(jsvalue v) : jsval(v) {};
  int toC() { return jsval->Uint32Value(); };
};


template <>
class JsValueConverter <unsigned long> {
public:
  jsvalue jsval;
  
  JsValueConverter(jsvalue v) : jsval(v) {};
  int toC() { return jsval->IntegerValue(); };
};


template <>
class JsValueConverter <double> {
public:
  jsvalue jsval;
  
JsValueConverter(jsvalue v) : jsval(v) {};
  double toC() { return jsval->NumberValue(); };
};


template <>
class JsValueConverter <int64_t> {
public:
  jsvalue jsval;
  
  JsValueConverter(jsvalue v) : jsval(v) {};
  int64_t toC() { return jsval->IntegerValue(); };
};


template <>
class JsValueConverter <bool> {
public:
  jsvalue jsval;
  
  JsValueConverter(jsvalue v) : jsval(v) {};
  bool toC()  { return jsval->BooleanValue(); };
};


/* const char * is JavaScript String */
template <>
class JsValueConverter <const char *> {
private:
  v8::String::AsciiValue av;

public: 
  JsValueConverter(jsvalue v) : av(v)   {};
  const char * toC()  { return *av;  };
};


/* char * is Node::Buffer */
template <>
class JsValueConverter <char *> {
public: 
  jsvalue jsval;
  JsValueConverter(jsvalue v) : jsval(v)   {};
  char * toC()  { 
    DEBUG_PRINT_DETAIL("Unwrapping Node buffer");
    return V8BINDER_UNWRAP_BUFFER(jsval);  
  };
};


/*****************************************************************
 toJs functions
 Value Conversion from C to JavaScript

 If we have a correct isPointer()<T> for every T, then toJS()
 should never be called with a pointer type at runtime.  
 See AsyncCall_Returning::jsReturnVal() at AsyncMethodCall.h line 130.
 
 The generic function should either be undefined or should throw a
 run-time assert.

 SPECIALIZATIONS should be over C PRIMITIVE types only.
******************************************************************/

// pointer types
template <typename T> Local<Value> toJS(T cptr) {
  /* This can't be done.  Use wrapPointerInObject() instead. */
  HandleScope scope;
  assert("WRONG TEMPLATE SPECIALIZATION" == 0);
  return scope.Close(Null());
}

// int
template <>
inline Local<Value> toJS<int>(int cval) { 
  return v8::Integer::New(cval);
}

// unsigned int
template <>
inline Local<Value> toJS<unsigned int>(unsigned int cval) {
  return v8::Integer::NewFromUnsigned(cval);
}

// short
template <>
inline Local<Value> toJS<short>(short cval) {
  return v8::Integer::New(cval);
}

// unsigned short
template <>
inline Local<Value> toJS<unsigned short>(unsigned short cval) {
  return v8::Integer::NewFromUnsigned(cval);
}

// long 
template <>
inline Local<Value> toJS<long>(long cval) {
  return v8::Integer::New(cval);
}

// unsigned long
template <>
inline Local<Value> toJS<unsigned long >(unsigned long cval) {
  return v8::Integer::NewFromUnsigned(cval);
}

// unsigned long long 
// (the value may actually be too large to represent in JS!?)
template <>
inline Local<Value> toJS<unsigned long long>(unsigned long long cval) {
  return v8::Integer::NewFromUnsigned(cval);
}

// double
template <>
inline Local<Value> toJS<double>(double cval) {
  return Number::New(cval);
};

// const char *
template <> 
inline Local<Value> toJS<const char *>(const char * cval) {
  return v8::String::New(cval);
}

// const bool * 
template <> 
inline Local<Value> toJS<const bool *>(const bool * cbp) {
  HandleScope scope;
  return BooleanObject::New(*cbp);
}


/*****************************************************************
 isPointer() functions
 Used in AsyncMethodCall.h: if(isPointer(return_val)) ... 

 The top generic method (the one that returns true) 
 contains a compile-time safety check.
******************************************************************/
#ifdef __GNUC__
#define UNUSED __attribute__((unused))
#else
#define UNUSED
#endif
template <typename T> bool isPointer(T typ)                {
  UNUSED void * v;
  v = static_cast<void *> (typ);
  return true; 
}

template <> inline bool isPointer(int typ)                 { return false; }
template <> inline bool isPointer(uint32_t typ)            { return false; }
template <> inline bool isPointer(double typ)              { return false; }
template <> inline bool isPointer(const char * typ)        { return false; }
template <> inline bool isPointer(int64_t typ)             { return false; }
template <> inline bool isPointer(unsigned long typ)       { return false; }
template <> inline bool isPointer(const bool * typ)        { return false; }
template <> inline bool isPointer(char * typ)              { return false; }
