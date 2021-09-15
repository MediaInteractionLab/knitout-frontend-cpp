/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Media Interaction Lab
 *  Licensed under the MIT License. See LICENSE file in the package root for license information.
 *--------------------------------------------------------------------------------------------*/

#include "knitout.h"

#include <cctype>
#include <fstream>
#include <sstream>
#include <iostream>

#include <iterator>
#include <algorithm>
#include <stdexcept>


namespace Knitout
{
	//---------------------
	//NOTE: taken from:
	// https://stackoverflow.com/questions/5288396/c-ostream-out-manipulation/5289170#5289170

	/*! note: delimiter cannot contain NUL characters
	 */
	template <typename Range, typename Value = typename Range::value_type>
	std::string join( const Range &elements, const char *const delimiter ) {
		std::ostringstream os;
		auto b = begin( elements ), e = end( elements );

		if( b != e )
		{
			std::copy( b, prev( e ), std::ostream_iterator<Value>( os, delimiter ) );
			b = prev( e );
		}
		if( b != e )
		{
			os << *b;
		}

		return os.str();
	}
	//---------------------

	//---------------------
	//NOTE: taken from
	// https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c
	std::vector<std::string> split( std::string s, std::string delimiter )
	{
		size_t pos_start = 0, pos_end, delim_len = delimiter.length();
		std::string token;
		std::vector<std::string> words;

		while( ( pos_end = s.find( delimiter, pos_start ) ) != std::string::npos )
		{
			token = s.substr( pos_start, pos_end - pos_start );
			pos_start = pos_end + delim_len;
			words.push_back( token );
		}
		words.push_back( s.substr( pos_start ) );

		return words;
	}
	//---------------------

	//NOTE: taken from
	// https://stackoverflow.com/questions/7621727/split-a-string-into-words-by-multiple-delimiters
	std::vector<std::string> splitAny( const std::string &s, const std::string &delimiters )
	{
		std::vector<std::string> words;

		size_t pos = 0, prev = 0;
		while( ( pos = s.find_first_of( &delimiters[0], prev ) ) != std::string::npos )
		{
			if( pos > prev )
				words.push_back( s.substr( prev, pos - prev ) );
			prev = pos + 1;
		}
		if( prev < s.length() )
			words.push_back( s.substr( prev ) );

		return words;
	}

	//---------------------
	//NOTE: taken from
	// https://stackoverflow.com/questions/4654636/how-to-determine-if-a-string-is-a-number-with-c
	bool isNumber( const std::string &s )
	{
		char *p = nullptr;
		long warningDummy = strtol( s.c_str(), &p, 10 );
		return *p == 0;
	}
	//---------------------

	//---------------------
	//NOTE: taken from
	// https://stackoverflow.com/questions/11635/case-insensitive-string-comparison-in-c
	bool iequals( const std::string &a, const std::string &b )
	{
		return std::equal( a.begin(), a.end(),
			b.begin(), b.end(),
			[] ( char a, char b )
			{
				return tolower( a ) == tolower( b );
			} );
	}
	//---------------------

	//---------------------
	//NOTE: taken from
	// https://stackoverflow.com/questions/1500363/compile-time-sizeof-array-without-using-a-macro
	template <typename T>
	constexpr auto sizeof_array( const T &iarray )
	{
		return ( sizeof( iarray ) / sizeof( iarray[0] ) );
	}
	//---------------------

	template<typename T>
	std::string toString( T n )
	{
		std::stringstream sstr;
		sstr << n;
		return sstr.str();
	}

	std::string toUpper( const std::string &str )
	{
		std::string ret( str );
		std::transform( ret.begin(), ret.end(), ret.begin(), toupper );
		return ret;
	}

	bool isFiniteNumber( float n )
	{
		return( isfinite( n ) && !isnan( n ) );
	}

	//---------------------
	//NOTE: taken from
	// https://stackoverflow.com/questions/216823/how-to-trim-a-stdstring
	// trim from start (in place)
	static inline void ltrim( std::string &s ) {
		s.erase( s.begin(), std::find_if( s.begin(), s.end(), [] ( unsigned char ch )
			{
				return !std::isspace( ch );
			} ) );
	}

	// trim from end (in place)
	static inline void rtrim( std::string &s ) {
		s.erase( std::find_if( s.rbegin(), s.rend(), [] ( unsigned char ch )
			{
				return !std::isspace( ch );
			} ).base(), s.end() );
	}

