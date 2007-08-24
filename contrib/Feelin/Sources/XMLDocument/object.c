#include "Private.h"

/*** Private ***************************************************************/

///xml_display_markups
void xml_display_markups(FList *list,uint32 level)
{
	FXMLMarkup *markup;
	
	for (markup = (FXMLMarkup *)(list -> Head) ; markup ; markup = markup -> Next)
	{
		FXMLAttribute *attribute;
 
		uint32 i;
 
		for (i = 0 ; i < level ; i++)
		{
			FPrintf(FeelinBase -> Console,"   ");
		}
		FPrintf(FeelinBase -> Console,"<%s",markup -> Name -> Key);

		for (attribute = (FXMLAttribute *)(markup -> AttributesList.Head) ; attribute ; attribute = attribute -> Next)
		{
			FPrintf(FeelinBase -> Console," %s='%s'",attribute -> Name -> Key,attribute -> Data);
		}

		FPrintf(FeelinBase -> Console,">\n");
		
		xml_display_markups(&markup -> ChildrenList,level + 1);
		
		for (i = 0 ; i < level ; i++)
		{
			FPrintf(FeelinBase -> Console,"   ");
		}
		FPrintf(FeelinBase -> Console,"</%s>\n",markup -> Name -> Key);
	}
}
//+

///f_xml_string_copy

/* Cette fonction clone une chaîne de  caractères.  La  fonction  prend  en
compte  les  caractères  spéciaux  définis entre (&) et (;) e.g. &amp; sera
transformé en (&). ou &lt et  &gt  en  (<)  et  (>),  qu'il  est  conseillé
d'utiliser.  Si  la fonction tombe sur une définition inconnue e.g. &value;
ou &true; elle est laisée telle quelle. */

typedef struct FeelinXMLReplace
{
	STRPTR                           Name;
	UBYTE                            Length;
	UBYTE                            Char;
}                                                           FXMLReplace;

static FXMLReplace sign_replace_table[]=
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

STRPTR f_xml_string_copy(struct LocalObjectData *LOD,STRPTR Source,STRPTR Destination,STRPTR *Result)
{
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
					FXMLReplace *replace;
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
///f_xml_parse_attributes

/* Cette fonction recherche la fin du marqueur. Elle prend  une  chaîne  de
caractères en argument, le début de la liste des attributs. e.g. "Size='12'
Pouic='true'>". Les caractères '/' et '?' sont  pris  en  compte  avant  la
balise  de  fermeture  '>'.  La  fonction  s'assure  de  la bonne forme des
attributs et des arguments. Elle prend en compte les guillement simples (')
et doubles (").

Si tout c'est bien passé, la fonction retourne l'endroit, dans la chaine de
caractère, où elle s'est arrétée e.g. ...> ou ...?> ou .../> */

STRPTR f_xml_parse_attributes(FClass *Class,FObject Obj,STRPTR Source,FList *AttributesList)
{
	struct LocalObjectData *LOD = F_LOD(Class,Obj);

	uint32 er=0;

	for (;;)
	{
		FXMLAttribute *attr;
		STRPTR item_s;
		uint8 quote;

		F_SKIPWHITE(Source); if (!*Source) F_ERROR(NULL);

//      F_Log(0,"xml_parse_attributes() [%16.16s](%02lx)",Source,*Source);

		/* should be attribute name or end */

		if (*Source == '?' || *Source == '/' )
		{
			if (Source[1] != '>')
			{
				F_ERROR(CLOSE);
			}

//         F_Log(0,"ATTR_END [%16.16s]",Source);

			return Source;
		}
		else if (*Source == '>')
		{
//         F_Log(0,"ATTR_END [%16.16s]",Source);

			return Source;
		}

/*** allocate attribute structure ******************************************/

		attr = F_NewP(LOD -> Pool,sizeof (FXMLAttribute));

		if (!attr)
		{
			F_ERROR(MEMORY);
		}

		F_LinkTail(AttributesList,(FNode *) attr);

/*** obtain name ***********************************************************/

		item_s = Source; // start of attribute's name

		while (*Source && (*Source != ' ' && *Source != '\t' && *Source != '\n' && *Source != '='))
		{
			Source++;
		}

		if (!*Source)
		{
			F_ERROR(NULL);
		}

		attr -> Line = LOD -> line;
		attr -> Name = (FDOCName *) F_Do(Obj,F_IDR(FM_Document_ObtainName),item_s,Source - item_s);
		
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
			F_ERROR(QUOTE)
		}

		item_s = ++Source;

		while (*Source && *Source != quote) Source++;

		if (!*Source)
		{
			F_ERROR(NULL);
		}

/*** copy attribute's data *************************************************/

		Source = f_xml_string_copy(LOD,item_s,Source,&attr -> Data);

		if (!Source)
		{
			F_ERROR(MEMORY);
		}

		if (*Source != quote)
		{
			F_ERROR(QUOTE);
		}

		Source++;
	}

/*** error handling ********************************************************/

__error:

	{
		STRPTR str = NULL;

		switch (er)
		{
			case FV_XML_ERROR_NULL:    str = "Unexpected end of data"; break;
			case FV_XML_ERROR_CLOSE:   str = "Expected closed barket"; break;
			case FV_XML_ERROR_EQUAL:   str = "Expected equal sign"; break;
			case FV_XML_ERROR_QUOTE:   str = "Missing quote"; break;
			case FV_XML_ERROR_MEMORY:  str = "Not enough memory"; break;
		}

		if (str)
		{
			F_Do(Obj,F_IDR(FM_Document_Log),LOD -> line,Source,str);
		}
	}

	return NULL;
}
//+
///f_xml_parse_markup

