//
// Created by weining on 2/1/21.
//

#ifndef AUTOTIMER_SCALING_HH
#define AUTOTIMER_SCALING_HH

#include "measurable.hh"
#include "time_record.hh"
#include "utilities.hh"

#include <iostream>
#include <tuple>
#include <chrono>
#include <vector>
#include <type_traits>
#include <memory>

namespace AutoTimer::Scaling
{
using namespace AutoTimer::TimeRecord;

template < typename... Ts >
using Param = std::tuple< Ts... >;

template < typename T >
struct ScalingParam
{
    virtual T begin() = 0;
    virtual bool end( T ) = 0;
    virtual T next( T ) = 0;
};

template < typename T >
struct Constant : public ScalingParam< T >
{
    T value;
    size_t times{ 1 };
    size_t it{ 0 };

    explicit Constant( T x, size_t n ) : value( x ), times( n )
    {
    }

    T begin() override
    {
        it = 0;
        return value;
    }

    bool end( T ) override
    {
        return it >= times;
    }

    T next( T ) override
    {
        it += 1;
        return value;
    }
};

template < typename T >
struct Linear : public ScalingParam< T >
{
    T startV;
    T endV;

    Linear( T s, T e ) : startV( s ), endV( e )
    {
    }

    T begin() override
    {
        return startV;
    }

    bool end( T curr ) override
    {
        return curr >= endV;
    }

    T next( T curr ) override
    {
        return ++curr;
    }
};

template < typename T >
struct Discrete : public ScalingParam< T >
{
    std::vector< T > values;
    size_t it{ 0 };

    template < typename... Ts >
    explicit Discrete( Ts... args )
    {
        ( values.push_back( args ), ... );
    }

    T begin() override
    {
        it = 0;
        return values[ it ];
    }

    bool end( T ) override
    {
        return it >= std::size( values );
    }

    T next( T ) override
    {
        ++it;
        return values[ it ];
    }
};

template < typename T >
using LabelledParameter = std::tuple< std::string, std::shared_ptr< ScalingParam< T > > >;

template < typename T >
LabelledParameter< T > makeConstant( T x, size_t times )
{
    return { "constant", std::make_shared< Constant< T > >( x, times ) };
}

template < typename T >
LabelledParameter< T > makeConstant( const char* s, T x, size_t times )
{
    return { s, std::make_shared< Constant< T > >( x, times ) };
}

template < typename T, typename... Ts >
LabelledParameter< T > makeDiscrete( T head, Ts... tail )
{
    return { "discrete", std::make_shared< Discrete< T > >( head, tail... ) };
}

template < typename T, typename... Ts >
LabelledParameter< T > makeDiscrete( const char* s, T head, Ts... tail )
{
    return { s, std::make_shared< Discrete< T > >( head, tail... ) };
}

template < typename T >
LabelledParameter< T > makeLinear( T a, T b )
{
    return { "linear", std::make_shared< Linear< T > >( a, b ) };
}

template < typename T >
LabelledParameter< T > makeLinear( const char* s, T a, T b )
{
    return { s, std::make_shared< Linear< T > >( a, b ) };
}

template < typename... Ps >
RecordMultiDim<> scale( Impl::Measurable< Ps... > me, Param< Ps... > param )
{
    RecordMultiDim<> r{};
    r.summary = std::apply( &Impl::Measurable< Ps... >::measure,
                            std::tuple_cat( std::make_tuple( me ), param ) );
    return r;
}

template < typename T, typename... Fs, typename... Ts, typename... Ps >
RecordMultiDim< T, Ts... > scale( Impl::Measurable< Fs... > me,
                                  Param< Ps... > param,
                                  LabelledParameter< T > labelledParameter,
                                  LabelledParameter< Ts >... parameters )
{
    RecordMultiDim< T, Ts... > record{};
    static_assert( std::is_same_v< std::tuple< T, RecordMultiDim< Ts... > >,
                                   typename decltype( record )::fields_type > );

    auto& [ label, arg ] = labelledParameter;
    record.label = label;
    for ( auto i = arg->begin(); !arg->end( i ); i = arg->next( i ) )
    {
        auto field = scale( me, std::tuple_cat( param, std::make_tuple( i ) ), parameters... );
        static_assert( std::is_same_v< RecordMultiDim< Ts... >, decltype( field ) > );
        record.fields.emplace_back( i, std::move( field ) );
    }

    return record;
}

template < typename... Ts >
RecordMultiDim< Ts... > scaleWith( Impl::Measurable< Ts... > me, LabelledParameter< Ts >... args )
{
    return scale( me, Param<>{}, args... );
}

template < typename... Ts >
RecordMultiDim< Ts... > scaleTu( Impl::Measurable< Ts... > me,
                                 std::tuple< LabelledParameter< Ts >... > tu )
{
    return std::apply( scaleWith< Ts... >, std::tuple_cat( std::make_tuple( me ), tu ) );
}

}  // namespace AutoTimer::Scaling

#endif  // AUTOTIMER_SCALING_HH
