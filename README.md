# Project 5 - RSA encryption

## Summary:
This program takes in a user input and and encrypts the data using functions from the library "numtheory.c." This also decrypts the data so that you get the original output. The original message and decypted message should be the same.

## Run:
To the run the program, one can type "make all" in order to compile the whole program. You first want to generate your key. This can be done using the default values by calling ./keygen or you can set your own values--use -h for more info on that. Once your keys are developed, you can use the following example to test: echo "hello" |./encrypt | ./decrypt. Again, you can use -h for more info on these programs.

## Expected Output:
The code should output the original message typed into the file or terminal

## Error Handling/Errors:
If the user inputs a "NULL" file to decrypt, then the program spits out an error. This also goes for keygen amd encrypt. The usage function of decrypt tells the user to only decrypt something that is encrypted.

## Scan-build:
running "scan-build make" gives me : "relocation R_AARCH64_ADR_PREL_PG_HI21 against symbol `stderr@@GLIBC_2.17' which may bind externally can not be used when making a shared object; recompile with -fPIC" -- no idea what that means so...

## Valgrind:
Gives no Leaks


