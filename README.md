# KeybdInput
Reads the keyboard input them according to the format specified (only works on Windows).

## How to use
First, instantiate the KeybdInput class with a type passed through template.

```cpp
KeybdInput< std::string > userin; // for example
```

Now, you can acess some properties to define the input options.

- **instant_verify (bool=false)**: if the restriction will be verified immediately or just after press ENTER.
- **is_password (bool=false)**: if is a password input.
- **clear_input (bool=false)**: if erase the input or repeat in next line if the user input don't match with the restrictons. 
- **separator (std::string=" ")**: separator character.
- **input_max_size (size_t=1000)**: max size of input.

These properties can be your default values setted in the instance too.

```cpp
KeybdInput< std::string > userin( instant_verify, is_password, clear_input, separator, input_max_size );
```

And then, all the calls of KeybdInput::solicit() method will follow these options, excepts in cases where the option be changed before a specific call of the method. In this cases, after the call, the option value backs to default setted in constructor. Follow the example:

```cpp
std::string cmd, cmd2;
KeybdInput< std::string > userin( false, false, false );

userin.is_password = true;
userin.solicit( "Type a password: ", std::regex( "^.+$" ), { &cmd } );

userin.solicit( "Type something: ", std::regex( "^.+$" ), { &cmd2 } );
```

In the example above, is_password is set by default as "false", but it's values is changed to "true" before the fist call of method. However, in the second call, is_password it's already with the default value ("false").

PS: it's essential prior knowledge of regular expressions (regex) to properly use the library.

### KeybdInput::solicit()
Requires the keyboard input. This method takes 3 arguments.

- **request_msg (std::string)**: message that will be displayed requesting the user input.
- **restriction (std::regex)**: regex that will be used to specify the formatting of input.
- **references (std::vector< T * >)**: the place(s) where it will be stored the input.

#### Examples
You can stores parts of the input in differents places. The default separator is the space, but you can change it for another. 

```cpp
KeybdInput< std::string > userin;
std::string cmd, cmd2;

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

userin.separator = "/";
userin.solicit(
	"Type a valid date (formatted as dd/mm/yyyy): ",
	std::regex( "([0-2]?[0-9]|3[0-1])/(0?[1-9]|1[012])/([0-9]{4})" ),
	{ &day, &month, &year }
);

std::cout << "day = " << day << "; month = " << month << "; year = " << year << std::endl;
```

```
Type a date (formatted as dd/mm/yyyy): 30/11/1995
day = 30; month = 11; year = 1995
```

### KeybdInput::requires_again()
Requires the user input again. This method takes 1 optional argument.

- **request_msg (std::string=request_msg)**: message that will be displayed requesting the user input.

#### Example
In some situation, you can need request the user input again. Take for example the code above: if the input was 29/02/2001? Or 31/11/2015? 2001 isn't a bissext year and november don't have 31 days. So, you could do something like this to solve:

```cpp
KeybdInput< int > userin;
int day, month, year;

userin.separator = "/";
userin.solicit(
	"Type a valid date (formatted as dd/mm/yyyy): ",
	std::regex( "([0-2]?[0-9]|3[0-1])/(0?[1-9]|1[012])/([0-9]{4})" ),
	{ &day, &month, &year }
);

while ( ( day >= 29 && month == 2 ) && !( ( year % 4 == 0 && year % 100 != 0 ) || year % 400 == 0 )
	||  day == 31 && ( month == 4 || month == 6 || month == 9 || month == 11 )
)
	userin.requires_again();
	
std::cout << "day = " << day << "; month = " << month << "; year = " << year << std::endl;
```
