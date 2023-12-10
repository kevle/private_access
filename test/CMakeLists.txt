add_library(test_warning_flags INTERFACE)
if(MSVC)
    target_compile_options(test_warning_flags INTERFACE /W4 /WX)
else()
    target_compile_options(test_warning_flags INTERFACE -Wall -Wextra -pedantic -Werror)
endif()

add_executable(compile_test "compile_test.cpp")
target_compile_features(compile_test PUBLIC cxx_std_20)
target_link_libraries(compile_test PUBLIC private_access test_warning_flags)
add_test(NAME compile_test COMMAND compile_test)

add_library(link_test SHARED "link_a.cpp" "link_b.cpp")
target_compile_features(link_test PUBLIC cxx_std_20)
target_link_libraries(link_test PUBLIC private_access test_warning_flags)

add_executable(usage "usage.cpp")
target_compile_features(usage PUBLIC cxx_std_20)
target_link_libraries(usage PUBLIC private_access test_warning_flags)
