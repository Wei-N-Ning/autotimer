//
// Created by weining on 2/1/21.
//

#include "impl/tasks.hh"

int main()
{
    using namespace AutoTimer;
    static_assert( TaskTrait< Task >::paramSize == 0 );
    static_assert( TaskTrait< Task1D< int > >::paramSize == 1 );
    static_assert( TaskTrait< Task2D< int, int > >::paramSize == 2 );
    static_assert( TaskTrait< TaskMultiDim< int, int, int > >::paramSize == 3 );
}