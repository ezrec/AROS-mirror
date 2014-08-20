/// System includes
#include <proto/muimaster.h>
#include <libraries/mui.h>
#include <workbench/workbench.h>
#include <proto/exec.h>
#include <intuition/intuition.h>
#include <exec/libraries.h>
#include <proto/intuition.h>
#include <stdio.h>
#include <string.h>

#include <proto/alib.h>
#include <proto/utility.h>
////

#include <private/vapor/vapor.h>
#include "util.h"
#include "application.h"
#include "pagenumberclass.h"
#include "system/functions.h"

#define TYPE_END    0
#define TYPE_NUMBER 1
#define TYPE_RANGE  2
 

typedef struct
{
 char type;
  int min;
  int max;
  int page_number;
}
PageNumberItem;


struct Data
{
	Object *str_pages;
	char reverse;
	int end;
	int max;
	int min;
	int pos;
	int sub_pos;
//	int skip_mode;
	PageNumberItem pages_bitmap[1024];
};

#define D(x)

DEFNEW
{
	Object *str_pages;
	char initial_range[128];

	if ((obj = DoSuperNew(cl, obj,
		
		Child, str_pages = StringObject, 
			 MUIA_String_Accept, "0123456789-, ",
			MUIA_Frame, MUIV_Frame_String,
		End,
	TAG_MORE, INITTAGS)))
	{
		GETDATA;
		
		data->min = GetTagData(MUIA_VPDFNumberGeneratorPageStart, 1, INITTAGS);
		data->max = GetTagData(MUIA_VPDFNumberGeneratorPageEnd, 999999999, INITTAGS); 
		data->reverse = FALSE;
		data->str_pages = str_pages;
		
		sprintf(initial_range, "1 - %d", data->end);
		set(data->str_pages, MUIA_String_Contents, initial_range);
		
		memset(data->pages_bitmap, 0, sizeof(data->pages_bitmap));
		data->pos = 0;
		data->sub_pos = -1;  // position within range
	}
	
	return (ULONG)obj;
}


DEFDISP
{
	GETDATA;
		
	return DOSUPER;
}

DEFMMETHOD(VPDFNumberGeneratorFirst)
{
	GETDATA;
	char *str;
	int total_pages = 0;	
	char *break_pos, *pch;
	int current_pos = 0;
	char buffer[1024];
	data->pos = 0;
	data->sub_pos = -1;  // position within range
		
	get(data->str_pages, MUIA_String_Contents, &str);
	strcpy(buffer, str);
	
	pch = strtok (buffer, ",");
	
	while ((pch != NULL))
	{
		// checking if we have range here
		if ((break_pos = (strchr(pch, '-'))))
		{

			data->pages_bitmap[current_pos].type = TYPE_RANGE; 
			data->pages_bitmap[current_pos].max  = atoi(break_pos+1); 
			data->pages_bitmap[current_pos].min  = atoi(pch);
			
			if (data->pages_bitmap[current_pos].max<data->pages_bitmap[current_pos].min)
			{
				int tmp;
				tmp = data->pages_bitmap[current_pos].min;
				data->pages_bitmap[current_pos].min = data->pages_bitmap[current_pos].max;
				data->pages_bitmap[current_pos].max = tmp;
				
			}
			
			if (data->pages_bitmap[current_pos].min==data->pages_bitmap[current_pos].max)
			{
				data->pages_bitmap[current_pos].type = TYPE_NUMBER; 
				data->pages_bitmap[current_pos].page_number = data->pages_bitmap[current_pos].max;
				
				if (data->pages_bitmap[current_pos].page_number>data->max)
				{
					data->pages_bitmap[current_pos].page_number = data->max;
				}
				total_pages++;
			}
			else 
			{
				if (data->pages_bitmap[current_pos].max>data->max)
				{
					data->pages_bitmap[current_pos].max = data->max;
				}
				total_pages += data->pages_bitmap[current_pos].max-data->pages_bitmap[current_pos].min+1;
			}
		
			D(kprintf("range [%d, %d]\n", data->pages_bitmap[current_pos].min, data->pages_bitmap[current_pos].max)); 
		}
		else // one page
		{
			data->pages_bitmap[current_pos].page_number = atoi(pch);
			data->pages_bitmap[current_pos].type = TYPE_NUMBER; 
			
			if (data->pages_bitmap[current_pos].page_number>data->max)
			{
				data->pages_bitmap[current_pos].page_number = data->max;
			}
			total_pages++;
			D(kprintf("number [%d]\n", data->pages_bitmap[current_pos].page_number)); 
		}
		current_pos++;
		
		pch = strtok (NULL, ",");
	}
	
	data->pages_bitmap[current_pos].type = TYPE_END;
	
	// if we have reverse mode turned on: start from end
	if (msg->reverse == TRUE)
	{
		data->pos = current_pos-1;
		data->reverse = TRUE;
	}
	else
	{
		data->reverse = FALSE;
	}
	
	
	return total_pages; 
}


