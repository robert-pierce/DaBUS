Robert Pierce
CS-2200
summer 2016

Project 1 Readme

To run the code first open up the project-1 file inside
Logisim. This file is named "project-1.circ"

Next, You may need to load the micro-instructions into the control 
unit ROM. The micro-instruction file is named "control-unit-rom-image" 
and it is already in the proper format. You must enter the control-unit 
sub-circuit from within Logisim to get to the ROM. The micro-instructions 
may already be loaded.

Next, load the machine code into the memory RAM. The machine code
file is named "project-1-machine-code. You must enter the Memory
sub-circuit in order to get to the RAM.

There is a detailed description of the ISA and micro-instruction
format in the file named "ISA-and-micro-instructions".

The assembly code for the project is located in the file named 
"project-1.asm".

I wrote an assembler for this ISA and assembly format using
Clojure. This project is in the "assembler" directory. The
main function can be found at 
"assembler\assembler-cs-2200\src\assembler_cs_2200/core.clj"
within the directory.

The stack pointer is initialized to address 0x2710 (10,000).