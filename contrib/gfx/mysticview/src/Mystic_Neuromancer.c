/*********************************************************************
----------------------------------------------------------------------

	MysticView
	neuromancer

	artificial intelligence stuff

----------------------------------------------------------------------
*********************************************************************/

#define	TRAIN_THRESHOLD		(0.0002)
#define QUIET	1

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#ifdef MATH_68882
	#include <m68881.h>
#endif

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/utility.h>
#ifndef NO_NEURALNET
#include <proto/neuralnet.h>
#endif
#include <proto/guigfx.h>

#ifndef NO_NEURALNET
#include <libraries/neuralnet.h>
#endif
#include <intuition/intuition.h>
#include <intuition/screens.h>
#include <guigfx/guigfx.h>
#include <libraries/mysticview.h>

#include <clib/macros.h>

#include "Mystic_Global.h"
#include "Mystic_Screen.h"
#include "Mystic_Window.h"
#include "Mystic_Tools.h"
#include "Mystic_InitExit.h"
#include "Mystic_Subtask.h"
#include "MysticView.h"
#include "Mystic_Settings.h"
#include "Mystic_Neuromancer.h"
#include "Mystic_Texts.h"

#ifndef NO_NEURALNET
static NEURALNET net = NULL;
#endif

enum NETINPUT
{
	PICWIDTH=0,
	PICHEIGHT,
	PICWIDTHFACTOR,
	PICHEIGHTFACTOR,
	PICFACTOREQUIVALENCE,
	PICDEPTH,
	DRAGGING_ACTIVE,
	NETINPUT_COUNT
};


enum NETOUTPUT
{
	ASPECTMIN = 0,
	IGNOREASPECT,
	NETOUTPUT_COUNT
};

#ifndef NO_NEURALNET
NEURON inputlayer[NETINPUT_COUNT];
NEURON outputlayer[NETOUTPUT_COUNT];
#endif

void MVNeuro_DeleteNet(void)
{
#ifndef NO_NEURALNET
	if (net)
	{
		NN_Save(net, "envarc:mysticview/neurobrain_displaymode", NULL);
		NN_Save(net, "env:mysticview/neurobrain_displaymode", NULL);
		NN_Delete(net);
	}
	net = NULL;
#endif
}

void MVNeuro_CreateNet(void)
{
#ifndef NO_NEURALNET
	if (NeuralBase)
	{
		MVNeuro_DeleteNet();
		
		if (net = NN_Create(NETINPUT_COUNT, 8, NETOUTPUT_COUNT, NULL))
		{
			int i;
			
			for (i=0; i < NETINPUT_COUNT; ++i)
			{
				inputlayer[i] = 0.0;
			}
	
			for (i=0; i < NETOUTPUT_COUNT; ++i)
			{
				outputlayer[i] = 0.0;
			}
	
			if (Exists("env:mysticview/neurobrain_displaymode"))
			{
				if (!NN_Load(net, "env:mysticview/neurobrain_displaymode", NULL))
				{
				//	printf("*** the format of the neural brainfile changed since the last version. you must train it again. sorry :-(\n");
				}
			}
			else if (Exists("envarc:mysticview/neurobrain_displaymode"))
			{
				if (!NN_Load(net, "envarc:mysticview/neurobrain_displaymode", NULL))
				{
				//	printf("*** the format of the neural brainfile changed since the last version. you must train it again. sorry :-(\n");
				}
			}
		}
	}	
#endif
}


