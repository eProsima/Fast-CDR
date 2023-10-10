// Copyright 2023 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <string>

#include <gtest/gtest.h>

#include <fastcdr/Cdr.h>
#include <fastcdr/CdrSizeCalculator.hpp>
#include <fastcdr/xcdr/external.hpp>
#include <fastcdr/xcdr/optional.hpp>

/// Auxiliary class
class ExternalClass
{
public:

    ExternalClass() noexcept = default;

    ExternalClass(
            const std::string text) noexcept
        : text_ {text}
    {
    }

    void text(
            const std::string& text) noexcept
    {
        text_ = text;
    }

    const std::string& text() const noexcept
    {
        return text_;
    }

private:

    std::string text_;
};

using namespace eprosima::fastcdr;

//! @test Tests the default constructor.
TEST(XCdrExternalAPITest, default_constructor)
{
    external<ExternalClass> ext;

    ASSERT_FALSE(ext);
    ASSERT_TRUE(nullptr == ext.get());
    ASSERT_FALSE(ext.is_locked());
}

//! @test Tests the constructor from a pointer.
TEST(XCdrExternalAPITest, constructor_from_pointer)
{
    // Calling constructor with locked = false
    ExternalClass* _ext {new ExternalClass("test")};
    external<ExternalClass> ext {_ext};

    ASSERT_TRUE(ext);
    ASSERT_TRUE(nullptr != ext.get());
    ASSERT_EQ((*ext).text(), "test");
    _ext->text("test2");
    ASSERT_EQ((*ext).text(), "test2");
    ASSERT_FALSE(ext.is_locked());

    // Calling constructor with locked = true
    ExternalClass* _ext2 {new ExternalClass("test")};
    external<ExternalClass> lock_ext {_ext2, true};

    ASSERT_TRUE(lock_ext);
    ASSERT_TRUE(nullptr != lock_ext.get());
    ASSERT_EQ((*lock_ext).text(), "test");
    _ext2->text("test2");
    ASSERT_EQ((*lock_ext).text(), "test2");
    ASSERT_TRUE(lock_ext.is_locked());
}

//! @test Tests the constructor from a shared pointer.
TEST(XCdrExternalAPITest, constructor_from_shared_pointer)
{
    // Calling constructor with a shared pointer managing an object.
    std::shared_ptr<ExternalClass> _ext {std::make_shared<ExternalClass>("test")};
    external<ExternalClass> ext {_ext};

    ASSERT_TRUE(ext);
    ASSERT_TRUE(nullptr != ext.get());
    ASSERT_EQ((*ext).text(), "test");
    _ext.get()->text("test2");
    ASSERT_EQ((*ext).text(), "test2");
    ASSERT_FALSE(ext.is_locked());

    // Calling constructor with a empty shared pointer.
    std::shared_ptr<ExternalClass> _empty_ext;
    external<ExternalClass> empty_ext {_empty_ext};

    ASSERT_FALSE(empty_ext);
    ASSERT_TRUE(nullptr == empty_ext.get());
    ASSERT_FALSE(empty_ext.is_locked());
}

//! @test Tests the copy constructor.
TEST(XCdrExternalAPITest, copy_contructor)
{
    // Calling constructor with empty external.
    external<ExternalClass> _empty_ext;
    external<ExternalClass> empty_ext {_empty_ext};

    ASSERT_FALSE(empty_ext);
    ASSERT_TRUE(nullptr == empty_ext.get());
    ASSERT_FALSE(empty_ext.is_locked());

    // Calling constructor with no-locked external.
    ExternalClass* __ext {new ExternalClass("test")};
    external<ExternalClass> _ext {__ext};
    external<ExternalClass> ext {_ext};

    ASSERT_TRUE(ext);
    ASSERT_TRUE(nullptr != ext.get());
    ASSERT_EQ((*ext).text(), "test");
    _ext->text("test2");
    ASSERT_EQ((*ext).text(), "test2");
    ASSERT_FALSE(ext.is_locked());

    // Calling constructor with locked = true
    ExternalClass* __lock_ext {new ExternalClass("test")};
    external<ExternalClass> _lock_ext {__lock_ext, true};
    external<ExternalClass> lock_ext {_lock_ext};

    ASSERT_TRUE(lock_ext);
    ASSERT_TRUE(nullptr != lock_ext.get());
    ASSERT_EQ((*lock_ext).text(), "test");
    _lock_ext->text("test2");
    ASSERT_EQ((*lock_ext).text(), "test");
    ASSERT_TRUE(lock_ext.is_locked());
}


