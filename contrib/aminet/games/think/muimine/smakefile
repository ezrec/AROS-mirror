
MUIMine: MUIMine.o MFStrings.o MFWindow.o BTWindow.o LEWindow.o ISWindow.o LevelData.o MineField.o FaceButton.o Digits.o
    sc link MUIMine.o MFStrings.o MFWindow.o BTWindow.o LEWindow.o ISWindow.o LevelData.o MineField.o FaceButton.o Digits.o to MUIMine

MUIMineLocale.h: MUIMine.cd
    catcomp MUIMine.cd CFILE MUIMineLocale.h NOBLOCK NOCODE

ISWindow.o: ISWindow.c ISWindow.h MUIMine.h MFStrings.h MUIMineLocale.h
    sc ISWindow.c

BTWindow.o: BTWindow.c BTWindow.h MUIMine.h MFStrings.h MUIMineLocale.h LevelData.h
    sc BTWindow.c

FaceButton.o: FaceButton.c FaceButton.h MUIMine.h
    sc FaceButton.c

LevelData.o: LevelData.c LevelData.h MUIMine.h MFStrings.h MUIMineLocale.h
    sc LevelData.c

LEWindow.o: LEWindow.c LEWindow.h MUIMine.h MFStrings.h MUIMineLocale.h LevelData.h
    sc LEWindow.c

MFStrings.o: MFStrings.c MFStrings.h MUIMineLocale.h MUIMine.h
    sc MFStrings.c

MFWindow.o: MFWindow.c MFWindow.h MUIMine.h MFStrings.h MUIMineLocale.h LevelData.h BTWindow.h LEWindow.h ISWindow.h MineField.h FaceButton.h Digits.h
    sc MFWindow.c

MineField.o: MineField.c MineField.h MUIMine.h
    sc MineField.c

MUIMine.o: MUIMine.c MUIMine.h MFWindow.h MFStrings.h MUIMineLocale.h
    sc MUIMine.c

Digits.o: Digits.c Digits.h MUIMine.h
    sc Digits.c


