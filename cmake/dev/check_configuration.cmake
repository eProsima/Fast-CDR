macro(check_stdcxx)
    # Check C++11
    include(CheckCXXCompilerFlag)
    if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANG)
        check_cxx_compiler_flag(--std=c++11 SUPPORTS_CXX11)
        if(SUPPORTS_CXX11)
            add_compile_options(--std=c++11)
            set(HAVE_CXX11 1)
        endif()
    endif()
endmacro()

macro(check_endianness)
    # Test endianness
    include(TestBigEndian)
    test_big_endian(BIG_ENDIAN)
    set(__BIG_ENDIAN__ ${BIG_ENDIAN})
endmacro()
