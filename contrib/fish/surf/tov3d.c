#include <stdio.h>
#include <math.h>

#define true 1
#define false 0

/*
 * types
 */
typedef float triplet[3];
typedef int  quadrlat[4];

typedef enum {
               TminX, TminY, TminZ,
               TmaxX, TmaxY, TmaxZ,
               Tcolorin, Tcolorout, Tnameout } ArgType;

extern char *malloc();
extern double atof();

char *ArgName[] = {
    "minx", "miny", "minz", "maxx", "maxy", "maxz", "ci", "co", "o", NULL
  };

/*
 * command line arguments
 */
triplet MinIn, MaxIn; /* define bounding box */
char MinSet[3] = {false}, MaxSet[3] = {false};

int colorin = -1,
    colorout = -1;
char *namein = NULL,
     *nameout = NULL;
/*
 * file arguments
 */
FILE *infile, *outfile= NULL;
int linenum;

/*
 * mucky global variables
 */
triplet *inring;
char *indlist;
int pnt_total; /* total number of points */
triplet Scale = { 1.0, 1.0, 1.0 };
triplet Disp = { 0.0, 0.0, 0.0 };


int NumRings, /* number of curved rings */
    PtsInPerRing, /* points read in per ring */
    PtsOutPerRing, /* points written per ring */
    IsClosed; /* true if revolution is 360 degrees */

/*
 * parse the command line arguments
 */
void getCliArgs(argc, argv)
    int argc;
    char *argv[];
{
    int i;
    for( i = 0; i <argc; i++ ) {
        if( *argv[i] == '-' ) {
            int j;

            for( j = 0; ArgName[j]; j++ ) {
                if( !strncmp(ArgName[j], argv[i]+1, strlen(ArgName[j])))
                    break;
            }

            if( !ArgName[j] ) {
                fprintf(stderr,"%s unknown, try one of below:\n", argv[i]);
                for( j = 0; ArgName[j]; j++ ) {
                    fprintf(stderr,"-%s val\n", ArgName[j] );
                }
                exit(-1);
            }
            else {
                char *argpos;
                register int element;

                if( strlen(argv[i]+1) > strlen(ArgName[j]) ) {
                    argpos = argv[i] + strlen(ArgName[j]) +1;
                }
                else {
                    i++;
                    if( i >= argc ) {
                        fprintf(stderr,"missing arg for -%s\n", argv[i-1] );
                        exit(-1);
                    }
                    argpos = argv[i];
                }
                switch( (ArgType)j ) {
                    case TminX:
                    case TminY:
                    case TminZ:
                        element = j - (int)TminX;
                        MinIn[element] = atof(argpos);
                        MinSet[element] = true;
                        break;
                    case TmaxX:
                    case TmaxY:
                    case TmaxZ:
                        element = j - (int)TmaxX;
                        MaxIn[element] = atof(argpos);
                        MaxSet[element] = true;
                        break;
                    case Tcolorin: colorin = atoi(argpos); break;
                    case Tcolorout: colorout = atoi(argpos); break;
                    case Tnameout: nameout = argpos;
                } /* end case */
            }
        }
        else {
                namein = argv[i];
        }
    } /* end for loop */

    if (!namein ) {
        fprintf(stderr,"No input file named\n");
        exit(-1);
    }
    if(!nameout) {
        nameout = (char *)malloc(200); /* replace with max path size */
        sprintf(nameout, "%s.v3d", namein);
    }
}

/*
 * read an integer number into variable
 */
void readInt(IntVar)
    int *IntVar;
{
    char c;
    linenum++;
    if( !fscanf(infile, "%d", IntVar)) {
        fprintf(stderr, "could not read integer at line %d\n", linenum);
        exit(-1);
    }
    /*
     * skip to end of line
     */
    while( c = getc(infile), c != EOF && c != '\n' ) ;
}

void readTriplet( tripvar )
    triplet tripvar;
{
    linenum++;
    if(!fscanf(infile, "%f %f %f",tripvar, tripvar+1, tripvar+2)) {
        fprintf(stderr,"could not read coordinate triplet at line %d\n",
            linenum);
        exit(-1);
    }
}
/*
 * read an entire ring
 */
void readRing() {
    int i;
    for( i = 0; i< PtsInPerRing; i++ ) {
        readTriplet(inring[i]);
    }
}

/*
 * read the first few lines in the file that tell us how many
 * rings, howmany points per ring, etc
 */
void readHeader() {
    int RevRange;
    int RevMesh;

    linenum = 0;
    readInt( &NumRings );
    readInt( &RevMesh );
    readInt( &RevRange );

    PtsInPerRing = RevMesh + 1;
    if( RevRange == 360 ) {
        IsClosed = true;
        PtsOutPerRing = RevMesh;
    }
    else {
        IsClosed = false;
        PtsOutPerRing = PtsInPerRing;
    }
}
/*
 * this procedure reads in a list of data points to determine the
 * following:
 *      total number of data points to write out
 *      maximun x,y,z spread
 */
