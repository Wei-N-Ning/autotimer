//
// Created by weining on 2/1/21.
//

#ifndef AUTOTIMER_TASKS_HH
#define AUTOTIMER_TASKS_HH

#include <functional>
#include <type_traits>

namespace AutoTimer
{
template < typename... Ts >
using TaskMultiDim = std::function< void( Ts... ) >;

using Task = TaskMultiDim<>;

template < typename T >
using Task1D = TaskMultiDim< T >;

template < typename A, typename B >
using Task2D = TaskMultiDim< A, B >;

template < typename T >
struct TaskTrait
{
};

template < typename... Ts >
struct TaskTrait< TaskMultiDim< Ts... > >
{
    static constexpr size_t paramSize = sizeof...( Ts );
};

}  // namespace AutoTimer

#endif  // AUTOTIMER_TASKS_HH
