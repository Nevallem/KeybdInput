/**
 * KeybdInput 1.0
 *
 * @author Roger Lima (rogerlima@outlook.com)
 * @date 31/aug/2014
 * @update 29/jan/2016
 * @desc Reads the keyboard input them according to the format specified (only works on Windows)
 * @example
	int day, month, year;
	KeybdInput< int > userin;

	userin.solicit(
		"Type a date (btw 01/01/1900 and 31/12/2009): ",
		std::regex( "([0-2]?[0-9]|3[0-1])/(0?[1-9]|1[012])/(19[0-9]{2}|200[0-9])" ),
		{ &day, &month, &year },
		false,
		false,
		false,
		'/'
	);
*/

#pragma once
#include <conio.h>
#include <iostream>
#include <regex>
#include <sstream>
#include <vector>
#include <Windows.h>

template< typename T >
class KeybdInput {
private:
	// Stores the values to be used in KeybdInput::reset()
	std::vector< T * > _references = {};
	std::regex _restriction;
	std::string _rmsg = "";
	bool _instverify = false,
		_reset = false,
		_ispw = false;
	char _sep = ' ';
	size_t _inmaxsize;

	// Stores the user input
	std::string input = "";

	// Receives the current X and Y cursor position from get_cursor_position()
	std::vector< size_t > cursor_position = { 0, 0 };

	// Erase the input of user
	// @param {size_t=input.size()} Erase range
	void erase_input( size_t = 0 );

	// Get the console cursor position and pass to the cursor_position
	void get_cursor_position();

	// Set the console cursor position
	// @param {short} X position of cursor
	// @param {short} Y position of cursor
	void set_cursor_position( short, short );

	// Split a string
	// @param {std::string} Target string
	// @param {char} Delimiter
	std::vector< std::string > split( std::string str, char delim );

	// Set the reference with input value
	// @param {const std::string&} Input value
	// @param {T*} Target place
	void set_reference( const std::string&, T * );
public:
	// Clipboard (arrow up or down to show the last inputs)
	std::vector< std::string > clipboard;

	// Requires the user input again
	// @param [{std::string=_rmsg}] Request message
	void reset( std::string = "" );

	// Requires the keyboard input
	// @param {string} Request message
	// @param {regex} The regex
	// @param {vector< T * >} The place(s) where it will be stored the input
	// @param {bool=false} Instant verify
	// @param {bool=false} Reset possibility
	// @param {bool=false} Is password
	// @param {char=' '} Separator
	// @param {size_t=1000} Input size
	void solicit( std::string, std::regex, std::vector< T * >, bool = false, bool = false, bool = false, char = ' ', size_t = 1000 );
};

template< typename T >
void KeybdInput< T >::erase_input( size_t erase_range = 0 ) {
	// Default erase range
	if ( !erase_range )
		erase_range = input.size();

	for ( size_t i = 0; i < erase_range; i++ )
		std::cout << "\b \b";

	input = "";
};

template < typename T >
void KeybdInput< T >::set_reference( const std::string& value, T *target ) {
	stringstream ss;
	ss << value;
	ss >> *target;
};

void KeybdInput< std::string >::set_reference( const std::string& value, std::string *target ) {
	*target = value;
};

template< typename T >
void KeybdInput< T >::get_cursor_position() {
	CONSOLE_SCREEN_BUFFER_INFO screen_buffer_info;
	HANDLE hStd = GetStdHandle( STD_OUTPUT_HANDLE );

	if ( !GetConsoleScreenBufferInfo( hStd, &screen_buffer_info ) )
		std::cout << "KeybdInput: An error occurred getting the console cursor position.";

	cursor_position[ 0 ] = screen_buffer_info.dwCursorPosition.X;
	cursor_position[ 1 ] = screen_buffer_info.dwCursorPosition.Y;
};

template< typename T >
void KeybdInput< T >::set_cursor_position( short x, short y ) {
	COORD p = { x, y };

	if ( !SetConsoleCursorPosition( GetStdHandle( STD_OUTPUT_HANDLE ), p ) )
		std::cout << "KeybdInput: an error occurred setting the console cursor position.";
};

template< typename T >
std::vector< std::string > KeybdInput< T >::split( std::string str, char delim ) {
	std::vector< std::string > elems;
	std::stringstream stream( str );
	std::string item;

	for ( ; getline( stream, item, delim ); elems.push_back( item ) );

	return elems;
};

template< typename T >
void KeybdInput< T >::reset( std::string msg = "" ) {
	// Default request message
	if ( msg == "" && _rmsg != "" )
		msg = _rmsg;

	if ( !_reset ) {
		std::cout << "KeybdInput: can't possible execute KeybdInput::reset() without set reset_possibility=true in KeybdInput::solicit()." << std::endl;
		return;
	}

	// Clear previously set values
	for ( size_t i = 0; i < _references.size(); *_references[ i++ ] = "" );

	// Sets the cursor in the previous line, erase all and requires input again
	get_cursor_position();
	set_cursor_position( short ( msg.size() + input.size() ), cursor_position[ 1 ] - 1 );
	erase_input( msg.size() + input.size() );
	solicit( msg, std::regex( _restriction ), _references, _instverify, _reset, _ispw, _sep, _inmaxsize );
};

