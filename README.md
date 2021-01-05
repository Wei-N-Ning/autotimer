# autotimer

A light weight header-only utility to record the function execution time plus other power features.

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

Here, the mini DSL creates a measuring suite for you that has the following features:

- withLabel: add a human readable string label to the report
- withMultiplier: run the test subject N times and calculate the average runtime
- withInit: similar to xUnit's setUp(), tell the suite to run the given init routine before executing each test subject
- measure: enqueue the test subject for later evaluation; you can call `measure()` multiple times in the same suite (just like you can have any number of test methods in an xUnit suite)
- assertFaster: execute all the test subjects (and their init routines) and ensure that the subsequent runtime is faster than the previous one, otherwise throw an execution.

Once compiled and executed it generates the following report:

```text
compare algorithm with for-loop
    measure 1: 13,123 micro (10 runs, fastest: 11,341, slowest: 15,981)
    measure 2: 10,813 micro (10 runs, fastest: 10,100, slowest: 11,234) speedup: 1.2136x
    
    result: passed
```

It's not compulsory to do assertion at the end of the measuring suite since sometimes we just want to compare the speed and runtime. In that case, simply omit the `assertFaster` step. The suite will execute all the test subjects (and their init routine) and generate the report once it calls its destructor (i.e. when it goes out of scope). Yep, you may call it lazy-evaluation.

You should also take advantage of the init routine to do time-consuming set up works (it could take much more time than the actual test subjects!) and dedicate the test subjects to your core algorithms. If the init routine and the test subjects share state, it has to be explicitly captured as a reference in the lambda function like shown in the above list `[&xs]`.

## Examples:

[examples](./examples)

more examples will be coming soon.

## Requirements

A C++17 compliant compiler. Tested on gcc-8, 9 and 10.

(optional) CMake 3.10+

## How to integrate it into your own project

You may simply copy this repo into your `3rdParty` or `vendor` directory and add `src` to your include path.

Or if your project use CMake (3.10+), you may add autotimer as a git submodule (or download the source tree as a zip
file from github and unzip it in your own project), then add this statement in your top-level CMakeLists.txt: 
`add_subdirectory(path/to/autotimer/src)`. Set up the dependencies of your own cmake targets,
`target_link_libraries(mytarget PRIVATE autotimer)`, this adds the header file into your targets' include path.

## Misc

Happy to take change suggestions and/or pull requests.

You may also look into other projects:

cpptime: <https://github.com/eglimi/cpptime>

boost timer library: <https://www.boost.org/doc/libs/1_53_0/libs/timer/doc/index.html>

**Quick-bench** looks really interesting: <https://quick-bench.com>

and I've been a fan of google's benchmark library: <https://github.com/google/benchmark>

(however it's a bit too heavy, hence my motivation to write a header-only version)

or take a look at these pro-level profilers:

orbit: <https://github.com/google/orbit>

tracy: <https://github.com/wolfpld/tracy>
