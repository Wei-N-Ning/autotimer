//
// Created by weining on 2/1/21.
//

#ifndef AUTOTIMER_ANALYTIC_HH
#define AUTOTIMER_ANALYTIC_HH

#include "export.hh"
#include "time_record.hh"

namespace AutoTimer::Analytic
{
size_t numSlowdown( const AutoTimer::Report<>& report )
{
    auto& base = report.timeRecords[ 0 ];
    auto slowdown =
        std::count_if( report.timeRecords.cbegin(),
                       report.timeRecords.cend(),
                       [ &base ]( const AutoTimer::TimeRecord::RecordMultiDim<>& r ) {
                           return std::get< 2 >( r.summary ) > std::get< 2 >( base.summary );
                       } );
    return slowdown;
}
}  // namespace AutoTimer::Analytic

#endif  // AUTOTIMER_ANALYTIC_HH
