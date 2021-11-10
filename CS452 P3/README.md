## Project 3 - Device Driver

* Author: Colten Davis
* Class: CS452/CS552 [Operating Systems] Section 001

## Overview
I spent more time trying to get this project to compile than I did actually implementing the functionality.
Man pages were my friend, as I tried to figure out how to properly use signals, copy_to_user, SIGTERM etc.
C is still a difficult language for me, and Google, StackOverflow etc is my friend during these projects.
This projet required more knowledge of various Kernel calls and signals than I had, so I had to consult various
sources to figure that out. Then came the compilation issue, which I'm still not 100% convinced has been solved.
This was a painful project, as I had to constantly add to the example_drive directory, run make and then keep 
changing the other files. Even when it did work, if I uploaded it to Github, then redownloaded and tried to run it, 
I'd get the compilation errors again. I'm hoping when it's graded it works flawlessly. 
 

## Manifest
Makefile - makefile for this project
README.md - this README
booga-test1.sh - test script that checks regular funtionality of driver
booga-test2.sh - test script that checks for thread safety
booga.c - device driver main code
booga.h - heade file for booga.c
booga_load - load function that loads driver
booga_unload - unload function that unloads the driver
test-booga.c - test srcript for manual testing


## Building the project
This project requires various sudo commands.
1. In the directory containnig all the files, run the following: sudo make
   - Note: if this does not work, or throws a compiler error, ensure the Modules.symvers file is deleted
   - Note: Also ensure that you have read/write/execute priveleges on the repo and all files in it: chmod -R 777 directoryName
2. To test, run the following commands in order:
   - sudo ./booga_load
   - Then run various of this command: sudo ./test-booga <0,1,2 or 3> <num characters> <read or write>. Example: sudo ./test-booga 0 100 read
3. To see results run: at /proc/driver/booga
4. Run: sudo ./booga_unload
 


## Features and usage
This project simulates a simple character driver. It features 4 drivers, booga0 through booga3, and phrases that
accompany them. It uses a random number generator to select one of the four phrases, which opens the driver. We keep
track of bytes written, read, times the phrase was selected, and how many total driver calls there were. Race conditions
are prevented using semaphores.


## Testing
In order to test the project, you can run the booga-test1.sh and booga-test2.sh. I ran both scripts and the output seemed correct.
I also ran several examples of test-booga.c, and various inputs of read, write. The output matched the output shown in the project
description on github.


## Known Bugs
I had an incredibly difficult time getting this project compile. I kept getting compiler errors saying
"CONFIG_RETPOLINE=y, but not supported by the compiler. Compiler update recommended..  Stop."
I have no idea why, or what casues this. I had to use the example_driver folder, and slowly and painfully
change just a little bit, run make over and over, to ensure it built. I finally got it to the point where
it runs. It seems its something to do with the Module.symvers file that is, or is not created. I have no 
idea what that is for, but I've included my solution in the above building sections. All that to say, 
I'm not 100% confident it will compile for the grader, because I have no idea what was causing the 
issues in the first place.


## Reflection and Self Assessment



## Sources used
https://www.rdocumentation.org/packages/installr/versions/0.23.2/topics/kill_pid
https://man7.org/linux/man-pages/man7/signal.7.html
https://docs.huihoo.com/doxygen/linux/kernel/3.7/pid_8h.html#a52cb632565571176cbfe87febc340099
https://www.tutorialspoint.com/cprogramming/c_strings.htm
https://stackoverflow.com/questions/1088622/how-do-i-create-an-array-of-strings-in-c/1095006
https://www.kernel.org/doc/htmldocs/kernel-api/API---copy-to-user.html
https://man.cx/siginfo(3)
https://man7.org/linux/man-pages/man3/memset.3.html
https://docs.oracle.com/cd/E19620-01/805-3177/6j31gjo54/index.html
https://man7.org/linux/man-pages/man3/sigqueue.3.html
