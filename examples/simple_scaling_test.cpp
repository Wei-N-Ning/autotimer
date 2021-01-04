//
// Created by weining on 4/1/21.
//

#include <autotimer.hh>

void scale_with_one_parameter()
{
    AutoTimer::Builder()
        .withLabel("one param")
        .withScaling( AutoTimer::Scaling::makeLinear( "size of work", 1, 6 ) )
        .measure( "do work", []( int ) {} );
}

void scale_with_two_parameters()
{
    AutoTimer::Builder()
        .withLabel("two params")
        .withScaling( AutoTimer::Scaling::makeLinear( "size of work", 1, 3 ),
                      AutoTimer::Scaling::makeDiscrete( "num workers", 4, 10 ) )
        .withInit( []( int, int ) {} )
        .withMultiplier( 10 )
        .measure( "some test", []( int, int ) {} );
}

int main()
{
    scale_with_one_parameter();
    scale_with_two_parameters();
    return 0;
}