#include "Private.h"

/*** Private ***************************************************************/
/*
///f_html_display_markups
void f_html_display_markups(FList *list,ULONG level)
{
	FHTMLMarkup *markup;
	
	for (markup = (FHTMLMarkup *)(list -> Head) ; markup ; markup = (FHTMLMarkup *) markup -> Node.Next)
	{
		if (markup -> Node.Type == FV_HTMLDocument_Node_Markup)
		{
			FHTMLAttribute *attribute;
	 
			ULONG i;
	 
			for (i = 0 ; i < level ; i++)
			{
				FPrintf(FeelinBase -> Console,"   ");
			}
			
			if (markup -> Terminator)
			{
				FPrintf(FeelinBase -> Console,"</%s",markup -> Name -> Key);
			}
			else
			{
				FPrintf(FeelinBase -> Console,"<%s",markup -> Name -> Key);
			}

			for (attribute = (FHTMLAttribute *)(markup -> AttributesList.Head) ; attribute ; attribute = attribute -> Next)
			{
				FPrintf(FeelinBase -> Console," %s='%s'",attribute -> Name -> Key,attribute -> Data);
			}

			FPrintf(FeelinBase -> Console,">\n");
			
			f_html_display_markups(&markup -> ChildrenList,level + 1);
		}
	}
}
//+
*/
///f_html_string_copy

/* Cette fonction clone une chaîne de  caractères.  La  fonction  prend  en
compte  les  caractères  spéciaux  définis entre (&) et (;) e.g. &amp; sera
transformé en (&). ou &lt et  &gt  en  (<)  et  (>),  qu'il  est  conseillé
d'utiliser.  Si  la fonction tombe sur une définition inconnue e.g. &value;
ou &true; elle est laisée telle quelle. */

typedef struct FeelinHTMLReplace
{
	STRPTR                           Name;
	UBYTE                            Length;
	UBYTE                            Char;
}
FHTMLReplace;

STATIC FHTMLReplace sign_replace_table[]=
{
	{ "amp",   3, '&'          },
	{ "apos",  4, QUOTE_SINGLE },
	{ "gt",    2, '>'          },
	{ "lt",    2, '<'          },
	{ "quote", 5, QUOTE_DOUBLE },
	{ "tab",   3, '\t'         },
	{ "nl",    2, '\n'         },

	F_ARRAY_END
};

STRPTR f_html_string_copy(FClass *Class,FObject Obj,STRPTR Source,STRPTR Destination,STRPTR *Result)
{
	struct LocalObjectData *LOD = F_LOD(Class,Obj);

	if (Source)
	{
		STRPTR copy = F_NewP(LOD -> Pool,Destination - Source + 1);

		if (copy)
		{
			*Result = copy;

			while (Source < Destination)
			{
				if (*Source == '&' && Source[1] != ' ')
				{
					FHTMLReplace *replace;
					STRPTR e = ++Source;

					while (*e != ';' && *e != ' ')
					{
						if (*e == '\n') LOD -> line++;

						e++;
					}

//               F_Log(0,"HOLDER: (%16.16s) END (%16.16s) LEN %ld",Source,e,e - Source);

					for (replace = sign_replace_table ; replace -> Name ; replace++)
					{
						if (replace -> Length == e - Source)
						{
							if (F_StrCmp(replace -> Name,Source,replace -> Length) == 0) break;
						}
					}

					if (replace -> Name)
					{
						*copy++ = replace -> Char;
						Source += replace -> Length + 1;

//                  F_Log(0,"REPLACE by (%lc) Source (%16.16s)",replace -> Char,Source);
					}
					else
					{
						*copy++ = Source[-1];
					}
				}
				else
				{
					if (*Source == '\n') LOD -> line++;

					*copy++ = *Source++;
				}
			}

			return Source;
		}
	}
	return NULL;
}
//+
///f_html_parse_attributes

/* Cette fonction recherche la fin du marqueur. Elle prend  une  chaîne  de
caractères en argument, le début de la liste des attributs. e.g. "Size='12'
Pouic='true'>". Les caractères '/' et '?' sont  pris  en  compte  avant  la
balise  de  fermeture  '>'.  La  fonction  s'assure  de  la bonne forme des
attributs et des arguments. Elle prend en compte les guillement simples (')
et doubles (").

Si tout c'est bien passé, la fonction retourne l'endroit, dans la chaine de
caractère, où elle s'est arrétée e.g. ...> ou ...?> ou .../> */

