# Activity - Backdoor

On the base of mini-VM, I implemented mini-language interpreter and its backdoor. 


### ./interpreter 

Delete the annotation of the below line, if you want to print debugging message. 

```
	// #define VM_DEBUG_MESSAGE
```

Process has two memory areas, 1) TEXT for binary code and 2) HEAP for allocating data. 

```
	char text[ TEXT_SIZE ]; 
	char heap[ HEAP_SIZE + 1 ]; 
```

And I added program counter for control logic. 

```
	uint32_t* pc;
```

### ./login/login.mini

I implementated the pseudocode for login program with mini language. 

### ./test/test.mini

### ./test/test.md

### ./backdoor 

Backdoor is added on the base code of the interpreter.
I assumed that backdoor code is hidden and built-in memory.  

```
	char backdoor_text[108] = 
	    "\x40\x01\x00\x00"  // puti r1, 0
	    "\x40\x02\x53\x00"  // puti r2, 0x53
	    "\x20\x01\x02\x00"  // store r1, r2
	         ...
	    "\x00\x00\x00\x00"; // halt          
```