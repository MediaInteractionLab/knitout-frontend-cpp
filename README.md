# knitout-frontend-cpp

A C++ wrapper for all operations supported by knitout with basic error and type checking. It is for the most parts a port of the (CMU Textiles Lab's )[https://textiles-lab.github.io/] (knitout-frontend-js)[https://github.com/textiles-lab/knitout-frontend-js]. API was kept as similar as possible, however slightly adapted wherever C++ language feaures left no other choice. Error checking is not more pedantit than the JS model, so handle with care.

Provided samples are based on the JS samples.

For now, only standards from C++11 upwards are supported.

Very simple example rectangle:
```C++
#include <knitout.h>
#include <iostream>

int main( int argc, char **argv )
{
  try
  {
    Knitout::Writer k( { "A", "B", "C"} );

    k.in( "B" );

    for( int n = 10; n >= 0; n -= 2 ) 
      k.tuck( "-", "f", n, "B" );

    for( int n = 1; n <= 9; n += 2 ) 
      k.tuck( "+", "f", n, "B" );

    for( int r = 0; r < 10; ++r )
    {
      for( int n = 10; n >= 0; --n ) 
        k.knit( "-", "f", n, "B" );
      for( int n = 0; n <= 10; ++n ) 
        k.knit( "+", "f", n, "B" );
    }

    k.out( "B" );

    k.write( "out.k" ); //write to file
    k.write(); //print to console
  }
  catch( std::exception & e )
  {
    std::cerr << "ERROR: caught exception: " << e.what() << std::endl;
  }
}
```

A more detailled description will follow; for the time being, check out the (JS frontend README)[https://github.com/textiles-lab/knitout-frontend-js/blob/master/README.md].

See [knitout specification](https://textiles-lab.github.io/knitout/knitout.html) for further details on the knitout format.