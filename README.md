# Stack Smashing
This program uses a buffer overflow attack against a 32-bit Ubuntu Linux server running a buggy server program, *webserver.c*, to get the server to execute code to spawn a shell and listen for commands sent from the attacker.

## Server Vulnerability
In webserver.c, there's a bug in the method 
    
    int check_filename_length(byte len) {
      if (len < 100)
        return 1;
      return 0;
    }

The method takes in an argument of type *byte* rather than *int* and checks to see it if is less than 100. The result is that any number whose lower 8 bits are lower than 100. (e.g. 8268 => 00100000 01001100, 01001100 => 76 < 100)

Since the buffer for filename in the method *handle* only allocates 100 bytes of memory, any request with a clever length can pass through *check_filename_length*, get the data copied into memory by the *strncpy* call, overflow the stack frame, and overwrite the return address for the *handle* function.

Modern systems randomizes the virtual address space to protect against this kind of attack. To test out the buggy server, run the following code.

    sysctl -w kernel.randomize_va_space=0
    gcc -z execstack -fno-stack-protector webserver.c -o server
    
This will produce the executable file *server*. To run it, use the following command and pass it a port number as an argument so the server will listen on that port. 

    ./server {portnumber}
    
## Buffer Overflow Attack
The code in generate.cpp will produce the data wrapped in a GET request to be sent to the server. It consists of three mainparts:

1. **Return address** - address value to overwrite the return address of the *handle* function. The actual address will be different on different machines. The address is repeated so that it will be large enough to overflow the stack frame and replace the return address
2. **No-op slide** - a bunch of no-op instructions in byte code, which will cause the machine to skip over them and execute whatever comes afterward
3. **Shell code** - code that spawns a shell to listen for incoming commands when executed

When the *handle* function returns, the machine would jump to the address specified and, if guess correctly, land somewhere in the no-op slide, which would cause the machine to execute the shell code and spawn a shell.

To run the program:

    g++ generate.cpp -o generate
    ./generate

This will produce a file called data.dat. Use *nc* to send the data to the server:

    cat data.dat | nc {server ip/hostname} {portnumber}
    
More info on Stack Vulnerabilities: ["Smashing the Stack for Fun and Profit"](http://inst.eecs.berkeley.edu/~cs161/fa08/papers/stack_smashing.pdf)