template< typename T >
void KeybdInput< T >::solicit( std::string request_msg, std::regex restriction, std::vector< T * > references, bool instant_verify, bool reset_possibility, bool is_password, char sep, size_t input_max_size ) {
	static size_t clipboard_index = 0;
	char letter = 0;
	bool is_function_key = false,
		arrow = false;
	string input_subtr;
	size_t cursor_pos_x;
	size_t i, inputLength = 0;
	std::vector< std::string > inputParts;

	if ( references.size() == 0 ) {
		std::cerr << "KeybdInput: \"refereces\" param need be set with at least one member." << std::endl;
		return;
	}

	input = "";
	std::cout << request_msg;

	// Retrieves the values to be used in KeybdInput::reset()
	if ( reset_possibility ) {
		_rmsg = request_msg;
		_restriction = restriction;
		_references = references;
		_instverify = instant_verify;
		_reset = reset_possibility;
		_ispw = is_password;
		_sep = sep;
		_inmaxsize = input_max_size;
	}

	// Clears previous data
	else if ( _reset ) {
		_rmsg = "";
		_restriction = "";
		_references = {};
		_instverify = false;
		_reset = false;
		_ispw = false;
		_sep = ' ';
		_inmaxsize = 0;
	}

	while ( true ) {
		letter = _getch();

		// Arrow keys prefix
		if ( letter == -32 ) {
			arrow = true;
			continue;
		}

		// Prevents function keys
		if ( letter == 0 ) {
			is_function_key = true;
			continue;
		} else if ( is_function_key ) {
			is_function_key = false;
			continue;
		}

		// Arrows
		if ( arrow ) {
			get_cursor_position();

			// LEFT
			if ( letter == 75 && ( cursor_position[ 0 ] > request_msg.size() ) )
				set_cursor_position( cursor_position[ 0 ] - 1, cursor_position[ 1 ] );

			// RIGHT
			else if ( letter == 77 && cursor_position[ 0 ] < ( input.size() + request_msg.size() ) )
				set_cursor_position( cursor_position[ 0 ] + 1, cursor_position[ 1 ] );

			// UP
			else if ( letter == 72 && !is_password && clipboard.size() > 0 && clipboard_index > 0 ) {
				erase_input();
				std::cout << clipboard[ --clipboard_index ];

				input = clipboard[ clipboard_index ];
				inputLength = clipboard[ clipboard_index ].size();
			}

			// DOWN
			else if ( letter == 80 && !is_password && clipboard.size() > 0 && clipboard_index < clipboard.size() - 1 ) {
				erase_input();
				std::cout << clipboard[ ++clipboard_index ];

				input = clipboard[ clipboard_index ];
				inputLength = clipboard[ clipboard_index ].size();

				if ( clipboard_index >= clipboard.size() )
					clipboard_index = clipboard.size() - 1;
			}

		}

		// Valid character
		else if ( letter != 8 && letter != 13 ) {
			// Inserts the character in current cursor position
			get_cursor_position();
			input.insert( cursor_position[ 0 ] - request_msg.size(), std::string( 1, letter ) );

			// If the user input satisfy the restrictions, removes the character
			if ( instant_verify && ( input.size() > input_max_size
				|| !std::regex_match( input, restriction )
			) ) {
				input.erase( cursor_position[ 0 ] - request_msg.size(), 1 );
			} else {
				// Appends the character if cursor is at the end, otherwise, interleaves
				if ( cursor_position[ 0 ] == ( request_msg.size() + input.size() ) - 1 ) {
					std::cout << ( ( is_password ) ? '*' : letter );
				} else {
					input_subtr = input.substr( input.size() - ( ( request_msg.size() + input.size() ) - cursor_position[ 0 ] - 1 ), input.size() );

					std::cout << ( ( is_password ) ? '*' : letter )
						<< ( ( is_password ) ? std::string( input_subtr.size(), '*' ) : input_subtr );
					set_cursor_position( cursor_position[ 0 ] + 1, cursor_position[ 1 ] );
				}

				inputLength++;
			}
		}

		// ENTER
		else if ( letter == 13 && inputLength ) {
			// If the user input satisfy the restrictions, clear input
			if ( input.size() > input_max_size || !std::regex_match( input, restriction ) ) {
				erase_input();
				inputLength = 0;
			} else {
				// Trim left and right
				input.erase( 0, input.find_first_not_of( ' ' ) );
				input.erase( input.find_last_not_of( ' ' ) + 1 );

				if ( references.size() == 1 )
					set_reference( input, references[ 0 ] );
				else
					for ( i = 0, inputParts = split( input, sep ); i < references.size(); i++ )
						if ( i < inputParts.size() )
							set_reference( inputParts[ i ], references[ i ] );

				std::cout << std::endl;

				// Prevents repetition
				if ( !is_password && ( clipboard.size() == 0 || input != clipboard[ clipboard.size() - 1 ] ) ) {
					clipboard.push_back( input );
					clipboard_index = clipboard.size();
				}

				break;
			}
		}

		// BACKSPACE
		else if ( letter == 8 && inputLength ) {
			get_cursor_position();
			cursor_pos_x = cursor_position[ 0 ];

			if ( cursor_pos_x == ( request_msg.size() + input.size() - 1 ) && inputLength == 1 )
				continue;

			std::cout << "\b \b";
			input.erase( cursor_pos_x - request_msg.size() - 1, 1 );
			inputLength--;

			if ( cursor_pos_x <= ( request_msg.size() + input.size() ) ) {
				// Put the cursor at the start and rewrites the input
				set_cursor_position( short ( request_msg.size() ), cursor_position[ 1 ] );
				std::cout << ( ( is_password ) ? std::string( input.size(), '*' ) : input );

				// Put the cursor at the end and erase the last char
				set_cursor_position( short ( request_msg.size() + input.size() + 1 ), cursor_position[ 1 ] );
				std::cout << "\b \b";

				// Put the cursor at the original position
				set_cursor_position( short ( cursor_pos_x - 1 ), cursor_position[ 1 ] );
			}
		}

		arrow = false;
	}
};
