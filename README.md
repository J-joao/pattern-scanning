# pattern-scanning
Pattern scanning is the process of searching through an executable's memory in order to find a certain function, variable, etc.

## compile with:
```
make pattern_scan
```
## usage:
```
./pattern_scan <directory> "00 00 ?? 00 ?? ..."
```

# example:
inside this directory there's a file called ```hookme.c``` your objective is to get the body of the functions ```mysum()``` and ```mysum2()``` <br>
## here's how you can do it:

**compile the file into an executable** <br>
```gcc hookme.c -o hookme```

**open it using a disassembler and locate both functions** <br>
```objdump -S hookme```

**copy the function's body** <br>
<img src="img/objdump.">

**run the pattern scanner inside the executable's directory and pass the function's body as argument** <br>
<img src="img/pattern_scanning.">