DEFMMETHOD(VPDFNumberGeneratorNext)
{
	GETDATA;

	if(!data->reverse)
	{
		switch(data->pages_bitmap[data->pos].type)
		{
		case TYPE_NUMBER:
			data->pos++;
			return data->pages_bitmap[data->pos - 1].page_number;
			break;
		case TYPE_RANGE:
			if(data->sub_pos == -1) // start of range
			{
				data->sub_pos = data->pages_bitmap[data->pos].min;
				return data->sub_pos;
			}
			else
			{
				data->sub_pos++;

				if(data->sub_pos == data->pages_bitmap[data->pos].max)
				{
					data->pos++;
					data->sub_pos = -1;
					return data->pages_bitmap[data->pos - 1].max;
				}
				return data->sub_pos;
			}
		break;
		default:
			return -1;  // end of page list
		break;
		}
	}
	else // reverse mode
	{
		if (data->pos>=0)
		{
			switch(data->pages_bitmap[data->pos].type)
			{
				case TYPE_NUMBER:
					data->pos--;
					return data->pages_bitmap[data->pos + 1].page_number;
				break;
				case TYPE_RANGE:
					if(data->sub_pos == -1)  // start of range
					{
						data->sub_pos = data->pages_bitmap[data->pos].max;
						return data->sub_pos;
					}
					else
					{
						data->sub_pos--;

						if(data->sub_pos == data->pages_bitmap[data->pos].min)
						{
							data->pos--;
							data->sub_pos = -1;
							return data->pages_bitmap[data->pos + 1].min;
						}
						return data->sub_pos;
					}
				break;
			}
		}
		else
			return -1;
	}
	return -1;
}


DEFSET
{
	char initial_range[128];
	GETDATA;

	FORTAG(INITTAGS)
	{
		/*case MUIA_VPDFNumberGeneratorPageSkip:
		{
			data->skip_mode = tag->ti_Data;
			break;
		}
		*/
		
		case MUIA_VPDFNumberGeneratorPageStart: 
			data->min = tag->ti_Data;
			sprintf(initial_range, "%d - %d", data->min, data->end);
			set(data->str_pages, MUIA_String_Contents, initial_range);
		break; 
		
		case MUIA_VPDFNumberGeneratorPageEnd:
			data->end = tag->ti_Data;
			sprintf(initial_range, "%d - %d", data->min, data->end);
			set(data->str_pages, MUIA_String_Contents, initial_range);
		break;
		
		case MUIA_VPDFNumberGeneratorPageMax:
			data->max = tag->ti_Data;
		break;
	}
	
	NEXTTAG

	return DOSUPER;
}



BEGINMTABLE
	DECNEW
	DECDISP
	DECSET
	DECMMETHOD(VPDFNumberGeneratorFirst)
	DECMMETHOD(VPDFNumberGeneratorNext)
ENDMTABLE

DECSUBCLASS_NC(MUIC_Group, VPDFNumberGeneratorClass)