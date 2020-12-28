//
// Created by weining on 28/12/20.
//

#ifndef AUTOTIMER_AUTOTIMER_HH
#define AUTOTIMER_AUTOTIMER_HH

#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>
#include <sstream>
#include <type_traits>
#include <functional>
#include <utility>
#include <vector>
#include <tuple>
#include <numeric>

using Function = std::function< void() >;

namespace AutoTimer
{
struct FormatOptions
{
    enum UnitOptions
    {
        Micro,
    };
    UnitOptions unit{ Micro };
};
}  // namespace AutoTimer

namespace impl
{
void nothing()
{
}

struct Closure
{
    Function f{ nothing };
    bool hasFunction{ false };

    Closure() = default;

    explicit Closure( Function fun ) : f( std::move( fun ) ), hasFunction( true )
    {
    }

    Closure( Closure&& other ) noexcept
        : f( std::move( other.f ) ), hasFunction( other.hasFunction )
    {
    }

    Closure& operator=( Closure&& other ) noexcept
    {
        this->f = std::move( other.f );
        this->hasFunction = other.hasFunction;
        return *this;
    }

    Closure& operator=( const Closure& other ) = default;
};

// encapsulate the subject closure and optionally a "setUp" closure

using Duration = std::chrono::duration< long, std::ratio< 1, 1000000000 > >;
using Summary =
    std::tuple< std::string, size_t, Duration, Duration, Duration >;  // label, runs, average,
                                                                      // slowest, fastest
using SummaryCasted = std::tuple< std::string, size_t, size_t, size_t, size_t >;

template < typename Precision >
SummaryCasted durationCast( Summary s )
{
    using namespace std;
    using namespace chrono;
    return std::make_tuple( std::get< 0 >( s ),
                            std::get< 1 >( s ),
                            duration_cast< Precision >( std::get< 2 >( s ) ).count(),
                            duration_cast< Precision >( std::get< 3 >( s ) ).count(),
                            duration_cast< Precision >( std::get< 4 >( s ) ).count() );
}

struct Measurable
{
    Closure init{};
    Closure subject{};
    size_t multiplier{ 1 };
    std::string label{};

    Measurable() = default;

    Measurable( Function subjectF, Function initF, std::size_t mult )
        : subject( Closure{ std::move( subjectF ) } )
        , init{ Closure{ std::move( initF ) } }
        , multiplier( mult )
    {
    }

    Measurable( Function subjectF, Function initF, std::size_t mult, const char* label_ )
        : subject( Closure{ std::move( subjectF ) } )
        , init{ Closure{ std::move( initF ) } }
        , multiplier( mult )
        , label{ label_ }
    {
    }

    Summary measure() const
    {
        if ( !subject.hasFunction )
        {
            return {};
        }
        if ( init.hasFunction )
        {
            init.f();
        }

        std::vector< Duration > ds( multiplier );
        for ( int i = 0; i < multiplier; ++i )
        {
            auto begin = std::chrono::high_resolution_clock::now();
            subject.f();
            ds[ i ] = std::chrono::high_resolution_clock::now() - begin;
        }
        auto avg = std::accumulate( ds.cbegin(), ds.cend(), Duration{} ) / ds.size();
        std::sort( ds.begin(), ds.end() );
        return std::make_tuple( label, multiplier, avg, ds.front(), ds.back() );
    }
};

// encapsulate the runtime data
struct ReportEntry
{
    Summary s{};

    explicit ReportEntry( Summary summary ) : s( std::move( summary ) )
    {
    }

    double speedUpFrom( const ReportEntry& other ) const
    {
        using namespace std;
        using namespace chrono;
        auto a = duration_cast< nanoseconds >( std::get< 2 >( other.s ) ).count();
        auto b = duration_cast< nanoseconds >( std::get< 2 >( s ) ).count();
        return ( double )a / ( double )b;
    }

    std::ostream& formatted( size_t idx, std::ostream& os, AutoTimer::FormatOptions opt ) const
    {
        using namespace std;
        using namespace chrono;

        SummaryCasted sc{};
        std::string unit{};
        if ( opt.unit == AutoTimer::FormatOptions::Micro )
        {
            sc = durationCast< microseconds >( s );
            unit = " micro ";
        }
        else
        {
            sc = durationCast< milliseconds >( s );
            unit = " milli ";
        }

        if ( std::get< 0 >( sc ).empty() )
        {
            os << "measure " << idx << ": ";
        }
        else
        {
            os << std::get< 0 >( sc ) << ": ";
        }

        os << std::get< 2 >( sc )
           << unit
           // (num runs, fastest, slowest)
           << "(" << std::get< 1 >( sc ) << " runs, fastest: " << std::get< 3 >( sc )
           << ", slowest: " << std::get< 4 >( sc ) << ')';
        return os;
    }
};
}  // namespace impl