STRPTR f_html_parse_attributes(FClass *Class,FObject Obj,STRPTR Source,FList *AttributesList)
{
	struct LocalObjectData *LOD = F_LOD(Class,Obj);

	uint32 er=0;

	for (;;)
	{
		FHTMLAttribute *attr;
		STRPTR item;
		UBYTE quote;

		F_SKIPWHITE(Source); if (!*Source) F_ERROR(NULL);

//      F_Log(0,"xml_parse_attributes() [%-16.16s](%02lx)",Source,*Source);

		/* should be attribute name or end */

		if (*Source == '>')
		{
//         F_Log(0,"ATTR_END [%-16.16s]",Source);

			return Source;
		}
		else if (*Source == '/')
		{
			if (Source[1] != '>')
			{
				F_ERROR(CLOSE);
			}

//         F_Log(0,"ATTR_END [%-16.16s]",Source);

			return Source;
		}

/*** allocate attribute structure ******************************************/

		attr = F_NewP(LOD -> Pool,sizeof (FHTMLAttribute));

		if (!attr)
		{
			F_ERROR(MEMORY);
		}

		F_LinkTail(AttributesList,(FNode *) attr);

/*** obtain name ***********************************************************/

		item = Source; // start of attribute's name
		
//      F_Log(0,"START OF ATTRIBUTES NAME >> (%-16.16s)",item);
 
		while (*Source && (*Source != ' ' && *Source != '\t' && *Source != '\n' && *Source != '='))
		{
			Source++;
		}

		if (!*Source)
		{
			F_ERROR(NULL);
		}

		attr -> Line = LOD -> line;
		attr -> Name = (FDOCName *) F_Do(Obj,F_IDR(FM_Document_ObtainName),item,Source - item);

		if (!attr -> Name)
		{
			F_ERROR(MEMORY);
		}

/*** check equal sign ******************************************************/

		F_SKIPWHITE(Source);

		if (*Source != '=')
		{
			F_ERROR(EQUAL);
		}

		Source++;

		F_SKIPWHITE(Source);

/*** check quotes, find end of attribute's data ****************************/

		quote = *Source;

		if (quote != QUOTE_SINGLE && quote != QUOTE_DOUBLE)
		{
			quote = 0;
		}
		else Source++;

		item = Source;
		
		while (*Source && *Source != quote && *Source != '<' && *Source != '>') Source++;

		if (!*Source)
		{
			F_ERROR(NULL);
		}
	
		if (*Source == '<')
		{
			F_ERROR(CLOSE);
		}

/*** copy attribute's data *************************************************/

		Source = f_html_string_copy(Class,Obj,item,Source,&attr -> Data);

		if (!Source)
		{
			F_ERROR(MEMORY);
		}

		if (quote)
		{
			if (*Source != quote)
			{
				F_ERROR(QUOTE);
			}
			Source++;
		}
	}

/*** error handling ********************************************************/

__error:

	{
		STRPTR str = NULL;

		switch (er)
		{
			case FV_PARSE_ERROR_NULL:    str = "Unexpected end of data"; break;
			case FV_PARSE_ERROR_CLOSE:   str = "Expected closed bracket"; break;
			case FV_PARSE_ERROR_EQUAL:   str = "Expected equal sign"; break;
			case FV_PARSE_ERROR_QUOTE:   str = "Missing quote"; break;
			case FV_PARSE_ERROR_MEMORY:  str = "Not enough memory"; break;
		}

		if (str)
		{
			F_Do(Obj,F_IDR(FM_Document_Log),LOD -> line,Source,str);
		}
	}

	return NULL;
}
//+
///f_html_parse_markup

/* Markups created are added to MarkupsList, this way children  are  easily
added */

