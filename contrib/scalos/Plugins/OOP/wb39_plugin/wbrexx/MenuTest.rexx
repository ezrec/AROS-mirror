/* test */
address WORKBENCH;

MENU REMOVE NAME test4
MENU REMOVE NAME test5
MENU REMOVE NAME test6

MENU ADD NAME test4 TITLE '"Say something\Hello"' CMD "'say hello'";
MENU ADD NAME test5 TITLE '"Say something\World"' CMD "'say world";

MENU ADD NAME test6 TITLE '"\New title\Say something\Hello"' CMD "'say hello";
