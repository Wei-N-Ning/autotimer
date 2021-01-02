//
// Created by weining on 2/1/21.
//

#ifndef AUTOTIMER_MEASURABLE_HH
#define AUTOTIMER_MEASURABLE_HH

#include "tasks.hh"
#include "time_record.hh"

#include <numeric>
#include <optional>

namespace AutoTimer
{
namespace Impl
{
using namespace TimeRecord;

template < typename... Ts >
struct Measurable
{
    std::optional< TaskMultiDim< Ts... > > init{};
    std::optional< TaskMultiDim< Ts... > > subject{};
    size_t multiplier{ 1 };
    std::string label{};

    Measurable() = delete;

    explicit Measurable( const TaskMultiDim< Ts... >& t ) : subject( t )
    {
    }

    Measurable<Ts...>& withLabel( const std::string& s )
    {
        label = s;
        return *this;
    }

    Measurable<Ts...>& withInit( const std::optional< TaskMultiDim< Ts... > >& t )
    {
        init = t;
        return *this;
    }

    Measurable<Ts...>& withMultiplier( size_t mult )
    {
        multiplier = mult;
        return *this;
    }

    [[nodiscard]] Summary measure( Ts&&... args ) const
    {
        if ( !subject.has_value() )
        {
            return {};
        }
        if ( init.has_value() )
        {
            init.value()( std::forward< Ts >( args )... );
        }

        std::vector< Duration > ds( multiplier );
        for ( int i = 0; i < multiplier; ++i )
        {
            auto begin = std::chrono::high_resolution_clock::now();
            subject.value()( std::forward< Ts >( args )... );
            ds[ i ] = std::chrono::high_resolution_clock::now() - begin;
        }
        auto avg = std::accumulate( ds.cbegin(), ds.cend(), Duration{} ) / ds.size();
        std::sort( ds.begin(), ds.end() );
        return std::make_tuple( label, multiplier, avg, ds.front(), ds.back() );
    }
};
}  // namespace Impl

}  // namespace AutoTimer
#endif  // AUTOTIMER_MEASURABLE_HH