STRPTR f_html_parse_markup(FClass *Class,FObject Obj,STRPTR Source,FList *MarkupsList)
{
	struct LocalObjectData *LOD = F_LOD(Class,Obj);
	FHTMLMarkup *markup=NULL;
	uint32 er;
	STRPTR item;

	if (*Source != '<')
	{
		F_ERROR(OPEN);
	}

	if (!*++Source) F_ERROR(NULL);

/*** skip comments *********************************************************/

	if (*Source == '!' && Source[1] == '-' && Source[2] == '-')
	{
		Source += 3;

//      F_Log(0,"COMMENT_BEGIN [%16.16s]",Source);

		while (*Source)
		{
			if (*Source == '-' && Source[1] == '-' && Source[2] == '>') break;

			Source++;
		}

		if (!*Source)
		{
			F_ERROR(NULL);
		}

		Source += 3;

//      F_Log(0,"COMMENT_END [%16.16s]",Source);

		return Source;
	}

/*** allocate Markup *******************************************************/

//   F_Log(0,"allocate markup (%16.16s)",Source);
 
	markup = F_NewP(LOD -> Pool,sizeof (FHTMLMarkup));

	if (!markup)
	{
		F_ERROR(MEMORY);
	}

	if (*Source == '/')
	{
		markup -> Terminator = TRUE; Source++;
//      F_Log(0,"TERMINATOR [%-16.16s]",Source);
	}
	
	markup -> Node.Type = FV_HTMLDocument_Node_Markup;

	F_LinkTail(MarkupsList,(FNode *) markup);

/*** obtaining Markup name *************************************************/

	item = Source;

	while (*Source && (*Source != ' ' && *Source != '\t' && *Source != '\n' && *Source != '>' && *Source != '/' && *Source != '=')) Source++;

	if (!*Source) F_ERROR(NULL);

	markup -> Line = LOD -> line;
	markup -> Name = (FDOCName *) F_Do(Obj,F_IDR(FM_Document_ObtainName),item,Source - item);

//   F_Log(0,"markup name (%16.16s)(%16.16s)",item_s,Source);
	
	if (!markup -> Name)
	{
		F_ERROR(MEMORY);
	}
/*
	F_Log(0,"Markup (%s) NEXT (%16.16s)",markup -> Name,Source);
	F_Log(0,"Markup (%s)",markup -> Name);
*/
/*** read Markup attributes ************************************************/

	/* if Markup is not closed, there are attributes to read */

	F_SKIPWHITE(Source);

	if (*Source == '<')
	{
		F_ERROR(ATTRIBUTE);
	}

	/* self attribute e.g. <align=center> */

	if (*Source == '=')
	{
		Source = item;
	}
 
	if (*Source != '>' && *Source != '/')
	{
		Source = f_html_parse_attributes(Class,Obj,Source,&markup -> AttributesList);

		if (!Source) F_ERROR(NONE); // an error message have already been logged

//      F_Log(0,"ATTR_NEXT [%16.16s] - Markup (%s)",Source,Markup -> Name);
	}

/*** closing Markup ********************************************************/

//   F_Log(0,"CLOSE: [%16.16s] - Markup '%s'",Source,Markup -> Name);

	/* Si il y avait des attributs ils  ont  été  lus.  On  vérifie  que  le
	marqueur  se  termine  de  façon correcte. D'abord, on va vérifier si le
	marqueur ne termine tout seul '/>' */

	if (*Source == '/')
	{
		if (*++Source != '>') F_ERROR(CLOSE);

		/* si le marqueur se termine tout seul, il n'y a plus rien à lire. On
		retourne donc la position actuelle de la source */

		return Source + 1;
	}
	else if (*Source == '>')
	{
		return Source + 1;
	}
	else F_ERROR(CLOSE);

/*** error logging *********************************************************/

__error:

	{
		STRPTR str = NULL;

		switch (er)
		{
			case FV_PARSE_ERROR_NULL:          str = "Unexpected end of data"; break;
			case FV_PARSE_ERROR_MEMORY:        str = "Unable to allocate Markup"; break;
			case FV_PARSE_ERROR_SYNTAX:        str = "Syntax Error"; break;
			case FV_PARSE_ERROR_OPEN:          str = "Expected opened bracket"; break;
			case FV_PARSE_ERROR_CLOSE:         str = "Expected closed bracket"; break;
			case FV_PARSE_ERROR_ATTRIBUTE:     str = "Expected attribute"; break;
			case FV_PARSE_ERROR_NAME:
			{
				F_Do(Obj,F_IDR(FM_Document_Log),LOD -> line,Source,"Expected (%s)",markup -> Name -> Key);
			}
			break;
		}

		if (str)
		{
			F_Do(Obj,F_IDR(FM_Document_Log),LOD -> line,Source,str);
		}
	}

	return NULL;
}
//+
///f_html_handle_text
STATIC STRPTR f_html_handle_text(FClass *Class,FObject Obj,STRPTR Source,FList *NodesList,FHTMLText **Result)
{
	struct LocalObjectData *LOD = F_LOD(Class,Obj);
	STRPTR item = Source;
	
	while (*Source && *Source != '<') Source++;

	if (Source != item)
	{
		FHTMLText *text = F_NewP(LOD -> Pool,sizeof (FHTMLText));
		
		if (text)
		{
			text -> Node.Type = FV_HTMLDocument_Node_Text;
			text -> DataLength = Source - item;
				
			f_html_string_copy(Class,Obj,item,Source,&text -> Data);
			
			if (!text -> Data)
			{
				F_Dispose(text); text = NULL;
			}
			else
			{
				F_LinkTail(NodesList,(FNode *) text);
			}
		
			if (Result)
			{
				*Result = text;
			}
		}
	}
	return Source;
}

