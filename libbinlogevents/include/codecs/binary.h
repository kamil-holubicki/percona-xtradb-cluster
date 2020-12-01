/* Copyright (c) 2019, 2020, Oracle and/or its affiliates.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License, version 2.0,
   as published by the Free Software Foundation.

   This program is also distributed with certain software (including
   but not limited to OpenSSL) that is licensed under separate terms,
   as designated in a particular file or component or in included license
   documentation.  The authors of MySQL hereby grant you an additional
   permission to link the program and your derivative works with the
   separately licensed software that they have included with MySQL.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License, version 2.0, for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA */

#ifndef CODECS_BINARY_INCLUDED
#define CODECS_BINARY_INCLUDED

#include "base.h"
#include "libbinlogevents/include/binary_log.h"

namespace binary_log {
namespace codecs {
namespace binary {

/**
  This is the abstract and base class for binary log BINARY codecs.
 */
class Base_codec : public binary_log::codecs::Codec {
 public:
  static const unsigned short UINT_64T_MIN_SIZE = 1;
  static const unsigned short UINT_64T_MAX_SIZE = 1;

 protected:
  Event_reader *m_reader;
  inline Event_reader &reader() { return *m_reader; }

 public:
  std::pair<std::size_t, bool> decode(const unsigned char *from,
                                      std::size_t size,
                                      Binary_log_event &to) const override = 0;
  std::pair<std::size_t, bool> encode(const Binary_log_event &from,
                                      unsigned char *to,
                                      std::size_t size) const override = 0;

  ~Base_codec() override {}
};

/**
  Binary codec for the transaction payload log event.
 */
class Transaction_payload : public Base_codec {
 public:
  Transaction_payload() {}
  /**
     This member function shall decode the contents of the buffer provided and
     fill in the event referenced. Note that the event provided needs to be of
     type TRANSACTION_PAYLOAD_EVENT.

     @param from the buffer to decode
     @param size the size of the buffer to decode.
     @param to the event to store the decoded information into.

     @return a pair containing the amount of bytes decoded and whether there was
             an error or not. False if no error, true otherwise.
   */
  std::pair<std::size_t, bool> decode(const unsigned char *from,
                                      std::size_t size,
                                      Binary_log_event &to) const override;

  /**
     This member function shall encode the contents of the event referenced and
     store the result in the buffer provided. Note that the event referenced
     needs to be of type TRANSACTION_PAYLOAD_EVENT.

     @param from the event to encode.
     @param to the buffer where to store the encoded event.
     @param size the size of the buffer.

     @return a pair containing the amount of bytes encoded and whether there was
             an error or not.
  */
  std::pair<std::size_t, bool> encode(const Binary_log_event &from,
                                      unsigned char *to,
                                      std::size_t size) const override;
};

}  // namespace binary
}  // namespace codecs
}  // namespace binary_log

#endif
