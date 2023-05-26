#include <gtest/gtest.h>

#include <fastcdr/Cdr.h>

using namespace eprosima::fastcdr;

TEST(XCdrTest, test)
{
    char buffer[12];
    FastBuffer fast_buffer(buffer, 12);
    Cdr cdr(fast_buffer);
    cdr << MemberId(3);

}
