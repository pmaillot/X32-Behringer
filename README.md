# X32 / OSC utilities #
## What is this repository for? ##

This is a repository for Behringer X32 OSC program files. I will try to push/commit most of the utilities I have been writing since 2013 for the X32 Standard console I bought then.
The first one I committed is X32_Command, a simple yet very useful and powerful command line tool for interacting with the X32 digital audio console.

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