void setinputlayer(struct mvscreen *scr, struct mvwindow *win, struct mainsettings *settings, struct viewdata *viewdata, PICTURE *pic)
{
#ifndef NO_NEURALNET
	if (net && settings && viewdata && pic)
	{
		ULONG w,h,f, ax, ay;
		float viewaspect, picaspect;
		float virtualpicwidth;
		float virtualwinwidth;
		float	p1,p2;

		GetPictureAttrs(pic, PICATTR_Width, &w,
			PICATTR_Height, &h, PICATTR_PixelFormat, &f, 
			PICATTR_AspectX, &ax, PICATTR_AspectY, &ay, TAG_DONE);

		viewaspect = (float) scr->aspectx / (float) scr->aspecty;
		picaspect = (float) ax / (float) ay;
		
		virtualpicwidth = (float) w * picaspect;
		virtualwinwidth = (float) win->innerwidth * viewaspect;

		p1 = virtualpicwidth / virtualwinwidth;
		p2 = (float) h / (float) win->innerheight;

		inputlayer[PICWIDTHFACTOR] = pow(p1, 0.3) - 0.5;
		inputlayer[PICHEIGHTFACTOR] = pow(p2, 0.3) - 0.5;
		inputlayer[PICFACTOREQUIVALENCE] = pow(fabs(p1-p2), 0.5);
		inputlayer[PICWIDTH] = pow(virtualpicwidth, 0.6) / 120;
		inputlayer[PICHEIGHT] = pow((float) h, 0.6) / 120;
		inputlayer[PICDEPTH] = (f == PIXFMT_CHUNKY_CLUT) ? 0.0 : 1.0;
		inputlayer[DRAGGING_ACTIVE] = (settings->leftmouseaction == MOUSEACTION_DRAG) ? 1.0 : 0.0;

	#ifndef QUIET
		printf("p1: %f, p2: %f\n", p1, p2);
		for(i=0; i < NETINPUT_COUNT; ++i)
		{
			printf("%0.3f; ", inputlayer[i]);
		}
		printf("\n");
	#endif


		NN_SetInputLayer(net, inputlayer);
	}
#endif
}


float MVNeuro_Train(struct mvscreen *scr, struct mvwindow *win, struct mainsettings *settings, struct viewdata *viewdata, PICTURE *pic)
{
#ifdef NO_NEURALNET
    return 1.0;
#else
	float error = 0.0;

	if (net && settings && viewdata && pic)
	{
		if (settings->displaymode == MVDISPMODE_KEEPASPECT_MIN || settings->displaymode == MVDISPMODE_IGNOREASPECT)
		{
			setinputlayer(scr, win, settings, viewdata, pic);
	
			if (settings->displaymode == MVDISPMODE_KEEPASPECT_MIN)
			{
				outputlayer[ASPECTMIN] = 1.0;
				outputlayer[IGNOREASPECT] = 0.0;
			}
			else
			{
				outputlayer[ASPECTMIN] = 0.0;
				outputlayer[IGNOREASPECT] = 1.0;
			}
	
			NN_SetOutputLayer(net, outputlayer);
			
			do
			{
				NN_Train(net, &error);
			} while (error > TRAIN_THRESHOLD);
			
		#ifndef QUIET
			printf("training cycles: %ld\n", i);
		#endif
		
		}
	}
	
	return error;
#endif
}

BOOL MVNeuro_RecallDisplayMode(int *displaymode, struct mvscreen *scr, struct mvwindow *win, struct mainsettings *settings, struct viewdata *viewdata, PICTURE *pic)
{
#ifdef NO_NEURALNET
    	return FALSE;
#else
	BOOL success = FALSE;

	if (net && settings && viewdata && pic)
	{
		int bestmode;
		float bestresult;
		
		setinputlayer(scr, win, settings, viewdata, pic);

		NN_Recall(net);
		NN_GetOutputLayer(net, outputlayer);

	#ifndef QUIET
		printf("recall: ");

		for(i=0; i < NETOUTPUT_COUNT; ++i)
		{
			printf("%0.3f; ", outputlayer[i]);
		}
		printf("\n");
	#endif		

		bestmode = MVDISPMODE_IGNOREASPECT;
		bestresult = outputlayer[IGNOREASPECT];

		if (outputlayer[ASPECTMIN] > bestresult)
		{
			bestmode = MVDISPMODE_KEEPASPECT_MIN;
			bestresult = outputlayer[ASPECTMIN];
		}

		if (displaymode) *displaymode = bestmode;
		success = TRUE;

	}

	return success;
#endif
}




