//Sounds
char    *Vault, *Expl, *Lose, *Bonus;

//Load the sounds
void LoadSounds ()
{
#if !NO_SOUND
  FILE  *SoundFile;
  long  WaveSize;

  //Open the file
  SoundFile = fopen ("BOMBSNDS.SSF", "rb");
  //Explosion
  fread (&WaveSize, 4, 1, SoundFile);
  Expl = (char *)malloc (WaveSize);
  fread (Expl, 1, WaveSize, SoundFile);
  //Lose
  fread (&WaveSize, 4, 1, SoundFile);
  Lose = (char *)malloc (WaveSize);
  fread (Lose, 1, WaveSize, SoundFile);
  //Bonus
  fread (&WaveSize, 4, 1, SoundFile);
  Bonus = (char *)malloc (WaveSize);
  fread (Bonus, 1, WaveSize, SoundFile);
  //Close the file
  fclose (SoundFile);
#endif
}
