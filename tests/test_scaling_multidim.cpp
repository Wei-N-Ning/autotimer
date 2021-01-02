//
// Created by weining on 2/1/21.
//

#include <impl/time_record.hh>
#include <impl/scaling.hh>
#include <impl/export.hh>
#include <impl/measurable.hh>

#include <cassert>
#include <functional>
#include <iostream>
#include <sstream>
#include <type_traits>

template < typename T >
class TypeTester;

void test_produce_multi_dimensional_time_record()
{
    using namespace AutoTimer::Scaling;
    static_assert( std::is_same_v< void, RecordMultiDim<>::value_type > );
    static_assert( std::is_same_v< int, RecordMultiDim< int, float, double >::value_type > );

    static_assert( std::is_same_v< std::tuple< int, RecordMultiDim< float, double > >,
                                   RecordMultiDim< int, float, double >::fields_type > );
}

int fib( int n )
{
    return ( n > 2 ) ? fib( n - 1 ) + fib( n - 2 ) : 1;
}

void test_scale_function()
{
    using namespace AutoTimer::Scaling;
    auto me = AutoTimer::Impl::Measurable< int, int, char >( []( int, int, char ) { fib( 20 ); } );
    auto summary = me.measure( 1, 1, 'a' );
    auto r = AutoTimer::Scaling::scaleWith(
        me, makeLinear( 1, 3 ), makeDiscrete( 1, 2 ), makeConstant( 'a', 2 ) );
    AutoTimer::render( std::cout, 0, AutoTimer::TimeUnitOptions::MicroSecond, r ) << '\n';
}

int main()
{
    test_produce_multi_dimensional_time_record();
    test_scale_function();
    return 0;
}