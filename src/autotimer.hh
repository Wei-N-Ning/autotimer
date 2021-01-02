//
// Created by weining on 28/12/20.
//

#ifndef AUTOTIMER_AUTOTIMER_HH
#define AUTOTIMER_AUTOTIMER_HH

#include <algorithm>
#include <functional>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <optional>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>
#include <tuple>

#include "impl/analytic.hh"
#include "impl/export.hh"
#include "impl/measurable.hh"
#include "impl/tasks.hh"
#include "impl/time_record.hh"
#include "impl/timer.hh"
#include "impl/scaling.hh"

namespace AutoTimer
{
template < typename... Ts >
class BasicBuilder
{
public:
    explicit BasicBuilder( AutoTimer::Scaling::LabelledParameter< Ts >... params )
        : scalingParameters( std::make_tuple( params... ) )
    {
    }

    BasicBuilder& withLabel( const char* s )
    {
        report.label = s;
        return *this;
    }

    BasicBuilder& withMultiplier( size_t n )
    {
        mult = n;
        for ( auto& m : ms )
        {
            m.multiplier = n;
        }
        return *this;
    }

    template < typename Function, typename = std::void_t< decltype( std::declval< Function >() ) > >
    BasicBuilder& withInit( Function&& f )
    {
        init = std::forward< Function >( f );
        for ( auto& m : ms )
        {
            m.init = init;
        }
        return *this;
    }

    BasicBuilder& withOutputStream( std::ostream& output )
    {
        os = &output;
        return *this;
    }

    template < typename... Ps >
    BasicBuilder< Ps... > withScaling( AutoTimer::Scaling::LabelledParameter< Ps >... args )
    {
        BasicBuilder< Ps... > builder( args... );
        return builder;
    }

    BasicBuilder< Ts... >& measure( const TaskMultiDim< Ts... >& task )
    {
        ms.emplace_back(
            AutoTimer::Impl::Measurable< Ts... >( task ).withInit( init ).withMultiplier( mult ) );
        return *this;
    }

    BasicBuilder< Ts... >& measure( const char* label, const TaskMultiDim< Ts... >& task )
    {
        ms.emplace_back( AutoTimer::Impl::Measurable< Ts... >( task )
                             .withInit( init )
                             .withMultiplier( mult )
                             .withLabel( label ) );
        return *this;
    }

    void runMeasures()
    {
        if constexpr ( sizeof...( Ts ) == 0 )
        {
            for ( const auto& m : ms )
            {
                auto summary = m.measure();
                report.timeRecords.emplace_back( summary );
            }
        }
        else
        {
            for ( const auto& m : ms )
            {
                auto r = AutoTimer::Scaling::scaleTu< Ts... >( m, scalingParameters );
                render( std::cout, 0, timeUnitOption, r );
            }
        }
    }

    void assertFaster()
    {
        if ( !fulfilled )
        {
            runMeasures();
            auto slowdown = Analytic::numSlowdown( report );
            std::string indent{ "    " };
            if ( slowdown == 0 )
            {
                report.formatted( std::cout, TimeUnitOptions::MicroSecond, indent )
                    << indent << "assertFaster: passed\n";
                fulfilled = true;
            }
            else
            {
                report.formatted( std::cerr, TimeUnitOptions::MicroSecond, indent )
                    << indent << "assertFaster: failed\n";
                exit( 1 );
            }
        }
    }

    ~BasicBuilder()
    {
        if ( !fulfilled )
        {
            runMeasures();
            report.formatted( os ? *os : std::cout, TimeUnitOptions::MicroSecond, "    " );
            fulfilled = true;
        }
    };

private:
    std::vector< AutoTimer::Impl::Measurable< Ts... > > ms;
    Report< Ts... > report{};
    TimeUnitOptions timeUnitOption{TimeUnitOptions::MicroSecond};
    std::ostream* os{ nullptr };
    bool fulfilled{ false };
    size_t mult{ 1 };
    std::optional< TaskMultiDim< Ts... > > init{};

    std::tuple< AutoTimer::Scaling::LabelledParameter< Ts >... > scalingParameters;
};

using Builder = BasicBuilder<>;

};  // namespace AutoTimer

#endif  // AUTOTIMER_AUTOTIMER_HH