	// trim from both ends (in place)
	static inline void trim( std::string &s ) {
		ltrim( s );
		rtrim( s );
	}

	// trim from start (copying)
	static inline std::string ltrim_copy( std::string s ) {
		ltrim( s );
		return s;
	}

	// trim from end (copying)
	static inline std::string rtrim_copy( std::string s ) {
		rtrim( s );
		return s;
	}

	// trim from both ends (copying)
	static inline std::string trim_copy( std::string s ) {
		trim( s );
		return s;
	}
	//---------------------

	std::string makeKey( const std::string &bed, int needle )
	{
		return trim_copy( bed ) + toString( needle );
	}


	const char *Writer::SupportedPositions[] =
	{
		"Left",
		"Center",
		"Right",
		"Keep"
	};

	const std::string Writer::CarrierDelimiters = " ,";


	Writer::Writer( const std::vector<std::string> &carriers ) :
		_currentRacking( 0 )
	{
		_carriers = carriers;

		for( auto c : carriers )
		{
			if( c.find( ' ' ) != std::string::npos )
				std::cerr << "Warning: carrier name '" << c << "' contains a space. Since list is separated by spaces, parser will have a hard time figuring out the actual name. Also, carrier sets are allowed to be separated by spaces, so this will cause trouble." << std::endl;
			if( c.find( ',' ) != std::string::npos )
				std::cerr << "Warning: carrier name '" << c << "' contains a comma. Since carrier sets are allowed to be separated by commas, this will cause trouble." << std::endl;
		}

		//build a 'carriers' header from the '_carriers' list:
		_headers.push_back( ";;Carriers: " + join( _carriers, " " ) );
	}

	// function that queues header information to header list
	void Writer::addHeader( const std::string &name, const std::string &value )
	{
		if( name.find( ": " ) != std::string::npos )
		{
			throw std::runtime_error( "Header names must be strings that don't contain the sequence ': '" );
		}
		if( value.find( '\n' ) != std::string::npos )
		{
			throw std::runtime_error( "Header values must be strings that do not contain the LF character ('\\n')." );
		}

		//Check for valid headers:
		if( iequals( name, "Carriers" ) )
		{
			throw std::runtime_error( "Writer.addHeader can't set Carriers header (use the 'carriers' option when creating the writer instead)." );
		}
		else if( iequals( name, "Machine" ) )
		{
			if( _machine.length() )
				throw std::runtime_error( "Machine already set earlier." );
			_machine = value;
			//no restrictions on value
		}
		else if( iequals( name, "Gauge" ) )
		{
			if( !isNumber( value ) )
				throw std::runtime_error( "Value of 'Gauge' header must be a string representing a number." );
		}
		else if( iequals( name, "Position" ) )
		{
			bool found = false;
			for( int i = 0; i < sizeof_array( Writer::SupportedPositions ); i++ )
				if( !value.compare( SupportedPositions[i] ) )
					found = true;
			if( !found )
			{
				std::vector<std::string> v;
				for( int i = 0; i < sizeof_array( Writer::SupportedPositions ); i++ )
					v.push_back( Writer::SupportedPositions[i] );

				throw std::runtime_error( "'Position' header must have one of the following values: " + join( v, ", " ) );
			}
		}
		else if( name.find( "Yarn-" ) == 0 )
		{
			//check for valid carrier name, warn otherwise
			std::string carrierName = name.substr( 5 );
			if( std::find( _carriers.begin(), _carriers.end(), carrierName ) != _carriers.end() )
			{
				std::cerr << "Warning: header '" << name << "' mentions a carrier that isn't in the carriers list." << std::endl;
			}
		}
		else if( name.find( "X-" ) == 0 )
		{
			//all extension header values are okay!
		}
		else
		{
			std::cerr << "Warning: header name '" << name << "' not recognized; header will still be written." << std::endl;
		}
		_headers.push_back( ";;" + name + ": " + value );
	}

	//throw warning if ;;Machine: header is included & machine doesn't support extension
	bool Writer::machineSupport( const std::string &extension, const std::string &supported )
	{
		if( toUpper( _machine ).find( supported ) == std::string::npos )
		{
			std::cerr << "Warning: " << extension << " is not supported on " << _machine << ". Including it anyway.";
			return false;
		}

		return true;
	}

