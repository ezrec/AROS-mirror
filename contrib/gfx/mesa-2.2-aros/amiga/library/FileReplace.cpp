#include <fstream.h>
#include <stdlib.h>

#define MAXBUF 4095

void copy(ifstream& infil,ostream& utfil)
	{
	char	ch;
	while (utfil && infil.get(ch))
		{
		utfil.put(ch);
		}
	}


int replace(ifstream& infil,ofstream& utfil,char* str,char *nystr)
	{
	char	buff[MAXBUF];
	int		pos,ant;
	pos=0;
	ant=0;
	while (utfil && infil.get(buff[pos]))
		{
		while(buff[pos]==str[pos]&&pos<strlen(str) )
			{
			infil.get(buff[++pos]);
			}

		if(pos==strlen(str))
			{
			utfil.write(nystr,strlen(nystr));
			utfil.put(buff[pos]);									// the missing char
			ant++;
			}
		else
			{
			utfil.write(buff,pos+1);
			}
		pos=0;
		}
	if (ant>0)
		{
		cout<<"Found "<<ant<<" apperece";
		if (ant>1)
			cout<<"s";
		cout<<".\n";
		}
	else
		{
		cout<<"Found no match\n";
		}
	return(ant);
	}

void PrepareStr(char *str,char *outstr)
	{
	char	ch;
	int 	I,U;
	I=U=0;
	while((ch=str[I++])!=0)
		{
		if (ch=='\\')					//check special char
			{
			if((ch=str[I++])==0)
				{							  // END of Str
				outstr[U++]='\\';		
				break;
				}
			else
				{
				if (ch=='n')						// Found /n
					{
					outstr[U++]='\n';
					}
				else
					{
					outstr[U++]='\\';  // fix it if not special char
					outstr[U++]=ch;
					}
				}
			}
		else
			{
			outstr[U++]=ch;
			}
		}
	outstr[U++]=0;	
	}

int filereplace(char *file1, char *file2,char *str1,char *str2)
	{
	char	str[MAXBUF];
	char	*filename;
	int rc = 0;              								// Returvärde från programmet..
	int	ant;
	ifstream	infil;
	ofstream	utfil;
	
	if(strlen(file2)==0)
		{																			// if no destfile then make a tmpfile.
		filename="t:FileReplace.tmp";
		}
	else
		{
		filename=file2;
		}

	infil.open(file1);										// öppna fil....
	utfil.open(filename);										// Destfil
	if (infil && utfil)
		{
		cout<<file1<<" ";
		PrepareStr(str1,str);
		ant=replace(infil,utfil,str,str2); 		// Skriv ut...
		infil.close();  											// Stäng....
		utfil.close();
		
		if(strlen(file2)==0&&ant!=0)
			{																			// if no destfile then make a tmpfile.
			infil.open(filename);										// öppna fil....
			utfil.open(file1,ios::trunc);										// Destfil
			if (infil && utfil)
				{
				copy(infil,utfil);
				}
			if (infil)
				infil.close();  											// Stäng....
			if (utfil)
				utfil.close();
    	}
    }
	else
		{
		if (!infil)
			{
			cout << "Kunde inte öppna " <<  file1 << endl; 
			}
		else
			{
			cout << "Kunde inte öppna " <<  filename << endl;
			}
		if (infil)
			infil.close();  											// Stäng....
		if (utfil)
			utfil.close();

		rc = 1;
		}
	return rc;
	}

fnuttparser(ifstream& infil,char *str)
	{
	char	ch;
	int	I;
	infil.get(ch);
	while(ch!='"'&& !infil.eof())				// parsa till ett "
		infil.get(ch);
	I=0;
	infil.get(ch);											// fill str with the string
	while(ch!='"'&&I<MAXBUF && !infil.eof())
		{
		str[I++]=ch;											// Spara tecken till nesta "
		infil.get(ch);
		}
	str[I]=0;														//och en Null terminator
	}


int datafil(char *file)
	{
	int rc;
	char	arg1[MAXBUF],arg2[MAXBUF],arg3[MAXBUF],arg4[MAXBUF];
	ifstream	infil;
	rc=0;
	
	infil.open(file);										// öppna fil....
	if (infil)
		{
		while(!infil.eof())
			{
			fnuttparser(infil,arg1);
			fnuttparser(infil,arg2);
			fnuttparser(infil,arg3);
			fnuttparser(infil,arg4);
			if(!infil.eof())
				rc=filereplace(arg1,arg2,arg3,arg4);
			}
			
		}
	else
		cout<<"Error cant open datafil :"<<file<<"\n";
	return(rc);
	}



main(int argc,char *argv[])
{
	int	rc;
	rc=0;
	if (!(argc==2||argc==5))
		{
		cout<<"Error wrong type of arguments.\n";
		cout<<argv[0]<<" [infile] [destfile] [findstring] [Replacesting]\n";
		cout<<"or\n";
		cout<<argv[0]<<" [datafile]\n";
		cout<<"Were datafile	is a file with a series of "<<'"'<<"strings"<<'"'<<" that are grouped\n";	
		cout<<"into 4:s.The data is formed in the same order as the 4 comand line.\n";
		cout<<"In findstring you could write \\n to locate returns.\n";
		cout<<"If destfile ìs "" than infile will be overwritten.\n";
		cout<<"This Program is written by StefanZ as a help programs when he ported mesa to amiga.\n";
		return(0);
		}
	if (argc==5)
		{
		rc=filereplace(argv[1],argv[2],argv[3],argv[4]);
		}
	if (argc==2)
		{
		rc=datafil(argv[1]);

		}
	return(rc);
}