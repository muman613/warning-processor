# Read Me First

## Overview

This repository contains the source-code to a tool which will scan a build log file and extract all the warnings. The tool writes a report of all the files containing warnings, and a break-down of the count of each type of warning encountered.

## Building

This project was created using the `CLion` IDE from JetBrains. The CLion IDE uses the `cmake` build system to configure and build the project.

### Steps

```bash
$> cd gitroot/warning-processor/
$> mkdir build && cd build
$> cmake ..
$> make
```

## Sample run

```bash
$> cd gitroot/warning-processor/
$> ./build/warning_processor -i sample/build-log051920-1620.txt -o warning-report-051920-1620.txt
   Parsing input file : sample/build-log051920-1620.txt
   Found input file, processing...
   Read 7688 lines...
   Extracting warnings from input file...
   Found 1560 warning lines...
   File processing complete...
$>
```

The report is in the file `warning-report-051920-1620.txt`.