	void Writer::validateCarrier( const std::string &c )
	{
		if( !c.length() )
			throw std::runtime_error( "Missing carrier name" );

		if( std::find( _carriers.begin(), _carriers.end(), c ) == _carriers.end() )
			std::cerr << "Warning: Carrier '" << c << "' is unknown." << std::endl;
	}

	void Writer::validateDirection( const std::string &d )
	{
		if( d != "+" && d != "-" )
			throw std::runtime_error( "Invalid direction '" + d + "'" );
	}

	void Writer::validateBed( const std::string &b )
	{
		if( b.size() >= 1 && ( b[0] == 'f' || b[0] == 'b' ) )
		{
			if( b.size() == 1 )
				return;
			if( b.size() == 2 && ( b[1] == 's' || b[1] == '-' || b[1] == '+' ) )
				return;
			if( b.size() == 3 && b[1] == 's' && ( b[2] == '-' || b[2] == '+' ) )
				return;
		}

		throw std::runtime_error( "Invalid bed '" + b + "'" );
	}

	void Writer::validateNeedle( int n )
	{
		if( n < 0 )
			throw std::runtime_error( "Needle number must be an integer greater or equal zero : '" + toString( n ) + "'" );
	}

	void Writer::parseBedNeedle( const std::string &bedNeedle, std::string &bed, int &needle )
	{
		int pos = -1;
		for( int i = 0; i < bedNeedle.size(); i++ )
			if( isdigit( bedNeedle[i] ) )
			{
				pos = i;
				break;
			}

		if( pos == -1 )
			throw std::runtime_error( "bedNeedle '" + bedNeedle + "' does not seem to be in proper format" );

		bed = bedNeedle.substr( 0, pos );
		std::string needleStr = bedNeedle.substr( pos );

		if( !isNumber( needleStr ) )
			throw std::runtime_error( "Needle index must be an integer ('" + bedNeedle + "')" );
		char *p = nullptr;
		needle = strtol( needleStr.c_str(), &p, 10 );
	}


	void Writer::internalIn( const std::string &c, bool useHook )
	{
		std::string cc( trim_copy( c ) );
		validateCarrier( cc );

		if( _currentCarriers.find( cc ) != _currentCarriers.end() )
			throw std::runtime_error( "Carrier '" + c + "' is already in." );

		_currentCarriers[cc] = useHook;
	}

	void Writer::internalReleaseHook( const std::string &c )
	{
		if( !c.length() )
			throw std::runtime_error( "Missing carrier name" );

		auto carrier = _currentCarriers.find( c );
		if( carrier == _currentCarriers.end() )
			throw std::runtime_error( "Carrier '" + c + "' isn't in." );

		if( !carrier->second )
			throw std::runtime_error( "Carrier '" + c + "' isn't in the hook." );

		carrier->second = false;
	}

	void Writer::internalOut( const std::string &c )
	{
		std::string cc( trim_copy( c ) );
		validateCarrier( cc );

		if( _currentCarriers.find( cc ) == _currentCarriers.end() )
			throw std::runtime_error( "Carrier '" + c + "' isn't in." );

		_currentCarriers.erase( cc );
	}

	void Writer::internalWrite( std::ostream &ostr )
	{
		ostr << ";!knitout-2" << std::endl;

		for( auto h : _headers )
			ostr << h << std::endl;
		for( auto op : _operations )
			ostr << op << std::endl;
	}





	// escape hatch to dump your custom instruction to knitout
	// if you know what you are doing
	void Writer::addRawOperation( const std::string &operation )
	{
		std::cerr << "Warning: operation added to list as is(string), no error checking performed." << std::endl;
		_operations.push_back( operation );
	}


	void Writer::in( const std::string &c )
	{
		if( c.find_first_of( Writer::CarrierDelimiters ) != std::string::npos )
		{
			in( splitAny( c, Writer::CarrierDelimiters ) );
			return;
		}

		internalIn( c );

		_operations.push_back( "in " + c );
	}

	void Writer::in( const std::vector<std::string> &cs )
	{
		if( !cs.size() )
			throw std::runtime_error( "It doesn't make sense to 'in' on an empty carrier set." );

		for( auto c : cs )
			internalIn( c );

		_operations.push_back( "in " + join( cs, " " ) );

	}

	void Writer::inhook( const std::string &c )
	{
		if( c.find_first_of( Writer::CarrierDelimiters ) != std::string::npos )
		{
			inhook( splitAny( c, Writer::CarrierDelimiters ) );
			return;
		}

		internalIn( c, true );

		_operations.push_back( "in " + c );
	}

