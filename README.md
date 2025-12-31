# Simple Shell (hsh)

A simple command line interpreter written in C, inspired by `sh`.
This project is part of the Holberton School curriculum.

## Description

`hsh` is a simple shell that reads user input, parses it, and executes commands.
It supports interactive mode (prompted) and non-interactive mode (reading from stdin or a file).

## Features Implemented

### Core Execution
- Display a prompt and execute commands (interactive mode)
- Execute commands with arguments
- Handle `PATH` resolution (do not `fork` if command does not exist)
- Error handling and exit status management
- Handle EOF (Ctrl+D)

### Built-ins
- `exit` (with optional status: `exit status`)
- `env`
- `setenv VARIABLE VALUE`
- `unsetenv VARIABLE`
- `cd [DIRECTORY]` and `cd -` with `PWD` update
- `help [BUILTIN]`
- `history` (read/write history file in `$HOME`)

### Operators / Parsing
- Command separator `;`
- Logical operators `&&` and `||`
- Variables replacement:
  - `$?` last command status
  - `$$` current process id
  - Environment variables like `$PATH`

### Extras
- Handle comments starting with `#`
- `alias` builtin:
  - `alias` list all
  - `alias name ...` show specific
  - `alias name='value'` set/replace

### File as input
- Usage: `hsh [filename]`
- When a file is provided:
  - The shell runs commands from the file (one per line)
  - The shell does **not** print a prompt
  - The shell does **not** read from stdin

## Requirements
- Allowed editors: `vi`, `vim`, `emacs`
- Compiled on Ubuntu with:
  ```bash
  gcc -Wall -Werror -Wextra -pedantic -std=gnu89 *.c -o hsh
  

## Files
- hsh.h : header file (prototypes, includes, macros)
- *.c : shell source files
- man_1_simple_shell : manual page for the shell
- AUTHORS : contributors list

## Usage
### Interactive mode:
./hsh
$ ls
$ /bin/ls
$ exit

### Non-Interactive mode:
echo "/bin/ls" | ./hsh

### File input mode:
./hsh commands.txt

## Examples
### Use PATH resolution:
$ ls
### Chain commands:
$ ls /var ; echo done
### Logical operators:
$ ls /var && echo ok
$ ls /notfound || echo failed
### Variables:
$ echo $$
$ echo $?
$ echo $PATH
### Comments:
$ echo $$ # this part is ignored

## Man Page
### To view the man page locally:
man ./man_1_simple_shell

## Authors
Moudhi Almutlaq <Moudhi.a.almutlaq@gmail.com>

Adwa Almuraikhy <Iadwaalmuraikhy@gmail.com>







