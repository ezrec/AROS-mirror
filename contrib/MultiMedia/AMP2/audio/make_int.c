/* make_int.c */

#include <stdio.h>

int main(int argc, char **argv)
{
  unsigned short a, b = 0;
  FILE *fp_in = fopen("paula_int.o", "rb");
  FILE *fp_out = fopen("paula_68k.c", "wt");

  if (fp_in != NULL) {
    fprintf(fp_out, "#include <amitypes.h>\n");
    fprintf(fp_out, "const u16 paula_interrupt[] = {\n");
    while (!feof(fp_in)) {
      fread(&a, 1, 2, fp_in);
      if (a == 0x48E7) b = 1; /* moveml ... */
      if (b) fprintf(fp_out, "  0x%04x,\n", a);
      if (a == 0x4E75) b = 0; /* rts */
    }
    fprintf(fp_out, "};\n");
  }
  fclose(fp_out);
  fclose(fp_in);
}