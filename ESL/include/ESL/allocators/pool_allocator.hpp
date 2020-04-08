/**
 * MIT License
 *
 * Copyright (c) 2020 Wmbat
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include <ESL/allocators/allocator_utils.hpp>

#include <cassert>
#include <cstdint>
#include <functional>
#include <memory>

namespace ESL
{
   /**
    * @class pool_allocator pool_allocator.hpp <ESL/allocators/pool_allocator.hpp>
    * @author wmbat wmbat@protonmail.com
    * @date Tuesday April 7th, 2020
    * @brief A allocator that gives out pools of memory.
    * @copyright MIT License
    */
   class pool_allocator final
   {
      struct block_header
      {
         block_header* p_next;
      };

   public:
      using pointer = std::byte*;
      using size_type = std::size_t;

      /**
       * @brief The data used to create the pool allocator.
       */
      struct create_info
      {
         size_type pool_count{ 1 };
         size_type pool_size{ 1024 };
      };

   public:
      /**
       * @brief Constructs the allocator with no memory allocated.
       */
      pool_allocator( ) = default;
      /**
       * @brief Construct an allocator with the data provided from the #create_info struct.
       *
       * @param[in]  create_info    The data used for the creation of the allocator.
       */
      pool_allocator( create_info const& create_info ) noexcept;

      /**
       * @brief Find and give out a pool in the allocator's memory.
       *
       * @param[in]  size           The desired size of the pool to give out.
       * @param[in]  alignment      The alignment in the pool to give out.
       *
       * @return A pointer to the pool of memory.
       */
      [[nodiscard( "Memory will go to waste" )]] pointer allocate(
         size_type size, size_type alignment = alignof( std::max_align_t ) ) noexcept;
      /**
       * @brief Release the memory of a pool thas has been previously given out.
       *
       * @param[in]  p_alloc        A pointer to the pool to return to the allocator.
       */
      void deallocate( pointer p_alloc ) noexcept;

      /**
       * @brief Return the size of a pool.
       *
       * @param[in]  p_alloc     The allocation of memory associated with a pool.
       *
       * @return The size of a pool in the allocator.
       */
      size_type allocation_capacity( pointer p_alloc ) const noexcept;

      /**
       * @brief Return the size of the allocator's memory.
       *
       * @return The size of the allocator's memory.
       */
      size_type max_size( ) const noexcept;
      /**
       * @brief Return the amount of memory that has been given out.
       */
      size_type memory_usage( ) const noexcept;
      /**
       * @brief Return the amount of times memory has been given out.
       */
      size_type allocation_count( ) const noexcept;

      /**
       * @brief Constructs an instance of type type_ in a pool of memory.
       *
       * @tparam     type_       The type of the object ot construct in the pool.
       * @tparam     args_       The parameters used to construct an instance of type type_.
       *
       * @param[in]  args        The parameters necessary for the construction of an object of type type_.
       *
       * @return A pointer to the newly constructed object in a memory pool.
       */
      template <class type_, class... args_>
      [[nodiscard( "Memory will go to waste" )]] type_* construct( args_&&... args ) noexcept
         requires std::constructible_from<type_, args_...>
      {
         if ( auto* p_alloc = allocate( sizeof( type_ ), alignof( type_ ) ) )
         {
            return new ( p_alloc ) type_( args... );
         }
         else
         {
            return nullptr;
         }
      }

      /**
       * @brief Destroy an object previously allocated by the allocator and free it's memory pool.
       *
       * @param[in]  p_type   A pointer to the object's memory.
       */
      template <class type_>
      void destroy( type_* p_type ) noexcept
      {
         if ( p_type )
         {
            p_type->~type_( );
            free( TO_BYTE_PTR( p_type ) );
         }
      }

      /**
       * @brief Create a unique handle to a object allocated in the allocator, it'll be automatically destroyed up
       * reaching the end of the handle's scope.
       *
       * @tparam     type_       The type of the object ot construct in the pool.
       * @tparam     args_       The parameters used to construct an instance of type type_.
       *
       * @param[in]  args        The parameters necessary for the construction of an object of type type_.
       *
       * @return A unique pointer to the newly constructed object in a memory pool.
       */
      template <class type_, class... args_>
      [[nodiscard( "Object will immediatelly be destroyed" )]] auto_ptr<type_> make_unique( args_&&... args ) noexcept
      {
         return auto_ptr<type_>( construct<type_>( args... ), [this]( type_* p_type ) {
            this->destroy( p_type );
         } );
      }

   private:
      size_type total_size{ 0 };
      size_type used_memory{ 0 };
      size_type num_allocations{ 0 };

      size_type pool_count{ 0 };
      size_type pool_size{ 0 };

      std::unique_ptr<std::byte[]> p_memory{ nullptr };
      block_header* p_first_free{ nullptr };
   };
} // namespace ESL