	void Writer::inhook( const std::vector<std::string> &cs )
	{
		if( !cs.size() )
			throw std::runtime_error( "It doesn't make sense to 'in' on an empty carrier set." );

		for( auto c : cs )
			internalIn( c, true );

		_operations.push_back( "in " + join( cs, " " ) );
	}

	void Writer::releasehook( const std::string &c )
	{
		if( c.find_first_of( Writer::CarrierDelimiters ) != std::string::npos )
		{
			releasehook( splitAny( c, Writer::CarrierDelimiters ) );
			return;
		}

		internalReleaseHook( c );

		_operations.push_back( "releasehook " + c );
	}

	void Writer::releasehook( const std::vector<std::string> &cs )
	{
		if( !cs.size() )
			throw std::runtime_error( "It doesn't make sense to 'releasehook' on an empty carrier set." );

		for( auto c : cs )
			internalReleaseHook( c );

		_operations.push_back( "releasehook " + join( cs, " " ) );
	}

	void Writer::out( const std::string &c )
	{
		if( c.find_first_of( Writer::CarrierDelimiters ) != std::string::npos )
		{
			out( splitAny( c, Writer::CarrierDelimiters ) );
			return;
		}

		internalOut( c );

		_operations.push_back( "out " + c );
	}

	void Writer::out( const std::vector<std::string> &cs )
	{
		if( !cs.size() )
			throw std::runtime_error( "It doesn't make sense to 'out' on an empty carrier set." );

		for( auto c : cs )
			internalOut( c );

		_operations.push_back( "out " + join( cs, " " ) );
	}

	void Writer::outhook( const std::string &c )
	{
		if( c.find_first_of( Writer::CarrierDelimiters ) != std::string::npos )
		{
			outhook( splitAny( c, Writer::CarrierDelimiters ) );
			return;
		}

		internalOut( c );

		_operations.push_back( "outhook " + c );
	}

	void Writer::outhook( const std::vector<std::string> &cs )
	{
		if( !cs.size() )
			throw std::runtime_error( "It doesn't make sense to 'out' on an empty carrier set." );

		for( auto c : cs )
			internalOut( c );

		_operations.push_back( "out " + join( cs, " " ) );
	}

	void Writer::stitch( int before, int after )
	{
		_operations.push_back( "stitch " + toString( before ) + " " + toString( after ) );
	}

	// --- extensions ---
	void Writer::stitchNumber( int stitchNumber )
	{
		if( stitchNumber < 0 )
			throw std::runtime_error( "Stitch numbers are non-negative integer values." );

		_operations.push_back( "x-stitch-number " + toString( stitchNumber ) );
	}

	void Writer::fabricPresser( const std::string &presserMode )
	{
		machineSupport( "presser mode", "SWG" );
		if( presserMode == "auto" )
			_operations.push_back( "x-presser-mode auto" );
		else if( presserMode == "on" )
			_operations.push_back( "x-presser-mode on" );
		else if( presserMode == "off" )
			_operations.push_back( "x-presser-mode off" );
		else
			std::cerr << "Ignoring presser mode extension, unknown mode " << presserMode << ". Valid modes: on, off, auto" << std::endl;
	}

	/*
	void Writer::visColor( hex, carrier )
	{
		//TODO: no idea what this extension is about
	}
	*/

	void Writer::speedNumber( int value )
	{
		//TODO: check to make sure it's within the accepted range
		if( value < 0 )
			std::cerr << "Ignoring speed number extension, since provided value : " << value << " is not a non - negative integer." << std::endl;
		else
			_operations.push_back( "x-speed-number " );
	}

	void Writer::rollerAdvance( int value )
	{
		machineSupport( "roller advance", "KNITERATE" );
		//TODO: check to make sure it's within the accepted range
		_operations.push_back( "x-speed-number " + toString( value ) );
		bool warning = false;
		if( !warning )
			_operations.push_back( "x-roller-advance " + toString( value ) );
	}

	void Writer::addRollerAdvance( int value )
	{
		machineSupport( "add roller advance", "KNITERATE" );
		//TODO: check to make sure it's within the accepted range
		_operations.push_back( "x-add-roller-advance " + toString( value ) );
	}

