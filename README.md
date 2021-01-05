# autotimer

A light weight utility to record the function execution time plus other power features.

## Why use it?

This utility focuses on usability and productivity. With two lines of code you can measure
any function's execution time:

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

However the true power of this utility is its "measuring suite".

Imaging you want to compare your brilliant new algorithm to some
old implementation and see **how much faster** it is.

Normally you would need to set up some test data, write a couple of functions
to call the old and the new algorithm, compile and execute...... and you
probably want to run it multiple times to get an average result. This
can easily become a long and dedious process. 

With **autotimer** you could do this instead:

```c++
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

Here, the mini DSL creates a measuring suite for you that has the following
features:

- withLabel: add a human readable string label to the report
- withMultiplier: run the test subject N times and calculate the average runtime
- withInit: similar to xUnit's setUp(), run the given closure before executing each subject
- measure: execute the test subject
- assertFaster: ensure that the subsequent runtime is faster than the previous one, otherwise throw an execution.

Once compiled and executed it generates the following report:

```text
compare algorithm with for-loop
    measure 1: 13,123 micro (10 runs, fastest: 11,341, slowest: 15,981)
    measure 2: 10,813 micro (10 runs, fastest: 10,100, slowest: 11,234) speedup: 1.2136x
    
    result: passed
```

## Examples:

[examples](./examples)

more examples will be coming soon.

## Requirements

A C++17 compliant compiler. Tested on gcc-8, 9 and 10.

(optional) CMake 3.10+

## How to integrate it into your own project

You may simply copy the header file `autotimer.hh` into your `3rdParty` or `vendor` directory.

Or if your project use CMake (3.10+), you may add autotimer as a git submodule (or download the source tree as a zip
file from github and unzip it in your own project), then add this statement in your top-level CMakeLists.txt: 
`add_subdirectory(path/to/autotimer/src)`. Your own cmake target could "link against" autotimer like so:
`target_link_libraries(mytarget PRIVATE autotimer)`, which adds the header file into your target'summary include path.

## Misc

Happy to take change suggestions and/or pull requests.

You may also look into other projects:

cpptime: <https://github.com/eglimi/cpptime>

boost timer library: <https://www.boost.org/doc/libs/1_53_0/libs/timer/doc/index.html>

**Quick-bench** looks really interesting: <https://quick-bench.com>

or take a look at these pro-level profilers:

orbit: <https://github.com/google/orbit>

tracy: <https://github.com/wolfpld/tracy>
