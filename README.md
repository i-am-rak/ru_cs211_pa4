###CS211 Pa4 y86 emulator
---

The y86 emulator first parses the argument (“prog1.y86”) and populates the memory with the specified
values. This includes first to malloc the size, and then store the .text, .byte, .long and other parts of the
file. If the input is –h, it prints out a message to explain how to use the program. There are also error
checking to see if the file exists and it is of correct format.
After the file is parsed and memory is populated, the fetch, decode and execute cycle starts. This is done
with a do while loop which starts at the position specified at text and checks the opcodes of the
instructions. This opcode is fed to a switch statement which calls the respective functions (for example
dorrmovl, doirmovl, or docall). The respective function then executes the instruction and performs the
task (for example, doaddl will add the two specified registers and store the result and set the flags. The
rest of the function behaviors was taken from the pdf and document provided). This will continue to run
until the halt opcode is found, which exits the while loop. After the while loop is done, the memory will
be freed and the program will exit. If the program ends on HLT, it will also print out the respective
message to state that it halted. Other ways the program can exit is through one of the other status
codes.
For the status codes, if there is an incorrect instruction or address, the program prints out the respective
message and then exits the program (exit(1);). The program always sets the AOK flag until another flag is
set (HLT, INS, or ADR) and will only print out AOK if the program ends prematurely. This can be done
through a signal interrupt (usually Ctrl-C).
CS211 Pa4 y86 disassembler
This program parses the .text file and prints out the respective y86 mnemonics. This is done with the
same do while loop + switch statement as the emulator but instead of executing the instructions it
prints out what the instruction is. This program also checks if the file exists/formatted, and if the –h flag
was invoked in the argument. The respective error or help messages will be printed out when this
occurs.
