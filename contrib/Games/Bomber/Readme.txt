BomberMan Source code (c) 1998 Steven Don
=========================================
Okay, this is the source for my Bomberman clone. You will need Microsoft
Visual C++ 5 or better in order to compile it.

Just to make sure you get everything right, here's how to set up a project to
compiler Bomberman directly:
- start Visual C++ 5.0
- in the "File" menu, select "New"
- select "Win32 Application"
- type "Bomber" in the "Project name" field
- in the "Location" field, select the directory where you've unzipped this file
- click "OK"
- in the "Project" menu, choose the submenu "Add To Project" and then "Files..."
- in the dialog box that appears, double click on "MAIN.C"
- in the "Project" menu, choose the submenu "Add To Project" and then "Files..."
- in the dialog box that appears, double click on "BOMBER.RC"
- in the "Project" menu, choose "Settings"
- change "Settings For" to "All Configurations"
- click on the "Link" tab
- add "winmm.lib" to the list of files in "Object/library modules"
- click "OK"

That sets up your project correctly. When you do a build, a file called
"BOMBER.EXE" will be created.