//! @test Tests the assignment.
TEST(XCdrExternalAPITest, assignment)
{
    // Calling assignment over an empty external with empty external.
    external<ExternalClass> _empty_ext;
    external<ExternalClass> empty_ext;
    empty_ext = _empty_ext;

    ASSERT_FALSE(empty_ext);
    ASSERT_TRUE(nullptr == empty_ext.get());
    ASSERT_FALSE(empty_ext.is_locked());

    // Calling assignment over an external with empty external.
    external<ExternalClass> not_empty_ext {new ExternalClass("test")};
    not_empty_ext = _empty_ext;

    ASSERT_FALSE(not_empty_ext);
    ASSERT_TRUE(nullptr == not_empty_ext.get());
    ASSERT_FALSE(not_empty_ext.is_locked());

    // Calling assignment over an empty external with an external;
    ExternalClass* __ext {new ExternalClass("test")};
    external<ExternalClass> _ext {__ext};
    external<ExternalClass> empty_ext2;
    empty_ext2 = _ext;

    ASSERT_TRUE(empty_ext2);
    ASSERT_TRUE(nullptr != empty_ext2.get());
    ASSERT_EQ((*empty_ext2).text(), "test");
    _ext->text("test2");
    ASSERT_EQ((*empty_ext2).text(), "test2");
    ASSERT_FALSE(empty_ext2.is_locked());

    // Calling assignment over an external with an external;
    external<ExternalClass> not_empty_ext2 {new ExternalClass("test")};
    not_empty_ext2 = _ext;

    ASSERT_TRUE(not_empty_ext2);
    ASSERT_TRUE(nullptr != not_empty_ext2.get());
    ASSERT_EQ((*not_empty_ext2).text(), "test2");
    _ext->text("test3");
    ASSERT_EQ((*not_empty_ext2).text(), "test3");
    ASSERT_FALSE(not_empty_ext2.is_locked());

    // Calling assignment over an empty external with an locked external;
    ExternalClass* __locked_ext {new ExternalClass("test")};
    external<ExternalClass> _locked_ext {__locked_ext, true};
    external<ExternalClass> empty_ext3;
    empty_ext3 = _locked_ext;

    ASSERT_TRUE(empty_ext3);
    ASSERT_TRUE(nullptr != empty_ext3.get());
    ASSERT_EQ((*empty_ext3).text(), "test");
    _locked_ext->text("test2");
    ASSERT_EQ((*empty_ext3).text(), "test");
    ASSERT_FALSE(empty_ext3.is_locked());

    // Calling assignment over an external with an locked external;
    external<ExternalClass> not_empty_ext3 {new ExternalClass("test")};
    not_empty_ext3 = _locked_ext;

    ASSERT_TRUE(not_empty_ext3);
    ASSERT_TRUE(nullptr != not_empty_ext3.get());
    ASSERT_EQ((*not_empty_ext3).text(), "test2");
    _locked_ext->text("test3");
    ASSERT_EQ((*not_empty_ext3).text(), "test2");
    ASSERT_FALSE(not_empty_ext3.is_locked());

    // Calling assignment over a locked external.
    external<ExternalClass> locked_ext {new ExternalClass("test"), true};
    EXPECT_THROW(locked_ext = _locked_ext, eprosima::fastcdr::exception::LockedExternalAccessException);
}