namespace AutoTimer
{
struct Timer
{
    std::string label{};
    std::chrono::time_point< std::chrono::high_resolution_clock > begin{};
    std::ostream& os;
    std::size_t* p_out{ nullptr };
    std::size_t target{};

    explicit Timer( std::string s, std::ostream& os_ = std::cout )
        : label{ std::move( s ) }, os{ os_ }
    {
        begin = std::chrono::high_resolution_clock::now();
    }

    explicit Timer( std::string s,
                    std::size_t* out,
                    std::size_t comparison_target = 0,
                    std::ostream& os_ = std::cout )
        : label( std::move( s ) ), p_out( out ), target{ comparison_target }, os{ os_ }
    {
        begin = std::chrono::high_resolution_clock::now();
    }

    ~Timer()
    {
        using namespace std;
        using namespace chrono;
        auto d{ high_resolution_clock::now() - begin };
        // TypeTester<decltype(d)>{};
        std::size_t count = std::chrono::duration_cast< microseconds >( d ).count();
        if ( p_out )
        {
            *p_out = count;
        }
        auto locale = std::cout.imbue( std::locale( "" ) );
        if ( !label.empty() )
        {
            os << label << " ";
        }
        if ( target )
        {
            double speed_up = static_cast< double >( target ) / static_cast< double >( count );
            os << count << " micro seconds, speedup: " << speed_up << '\n';
        }
        else
        {
            os << std::fixed << count << " micro-secs" << '\n';
        }
        std::cout.imbue( locale );
    }
};

struct Report
{
    std::string label{};
    std::vector< impl::ReportEntry > rs{};

    std::ostream& formatted( std::ostream& os, AutoTimer::FormatOptions opt )
    {
        os << label << '\n';
        size_t idx{ 0 };
        for ( const auto& r : rs )
        {
            r.formatted( idx + 1, os << "    ", opt );
            if ( idx > 0 )
            {
                os << " speedup: " << r.speedUpFrom( rs[ 0 ] );
            }
            os << '\n';
            idx += 1;
        }
        return os;
    }
};

class Builder
{
public:
    Builder() = default;

    Builder& withLabel( const char* s )
    {
        report.label = s;
        return *this;
    }

    Builder& withMultiplier( size_t n )
    {
        mult = n;
        for ( auto& m : ms )
        {
            m.multiplier = n;
        }
        return *this;
    }

    template < typename Function, typename = std::void_t< decltype( std::declval< Function >() ) > >
    Builder& withInit( Function&& f )
    {
        init = impl::Closure{ std::forward< Function >( f ) };
        for ( auto& m : ms )
        {
            m.init = init;
        }
        return *this;
    }

    Builder& withOutputStream( std::ostream& output )
    {
        os = &output;
        return *this;
    }

    template < typename Function, typename = std::void_t< decltype( std::declval< Function >() ) > >
    Builder& measure( Function&& f )
    {
        ms.emplace_back( std::forward< Function >( f ), init.f, mult );
        return *this;
    }

    template < typename Function, typename = std::void_t< decltype( std::declval< Function >() ) > >
    Builder& measure( const char* label, Function&& f )
    {
        ms.emplace_back( std::forward< Function >( f ), init.f, mult, label );
        return *this;
    }

    void fulfill()
    {
        if ( !fulfilled )
        {
            for ( const auto& m : ms )
            {
                auto d = m.measure();
                report.rs.emplace_back( d );
            }
            report.formatted( os ? *os : std::cout, FormatOptions{} );
            fulfilled = true;
        }
    }

    void assertFaster()
    {
        if ( !fulfilled )
        {
            for ( const auto& m : ms )
            {
                auto d = m.measure();
                report.rs.emplace_back( d );
            }
            auto& base = report.rs[ 0 ];
            auto slowdown = std::count_if(
                report.rs.cbegin(), report.rs.cend(), [ &base ]( const impl::ReportEntry& r ) {
                    return std::get< 2 >( r.s ) > std::get< 2 >( base.s );
                } );

            if ( slowdown == 0 )
            {
                report.formatted( std::cout, FormatOptions{} ) << "assertFaster: passed\n";
                fulfilled = true;
            }
            else
            {
                report.formatted( std::cerr, FormatOptions{} ) << "assertFaster: failed\n";
                exit( 1 );
            }
        }
    }

    ~Builder()
    {
        fulfill();
    };

private:
    std::vector< impl::Measurable > ms;
    Report report{};
    std::ostream* os{ nullptr };
    bool fulfilled{ false };
    size_t mult{ 1 };
    impl::Closure init{};
};
};      // namespace AutoTimer
#endif  // AUTOTIMER_AUTOTIMER_HH