/* Markups created are added to MarkupsList, this way children  are  easily
added */

STRPTR f_xml_parse_markup(FClass *Class,FObject Obj,STRPTR Source,FList *MarkupsList)
{
	struct LocalObjectData *LOD = F_LOD(Class,Obj);
	FXMLMarkup *markup=NULL;
	uint32 er;
	STRPTR item_s;

/*** skip possible leading spaces ******************************************/

	F_SKIPWHITE(Source);

//   F_Log(0,"f_xml_parse() (%16.16s)",Source);

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

/*** check '?' *************************************************************/

	if (*Source == '?')
	{
//      F_Log(0,"found '?' (%32.32s)",Source);

		Source++;

		if (Source[0] == 'x' && Source[1] == 'm' && Source[2] == 'l' )
		{
			Source = f_xml_parse_attributes(Class,Obj,Source + 3,&LOD -> AttributesList);

			if (!Source)
			{
				F_ERROR(NULL); // an error message has already been logged
			}
		}
		else
		{
			while (*Source && (*Source != '?' && *Source != '>' && *Source != '<')) Source++;
		}
		
		if (*Source != '?')
		{
			F_ERROR(INTEROGATION);
		}

		Source++;

		if (*Source != '>')
		{
			F_ERROR(CLOSE);
		}

		Source++;
		
		return Source;
	}

/*** allocate Markup *******************************************************/

//   F_Log(0,"allocate markup (%16.16s)",Source);
 
	markup = F_NewP(LOD -> Pool,sizeof (FXMLMarkup));

	if (!markup)
	{
		F_ERROR(MEMORY);
	}

	F_LinkTail(MarkupsList,(FNode *) markup);

/*** obtaining Markup name *************************************************/

	item_s = Source;

	while (*Source && (*Source != ' ' && *Source != '\t' && *Source != '\n' && *Source != '>' && *Source != '/')) Source++;

	if (!*Source) F_ERROR(NULL);

//   F_Log(0,"markup name (%16.16s)(%16.16s)",item_s,Source);
 
	markup -> Line = LOD -> line;
	markup -> Name = (FDOCName *) F_Do(Obj,F_IDR(FM_Document_ObtainName),item_s,Source - item_s);
	
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
 
	if (*Source != '>' && *Source != '/')
	{
		//  F_Log(0,"should parse arguments (%16.16s)",Source);

		Source = f_xml_parse_attributes(Class,Obj,Source,&markup -> AttributesList);

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

		/* Le marqueur est fermé. Il y a peut être  des  données  à  lire  ou
		encore  des  enfants  à  ajouter,  ou  bien  un  marqueur de fin e.g.
		<Button></Button>.  D'abord  on  va  s'occuper  des  données  et  des
		enfants,  ensuite,  on  aura plus qu'a regarder si le terminateur est
		bien là. */

		Source++;

		F_SKIPWHITE(Source);

		if (*Source != '<')
		{

			/* Des données sont définies. Sinon il y aurait  un  '<'  pour  un
			enfant, ou le marqueur de fin */

			item_s = Source;

			while (*Source && *Source != '<') Source++;

			if (!*Source) F_ERROR(NULL);

			Source = f_xml_string_copy(LOD,item_s,Source,&markup -> Body);

			if (!Source)
			{
				F_ERROR(MEMORY);
			}
		}

		if (*Source != '<') F_ERROR(OPEN);

		/* Vérification du type de marqueur. Soit des enfants <Name...>, soit
		le marqueur de fin </...> */

		if (Source[1] != '/')
		{

			/* Définition d'enfants */

			while (*Source && (*Source == '<' && Source[1] != '/'))
			{
//            F_Log(0,"ADDING [%16.16s] Markup (%s)",Source,Markup -> Name);

				Source = f_xml_parse_markup(Class,Obj,Source,&markup -> ChildrenList);

				if (!Source) F_ERROR(NONE); // an error has already been logged

				F_SKIPWHITE(Source);

//            F_Log(0,"RETURN [%16.16s] Markup (%s)",Source,Markup -> Name);
			}

			if (!*Source) F_ERROR(NULL);
		}

		/* Si il y avait des enfants ils ont tous été  ajouté.  Mainteant  la
		Source doit être sur le marqueur de fin </...> */

		if (Source[1] == '/')
		{
			/* Marqueur de fin. On vérifie quand même qu'il s'agit du bon. */

			Source += 2;

			if (F_StrCmp(markup -> Name -> Key,Source,markup -> Name -> KeyLength) != 0)
			{
				F_ERROR(NAME);
			}

//         F_Log(0,"END OF (%s) NEXT [%16.16s]",Markup -> Name,Source);

			if (Source[markup -> Name -> KeyLength] != '>')
			{
				F_ERROR(CLOSE);
			}

			return Source + markup -> Name -> KeyLength + 1;
		}
		else F_ERROR(SYNTAX);
	}
	else F_ERROR(CLOSE);

/*** error logging *********************************************************/

__error:

	{
		STRPTR str = NULL;

		switch (er)
		{
			case FV_XML_ERROR_NULL:          str = "Unexpected end of data"; break;
			case FV_XML_ERROR_MEMORY:        str = "Unable to allocate Markup"; break;
			case FV_XML_ERROR_SYNTAX:        str = "Syntax Error"; break;
			case FV_XML_ERROR_OPEN:          str = "Expected opened barket"; break;
			case FV_XML_ERROR_CLOSE:         str = "Expected closed barket"; break;
			case FV_XML_ERROR_ATTRIBUTE:     str = "Expected attribute"; break;
			case FV_XML_ERROR_INTEROGATION:  str = "Expected '?'"; break;
			case FV_XML_ERROR_NAME:
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

/*** Methods ***************************************************************/

///XMLDocument_Get
F_METHOD(void,XMLDocument_Get)
{
	struct LocalObjectData *LOD = F_LOD(Class,Obj);
	struct TagItem *Tags = Msg,item;

	while  (F_DynamicNTI(&Tags,&item,Class))
	switch (item.ti_Tag)
	{
		case FA_XMLDocument_Attributes:  F_STORE(LOD -> AttributesList.Head); break;
		case FA_XMLDocument_Markups:     F_STORE(LOD -> MarkupsList.Head); break;
	}

	F_SUPERDO();
}
//+
///XMLDocument_Parse
F_METHODM(int32,XMLDocument_Parse,FS_Document_Parse)
{
	struct LocalObjectData *LOD = F_LOD(Class,Obj);
	STRPTR Source = Msg -> Source;

	LOD -> line = 1;
	LOD -> Pool = Msg -> Pool;

	if (Source)
	{
		for (;;)
		{
			Source = f_xml_parse_markup(Class,Obj,Source,&LOD -> MarkupsList);

			if (Source)
			{
				F_SKIPWHITE(Source); if (!*Source) break;
			}
			else break;
		}
	
		if (Source)
		{
			STATIC FDOCID xml_document_ids[] =
			{
				{ "version",  7, FV_XMLDOCUMENT_ID_VERSION },

				F_ARRAY_END
			};
			
			FXMLAttribute *attribute;

			F_Do(Obj,F_IDR(FM_Document_AddIDs),xml_document_ids);

/*** store attributes ******************************************************/

			for (attribute = (FXMLAttribute *)(LOD -> AttributesList.Head) ; attribute ; attribute = attribute -> Next)
			{
				if (attribute -> Name -> ID == FV_XMLDOCUMENT_ID_VERSION)
				{
					int32 val;
					uint32 len = stcd_l(attribute -> Data,&val);

					if (len)
					{
						F_Set(Obj,F_IDR(FA_Document_Version),val);

						if (attribute -> Data[len] == '.')
						{
							len = stcd_l(attribute -> Data + len + 1,&val);

							if (len)
							{
								F_Set(Obj,F_IDR(FA_Document_Revision),val);
							}
						}
					}
				}
			}
		
//         xml_display_markups(&LOD -> MarkupsList,0);
			
			return TRUE;
		}
/*
		else
		{
			F_Log(0,"XML source is not valid (0x%08lx) >> DUMPING !",Msg -> Source);
			FPrintf(FeelinBase -> Console,Msg -> Source);
		}
*/
	}
	return FALSE;
}
//+
///XMLDocument_Clear
F_METHOD(void,XMLDocument_Clear)
{
	struct LocalObjectData *LOD = F_LOD(Class,Obj);
	
	LOD -> AttributesList.Head = NULL;
	LOD -> AttributesList.Tail = NULL;
	
	LOD -> MarkupsList.Head = NULL;
	LOD -> MarkupsList.Tail = NULL;
	
	LOD -> Pool = NULL;
	LOD -> line = 0;

	F_SUPERDO();
}
//+

