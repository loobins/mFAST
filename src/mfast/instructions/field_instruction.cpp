// Copyright (c) 2013, 2014, Huang-Ming Huang,  Object Computing, Inc.
// All rights reserved.
//
// This file is part of mFAST.
//
//     mFAST is free software: you can redistribute it and/or modify
//     it under the terms of the GNU Lesser General Public License as published by
//     the Free Software Foundation, either version 3 of the License, or
//     (at your option) any later version.
//
//     mFAST is distributed in the hope that it will be useful,
//     but WITHOUT ANY WARRANTY; without even the implied warranty of
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//     GNU General Public License for more details.
//
//     You should have received a copy of the GNU Lesser General Public License
//     along with mFast.  If not, see <http://www.gnu.org/licenses/>.

#include <cstring>
#include "field_instruction.h"

namespace mfast {

  field_instruction::field_instruction(uint16_t        field_index,
                                       operator_enum_t operator_id,
                                       int             field_type,
                                       presence_enum_t optional,
                                       uint32_t        id,
                                       const char*     name,
                                       const char*     ns)
    : field_index_(field_index)
    , operator_id_(operator_id)
    , is_array_(field_type >= field_type_ascii_string && field_type <= field_type_sequence )
    , optional_flag_(optional)
    , nullable_flag_( optional &&  (operator_id != operator_constant) )
    , has_pmap_bit_(operator_id > operator_delta || ((operator_id == operator_constant) && optional))
    , mandatory_no_initial_value_(false)
    , field_type_(field_type)
    , id_(id)
    , name_(name)
    , ns_(ns)
  {
  }

  void
  field_instruction::destruct_value(value_storage&,
                                    allocator*) const
  {
  }

  void
  field_instruction::copy_construct_value(const value_storage& src,
                                          value_storage&       dest,
                                          allocator* /* alloc */,
                                          value_storage*) const
  {
    dest.of_array.content_ = src.of_array.content_;
    dest.of_array.len_ = src.of_array.len_;
    dest.of_array.capacity_in_bytes_ = 0;
  }

  const char*
  field_instruction::field_type_name() const
  {
    static const char* names [] = {
      "field_type_int32",
      "field_type_uint32",
      "field_type_int64",
      "field_type_uint64",
      "field_type_decimal",
      "field_type_exponent",
      "field_type_templateref",
      "field_type_ascii_string",
      "field_type_unicode_string",
      "field_type_byte_vector",
      "field_type_group",
      "field_type_sequence",
      "field_type_template",
      "field_type_enum"
    };
    return names[this->field_type()];
  }

  std::size_t
  field_instruction::pmap_size() const
  {
    return has_pmap_bit_;
  }
}