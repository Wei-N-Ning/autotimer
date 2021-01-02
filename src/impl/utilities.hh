//
// Created by weining on 2/1/21.
//

#ifndef AUTOTIMER_UTILITIES_HH
#define AUTOTIMER_UTILITIES_HH

#include <iostream>
#include <tuple>
#include <functional>

template < typename... Ts >
auto& operator<<( std::ostream& os, const std::tuple< Ts... >& tu )
{
    std::apply( [ &os ]( auto&&... xs ) { ( ( os << xs << " " ), ... ); }, tu );
    return os;
}

#endif  // AUTOTIMER_UTILITIES_HH
