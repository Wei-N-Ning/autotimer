//
// Created by weining on 2/1/21.
//

#ifndef AUTOTIMER_EXPORT_HH
#define AUTOTIMER_EXPORT_HH

#include "time_record.hh"

#include <iostream>
#include <iomanip>
#include <tuple>

namespace AutoTimer
{
using namespace TimeRecord;

std::ostream& renderCastedSummary( std::ostream& os,
                                   size_t indent,
                                   AutoTimer::TimeUnitOptions opt,
                                   const SummaryCasted& summary )
{
    std::string label{};
    if ( std::get< 0 >( summary ).empty() )
    {
        label = "measure";
    }
    else
    {
        label = std::get< 0 >( summary );
    }
    std::string unit{};
    if ( opt == AutoTimer::TimeUnitOptions::MicroSecond )
    {
        unit = " micro ";
    }
    else if ( opt == AutoTimer::TimeUnitOptions::MilliSecond )
    {
        unit = " milli ";
    }
    else
    {
        unit = " nano ";
    }
    os << std::string( indent, ' ' ) << label << ": ";
    if ( std::get< 1 >( summary ) == 1 )
    {
        os << std::get< 2 >( summary ) << unit;
    }
    else
    {
        os << std::get< 2 >( summary ) << unit << " (" << std::get< 1 >( summary ) << " runs, "
           << std::get< 3 >( summary ) << " - " << std::get< 4 >( summary ) << ')';
    }
    return os;
}

template < typename... Ts >
std::ostream& render( std::ostream& os,
                      size_t indent,
                      AutoTimer::TimeUnitOptions opt,
                      const RecordMultiDim< Ts... >& record )
{
    if constexpr ( sizeof...( Ts ) > 1 )
    {
        // header (more useful for serializing to a document format)
        // do nothing here...

        // body
        for ( const auto& [ parameter, field ] : record.fields )
        {
            os << std::string( indent, ' ' );
            os << record.label << "(" << parameter << ")\n";
            render( os, indent + 4, opt, field ) << '\n';
        }

        // footer (close the open bracket etc.)
        // do nothing here
    }
    else
    {
        if constexpr ( sizeof...( Ts ) == 1 )
        {
            for ( const auto& [ parameter, field ] : record.fields )
            {
                os << std::string( indent, ' ' );
                os << record.label << "(" << parameter << ") ";
                auto castedSummary = field.castSummary( opt );
                renderCastedSummary( os, 0, opt, castedSummary ) << '\n';
            }
        }
        else
        {
            auto castedSummary = record.castSummary( opt );
            renderCastedSummary( os, indent, opt, castedSummary ) << '\n';
        }
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
        for ( const auto& timeRecord : timeRecords )
        {
            render( os, 4, opt, timeRecord );
        }
        return os;
    }
};
}  // namespace AutoTimer
#endif  // AUTOTIMER_EXPORT_HH
