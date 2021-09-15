/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Media Interaction Lab
 *  Licensed under the MIT License. See LICENSE file in the package root for license information.
 *--------------------------------------------------------------------------------------------*/

#pragma once

#include <set>
#include <map>
#include <list>
#include <vector>

#include <string>

namespace Knitout
{
	class Writer
	{
	private:
		static const std::string CarrierDelimiters;
		static const char *SupportedPositions[];

		//public data:
		std::map<std::string, bool>	_currentCarriers;	//all currently active carriers (key = name, value = hook)
		std::set<std::string>		_currentNeedles;	//all currently-holding-loops needles (key = name)

		float _currentRacking;							//current racking value

		//private data:
		std::vector<std::string>	_carriers;			//array of carrier names, front-to-back order
		std::vector<std::string>	_operations;		//array of operations, stored as strings
		std::list<std::string>		_headers;			//array of headers. stored as strings

		std::string _machine;							//machine name

		//throw warning if ;;Machine: header is included & machine doesn't support extension
		bool machineSupport( const std::string &extension, const std::string &supported );

		void validateCarrier( const std::string &c );
		void validateDirection( const std::string &d );
		void validateBed( const std::string &b );
		void validateNeedle( int n );

		void parseBedNeedle( const std::string &bedNeedle, std::string &bed, int &needle );

		void internalIn( const std::string &c, bool useHook = false );
		void internalReleaseHook( const std::string &c );
		void internalOut( const std::string &c );
		void internalWrite( std::ostream &ostr );

	public:
		explicit Writer( const std::vector<std::string> &carriers );

		// function that queues header information to header list
		void addHeader( const std::string &name, const std::string &value );

		// escape hatch to dump your custom instruction to knitout
		// if you know what you are doing
		void addRawOperation( const std::string &operation );

		void in( const std::string &c );
		void in( const std::vector<std::string> &cs );

		void inhook( const std::string &c );
		void inhook( const std::vector<std::string> &cs );

		void releasehook( const std::string &c );
		void releasehook( const std::vector<std::string> &cs );

		void out( const std::string &c );
		void out( const std::vector<std::string> &cs );

		void outhook( const std::string &c );
		void outhook( const std::vector<std::string> &cs );

		void stitch( int before, int after );

		// --- extensions ---
		void stitchNumber( int stitchNumber );

		void fabricPresser( const std::string &presserMode );

		void speedNumber( int value );

		void rollerAdvance( int value );

		void addRollerAdvance( int value );

		void carrierSpacing( int value );

		void carrierStoppingDistance( int value );

		// --- operations ---//
		void rack( float rack );

		void knit( const std::string &dir, const std::string &bed, int needle, const std::string &c = "" );
		void knit( const std::string &dir, const std::string &bed, int needle, const std::vector<std::string> &cs );
		void knit( const std::string &dir, const std::string &bedNeedle, const std::string &c = "" );
		void knit( const std::string &dir, const std::string &bedNeedle, const std::vector<std::string> &cs );

		void tuck( const std::string &dir, const std::string &bed, int needle, const std::string &c = "" );
		void tuck( const std::string &dir, const std::string &bed, int needle, const std::vector<std::string> &cs );
		void tuck( const std::string &dir, const std::string &bedNeedle, const std::string &c = "" );
		void tuck( const std::string &dir, const std::string &bedNeedle, const std::vector<std::string> &cs );

		void split( const std::string &dir, const std::string &fromBed, int fromNeedle, const std::string &toBed, int toNeedle, const std::string &c = "" );
		void split( const std::string &dir, const std::string &fromBed, int fromNeedle, const std::string &toBed, int toNeedle, const std::vector<std::string> &cs );
		void split( const std::string &dir, const std::string &fromBedNeedle, const std::string &toBedNeedle, const std::string &c = "" );
		void split( const std::string &dir, const std::string &fromBedNeedle, const std::string &toBedNeedle, const std::vector<std::string> &cs );

		void miss( const std::string &dir, const std::string &bed, int needle, const std::string &c );
		void miss( const std::string &dir, const std::string &bed, int needle, const std::vector<std::string> &cs );
		void miss( const std::string &dir, const std::string &bedNeedle, const std::string &c );
		void miss( const std::string &dir, const std::string &bedNeedle, const std::vector<std::string> &cs );

		// drop -> knit without yarn, but supported in knitout
		void drop( const std::string &bed, int needle );
		void drop( const std::string &bedNeedle );

		// amiss -> tuck without yarn, but supported in knitout
		void amiss( const std::string &bed, int needle );
		void amiss( const std::string &bedNeedle );

		// xfer -> split without yarn, but supported in knitout
		void xfer( const std::string &fromBed, int fromNeedle, const std::string &toBed, int toNeedle );
		void xfer( const std::string &fromBedNeedle, const std::string &toBedNeedle );

		// add comments to knitout 
		void comment( const std::string &str );

		void pause( const std::string &comment );

		void write( const std::string &filename = "" );
	};
}