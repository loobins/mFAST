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
//
#ifndef DICTIONARY_BUILDER_H_F26FXFII
#define DICTIONARY_BUILDER_H_F26FXFII
#include <tuple>
#include "../mfast_coder_export.h"
#include "mfast/field_instructions.h"
#include "mfast/arena_allocator.h"
#include "mfast/coder/mfast_coder_export.h"
#include <vector>
#include <map>
#include <string>
#include <stdexcept>

#ifdef _MSC_VER
#pragma warning(disable: 4251) // non dll-interface class used as a member for dll-interface class
#endif //_MSC_VER


namespace mfast {

  class template_repo_base;
  class MFAST_CODER_EXPORT dictionary_builder
    : private field_instruction_visitor
  {
  public:

    dictionary_builder(template_repo_base& repo_base);

    void build_by_description(const templates_description* def);

    template <typename Operation>
    void build(const templates_description* def , const Operation& op)
    {
      current_ns_ = def->template_ns();
      current_dictionary_ = (def->dictionary()[0] == 0) ?  "global" : def->dictionary();
      for (uint32_t i  = 0; i < def->size(); ++i ) {
        uint32_t id = (*def)[i]->id();
        if (id > 0) {
          op(this->clone_instruction((*def)[i]));
        }
      }
    }

    template <typename DescriptionTuple, typename Operation>
    void build(const DescriptionTuple& tp, const Operation& op)
    {
      builder_helper<0,std::tuple_size<DescriptionTuple>::value, DescriptionTuple, Operation>::build(*this, tp, op);
    }

  private:


    template <int BeginIndex, int EndIndex, typename DescriptionTuple, typename Operation>
    struct builder_helper
    {
      static void build(dictionary_builder& builder, const DescriptionTuple& tp, const Operation& op)
      {
        builder.build_description(std::get<BeginIndex>(tp), op) ;
        builder_helper<BeginIndex+1, EndIndex, DescriptionTuple, Operation>::build(builder, tp, op);
      }
    };

    template <int EndIndex, typename DescriptionTuple, typename Operation>
    struct builder_helper<EndIndex, EndIndex, DescriptionTuple, Operation>
    {
      static void build(dictionary_builder& , const DescriptionTuple& , const Operation& )
      {
      }
    };

    template <int BeginIndex, int EndIndex, typename MessageTuple, typename Operation>
    struct message_info_helper
    {
      static void build(dictionary_builder& builder, const Operation& op)
      {
        using Message = typename std::tuple_element<BeginIndex, MessageTuple>::type;
        if (Message::the_id == 0)
          return;
        op(builder.clone_instruction(Message::instruction()), static_cast<Message*>(0));

        message_info_helper<BeginIndex+1, EndIndex, MessageTuple, Operation>::build(builder, op);
      }
    };

    template <int EndIndex, typename MessageTuple, typename Operation>
    struct message_info_helper<EndIndex, EndIndex, MessageTuple, Operation>
    {
      static void build(dictionary_builder&, const Operation&)
      {
      }
    };


    template <typename Description, typename Operation>
    void build_description(const Description* def, const Operation& op)
    {
      this->current_ns_ = def->template_ns();
      this->current_dictionary_ = (def->dictionary()[0] == 0) ?  "global" : def->dictionary();
      const int num_messages = std::tuple_size<typename Description::types>::value;

      message_info_helper<0, num_messages, typename Description::types, Operation>::build ( *this, op);
    }

    // template <int BeginIndex, int EndIndex, typename MessageTuple, typename Operation>
    // friend struct builder_helper;
    //
    // template <int BeginIndex, int EndIndex, typename MessageTuple, typename Operation>
    // friend struct message_info_helper;

    virtual void visit(const int32_field_instruction*, void*);
    virtual void visit(const uint32_field_instruction*, void*);
    virtual void visit(const int64_field_instruction*, void*);
    virtual void visit(const uint64_field_instruction*, void*);
    virtual void visit(const decimal_field_instruction*, void*);
    virtual void visit(const ascii_field_instruction*, void*);
    virtual void visit(const unicode_field_instruction*, void*);
    virtual void visit(const byte_vector_field_instruction*, void*);
    virtual void visit(const int32_vector_field_instruction*, void*);
    virtual void visit(const uint32_vector_field_instruction*, void*);
    virtual void visit(const int64_vector_field_instruction*, void*);
    virtual void visit(const uint64_vector_field_instruction*, void*);
    virtual void visit(const group_field_instruction*, void*);
    virtual void visit(const sequence_field_instruction*, void*);
    virtual void visit(const template_instruction*, void*);
    virtual void visit(const templateref_instruction*, void*);

    virtual void visit(const enum_field_instruction*, void*);

    template_instruction* clone_instruction(const template_instruction*);

    void build_group(const field_instruction*       fi,
                     const group_field_instruction* src,
                     group_field_instruction*       dest);

    value_storage*
    get_dictionary_storage(const char*         key,
                           const char*         ns,
                           const op_context_t* op_context,
                           field_type_enum_t   field_type,
                           value_storage*      candidate_storage,
                           field_instruction* instruction);

    template_instruction* find_template(uint32_t template_id);

    struct indexer_value_type
    {
      field_type_enum_t field_type_;
      field_instruction* instruction_;
      value_storage*  storage_;
    };

    typedef std::map<std::string, indexer_value_type>  indexer_t;
    indexer_t indexer_;
    std::string current_template_;
    std::string current_type_;
    const char* current_ns_;
    const char* current_dictionary_;

    template_repo_base& repo_base_;
    arena_allocator& alloc_;
  };


}


#endif /* end of include guard: DICTIONARY_BUILDER_H_F26FXFII */
