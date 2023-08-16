# l
Alternative to ls classical application

There are no external dependencies, only standard library.

# Output customization
In home directory(~ for example) .lconfig file is created at first run of program<br>
and it contains data about colors and number of element per line for table output.<br>
P.S. Every option can be customized in future.

# help page
Overall, this program is dedicated to show directory's content and its properties.<br>
It can show file/dir permissions, size, creation and modification time.<br>
It has 3 different modes of output: list, table, tree.<br>
All properties are shown in list mode, so if you wanna show size of files in tree mode,<br>
than program will be forced to print all requested data in list mode.<br>
To see all files and dirs just type l without any flags.<br>
If you don't provide any path as argument, l will show content of current dir,<br>
otherwise it will show content of provided directory.<br>
List of flags:<br>
-d show only directories<br>
-f show only files<br>
-s sort output( -d -f and -f -d are two ways to manage sorted output)<br>
-r recursivly walk through directory<br>
-p show permissions of files/dirs<br>
-T show modification time<br>
-C show creation time<br>
-S show size of each file in directory<br>
-l show as list(by default)<br>
-m show as table<br>
-t show as tree<br>
-h print help page and break program execution<br>
