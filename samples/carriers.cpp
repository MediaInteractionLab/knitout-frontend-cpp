/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Media Interaction Lab
 *  Licensed under the MIT License. See LICENSE file in the package root for license information.
 *--------------------------------------------------------------------------------------------*/

#include "../knitout.h"

#include <vector>
#include <iostream>
#include <stdexcept>




int main( int argc, char **argv )
{
	try
	{
		Knitout::Writer k( { "1", "2", "3", "4", "10" } );
		k.addHeader( "Machine", "SWGXYZ" );
		k.addHeader( "Gauge", "15" );
		k.addHeader( "Presser", "On" );
		k.addHeader( "X-Presser", "On" );
		k.addHeader( "X-Takedown", "On" );

		k.knit( "-", "b3", "10" );
		k.knit( "-", "b3", "   10" );
		//k.knit( "+", "f1", "2", "3" );	//afaik, not possible to use ellipsis in C++ without hinting the number of arguments somewhere.
		k.knit( "+", "f1", std::vector<std::string>( { "1", "2" } ) );
		k.knit( "-", "b3", "1    3" );
		k.tuck( "-", "b3", "1,   4" );
		k.write( "carriers.k" );
	}
	catch( std::exception & e )
	{
		std::cerr << "ERROR: caught exception: " << e.what() << std::endl;
	}
}