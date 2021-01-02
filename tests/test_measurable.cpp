//
// Created by weining on 2/1/21.
//

#include <cassert>
#include <chrono>
#include <sstream>
#include <type_traits>

#include "impl/measurable.hh"
#include "impl/time_record.hh"
#include "impl/utilities.hh"

int main()
{
    using Measurable = AutoTimer::Impl::Measurable<>;

    // measure can only be constructed with a task object
    static_assert( !std::is_default_constructible_v< Measurable > );

    Measurable me( []() {} );
    assert( me.multiplier == 1 );
    assert( !me.init.has_value() );

    std::ostringstream oss;
    size_t state{ 0 };
    auto summary = AutoTimer::TimeRecord::durationCast< std::chrono::nanoseconds >(
        me.withInit( [ &state ]() { state = 1; } ).withMultiplier( 100 ).measure() );
    oss << summary;
    assert( !oss.str().empty() );
    assert( state == 1 );

    return 0;
}
