README.txt
**********

Known Issues:
-------------

Removing file will redraw all rows
Added file will redraw all rows

Large number of files untested

Add support for Mac

Improve encapsulation


Compilation:
------------

Linux:
    GCC 4.8.4 x86 64bit (Ubuntu 4.8.4-2ubuntu1~14.04.4)
    Qt 5.7.1

Windows:
    MSVC 2013 64bit
    Qt 5.6.2


Requirements:
-------------

Windows and Mac

QTableView - control
        Files only (no subdirectories)
        Two columns
                File Names
                File Sizes

User selected folder

No scanning on interval

Detect change
        deletion
                only remove deleted folder
        adding
                add file to end of list
        resizing
                only file size value should change

Should be able to handle large number of files



