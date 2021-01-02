//
// Created by weining on 2/1/21.
//

#ifndef AUTOTIMER_TIMER_HH
#define AUTOTIMER_TIMER_HH

#include <chrono>
#include <iostream>
#include <string>

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
}  // namespace AutoTimer

#endif  // AUTOTIMER_TIMER_HH
