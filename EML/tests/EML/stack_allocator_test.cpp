#include <EML/stack_allocator.hpp>

#include <gtest/gtest.h>

struct stack_allocator_test : public testing::Test
{
   stack_allocator_test( ) : my_allocator( 1024 ) {}

   EML::stack_allocator my_allocator;
};

TEST_F( stack_allocator_test, make_new_inplace_args_test )
{
   auto* test_alloc = my_allocator.make_new<int>( 10 );

   EXPECT_NE( nullptr, test_alloc );
   EXPECT_EQ( 10, *test_alloc );
}

TEST_F( stack_allocator_test, over_capacity_test )
{
   EXPECT_EQ( nullptr, my_allocator.allocate( 1050, alignof( std::size_t ) ) );
}

TEST_F( stack_allocator_test, deletion_test )
{
   auto* p_alloc_1 = my_allocator.make_new<int>( 10 );

   EXPECT_NE( nullptr, p_alloc_1 );
   EXPECT_EQ( 10, *p_alloc_1 );

   auto* p_alloc_2 = my_allocator.make_new<int>( 20 );

   EXPECT_NE( nullptr, p_alloc_2 );
   EXPECT_EQ( 20, *p_alloc_2 );

   my_allocator.make_delete( p_alloc_2 );
}