	void Writer::carrierSpacing( int value )
	{
		machineSupport( "carrier spacing", "KNITERATE" );
		if( value <= 0 )
			std::cerr << "Ignoring carrier spacing extension, since provided value : " << value << " is not a positive integer." << std::endl;
		else
			_operations.push_back( "x-carrier-spacing " + toString( value ) );
	}

	void Writer::carrierStoppingDistance( int value )
	{
		machineSupport( "carrier stopping distance", "KNITERATE" );
		if( value <= 0 )
			std::cerr << "Ignoring carrier stopping distance extension, since provided value : " << value << " is not a positive integer." << std::endl;
		else
			_operations.push_back( "x-carrier-stopping-distance " + toString( value ) );
	}

	// --- operations ---//
	void Writer::rack( float rack )
	{
		if( !isFiniteNumber( rack ) )
			throw std::runtime_error( "Racking values must be finite numbers." );

		_currentRacking = rack;

		_currentRacking = std::roundf( _currentRacking * 4.0f ) / 4.0f;
		if( std::abs( _currentRacking - rack ) > 0.001f )
			std::cerr << "Warning: only racking value with multiple of 1/4 are supported. Corrected from " << rack << " to " << _currentRacking << std::endl;

		_operations.push_back( "rack " + toString( _currentRacking ) );
	}

	void Writer::knit( const std::string &dir, const std::string &bed, int needle, const std::string &c )
	{
		if( c.find_first_of( Writer::CarrierDelimiters ) != std::string::npos )
		{
			knit( dir, bed, needle, splitAny( c, Writer::CarrierDelimiters ) );
			return;
		}

		validateDirection( dir );
		validateBed( bed );
		validateNeedle( needle );

		if( c.length() )
		{
			validateCarrier( trim_copy( c ) );

			_currentNeedles.insert( makeKey( bed, needle ) );
		}
		else
			_currentNeedles.erase( makeKey( bed, needle ) );

		_operations.push_back( "knit " + dir + " " + bed + toString( needle ) + " " + c );
	}

	void Writer::knit( const std::string &dir, const std::string &bed, int needle, const std::vector<std::string> &cs )
	{
		validateDirection( dir );
		validateBed( bed );
		validateNeedle( needle );

		if( cs.size() )
		{
			for( auto c : cs )
				validateCarrier( trim_copy( c ) );
			_currentNeedles.insert( makeKey( bed, needle ) );
		}
		else
			_currentNeedles.erase( makeKey( bed, needle ) );

		_operations.push_back( "knit " + dir + " " + bed + toString( needle ) + " " + join( cs, " " ) );
	}

	void Writer::knit( const std::string &dir, const std::string &bedNeedle, const std::string &c )
	{
		std::string bed;
		int needle = 0;

		parseBedNeedle( bedNeedle, bed, needle );

		knit( dir, bed, needle, c );
	}

	void Writer::knit( const std::string &dir, const std::string &bedNeedle, const std::vector<std::string> &cs )
	{
		std::string bed;
		int needle = 0;

		parseBedNeedle( bedNeedle, bed, needle );

		knit( dir, bed, needle, cs );
	}

	void Writer::tuck( const std::string &dir, const std::string &bed, int needle, const std::string &c )
	{
		if( c.find_first_of( Writer::CarrierDelimiters ) != std::string::npos )
		{
			tuck( dir, bed, needle, splitAny( c, Writer::CarrierDelimiters ) );
			return;
		}

		validateDirection( dir );
		validateBed( bed );
		validateNeedle( needle );

		if( c.length() )
			validateCarrier( trim_copy( c ) );

		_currentNeedles.insert( makeKey( bed, needle ) );

		_operations.push_back( "tuck " + dir + " " + bed + toString( needle ) + " " + c );
	}

	void Writer::tuck( const std::string &dir, const std::string &bed, int needle, const std::vector<std::string> &cs )
	{
		validateDirection( dir );
		validateBed( bed );
		validateNeedle( needle );

		if( cs.size() )
		{
			for( auto c : cs )
				validateCarrier( trim_copy( c ) );
		}

		_currentNeedles.insert( makeKey( bed, needle ) );

		_operations.push_back( "tuck " + dir + " " + bed + toString( needle ) + " " + join( cs, " " ) );
	}

	void Writer::tuck( const std::string &dir, const std::string &bedNeedle, const std::string &c )
	{
		std::string bed;
		int needle = 0;

		parseBedNeedle( bedNeedle, bed, needle );

		tuck( dir, bed, needle, c );
	}

