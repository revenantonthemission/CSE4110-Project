# CSE4110 Project - Convenience Store Database System

## Overview
This project implements a Convenience Store Database System that allows users to interact with a database to retrieve various information related to products, stores, vendors, and customer purchase patterns. The system supports both MySQL C API and ODBC for database connectivity.

## Files
- **database.h**: Defines the `DatabaseConnector` class, which abstracts database connection and query execution. It supports both MySQL C API and ODBC. It includes methods for connecting, disconnecting, and executing queries.
  
- **database.cpp**: Implements the methods declared in `database.h`. It contains the logic for connecting to the database using either the MySQL C API or ODBC, executing queries, and handling results.
  
- **main.cpp**: Serves as the entry point of the application. It interacts with the user, displays a menu, and calls the appropriate query functions based on user input. It uses the `DatabaseConnector` class to perform database operations.
  
- **Makefile**: Contains the build instructions for compiling the project. It specifies how to compile `database.cpp` and `main.cpp` into an executable.

## Setup Instructions
1. Ensure you have a MySQL server running and accessible.
2. Update the database connection details in `main.cpp` with your MySQL credentials.
3. Install the necessary libraries for MySQL C API and ODBC if not already installed.

## Build Instructions
To compile the project, run the following command in the terminal:

```
make
```

This will generate an executable file that you can run to start the application.

## Usage
After compiling, run the executable. You will be presented with a menu to choose from various queries related to the convenience store database. Follow the prompts to enter the required information and view the results.

## License
This project is licensed under the MIT License. See the LICENSE file for more details.
