//
// Created by weining on 28/12/20.
//

#include <algorithm>
#include <autotimer.hh>
#include <random>
#include <string>
#include <vector>

int fib( int n )
{
    return ( n > 2 ) ? fib( n - 1 ) + fib( n - 2 ) : 1;
}

void test_algorithm_is_faster()
{
    std::vector< std::string > xs( 10000 );

    AutoTimer::Builder()
        .withLabel( "compare algorithm with for-loop" )
        .withInit( [ &xs ]() {
            std::uniform_int_distribution< char > aToD( 'a', 'd' );
            std::random_device random_device;
            std::mt19937 eng( random_device() );
            for ( auto &s : xs )
            {
                s.resize( 4 );
                std::generate( s.begin(), s.end(), [ & ]() { return aToD( eng ); } );
            }
        } )
        .measure( [ &xs ]() {
            std::vector< std::string > found;
            for ( auto x : xs )
            {
                if ( x == "aaaa" )
                {
                    found.push_back( x );
                }
            }
        } )
        .measure( [ &xs ]() {
            auto last = std::partition( xs.begin(),
                                        xs.end(),
                                        // predicate
                                        []( const auto &s ) { return s == "aaaa"; } );
        } );
}

void test_algorithm_is_faster_specify_labels()
{
    std::vector< std::string > xs( 10000 );

    AutoTimer::Builder()
        .withLabel( "compare algorithm with for-loop (set subject labels and call assertion)" )
        .withMultiplier( 20 )
        .withInit( [ &xs ]() {
            std::uniform_int_distribution< char > aToD( 'a', 'd' );
            std::random_device random_device;
            std::mt19937 eng( random_device() );
            for ( auto &s : xs )
            {
                s.resize( 4 );
                std::generate( s.begin(), s.end(), [ & ]() { return aToD( eng ); } );
            }
        } )
        .measure(
            //
            "use for-loop      ",
            [ &xs ]() {
                std::vector< std::string > found;
                for ( auto x : xs )
                {
                    if ( x == "aaaa" )
                    {
                        found.push_back( x );
                    }
                }
            } )
        .measure(
            //
            "use std::partition",
            [ &xs ]() {
                auto last = std::partition( xs.begin(),
                                            xs.end(),
                                            // predicate
                                            []( const auto &s ) { return s == "aaaa"; } );
            } )
        .assertFaster();
}

int main()
{
    test_algorithm_is_faster();
    test_algorithm_is_faster_specify_labels();
    return 0;
}