void readPass1() {
    triplet max3, min3;
    int i;
    pnt_total = 0;

    inring = (triplet *)malloc(sizeof(triplet)*PtsInPerRing);
    indlist = (char *)malloc(sizeof(char)*NumRings);

    for( i = 0; i < NumRings; i++ ) {
        int j;
        readRing();

        for( j = 0; j < PtsOutPerRing; j++ ) {
            int k;
            for( k = 0; k < 3; k++ ) {
                if( max3[k] < inring[j][k] || !i && !j ) max3[k] = inring[j][k];
                if( min3[k] > inring[j][k] || !i && !j ) min3[k] = inring[j][k];
            }
        }
        if( inring[0][0] == inring[1][0] &&
            inring[0][1] == inring[1][1] ) {
            indlist[i] = 0;
            pnt_total += 1;
        }
        else {
            indlist[i] = 1;
            pnt_total += PtsOutPerRing;
        }
    }
    /*
     * compute scaling and displacement
     */
    for( i = 0; i < 3; i++ ) {
        if( MaxSet[i]  && !MinSet[i] )
            MinIn[i] = -MaxIn[i];
        else if( !MaxSet[i] && MinSet[i] )
            MaxIn[i] = -MinIn[i];

        Scale[i] = 1.0;
        Disp[i] = 0.0;
        if( MaxSet[i] || MinSet[i] ) {
            float diffOut, diffIn;

            diffOut = MaxIn[i] - MinIn[i];
            diffIn = max3[i] - min3[i];
            if( diffIn > 0 ) Scale[i] = diffOut/diffIn;
            Disp[i] = MaxIn[i] - max3[i] * Scale[i];
        }
    }
}

void writeTriplet(trip)
    triplet trip;
{
    int i;
    for( i = 0; i < 3; i++ ) {
        fprintf(outfile,"%f%c", trip[i]*Scale[i]+Disp[i], i==2?'\n':' ');
    }
}


void writeRing()
{
    int i;
    for( i = 0; i < PtsOutPerRing; i++ ) {
        writeTriplet(inring[i]);
    }
}



void writePoints()
{
    int ringno;

    fprintf(outfile, "%d\n", pnt_total);
    for( ringno = 0; ringno < NumRings; ringno++ ) {
        readRing();
        if( indlist[ringno] )
            writeRing();
        else
            writeTriplet( inring[0] );
    }
}


void writeQuad(quad, numpts, color, order)
    quadrlat quad;
    int numpts, color, order;
{
    int i;
    fprintf(outfile, "%d ", numpts);
    for(i=0; i <numpts; i++ ) {
        fprintf(outfile,"%d ", quad[order?i:(numpts-i -1)]);
    }
    fprintf(outfile,"%d \n", color);
}

void writePolys() {
    int RevA, RevB;
    int PtCntA, PtCntB;
    int RingA, RingB;
    int PolysPerRing;

    PolysPerRing = PtsInPerRing -1;
    PtCntB = RingA = 0;
    for( RingB =1; PtCntA = PtCntB, RingB < NumRings; RingA = RingB++ ) {

        PtCntB += indlist[RingA]? PtsOutPerRing: 1;
        for( RevA =0; RevA < PolysPerRing; RevA++ ) {
            quadrlat quad;
            int qcnt;

            RevB = RevA +1;
            if( RevB >= PtsOutPerRing ) RevB = 0;

            qcnt = 0;
            if(indlist[RingA] ) {
                quad[qcnt++] = PtCntA + RevA;
                quad[qcnt++] = PtCntA + RevB;
            }
            else
                quad[qcnt++] = PtCntA;

            if(indlist[RingB] ) {
                quad[qcnt++] = PtCntB + RevB;
                quad[qcnt++] = PtCntB + RevA;
            }
            else
                quad[qcnt++] = PtCntB;

            if( qcnt >= 3 ) {
                if( colorin >= 0 ) writeQuad(quad, qcnt, colorin, false);
                if( colorout >=0 ) writeQuad(quad, qcnt, colorout, true);
            }
        }
    }
}

main(argc, argv)
    int argc;
    char *argv[];
{
    getCliArgs(argc, argv);
    infile = fopen(namein, "r");
    if(!infile) {
        fprintf(stderr,"can not read \"%s\"\n", namein);
        exit(-1);
    }

    readHeader();
    readPass1();
    fclose(infile);

    outfile = fopen(nameout, "w");
    if( !outfile) {
        fprintf(stderr, "can not write to \"%s\"\n", nameout);
        exit(-1);
    }

    infile = fopen(namein, "r");
    readHeader();
    writePoints();
    fclose(infile);
    writePolys();
    fclose(outfile);
}