	void Writer::tuck( const std::string &dir, const std::string &bedNeedle, const std::vector<std::string> &cs )
	{
		std::string bed;
		int needle = 0;

		parseBedNeedle( bedNeedle, bed, needle );

		tuck( dir, bed, needle, cs );
	}

	void Writer::split( const std::string &dir, const std::string &fromBed, int fromNeedle, const std::string &toBed, int toNeedle, const std::string &c )
	{
		if( c.find_first_of( Writer::CarrierDelimiters ) != std::string::npos )
		{
			split( dir, fromBed, fromNeedle, toBed, toNeedle, splitAny( c, Writer::CarrierDelimiters ) );
			return;
		}

		validateDirection( dir );
		validateBed( fromBed );
		validateNeedle( fromNeedle );
		validateBed( toBed );
		validateNeedle( toNeedle );

		if( c.length() )
			validateCarrier( trim_copy( c ) );

		if( fromBed == toBed )
			throw std::runtime_error( "Cannot split to same bed." );

		auto from = _currentNeedles.find( makeKey( fromBed, fromNeedle ) );
		if( from != _currentNeedles.end() )
		{
			_currentNeedles.insert( makeKey( toBed, toNeedle ) );
			_currentNeedles.erase( makeKey( fromBed, fromNeedle ) );
		}
		if( c.length() > 0 )
			_currentNeedles.insert( makeKey( fromBed, fromNeedle ) );

		_operations.push_back( "split " + dir + " " + fromBed + toString( fromNeedle ) + " " + toBed + toString( toNeedle ) + " " + c );
	}

	void Writer::split( const std::string &dir, const std::string &fromBed, int fromNeedle, const std::string &toBed, int toNeedle, const std::vector<std::string> &cs )
	{
		validateDirection( dir );
		validateBed( fromBed );
		validateNeedle( fromNeedle );
		validateBed( toBed );
		validateNeedle( toNeedle );

		if( cs.size() )
		{
			for( auto c : cs )
				validateCarrier( trim_copy( c ) );
		}

		if( fromBed == toBed )
			throw std::runtime_error( "Cannot split to same bed." );

		auto from = _currentNeedles.find( makeKey( fromBed, fromNeedle ) );
		if( from != _currentNeedles.end() )
		{
			_currentNeedles.insert( makeKey( toBed, toNeedle ) );
			_currentNeedles.erase( makeKey( fromBed, fromNeedle ) );
		}
		if( cs.size() > 0 )
			_currentNeedles.insert( makeKey( fromBed, fromNeedle ) );

		_operations.push_back( "split " + dir + " " + fromBed + toString( fromNeedle ) + " " + toBed + toString( toNeedle ) + " " + join( cs, " " ) );
	}

	void Writer::split( const std::string &dir, const std::string &fromBedNeedle, const std::string &toBedNeedle, const std::string &c )
	{
		std::string fromBed;
		std::string toBed;
		int fromNeedle = 0;
		int toNeedle = 0;

		parseBedNeedle( fromBedNeedle, fromBed, fromNeedle );
		parseBedNeedle( toBedNeedle, toBed, toNeedle );

		split( dir, fromBed, fromNeedle, toBed, toNeedle, c );
	}

	void Writer::split( const std::string &dir, const std::string &fromBedNeedle, const std::string &toBedNeedle, const std::vector<std::string> &cs )
	{
		std::string fromBed;
		std::string toBed;
		int fromNeedle = 0;
		int toNeedle = 0;

		parseBedNeedle( fromBedNeedle, fromBed, fromNeedle );
		parseBedNeedle( toBedNeedle, toBed, toNeedle );

		split( dir, fromBed, fromNeedle, toBed, toNeedle, cs );
	}

	void Writer::miss( const std::string &dir, const std::string &bed, int needle, const std::string &c )
	{
		if( c.find_first_of( Writer::CarrierDelimiters ) != std::string::npos )
		{
			miss( dir, bed, needle, splitAny( c, Writer::CarrierDelimiters ) );
			return;
		}

		validateDirection( dir );
		validateBed( bed );
		validateNeedle( needle );

		if( c.length() == 0 )
			throw std::runtime_error( "It doesn't make sense to miss with no carriers." );

		validateCarrier( trim_copy( c ) );

		_operations.push_back( "miss " + dir + " " + bed + toString( needle ) + " " + c );
	}

