#pragma once

#include <EML/allocator_interface.hpp>
#include <EML/allocator_utils.hpp>

#include <cassert>
#include <cstdint>
#include <functional>
#include <memory>

namespace EML
{
   class pool_allocator final : public allocator_interface
   {
   private:
      struct block_header
      {
         block_header* p_next;
      };

   public:
      pool_allocator( std::size_t const block_count, std::size_t const block_size ) noexcept;

      [[nodiscard]] std::byte* allocate( std::size_t size, std::size_t alignment ) noexcept override;
      void free( std::byte* p_location ) noexcept override;

      void clear( ) noexcept override;

      template <class type_, class... args_>
      [[nodiscard]] uptr<type_> make_unique( args_&&... args ) noexcept
      {
         if ( auto* p_alloc = allocate( sizeof( type_ ), alignof( type_ ) ) )
         {
            return uptr<type_>( new ( p_alloc ) type_( args... ), [this]( type_* p_type ) {
               this->make_delete( p_type );
            } );
         }
         else
         {
            return uptr<type_>( nullptr, [this]( type_* p_type ) {
               this->make_delete( p_type );
            } );
         }
      }

   private:
      std::size_t block_count = 0;
      std::size_t block_size = 0;

      std::unique_ptr<std::byte[]> p_memory;
      block_header* p_first_free = nullptr;
   };
} // namespace EML
