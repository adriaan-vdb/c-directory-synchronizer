# mysync

A C-based directory synchronization tool supporting multi-directory sync, file pattern matching, and preservation of file metadata. Developed for CITS2002 Project 2 (2023).

---

## Summary

This project implements a robust directory synchronization tool in C, named `mysync`, developed for CITS2002 Project 2 (2023). The tool synchronizes the contents of two or more directories, ensuring that files are consistent across all specified locations. It supports advanced options for file inclusion/exclusion, preserves file permissions and modification times, and provides verbose output for transparency. The project demonstrates practical use of file system operations, hashing, pattern matching, and command-line argument parsing in C.

---

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Project Structure](#project-structure)
- [Build & Run](#build--run)
- [Usage](#usage)
- [Options](#options)
- [How It Works](#how-it-works)
- [Authors](#authors)
- [License](#license)

---

## Overview

`mysync` is a command-line tool that synchronizes the contents of two or more directories. It ensures that all files (and their subdirectories) are present and up-to-date in each specified directory. The tool supports advanced options for including or excluding files based on patterns, preserving file permissions and modification times, and providing detailed output of the synchronization process.

---

## Features

- **Multi-directory Synchronization**: Syncs any number of directories, not just pairs.
- **File Pattern Matching**: Supports glob patterns for including/excluding files (`-i`, `-o`).
- **Preserves Metadata**: Optionally preserves file permissions and modification times (`-p`).
- **Verbose Output**: Provides detailed logs of actions taken (`-v`).
- **Efficient File Comparison**: Uses hashing and metadata to detect changes.
- **Safe Operations**: Skips hidden files by default, with options to include them.
- **Dry Run**: Optionally shows what would be done without making changes (`-n`).
- **Recursive**: Handles nested subdirectories, creating them as needed.

---

## Project Structure

```
cits2002-project2/
├── mysync.c           # Main synchronization logic
├── mysync.h           # Header file with data structures and function declarations
├── utilities.c        # File operations and helpers
├── hashtable.c        # Hash table implementation for file tracking
├── List.c             # Linked list implementation for patterns and files
├── glob2regex.c       # Glob pattern to regex conversion
├── makefile           # Build instructions
├── notes.txt          # Project notes and design questions
├── test.c             # Test code
├── [other files...]
```

---

## Build & Run

### Requirements

- C compiler supporting C11 (e.g., `gcc`, `clang`)
- Unix-like environment (tested on macOS)

### Compilation

```sh
make
```
or manually:
```sh
gcc -std=c11 -Wall -Werror -o mysync mysync.c List.c hashtable.c glob2regex.c utilities.c
```

---

## Usage

```sh
./mysync [options] directory1 directory2 [directory3 ...]
```

- At least two directories must be specified.

---

## Options

- `-a` : Include hidden files (those starting with a dot).
- `-i pattern` : Ignore files matching the given glob pattern (can be used multiple times).
- `-o pattern` : Only include files matching the given glob pattern (can be used multiple times).
- `-n` : Dry run; show what would be done without making changes (enables `-v`).
- `-p` : Preserve file permissions and modification times.
- `-r` : (Reserved for future use or recursive, if implemented).
- `-v` : Verbose output; print detailed actions.

### Example

```sh
./mysync -a -p -v -i '*.tmp' dirA dirB dirC
```
This will synchronize `dirA`, `dirB`, and `dirC`, including hidden files, preserving permissions and modification times, ignoring files ending in `.tmp`, and printing detailed output.

---

## How It Works

- **Directory Scanning**: Recursively scans all specified directories, building a hash table of files based on their relative paths.
- **File Comparison**: Compares files by name, location, and optionally metadata.
- **Pattern Matching**: Applies inclusion/exclusion patterns using glob-to-regex conversion.
- **Sync Instructions**: Determines which files need to be copied or updated in each directory.
- **File Operations**: Copies files as needed, creating subdirectories and preserving metadata if requested.
- **Verbose Logging**: Prints actions taken if `-v` is enabled.

---

## Authors

- Joshua Then (23432725)
- Adriaan van der Berg (23336556)

---

## License

This project is for educational purposes as part of the CITS2002 course at UWA. 