//
// Created by weining on 2/1/21.
//

#ifndef AUTOTIMER_EXPORT_HH
#define AUTOTIMER_EXPORT_HH

#include "time_record.hh"

#include <iostream>
#include <iomanip>

namespace AutoTimer
{
using namespace TimeRecord;

template < typename... Ts >
std::ostream& render( std::ostream& os,
                      size_t indent,
                      AutoTimer::TimeUnitOptions opt,
                      const RecordMultiDim< Ts... >& record )
{
    if constexpr ( sizeof...( Ts ) > 0 )
    {
        // header (more useful for serializing to a document format)
        // do nothing here...

        // body
        for ( const auto& [ parameter, field ] : record.fields )
        {
            os << std::string( indent, ' ' );
            os << record.label << "(" << parameter << ")\n";
            render( os, indent + 4, opt, field );
        }

        // footer (close the open bracket etc.)
        // do nothing here
    }
    else
    {
        os << std::string( indent, ' ' ) << "{" << record.castSummary( opt ) << "}\n";
    }
    return os;
}

template < typename... Ts >
struct Report
{
    std::string label{};
    std::vector< RecordMultiDim< Ts... > > timeRecords{};

    std::ostream& formatted( std::ostream& os,
                             AutoTimer::TimeUnitOptions opt,
                             const std::string& entryIndent ) const
    {
        os << label << '\n';
        size_t idx{ 0 };
        size_t labelLength{ 0 };
        std::for_each( timeRecords.cbegin(),
                       timeRecords.cend(),
                       [ &labelLength ]( const auto& timeRecord ) { labelLength = 0; } );

        for ( const auto& timeRecord : timeRecords )
        {
            render( os, 4, opt, timeRecord );
        }
        os << '\n';
        return os;
    }
};
}  // namespace AutoTimer
#endif  // AUTOTIMER_EXPORT_HH
