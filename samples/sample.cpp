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
		Knitout::Writer k( { "B","A","2","C" } );
		k.addHeader( "Machine", "SWGXYZ" );
		k.addHeader( "Gauge", "15" );
		k.addHeader( "Presser", "On" );
		k.addHeader( "X-Presser", "On" );
		k.addHeader( "X-Takedown", "On" );
		k.in( { "A", "B", "C" } );
		try
		{
			k.inhook( "C" );		//carrier C is already in
		}
		catch( std::exception & e )
		{
			std::cerr << "caught exception: " << e.what() << std::endl;
		}
		try
		{
			k.releasehook( "A" );	//carrier A is not in hook
		}
		catch( std::exception & e )
		{
			std::cerr << "caught exception: " << e.what() << std::endl;
		}
		k.stitch( 10, 20 );
		k.stitch( 10.0, 20 );
		k.stitch( 10.33, 20 );
		k.knit( "+", "f10", "A, 2" );
		k.knit( "+", "f10", "A, B, C" );
		k.comment( "the following is a badly named single string carrier:" );
		try
		{
			k.knit( "+", "f10", "1,2" ); //invalid carrier name
		}
		catch( std::exception &e )
		{
			std::cerr << "caught exception: " << e.what() << std::endl;
		}
		k.knit( "+", "f1099" );
		try
		{
			k.knit( "_", "f1099" ); //_ is not a direction
		}
		catch( std::exception & e )
		{
			std::cerr << "caught exception: " << e.what() << std::endl;
		}
		k.rack( 0.5 );
		try
		{
			k.xfer( "fs10", "fs10.5" ); //10.5 is not a valid needle
		}
		catch( std::exception & e )
		{
			std::cerr << "caught exception: " << e.what() << std::endl;
		}
		k.drop( "f10" );
		k.amiss( "f20" );
		k.comment( "transfers are supported in knitout as an opcode" );
		k.xfer( "f20", "b20" );
		k.split( "+", "b10", "f20", "A, B" );
		try
		{
			k.split( "+", "b10", "c20", "A, B" ); //c20 isn't a valid bed
		}
		catch( std::exception & e )
		{
			std::cerr << "caught exception: " << e.what() << std::endl;
		}
		try
		{
			k.knit( "-", "f", 10000, "1" ); //"1" not a carrier
		}
		catch( std::exception & e )
		{
			std::cerr << "caught exception: " << e.what() << std::endl;
		}
		try
		{
			k.knit( "-", "b", 40000, "20000" ); //not a valid carrier name
		}
		catch( std::exception & e )
		{
			std::cerr << "caught exception: " << e.what() << std::endl;
		}
		try
		{
			k.knit( "-", "b", 40000, "20000" ); //not a valid carrier name
		}
		catch( std::exception & e )
		{
			std::cerr << "caught exception: " << e.what() << std::endl;
		}
		try
		{
			k.miss( "+", "b", -100, "4" ); //not a valid needle
		}
		catch( std::exception & e )
		{
			std::cerr << "caught exception: " << e.what() << std::endl;
		}
		k.out( "A, B, C" );
		try
		{
			k.outhook( { "A", "B", "C" } );
		}
		catch( std::exception & e )
		{
			std::cerr << "caught exception: " << e.what() << std::endl;
		}
		k.pause( "\tsome comment about the pause" );
		k.comment( "a \tcomment; \n another comment;" );
		k.comment( ";another comment" );
		k.comment( ";;;more comments" );
		k.write( "out.k" );
	}
	catch( std::exception & e )
	{
		std::cerr << "ERROR: caught exception: " << e.what() << std::endl;
	}
}