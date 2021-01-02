//
// Created by weining on 2/1/21.
//

#ifndef AUTOTIMER_TIME_RECORD_HH
#define AUTOTIMER_TIME_RECORD_HH

#include "utilities.hh"

#include <chrono>
#include <tuple>

namespace AutoTimer
{
enum class TimeUnitOptions
{
    MicroSecond,
    MilliSecond,
};

namespace TimeRecord
{
using Duration = std::chrono::duration< long, std::ratio< 1, 1000000000 > >;

using Summary = std::tuple< std::string, size_t, Duration, Duration, Duration >;

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

// encapsulate the runtime data
template < typename... Ts >
struct RecordMultiDim
{
};

template <>
struct RecordMultiDim<>
{
    bool value{};
    using value_type = void;

    bool fields{};
    using fields_type = void;

    AutoTimer::TimeRecord::Summary summary{};

    RecordMultiDim<>() = default;

    explicit RecordMultiDim<>( AutoTimer::TimeRecord::Summary summary )
        : summary( std::move( summary ) )
    {
    }

    [[nodiscard]] SummaryCasted castSummary( TimeUnitOptions opt ) const
    {
        if ( opt == TimeUnitOptions::MicroSecond )
        {
            return durationCast< std::chrono::microseconds >( summary );
        }
        else if ( opt == TimeUnitOptions::MilliSecond )
        {
            return durationCast< std::chrono::milliseconds >( summary );
        }
        else
        {
            return durationCast< std::chrono::nanoseconds >( summary );
        }
    }

    [[nodiscard]] double speedUpFrom( const RecordMultiDim<>& other ) const
    {
        using namespace std;
        using namespace chrono;
        auto a = duration_cast< nanoseconds >( std::get< 2 >( other.summary ) ).count();
        auto b = duration_cast< nanoseconds >( std::get< 2 >( summary ) ).count();
        return ( double )a / ( double )b;
    }

    [[nodiscard]] const std::string& label() const
    {
        return std::get< 0 >( summary );
    }
};

template < typename T, typename... Ts >
struct RecordMultiDim< T, Ts... >
{
    T value;
    using value_type = T;

    std::vector< std::tuple< T, RecordMultiDim< Ts... > > > fields;
    using fields_type = typename decltype( fields )::value_type;

    std::string label{};
};

}  // namespace TimeRecord

}  // namespace AutoTimer

#endif  // AUTOTIMER_TIME_RECORD_HH