//! @test Tests the deference object functions.
TEST(XCdrExternalAPITest, deference_object)
{
    external<ExternalClass> ext {new ExternalClass("test")};

    ASSERT_EQ((*ext).text(), "test");
    (*ext).text("test2");
    ASSERT_EQ((*ext).text(), "test2");

    const external<ExternalClass>& ext_ref = ext;
    ASSERT_EQ((*ext_ref).text(), "test2");
}

//! @test Tests the functions to get the pointer.
TEST(XCdrExternalAPITest, get_pointer)
{
    external<ExternalClass> ext {new ExternalClass("test")};

    ASSERT_EQ(ext.get()->text(), "test");
    ext.get()->text("test2");
    ASSERT_EQ(ext.get()->text(), "test2");

    const external<ExternalClass>& ext_ref = ext;
    ASSERT_EQ(ext_ref.get()->text(), "test2");
}

//! @test Tests the function to get the shared pointer.
TEST(XCdrExternalAPITest, get_shared_pointer)
{
    external<ExternalClass> ext {new ExternalClass("test")};

    ASSERT_EQ(ext.get_shared_ptr()->text(), "test");
    ext.get_shared_ptr()->text("test2");
    ASSERT_EQ(ext.get_shared_ptr()->text(), "test2");
}

//! @test Tests the deference object member functions.
TEST(XCdrExternalAPITest, deference_member)
{
    external<ExternalClass> ext {new ExternalClass("test")};

    ASSERT_EQ(ext->text(), "test");
    ext->text("test2");
    ASSERT_EQ(ext->text(), "test2");

    const external<ExternalClass>& ext_ref = ext;
    ASSERT_EQ(ext_ref->text(), "test2");
}

//! @test Tests the comparison functions.
TEST(XCdrExternalAPITest, compare)
{
    external<ExternalClass> empty_ext;
    external<ExternalClass> ext {new ExternalClass("test")};
    external<ExternalClass> ext2 {new ExternalClass("test")};

    ASSERT_FALSE(ext == empty_ext);
    ASSERT_TRUE(ext != empty_ext);

    ASSERT_FALSE(ext == ext2);
    ASSERT_TRUE(ext != ext2);

    ext = ext2;

    ASSERT_TRUE(ext == ext2);
    ASSERT_FALSE(ext != ext2);

    ext = empty_ext;

    ASSERT_TRUE(ext == empty_ext);
    ASSERT_FALSE(ext != empty_ext);

    ASSERT_FALSE(ext == ext2);
    ASSERT_TRUE(ext != ext2);

}

//! @test Tests function to check it is null.
TEST(XCdrExternalAPITest, check_null)
{
    external<ExternalClass> empty_ext;
    ASSERT_FALSE(empty_ext);
    ASSERT_TRUE(!empty_ext);
    external<ExternalClass> ext {new ExternalClass("test")};
    ASSERT_TRUE(ext);
    ASSERT_FALSE(!ext);
}


//! @test Tests the function to check it is locked.
TEST(XCdrExternalAPITest, is_locked)
{
    external<ExternalClass> empty_ext;
    ASSERT_FALSE(empty_ext.is_locked());

    external<ExternalClass> ext {new ExternalClass("test")};
    ASSERT_FALSE(ext.is_locked());

    external<ExternalClass> locked_ext {new ExternalClass("test"), true};
    ASSERT_TRUE(locked_ext.is_locked());
}

//! @test Tests the function to lock.
TEST(XCdrExternalAPITest, lock)
{
    external<ExternalClass> empty_ext;
    ASSERT_FALSE(empty_ext.is_locked());
    empty_ext.lock();
    ASSERT_TRUE(empty_ext.is_locked());

    external<ExternalClass> ext {new ExternalClass("test")};
    ASSERT_FALSE(ext.is_locked());
    ext.lock();
    ASSERT_TRUE(ext.is_locked());

    external<ExternalClass> locked_ext {new ExternalClass("test"), true};
    ASSERT_TRUE(locked_ext.is_locked());
    locked_ext.lock();
    ASSERT_TRUE(locked_ext.is_locked());
}
