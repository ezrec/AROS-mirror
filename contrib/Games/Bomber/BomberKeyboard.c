//Virtual keycodes
#define VK_A 0x20 
#define VK_B 0x35
#define VK_C 0x33
#define VK_D 0x22
#define VK_E 0x12
#define VK_F 0x23
#define VK_G 0x24
#define VK_H 0x25
#define VK_I 0x17
#define VK_J 0x26
#define VK_K 0x27
#define VK_L 0x28
#define VK_M 0x37
#define VK_N 0x36
#define VK_O 0x18 
#define VK_P 0x19 
#define VK_Q 0x10 
#define VK_R 0x13
#define VK_S 0x21
#define VK_T 0x14
#define VK_U 0x1a
#define VK_V 0x34
#define VK_W 0x11
#define VK_X 0x32
#define VK_Y 0x31
#define VK_Z 0x15

#define VK_UP 0x4c
#define VK_DOWN 0x4d
#define VK_LEFT 0x4f
#define VK_RIGHT 0x4e
#define VK_CONTROL 0x63
#define VK_TAB 0x42
#define VK_ESCAPE 0x45

//Keyboard variables for each player
int             ChosenKeys [3 * 5];
int             Up [5], 
                Down [5], 
                Left [5], 
                Right [5], 
                Fire [5];
//Various keys
int             Esc;

//For joystick
unsigned int    JoyUp, JoyLeft, JoyRight, JoyDown;
int             JoyAvail;

//Initializes the standard keys
void InitKeyboard ()
{
    ChosenKeys [ 0] = VK_UP;
    ChosenKeys [ 1] = VK_DOWN;
    ChosenKeys [ 2] = VK_LEFT;
    ChosenKeys [ 3] = VK_RIGHT;
    ChosenKeys [ 4] = VK_CONTROL;
    ChosenKeys [ 5] = VK_W;
    ChosenKeys [ 6] = VK_X;
    ChosenKeys [ 7] = VK_A;
    ChosenKeys [ 8] = VK_D;
    ChosenKeys [ 9] = VK_TAB;
    ChosenKeys [10] = VK_U;
    ChosenKeys [11] = VK_N;
    ChosenKeys [12] = VK_H;
    ChosenKeys [13] = VK_K;
    ChosenKeys [14] = VK_B;

#if !NO_JOY
  
  //Get info about joystick axes
  JoyAvail = (joyGetDevCaps (JOYSTICKID1, &JoyCaps, sizeof (JoyCaps)) == JOYERR_NOERROR);
  if (JoyAvail) {
    JoyUp = (JoyCaps.wYmax - JoyCaps.wYmin) * 1 / 6;
    JoyDown = (JoyCaps.wYmax - JoyCaps.wYmin) * 5 / 6;
    JoyLeft = (JoyCaps.wXmax - JoyCaps.wXmin) * 1 / 6;
    JoyRight = (JoyCaps.wXmax - JoyCaps.wXmin) * 5 / 6;
  }
#endif

}

//Stores keyboard definition in registry
void SaveKeys ()
{
#if 0 
  HKEY  KeyHandle;
  RegCreateKey (HKEY_LOCAL_MACHINE, "Software\\Bomberman", &KeyHandle);
  RegSetValueEx (KeyHandle, "Keys", 0, REG_BINARY, (LPBYTE)ChosenKeys, 60); 
  RegCloseKey (KeyHandle);
#endif
}

//Checks for keyboard input
void CheckKeyboard ()
{
#if !NO_JOY
  JOYINFO   JoyInfo;
#endif

  //Player 1
  Up    [0] = GetAsyncKeyState (ChosenKeys [ 0]);
  Down  [0] = GetAsyncKeyState (ChosenKeys [ 1]);
  Left  [0] = GetAsyncKeyState (ChosenKeys [ 2]);
  Right [0] = GetAsyncKeyState (ChosenKeys [ 3]);
  Fire  [0] = GetAsyncKeyState (ChosenKeys [ 4]);
  //Player 2
  Up    [1] = GetAsyncKeyState (ChosenKeys [ 5]);
  Down  [1] = GetAsyncKeyState (ChosenKeys [ 6]);
  Left  [1] = GetAsyncKeyState (ChosenKeys [ 7]);
  Right [1] = GetAsyncKeyState (ChosenKeys [ 8]);
  Fire  [1] = GetAsyncKeyState (ChosenKeys [ 9]);
  //Player 3
  Up    [2] = GetAsyncKeyState (ChosenKeys [10]);
  Down  [2] = GetAsyncKeyState (ChosenKeys [11]);
  Left  [2] = GetAsyncKeyState (ChosenKeys [12]);
  Right [2] = GetAsyncKeyState (ChosenKeys [13]);
  Fire  [2] = GetAsyncKeyState (ChosenKeys [14]);

#if !NO_JOY
  //Player 4 - joystick
  if (JoyAvail) {
    joyGetPos (JOYSTICKID1, &JoyInfo);
    Up    [3] = (JoyInfo.wYpos < JoyUp);
    Down  [3] = (JoyInfo.wYpos > JoyDown);
    Left  [3] = (JoyInfo.wXpos < JoyLeft);
    Right [3] = (JoyInfo.wXpos > JoyRight);
    Fire  [3] = (JoyInfo.wButtons & JOY_BUTTON1);
  }
#endif

  Esc       = GetAsyncKeyState (VK_ESCAPE);
}

//Flush the keyboard buffer
void FlushKeys ()
{
  InPointer = OutPointer;
}

//Returns a character
char getch ()
{
  int t;

  if (InPointer == OutPointer) 
    return -1;
  else {
    t = OutPointer++;
    OutPointer %= 50;
    return InBuffer [t];
  }

}
