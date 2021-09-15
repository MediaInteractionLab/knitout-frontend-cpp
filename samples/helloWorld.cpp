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
		Knitout::Writer k( { "1", "2", "3", "4", "5", "6", "7", "8", "9", "10" } );

		// add some headers relevant to this job
		k.addHeader( "Machine", "SWGXYZ" );
		k.addHeader( "Gauge", "15" );

		// swatch variables
		int height = 10;
		int width = 10;
		std::string carrier = "6";
		k.fabricPresser( "auto" );
		// bring in carrier using yarn inserting hook
		k.inhook( carrier );
		// tuck on alternate needles to cast on
		for( int s = width; s > 0; s-- )
		{
			if( s % 2 == 0 )
			{
				k.tuck( "-", "f", s, carrier );
			}
			else
			{
				k.miss( "-", "f", s, carrier );
			}
		}
		for( int s = 1; s <= width; s++ )
		{
			if( s % 2 != 0 )
			{
				k.tuck( "+", "f", s, carrier );
			}
			else
			{
				k.miss( "+", "f", s, carrier );
			}
		}

		// release the yarn inserting hook
		k.releasehook( carrier );

		// knit some rows back and forth
		for( int h = 0; h < height; h++ )
		{
			for( int s = width; s > 0; s-- )
			{
				k.knit( "-", "f", s, carrier );
			}
			for( int s = 1; s <= width; s++ )
			{
				k.knit( "+", "f", s, carrier );
			}
		}

		// bring the yarn out with the yarn inserting hook
		k.outhook( carrier );

		// write the knitout to a file called "out.k"
		k.write( "out.k" );

		std::cout << "wrote out.k" << std::endl;
	}
	catch( std::exception & e )
	{
		std::cerr << "ERROR: caught exception: " << e.what() << std::endl;
	}
}
