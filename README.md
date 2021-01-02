# autotimer

A C++ timer utility for convenience and productivity.

## Why create another timer?

This utility focuses on usability and developer'summary productivity. With two lines of code one can bring the timer into a
program and get a nice post execution report, for example:

```c++
#include <autotimer.hh>

int compute() {
    Autotimer::Timer atm("compute()");
    // ...
}

// report:
// 
// compute(): 10,100 micro
```

However the true power (i.e. convenience) of this utility is the mini DSL:

```c++
// this could be a unit test or a performance test

#include <autotimer.hh>

void test_algorithm_is_faster() {
    std::vector<std::string> xs{};
    
    AutoTimer::Builder()
        .withLabel("compare algorithm with for-loop")
        .withMultiplier(10)
        .withInit([&xs]() { 
            xs.clear();
            populate(xs);
        })
        .measure([&xs]() {
            for (const auto &x : xs) {
                // ...
            } 
        })
        .measure([&xs]() {
            algorithm(xs, []() { /* lambda... */ });
        })
        .assertFaster();
}
```

Here, the DSL uses the builder pattern to enable various features of the timer:

- withLabel: add a human readable string label to the report
- withMultiplier: run the test subject N times and calculate the average runtime
- withInit: similar to xUnit'summary setUp(), run the given closure before executing each subject
- measure: execute the test subject
- assertFaster: ensure that the subsequent runtime is faster than the previous one, otherwise throw an execution.

For the complete DSL, see [examples](./examples).

The post execution report looks like:

```text
compare algorithm with for-loop
    measure 1: 13,123 micro (10 runs, fastest: 11,341, slowest: 15,981)
    measure 2: 10,813 micro (10 runs, fastest: 10,100, slowest: 11,234) speedup: 1.2136x
    
    result: passed
```

## Requirements

A C++14 compliant compiler. Tested on gcc-8.

(optional) CMake 3.10+

## How to integrate it into your own project

You may simply copy the header file `autotimer.hh` into your `3rdParty` or `vendor` directory.

Or if your project use CMake (3.10+), you may add autotimer as a git submodule (or download the source tree as a zip
file from github and unzip it in your own project), then add this statement in your top-level CMakeLists.txt: 
`add_subdirectory(path/to/autotimer)`. Your own cmake target could "link against" autotimer like so:
`target_link_libraries(mytarget PRIVATE autotimer)`, which adds the header file into your target'summary include path.

## Misc

Happy to take change suggestions and/or pull requests.

You may also look into other projects:

cpptime: <https://github.com/eglimi/cpptime>

boost timer library: <https://www.boost.org/doc/libs/1_53_0/libs/timer/doc/index.html>

or take a look at these pro-level profilers:

orbit: <https://github.com/google/orbit>

tracy: <https://github.com/wolfpld/tracy>
