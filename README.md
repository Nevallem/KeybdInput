# KeybdInput
Reads the keyboard input them according to the format specified (only works on Windows).

## How to use
First, instantiate the KeybdInput class with a type passed through template.

```cpp
KeybdInput< std::string > userin; // for example
```

### KeybdInput::solicit()
Requires the keyboard input. This method takes 8 arguments, 4 optional.

- **request_msg (std::string)**: message that will be displayed requesting the user input.
- **restriction (std::regex)**: regex that will be used to specify the formatting of input.
- **references (std::vector< T * >)**: the place(s) where it will be stored the input.
- **instant_verify (bool=false)**: if the restriction will be verified immediately or just after press ENTER.
- **reset_possibility (bool=false)**: if there is reset possibility (see KeybdInput::reset()).
- **is_password (bool=false)**: if is a password input.
- **separator (std::string=" ")**: separator character.
- **input_max_size (size_t=1000)**: max size of input.

#### Examples
You can stores parts of the input in differents places. The default separator is the space, but you can change it for another. 

```cpp
string cmd, cmd2;

userin.solicit( 
	">>> ", // request message
	std::regex( ".+" ), // formatting, reads any character 
	{ &cmd, &cmd2 } // address of storage places
);

cout << "CMD: " << cmd << endl << "CMD2: " << cmd2 << endl << endl;
```

```
>>> test message
CMD: test
CMD2: message
```

Remember: if only one address is passed, all content of input is stored on it.

This can be useful to manipulate dates too.

```cpp
KeybdInput< int > userin;
int day, month, year;

userin.solicit(
	"Type a valid date (formatted as dd/mm/yyyy): ",
	std::regex( "([0-2]?[0-9]|3[0-1])/(0?[1-9]|1[012])/([0-9]{4})" ), // formatting, reads any date
	{ &day, &month, &year },
	false, // only verify restriction after ENTER
	false, // not reset possibility
	false, // no is a password
	"/" // separator
);

std::cout << "day = " << day << "; month = " << month << "; year = " << year << std::endl;
```

```
Type a date (formatted as dd/mm/yyyy): 30/11/1995
day = 30; month = 11; year = 1995
```

### KeybdInput::reset()
Requires the user input again. This method takes 1 optional argument.

- **request_msg (std::string=request_msg)**: message that will be displayed requesting the user input.

#### Example
In some situation, you can need request the user to input again. Take for example the code above: if the input was 29/02/2001? 2001 isn't a bissext year, so you can do this:

```cpp
while ( true ) {
	userin.solicit(
		"Type a valid date (formatted as dd/mm/yyyy): ",
		std::regex( "([0-2]?[0-9]|3[0-1])/(0?[1-9]|1[012])/([0-9]{4})" ),
		{ &day, &month, &year },
		false,
		true, // now, reset_possibility argument need be set as true
		false,
		"/"
	);

	if ( ( day >= 29 && month == 2 ) && !( ( year % 4 == 0 && year % 100 != 0 ) || year % 400 == 0 ) )
		userin.reset(); // request input again
}
```
