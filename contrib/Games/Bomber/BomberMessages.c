  //MIDI repeat
  if (wMessage == MM_MCINOTIFY) {
    Playing = 0;
    return -1;
  }

  //Keyboard
  if (wMessage == WM_KEYDOWN) {
    if ((HIWORD(lParam) & KF_UP) != KF_UP) {
      InBuffer [InPointer++] = (char)wParam;
      InPointer %= 50;
    }
  }
