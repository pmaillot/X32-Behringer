[# X32 / OSC utilities #
![X32-pic.jpg](https://bitbucket.org/repo/K9Ae7b/images/3905851088-X32-pic.jpg)
## What is this repository for? ##

This is a repository for Behringer X32 OSC program files. I will try to push/commit most of the utilities I have been writing since 2014 for the X32 Standard console I bought then.
The first one I committed is X32_Command, a simple yet very useful and powerful command line tool for interacting with the X32 digital audio console.

## Demos and documentation ##
The latest version of the unofficial X32 OSC Protocol documentation, and compiled/built versions of the tools, demos, examples and snapshots can be found at 
https://sites.google.com/site/patrickmaillot/x32

## How do I get set up? ##
The tools are generally simple C programs, static linked applications; 

Source code is set for 4-space tabs in Eclipse. Some utilities are just command line tools, others (to come on this git repo) are Windows applications using Windows MSC, or GTK3 for some of them.

### Contribution guidelines ###
Feel free to use :-). If you find bugs, please report them; if you fix some bugs, please propose you code so everyone can benefit.

### Who do I talk to? ###
patrick.maillot@gmail.com

## Utilities ##

Before downloading, building or using the following utilities, please read the X32 manual(s) associated with your X32 system. The X32 OSC protocol document available on my website (see below) is also quite useful, especially when using the X32_Command tool (you have to know at least a few commands, and their syntax).


### X32_Command ###

![X32_Commad.jpg](https://bitbucket.org/repo/K9Ae7b/images/703636108-X32_Commad.jpg)

X32_Command: Sends OSC commands to X32, allows listening to X32 too...

This command line Windows tool enables sending and receiving OSC data in many ways.


```
#!bash

usage: X32_command [-i X32 console ipv4 address]
                   [-d 0/1, [0], debug option]
                   [-v 0/1  [1], verbose option]
                   [-k 0/1  [1], keyboard mode on]
                   [-t int  [10], sets delay between batch commands in ms]
                   [-f file, sets batch mode on, getting input data from 'file']
                     default IP is 192.168.0.64

If option -f is used, the program runs in batch mode, taking data from the provided file until 
EOF has been reached, or 'exit' or 'kill' entered. If not killed or no -f option, the program 
runs in standard mode, taking data from the keyboard or <stdin> on linux systems.

Note: make sure the batch file is respecting unix standards (i.e. use notepad++ to create the 
file under Windows so EOL are made of only "\n" and not "\r\n").

 While executing, the following commands can be used (without the quotes):
   '#line of text.....': will print out the input line as a comment line
   'exit' | 'quit': will quit the current mode
   'kill': [batch mode only] will exit the program
   'time <value>': [batch mode only] will change the delay between batch commands to <value>
   'verbose <on|off>': will change the verbose mode
   'verbose': will return the current verbose mode
   'xremote <on|off>': will change the xremote mode
   'xremote': will return the current xremote mode
   '' (empty line) [standard mode only]: will repeat the last entered command

All other commands are parsed and sent to X32.
 Typical X32 OSC command structure:
   <command> [<format> [<data> [<data> [...]]]], where for example:
      command: /info, /status, /ch/04/mix/fader, ...
      format: ',i' ',f' ',s' or a combination: ',siss' ',ffiss' ...
      data: a list of int, float or string types separated by a space char...

Examples:
/ch/01/mix/fader ,f 0.5    - set mixing fader of channel 01 to mid-position
/node ,s fx/01/par         - retrieve the 64 parameters of effect at FX slot 1
```


### X32Tap ###
![X32Tap.jpg](https://bitbucket.org/repo/K9Ae7b/images/3888357480-X32Tap.jpg)

A small utility to set tap tempo on X32. It checks for DLY type effect to be on FX slots 1 to 4. If a DLY effect is found, entering <cr> will set tempo; i.e. the actual tempo will be set for any two consecutive <cr> hits on the keyboard. On X32, tempo tap can be set between 0 and 3000ms.



```
#!bash

usage: X32Tap [-i X32 console ipv4 address]
 then:
 '1'...'4' <cr> to select FX slot with DLY,
 'q' <cr> to exit,
 <cr> to set tempo
```

### X32 ###
![x32.jpg](https://bitbucket.org/repo/K9Ae7b/images/3945386401-x32.jpg)

X32 is... an X32 emulator. This tool parses and manages X32 commands (as a real X32 would), keeps up to 4 xremote clients updated - Of course no sound, and even if all 32 Channels, 16 Sends, 8 FXreturns, 8 Aux, 6 Matrix, 8 DCA, Main and all FX parameters are fully implemented along with multi-client xremote update and many more, not all X32 commands are supported (and that's not the goal), but the emulator is handy for developing X32 applications.

```
#!bash

usage: X32 [-d 0/1, debug option] -default: 0
           [-v 0/1, verbose option] -default: 1
       The options below apply in conjunction with -v 1
           [-x 0/1, echoes incoming verbose for /xremote] -default: 0
           [-b 0/1, echoes incoming verbose for /batchsubscribe] -default: 0
           [-f 0/1, echoes incoming verbose for /formatsubscribe] -default: 0
           [-r 0/1, echoes incoming verbose for /renew] -default: 0
           [-m 0/1, echoes incoming verbose for /meters] -default: 0

   The (non-Behringer) command "/shutdown" will save data and quit
```


In the above example of use, the following commands were sent from an X32_Command window (the -> lines show dialog data going to and from the X32 emulator):

```
#!bash

/ch/01/mix/fader
->X,   20 B: /ch/01/mix/fader~~~~
X->,   28 B: /ch/01/mix/fader~~~~,f~~[1.0000]
/ch/01/mix/fader ,f .5
->X,   28 B: /ch/01/mix/fader~~~~,f~~[0.5000]
/node ,s ch/01/config
->X,   28 B: /node~~~,s~~ch/01/config~~~~
X->,   40 B: node~~~~,s~~/ch/01/config "" 0 OFF 0~~~~
/shutdown
->X,   12 B: /shutdown~~~
```

Please note: You should run the X32 a first time and issue a "/shutdown" command from a connected client; this will create a file preserving all X32 parameters, that will be read at next start. Make sure you end your client sessions with "/shutdown" to update the file with the changes made to X32.

Some X32 commands are not implemented (lack of time mostly), some are also not present as this program was developed before FW2.14 was released. You are welcome to dive in the code and add them :-)

### X32UDP ###
This is a set of 3 functions to connect to X32, send and receive data (non-blocking IO). I use these functions to interface with Pascal (Lazarus) code I use when writing or porting my apps to Raspberry Pi. Lazarus enables writing GUI apps very easily (and quickly). The interfacing with C is quite simple too and works great.

### X32TCP ###
![X32TCP.jpg](https://bitbucket.org/repo/K9Ae7b/images/2597724007-X32TCP.jpg)

A multi threaded TCP server to serve X32 commands. This program starts a TCP server and handles requests for X32 (changing to UDP to manage dialog with X32) from multiple clients. The goal is to provide a basis for people who want to access X32 via TCP protocol, rather than UDP. 
It will be slower than UDP, and may suffer from data volume back from the X32, but will offer the possibility to access (with some modifications) an X32 via HTTP protocol for example.


```
#!bash

usage: X32TCP [-b [10] server max connections backlog]
              [-i X32 console ipv4 address]
              [-d 0/1, [0], debug option]
              [-v 0/1  [1], verbose option]
              [-p [10041] server port]

   After starting, the server waits for clients to connect and send X32_command
   format like commands, sent as character strings to the TCP server.
   All commands from connected clients are parsed and formatted to X32 OSC standard
   before being sent to X32. If X32 answers, the X32 OSC data is formatted to readable
   format before being returned to the connected client as a string (null characters
   are replaced with '~' to ease printing or parsing).
   If no answer after a timeout of 10ms from X32, the string 'no data' is returned.

   The command 'exit' from a connected client input closes the respective client stream.
```

### X32Ssaver ###
![X32Ssave.jpg](https://bitbucket.org/repo/K9Ae7b/images/793849478-X32Ssave.jpg)

X32SsaverGW.c - This is the Windows GUI version; the Linux and windows command line versions are in the "Unofficial OSC Protocol" document [http://www.academia.edu/9709659/UNOFFICIAL_X32_OSC_REMOTE_PROTOCOL].

Set the X32 IP address and hit the "Connect" button  to connect to X32. 
Set the utility to "ON"; After the number of seconds set in the "Delay before low light" box, the X32 LCD bright and LED intensity values will be lowered to their minimum, unless an / as long as no action takes place on the X32. As soon as a fader is moved, or a button is changing, LCD and LED will take their initial brightness values, and will return to low after a new delay with no action on the X32 deck.]