	void Writer::miss( const std::string &dir, const std::string &bed, int needle, const std::vector<std::string> &cs )
	{
		validateDirection( dir );
		validateBed( bed );
		validateNeedle( needle );

		if( cs.size() == 0 )
			throw std::runtime_error( "It doesn't make sense to miss with no carriers." );

		for( auto c : cs )
			validateCarrier( trim_copy( c ) );

		_operations.push_back( "miss " + dir + " " + bed + toString( needle ) + " " + join( cs, " " ) );
	}

	void Writer::miss( const std::string &dir, const std::string &bedNeedle, const std::string &c )
	{
		std::string bed;
		int needle = 0;

		parseBedNeedle( bedNeedle, bed, needle );

		miss( dir, bed, needle, c );
	}

	void Writer::miss( const std::string &dir, const std::string &bedNeedle, const std::vector<std::string> &cs )
	{
		std::string bed;
		int needle = 0;

		parseBedNeedle( bedNeedle, bed, needle );

		miss( dir, bed, needle, cs );
	}

	// drop -> knit without yarn, but supported in knitout
	void Writer::drop( const std::string &bed, int needle )
	{
		validateBed( bed );
		validateNeedle( needle );

		_currentNeedles.erase( makeKey( bed, needle ) );

		_operations.push_back( "drop " + bed + toString( needle ) );
	}

	void Writer::drop( const std::string &bedNeedle )
	{
		std::string bed;
		int needle = 0;

		parseBedNeedle( bedNeedle, bed, needle );

		drop( bed, needle );
	}

	// amiss -> tuck without yarn, but supported in knitout
	void Writer::amiss( const std::string &bed, int needle )
	{
		validateBed( bed );
		validateNeedle( needle );

		_operations.push_back( "amiss " + bed + toString( needle ) );
	}

	void Writer::amiss( const std::string &bedNeedle )
	{
		std::string bed;
		int needle = 0;

		parseBedNeedle( bedNeedle, bed, needle );

		amiss( bed, needle );
	}

	// xfer -> split without yarn, but supported in knitout
	void Writer::xfer( const std::string &fromBed, int fromNeedle, const std::string &toBed, int toNeedle )
	{
		validateBed( fromBed );
		validateNeedle( fromNeedle );
		validateBed( toBed );
		validateNeedle( toNeedle );

		auto from = _currentNeedles.find( makeKey( fromBed, fromNeedle ) );
		if( from != _currentNeedles.end() )
		{
			_currentNeedles.insert( makeKey( toBed, toNeedle ) );
			_currentNeedles.erase( makeKey( fromBed, fromNeedle ) );
		}

		_operations.push_back( "xfer " + fromBed + toString( fromNeedle ) + " " + toBed + toString( toNeedle ) );
	}

	void Writer::xfer( const std::string &fromBedNeedle, const std::string &toBedNeedle )
	{
		std::string fromBed;
		std::string toBed;
		int fromNeedle = 0;
		int toNeedle = 0;

		parseBedNeedle( fromBedNeedle, fromBed, fromNeedle );
		parseBedNeedle( toBedNeedle, toBed, toNeedle );

		xfer( fromBed, fromNeedle, toBed, toNeedle );
	}

	// add comments to knitout 
	void Writer::comment( const std::string &str )
	{
		auto subs = Knitout::split( str, "\n" );

		for( auto s : subs )
		{
			int cntr = 0;
			for( int i = 0; i < s.length(); i++ )
			{
				if( s[i] == ';' )
					cntr++;
				else
					break;
			}
			if( cntr )
				std::cerr << "Warning: comment starts with ; use addHeader for adding header comments." << std::endl;

			_operations.push_back( ";" + s.substr( cntr ) );
		}
	}

	void Writer::pause( const std::string &comment )
	{
		// deals with multi-line comments
		this->comment( comment );
		_operations.push_back( "pause" );
	}

	void Writer::write( const std::string &filename )
	{
		if( !filename.size() )
		{
			std::cerr << "filename not passed to Writer.write; writing to stdout." << std::endl;
			internalWrite( std::cout );
		}
		else
		{
			std::fstream file( filename, std::fstream::out );
			if( !file.is_open() )
				throw std::runtime_error( "unable to open file '" + filename + "' for writing" );
			internalWrite( file );
		}
	}
}
