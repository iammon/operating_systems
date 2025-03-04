# Simple Linux Shell (hlm21 Shell)

## Overview
This is a simple command-line shell written in C for Linux. The shell allows users to execute programs, manage files, and perform basic system operations.

## Features
- Execute Linux commands using `execvp()`
- Supports internal commands:
  - `C file1 file2`  → Copy file1 to file2
  - `D file`  → Delete a file
  - `E comment`  → Echo a message
  - `H`  → Display the help menu
  - `L`  → List directory contents
  - `M file`  → Create a new text file and open it in `nano`
  - `P file`  → Print file contents
  - `Q`  → Quit the shell
  - `W`  → Clear the screen
  - `X program [args]`  → Execute an external program with arguments

## Installation
### 1. Clone the Repository
```sh
git clone https://github.com/iammon/operating_systems.git
cd operating_systems
```
### 2. Make sure you have GCC Installed
Make sure `gcc` is installed:
```sh
gcc --version
```
If not installed, use:
```sh
sudo apt update && sudo apt install gcc -y
```
### 3. Create Makefile (if missing)
Create a file named `Makefile` in the same directory as the C file and add:
```
CC = gcc
CFLAGS = -Wall -Wextra -std=c11
TARGET = simpleShell
SRC = simpleShell.c
OBJ = simpleShell.o

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

$(OBJ): $(SRC)
	$(CC) $(CFLAGS) -c $(SRC) -o $(OBJ)

clean:
	rm -f $(TARGET) $(OBJ)

run: all
	./$(TARGET)
```
## Running the Shell
### Compile the Shell
```sh
make
```
### Run the Shell
```sh
make run
```
## Usage
One inside the shell, use the following commands:
```sh
linux (hlm21) |> C file1.txt file2.txt   # Copy file1.txt to file2.txt
linux (hlm21) |> D file.txt              # Delete file.txt
linux (hlm21) |> E Hello World!          # Print "Hello World!"
linux (hlm21) |> H                       # Show help menu
linux (hlm21) |> L                       # List directory contents
linux (hlm21) |> M myfile.txt            # Create a new file and open in nano
linux (hlm21) |> P myfile.txt            # Print the contents of myfile.txt
linux (hlm21) |> W                       # Clear the screen
linux (hlm21) |> X ls -l                 # Execute "ls -l"
linux (hlm21) |> X echo Hello World!     # Execute "echo Hello World!"
```

## Debugging and Cleaning
- To clean up compiled files:
```sh
make clean
```
- To debug using `gdb`:
```sh
gdb ./simpleShell
```

## License
This project is for educational purposes only.
## Contributors
- ### Htaw Mon
- ### Tim O'Neil (provided the template for this project as well as the necessary knowledge to complete it)