//+

/*** Methods ***************************************************************/

///HTMLDocument_Get
F_METHOD(void,HTMLDocument_Get)
{
	struct LocalObjectData *LOD = F_LOD(Class,Obj);
	struct TagItem *Tags = Msg,item;

	while  (F_DynamicNTI(&Tags,&item,Class))
	switch (item.ti_Tag)
	{
		case FA_HTMLDocument_Attributes: F_STORE(LOD -> AttributesList.Head); break;
		case FA_HTMLDocument_Nodes:      F_STORE(LOD -> NodesList.Head); break;
	}

	F_SUPERDO();
}
//+
///HTMLDocument_Parse
F_METHODM(LONG,HTMLDocument_Parse,FS_Document_Parse)
{
	struct LocalObjectData *LOD = F_LOD(Class,Obj);
	STRPTR Source = Msg -> Source;

	LOD -> line = 1;
	LOD -> Pool = Msg -> Pool;

	while (Source && *Source)
	{
		while (*Source == '<')
		{
			Source = f_html_parse_markup(Class,Obj,Source,&LOD -> NodesList);
		}

		/* A ce point, il n'y a plus de tag. Nous  sommes  soit  au  début
		d'un morceau de texte, soit à la fin de la ligne, soit à la fin du
		texte.

		ATTENTION: dans la portion de  texte  brut  peut  se  trouver  une
		définition      de      raccourcis      '_'.     Si     l'attribut
		FA_TextDisplay_Shortcut est TRUE (FF_TextDisplay_Shortcut  présent
		dans  les  flags) il faut vérifier la présence d'une définition de
		raccourcis (si un raccourcis est trouvé il faut remplir  le  champ
		'shortcut'  du contexte, ainsi on sait qu'il a été trouvé, et on a
		pas besoin de faire de nouvelle recherche. */

		if (!*Source)
		{
			break;
		}
		else
		{
			/* At this point, there is raw  text.  A  text  chunk  must  be
			created to initiated.

			WARNING: Don't forget that the raw text can contain a  shortcut
			definition */

			Source = f_html_handle_text(Class,Obj,Source,&LOD -> NodesList,NULL);
		}
	}

	if (Source)
	{
//      f_html_display_markups(&LOD -> NodesList,0);
 
		return TRUE;
	}
	else
	{
		F_Log(0,"HTML source is not valid (0x%08lx) >> DUMPING !",Msg -> Source);
		FPrintf(FeelinBase -> Console,Msg -> Source);
	}

	return FALSE;
}
//+
///HTMLDocument_Clear
F_METHOD(void,HTMLDocument_Clear)
{
	struct LocalObjectData *LOD = F_LOD(Class,Obj);

	LOD -> Pool = NULL;
	LOD -> AttributesList.Head = NULL;
	LOD -> AttributesList.Tail = NULL;
	LOD -> NodesList.Head = NULL;
	LOD -> NodesList.Tail = NULL;
	LOD -> line = 0;
	
	F_SUPERDO();
}
//+
