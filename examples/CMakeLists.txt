add_executable(autotimer_example_basic basic.cpp)
target_link_libraries(autotimer_example_basic PRIVATE autotimer)
add_test(NAME "autotimer::examples::basic" COMMAND autotimer_example_basic)

add_executable(autotimer_example_builder_dsl builder_dsl.cpp)
target_link_libraries(autotimer_example_builder_dsl PRIVATE autotimer)
add_test(NAME "autotimer::examples::builder_dsl" COMMAND autotimer_example_builder_dsl)

