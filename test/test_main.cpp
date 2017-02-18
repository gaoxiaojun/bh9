#define CATCH_CONFIG_RUNNER
#include "catch.hpp"
#include "backward.hpp"

int main( int argc, char* argv[] )
{
  // global setup...
    backward::SignalHandling sh;
  int result = Catch::Session().run( argc, argv );

  // global clean-up...

  return ( result < 0xff ? result : 0xff );
}
