#if !NO_MIDI
MCIDEVICEID gDeviceID = -1;
#endif

int         Playing = 0;

void PlayMidi (char *FileName)
{
#if !NO_MIDI
  char              PathName [2048];
  MCI_OPEN_PARMS    OpenStructure;
  MCI_PLAY_PARMS    PlayStructure;

  if (gDeviceID != -1) {
    mciSendCommand (gDeviceID, MCI_STOP, 0, 0);
    mciSendCommand (gDeviceID, MCI_CLOSE, 0, 0);
    gDeviceID = -1;
  }

  //Append full path to filename
  strcpy (PathName, StartDir);
  strcat (PathName, FileName);

  OpenStructure.lpstrDeviceType = "sequencer";
  OpenStructure.lpstrElementName = PathName;
  mciSendCommand (0, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_ELEMENT, (DWORD)&OpenStructure);
  gDeviceID = OpenStructure.wDeviceID;

  Playing = -1;

  PlayStructure.dwCallback = (DWORD)WindowHandle;
  mciSendCommand (gDeviceID, MCI_PLAY, MCI_NOTIFY, (DWORD)&PlayStructure);
#endif
}

void StopMidi ()
{
#if !NO_MIDI
  if (gDeviceID != -1) {
    mciSendCommand (gDeviceID, MCI_STOP, 0, 0);
    mciSendCommand (gDeviceID, MCI_CLOSE, 0, 0);
    gDeviceID = -1;
    Playing = 0;
  }
#endif
}

void UnloadMidi ()
{
#if !NO_MIDI
  if (gDeviceID != -1) {
    mciSendCommand (gDeviceID, MCI_STOP, 0, 0);
    mciSendCommand (gDeviceID, MCI_CLOSE, 0, 0);
  }
#endif
}
