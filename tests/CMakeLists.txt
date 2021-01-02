add_executable(test_scaling_multidim test_scaling_multidim.cpp)
target_link_libraries(test_scaling_multidim PRIVATE autotimer)
add_test(NAME "autotimer::tests::scaling_multi_dimension" COMMAND test_scaling_multidim)

add_executable(test_tasks test_tasks.cpp)
target_link_libraries(test_tasks PRIVATE autotimer)
add_test(NAME "autotimer::tests::tasks" COMMAND test_tasks)

add_executable(test_measurable test_measurable.cpp)
target_link_libraries(test_measurable PRIVATE autotimer)
add_test(NAME "autotimer::tests::measurable" COMMAND test_measurable)
