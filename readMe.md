# RoveBoard! (for specifically energia)

## Overview
Programmers: 
Primary structure: Drue Satterfield 
Individual files: Varied

This is rover's official Hardware Abstraction Layer (HAL), a series of functions designed to a) be able to be used on any microcontroller that rover uses (in theory, check which boards are supported and which types of functions they support further down) b) allow the programmer to work with the hardware on the microcontroller easily and accessibly with easy-to-read and understand function calls C) allow roveware files like RoveComm, which are designed to be used across rover, to be used on every microcontroller we're using by using the functions provided in this framework, which are promised to work on every microcontroller that supports them.

It's fairly similar to arduino or energia, if you know what those are (directly inspired by, in fact) but unlike those two, the functions in roveboard are well documented and flat out tell you WHAT parts of the hardware the functions use, without you having to know what they're specifically DOING to carry out the function. In this way, you know what's going on in the microcontroller when you use roveboard, while still getting the easy hardware-accessing universal functions to use. Also unlike the former two, each microcontroller not only has standardized functions provided for roveware files to use on every microcontroller that supports the hardware feature they use, but also a series of functions that take specific advantage of the board for main.cpp and related, non universal files to use if they wish. The latter functions will only work with their own processor, but that's perfectly fine for non-portable files like main.cpp.

This version of the framework is designed to be used within Energia

It's not necessary for every microcontroller to implement every feature noted in generics, but if they do then they must at least implement the standard functions for that feature.

Be sure to read the wiki for more helpful things such as visual pinmaps.

## Files and folders
* At the top, you'll see a series of files that follow the naming pattern of `RoveBoard.h` and `RoveBoard_(microcontroller name).h`. 
  - The former is the primary include for non-board-specific roveware files like RoveComm to include, and returns a list of prototyped functions for it to use. But, `Roveboard.h` importantly doesn't return the actual definitions of those functions, just the names and the fact that they exist. 
  - The other roveboard files like `RoveBoard_TM4C1294NCPDT.h` return those functions and others that work specifically on the tm4c and define them. You need to include the roveboard for your specific microcontroller in your main.cpp file for everything to work.

* The folders inside RoveBoard are split into three categories. 
  - `standardized_API`, which contain files that prototype the standardized functions for each feature, and the typenames and constants associated with the features.
  - `supportingUtilities`, which contain supporting functions, macros, and classes that aren't device-specific so are there to be helpful to the entire framework 
  - lastly you'll see folders devoted to a specific processor like 'tm4c1294ncpdt_API'. Those folders contain the files that list and define the functions made for their processor, and what most users of RoveBoard will be interested in looking at.

## Supported microcontrollers and what features they implement
* Tiva tm4c1294ncpdt - pwm writing, pwm reading, clocking, digital pin, Ethernet, Uart/serial, timerInterface, I2c, PermanentMemory(EEPROM version), and energia's own libraries

## Dependencies
* Energia, natch.

## Usage
1) In your main.cpp, before anything else include the roveboard_x.h file for the processor you're using
2) From a coding standpoint, that's it. Make sure to read the H files for your processor, as they contain not only the functions you're free to use in your files, but also any warnings, bugs or cautions.
3) Make sure that whatever roveware you're using is supported by the microcontroller; for instance, RoveComm can only be used on boards that support RoveEthernet.

99) If you're making a new roveware component, have it include "RoveBoard.h" to get all the generic, universally supported functions you can use.

## Conflicts with Energia:
* Tiva tm4c1294ncpdt: Pwm reading and AnalogWrite conflict, as they both use timers. Typically try to use pwmWrite instead; if you do use analogWrite, try and read through any included roveware libraries to make sure they're not using pwm read
  - Also, RoveUart doesn't include the ability to use callbacks or change the FIFO size
  - Also, EthernetUDP and RoveEthernet cannot both work at the same time. You choose one or the other; RoveEthernet is enabled by   default, the user has the option of disabling it in favor of EthernetUDP if they wish
  

## Adding more boards/functions to existing boards
1) Add a RoveBoard_x.h for it. 
2) If your board supports one of the standard listed features under generics, make sure you implement at least the standard functions before you put it in.
3) Make sure that you aren't trying to re-define energia's commands; if you're importing an update from the main version, you have to a) take out functions and consts that conflict with energia and b) put everything into one folder, so also remove any folder directories in all the #include statements.
4) Make sure it compiles and test it first; you can use the included example code from 2016's arm, if you want
5) For pete's sake, make sure the H files at the very least include halfway decent comments/documentation; make sure at the least the programmers know what hardware components are used when a function is called so they can avoid conflicts if necessary and aren't blindsided by anything. High visibility is a key difference between what we do and what arduino/energia does
6) Be sure to update the pinmap in the wiki, if relevant

## Differences between this and the main version
They're functionally identical; the difference is a) no more subfolders b) the individual files have had all functions and consts that energia itself provides shaved out
