#include "Private.h"
#include "parse.h"
#include "context.h"
#include "create.h"

///td_chunk_text_new
STATIC STRPTR td_chunk_text_new(STRPTR s,FTDContext *Context,FTDChunkText **Save)
{
   FTDChunkText *chunk = F_NewP(CUD -> Pool,sizeof (FTDChunkText));

   if (chunk)
   {
      chunk -> Header.Type = FV_TD_CHUNK_TEXT;

      if (Save)
      {
         *Save = chunk;
      }

      F_LinkTail(&Context -> line -> ChunkList,(FNode *) chunk);

      chunk -> style = Context -> style;
      chunk -> mode  = Context -> mode;

      if (Context -> container_font != NULL)
      {
         chunk -> font = Context -> container_font -> Data;

         if (Context -> container_font -> Stolen == FALSE)
         {
            chunk -> hold |= FF_TD_CHUNK_TEXT_HOLD_FONT;
            Context -> container_font -> Stolen = TRUE;
         }
      }

      if (Context -> container_pens != NULL)
      {
         chunk -> pens = Context -> container_pens -> Data;

         if (Context -> container_pens -> Stolen == FALSE)
         {
            chunk -> hold |= FF_TD_CHUNK_TEXT_HOLD_PENS;
            Context -> container_pens -> Stolen = TRUE;
         }
      }

      chunk -> text = s;
   }

   /* search stop character */

   if (Context -> stop_engine)
   {
      uint8 c;

      while ((c = *s) != 0)
      {
         if (c == '\n')
         {
            break;
         }
         else if (Context -> stop_shortcut == FALSE && c == '_')
         {
            break;
         }
         else if (c == '<')
         {
            s++;

            if (*s == '/')
            {
               s++;

               if (*((uint32 *)(s)) == MAKE_ID('s','t','o','p'))
               {
                  s += 4;

                  if (*s == '>')
                  {
                     Context -> stop_engine = FALSE; s -= 6; break;
                  }
               }
            }
         }
         else
         {
            s++;
         }
      }
   }
   else if (Context -> stop_shortcut)
   {
      uint8 c;

      while ((c = *s) != 0)
      {
         if (c == '<' || c == '\n') break; s++;
      }
   }
   else
   {
      uint8 c;

      while ((c = *s) != 0)
      {
         if (c == '<' || c == '_' || c == '\n') break; s++;
      }
   }

   /* finish chunk text, adding the number of chars */

   if (chunk != NULL)
   {
      chunk -> chars = s - chunk -> text;
   }

   return s;
}

//+
///td_short_check
STATIC STRPTR td_short_check(STRPTR s,FTDContext *context)
{
   FTDChunkText *chunk;

   if (*s == '_')
   {
      s++;

      context -> shortcut = *s;
      context -> stop_shortcut = TRUE;

      s = td_chunk_text_new(s,context,&chunk);

      chunk -> Header.Flags |= FF_TD_CHUNK_TEXT_SHORTCUT;
   }
   return s;
}
//+
///td_parse_tag
STATIC STRPTR td_parse_tag(STRPTR s,FTDContext *Context)
{
   /* '< ' is not considered as a command, but plain text */

   if (s[1] == ' ')
   {
      FTDChunkText *tc;

      s = td_chunk_text_new(s + 1,Context,&tc);

      if (tc)
      {
         tc -> text--;
         tc -> chars++;
      }

      return s;
   }

   s = td_parse_markup(++s,&Context -> result);

   switch (Context -> result.ID)
   {

///FV_TD_ALIGN
      case FV_TD_ALIGN:
      {
         Context -> align = _td_result_attr(FV_TD_RESULT_ALIGN);
         Context -> line -> align = Context -> align;
      }
      break;
//+
///FV_TD_BOLD
      case FV_TD_BOLD_ON:
      {
         Context -> style |= FSF_BOLD;
      }
      break;

      case FV_TD_BOLD_OFF:
      {
         Context -> style &= ~FSF_BOLD;
      }
      break;
//+
///FV_TD_BR
      case FV_TD_BR:
      {
         Context -> line = NULL;
      }
      break;
//+
///FV_TD_FONT
      case FV_TD_FONT_ON:
      {
         FTDContextFont *prev,*font;

         if (Context -> container_font != NULL)
         {
            prev = Context -> container_font -> Data;
         }
         else prev = NULL;

         if ((font = td_context_push(Context,FV_TD_CONTEXT_FONT)) != NULL)
         {
            if (_td_result_type(FV_TD_RESULT_FONT_COLOR))
            {
               font -> color = td_context_add_color(Context,_td_result_attr(FV_TD_RESULT_FONT_COLOR),_td_result_type(FV_TD_RESULT_FONT_COLOR));
/*
               switch (_td_result_type(FV_TD_RESULT_FONT_COLOR))
               {
                  case FV_TD_RESULT_TYPE_DEC:    font -> spec_type = FV_TD_CONTEXT_FONT_TYPE_PEN;  break;
                  case FV_TD_RESULT_TYPE_HEX:    font -> spec_type = FV_TD_CONTEXT_FONT_TYPE_RGB;  break;
                  case FV_TD_RESULT_TYPE_STRING: font -> spec_type = FV_TD_CONTEXT_FONT_TYPE_SPEC; break;
               }

               font -> spec = (STRPTR) _td_result_attr(FV_TD_RESULT_FONT_COLOR);
*/
            }
            else if (prev)
            {
               font -> color = prev -> color;
//               font -> spec = prev -> spec;
//               font -> spec_type = prev -> spec_type;
            }
/*
            else
            {
               font -> spec_type = FV_TD_CONTEXT_FONT_TYPE_NONE;
            }
*/
            if (_td_result_type(FV_TD_RESULT_FONT_FACE))
            {
               font -> face = (STRPTR) _td_result_attr(FV_TD_RESULT_FONT_FACE);
            }
            else if (prev != NULL)
            {
               ULONG len;

               if ((len = F_StrLen(prev -> face)) != 0)
               {
                  if ((font -> face = F_NewP(CUD -> Pool,len + 1)) != NULL)
                  {
                     CopyMem(prev -> face,font -> face,len);
                  }
               }
            }

            if (_td_result_type(FV_TD_RESULT_FONT_SIZE))
            {
               font -> size = _td_result_attr(FV_TD_RESULT_FONT_SIZE);
            }
            else if (prev != NULL)
            {
               font -> size = prev -> size;
            }

            if (!font -> size)
            {
               font -> size = 8;
            }

            switch (_td_result_type(FV_TD_RESULT_FONT_SIZE))
            {
               case FV_TD_RESULT_TYPE_DEC:
               case FV_TD_RESULT_TYPE_HEX:
               case FV_TD_RESULT_TYPE_PIXEL:    font -> setup_size_type = FV_TD_CONTEXT_FONT_SIZE_PIXELS; break;
               case FV_TD_RESULT_TYPE_PERCENT:  font -> setup_size_type = FV_TD_CONTEXT_FONT_SIZE_PERCENT; break;
               default:                         font -> setup_size_type = FV_TD_CONTEXT_FONT_SIZE_SAME; break;
            }
         }
      }
      break;

      case FV_TD_FONT_OFF:
      {
         td_context_pop(Context,FV_TD_CONTEXT_FONT);
      }
      break;
//+
///FV_TD_HR
      case FV_TD_HR:
      {
         FTDHRule *hr;

         if ((hr = F_NewP(CUD -> Pool,sizeof (FTDHRule))) != NULL)
         {
            hr -> align    = _td_result_attr(FV_TD_RESULT_HR_ALIGN);
            hr -> compact  = _td_result_attr(FV_TD_RESULT_HR_COMPACT);
            hr -> noshade  = _td_result_attr(FV_TD_RESULT_HR_NOSHADE);
            hr -> shine    = _td_result_attr(FV_TD_RESULT_HR_SHINE);
            hr -> shadow   = _td_result_attr(FV_TD_RESULT_HR_SHADOW);
            hr -> size     = _td_result_attr(FV_TD_RESULT_HR_SIZE);
            hr -> width    = _td_result_attr(FV_TD_RESULT_HR_WIDTH);

            if (_td_result_type(FV_TD_RESULT_HR_WIDTH) == FV_TD_RESULT_TYPE_PERCENT)
            {
               hr -> percent = TRUE;
            }
            else if (_td_result_type(FV_TD_RESULT_HR_WIDTH))
            {
               hr -> percent = FALSE;
            }
            else
            {
               hr -> percent = TRUE;
            }
         }

         Context -> line -> hr = hr;
         Context -> line = NULL;
      }
      break;
//+
///FV_TD_ITALIC_ON
      case FV_TD_ITALIC_ON:
      {
         Context -> style |= FSF_ITALIC;
      }
      break;
//+
///FV_TD_ITALIC_OFF
      case FV_TD_ITALIC_OFF:
      {
         Context -> style &= ~FSF_ITALIC;
      }
      break;
//+
///FV_TD_IMAGE
      case FV_TD_IMAGE:
      {
         FTDChunkImage *image;

         if ((image = F_NewP(CUD -> Pool,sizeof (FTDChunkImage))) != NULL)
         {
            image -> Header.Type = FV_TD_CHUNK_IMAGE;

            image -> spec = (STRPTR) _td_result_attr(FV_TD_RESULT_IMAGE_SPEC);

            F_LinkTail(&Context -> line -> ChunkList,(FNode *) image);
         }
         else
         {
            F_Dispose((APTR)(_td_result_attr(FV_TD_RESULT_IMAGE_SPEC)));
         }
      }
      break;
//+
///FV_TD_PENS_ON
      case FV_TD_PENS_ON:
      {
         FTDContextPens *pens,*prev;

         if (Context -> container_pens != NULL)
         {
            prev = Context -> container_pens -> Data;
         }
         else prev = NULL;

         if ((pens = td_context_push(Context,FV_TD_CONTEXT_PENS)) != NULL)
         {
            STATIC uint8 translate[] =
            {
               FV_TD_RESULT_PENS_DOWN,
               FV_TD_RESULT_PENS_LIGHT,
               FV_TD_RESULT_PENS_SHADOW,
               FV_TD_RESULT_PENS_TEXT,
               FV_TD_RESULT_PENS_UP
            };

            uint8 i;

            FTDColor **colors = &pens -> down;

            for (i = 0 ; i < FV_TD_CONTEXT_PENS_COUNT ; i++)
            {
               colors[i] = NULL;
            }

            for (i = 0 ; i < FV_TD_CONTEXT_PENS_COUNT ; i++)
            {
               if (_td_result_type(translate[i]))
               {
                  colors[i] = td_context_add_color(Context,_td_result_attr(translate[i]),_td_result_type(translate[i]));
               }
            }

            switch (_td_result_attr(FV_TD_RESULT_PENS_STYLE))
            {
               case FV_TD_PENS_STYLE_EMBOSS:
               {
                  if (!colors[FV_TD_CONTEXT_PENS_TEXT])
                  {
                     colors[FV_TD_CONTEXT_PENS_TEXT] = td_context_add_color(Context,FV_Pen_Text,FV_TD_RESULT_TYPE_DEC);
                  }
                  if (!colors[FV_TD_CONTEXT_PENS_UP])
                  {
                     colors[FV_TD_CONTEXT_PENS_UP] = td_context_add_color(Context,FV_Pen_Shine,FV_TD_RESULT_TYPE_DEC);
                  }
               }
               break;

               case FV_TD_PENS_STYLE_GHOST:
               {
                  if (!colors[FV_TD_CONTEXT_PENS_TEXT])
                  {
                     colors[FV_TD_CONTEXT_PENS_TEXT] = td_context_add_color(Context,FV_Pen_HalfShadow,FV_TD_RESULT_TYPE_DEC);
                  }
                  if (!colors[FV_TD_CONTEXT_PENS_SHADOW])
                  {
                     colors[FV_TD_CONTEXT_PENS_SHADOW] = td_context_add_color(Context,FV_Pen_HalfShine,FV_TD_RESULT_TYPE_DEC);
                  }
               }
               break;

               case FV_TD_PENS_STYLE_GLOW:
               {
                  if (!colors[FV_TD_CONTEXT_PENS_TEXT])
                  {
                     colors[FV_TD_CONTEXT_PENS_TEXT] = td_context_add_color(Context,FV_Pen_Text,FV_TD_RESULT_TYPE_DEC);
                  }
                  if (!colors[FV_TD_CONTEXT_PENS_LIGHT])
                  {
                     colors[FV_TD_CONTEXT_PENS_LIGHT] = td_context_add_color(Context,FV_Pen_Shine,FV_TD_RESULT_TYPE_DEC);
                  }
                  if (!colors[FV_TD_CONTEXT_PENS_SHADOW])
                  {
                     colors[FV_TD_CONTEXT_PENS_SHADOW] = td_context_add_color(Context,FV_Pen_HalfShadow,FV_TD_RESULT_TYPE_DEC);
                  }
               }
               break;

               case FV_TD_PENS_STYLE_LIGHT:
               {
                  if (!colors[FV_TD_CONTEXT_PENS_TEXT])
                  {
                     colors[FV_TD_CONTEXT_PENS_TEXT] = td_context_add_color(Context,FV_Pen_Text,FV_TD_RESULT_TYPE_DEC);
                  }
                  if (!colors[FV_TD_CONTEXT_PENS_LIGHT])
                  {
                     colors[FV_TD_CONTEXT_PENS_LIGHT] = td_context_add_color(Context,FV_Pen_Shine,FV_TD_RESULT_TYPE_DEC);
                  }
               }
               break;

               case FV_TD_PENS_STYLE_SHADOW:
               {
                  if (!colors[FV_TD_CONTEXT_PENS_TEXT])
                  {
                     colors[FV_TD_CONTEXT_PENS_TEXT] = td_context_add_color(Context,FV_Pen_Text,FV_TD_RESULT_TYPE_DEC);
                  }
                  if (!colors[FV_TD_CONTEXT_PENS_SHADOW])
                  {
                     colors[FV_TD_CONTEXT_PENS_SHADOW] = td_context_add_color(Context,FV_Pen_HalfShadow,FV_TD_RESULT_TYPE_DEC);
                  }
               }
               break;
            }

            if (prev)
            {
               for (i = 0 ; i < FV_TD_CONTEXT_PENS_COUNT ; i++)
               {
                  if (!colors[i])
                  {
                     colors[i] = ((FTDColor **)(&prev -> down))[i];
                  }
               }
            }

            if (colors[FV_TD_CONTEXT_PENS_UP] || colors[FV_TD_CONTEXT_PENS_LIGHT])
            {
               Context -> flags |= FF_TD_CONTEXT_EXTRA_TOPLEFT;
            }

            if (colors[FV_TD_CONTEXT_PENS_DOWN] || colors[FV_TD_CONTEXT_PENS_SHADOW])
            {
               Context -> flags |= FF_TD_CONTEXT_EXTRA_BOTTOMRIGHT;
            }
         }
      }
      break;
//+
///FV_TD_PENS_OFF
      case FV_TD_PENS_OFF:
      {
         td_context_pop(Context,FV_TD_CONTEXT_PENS);
      }
      break;
//+
///FV_TD_SPACING
      case FV_TD_SPACING:
      {
         Context -> spacing = _td_result_attr(FV_TD_RESULT_SPACING);
         Context -> line -> spacing = Context -> spacing;
      }
      break;
//+

      case FV_TD_STOP_ON:        Context -> stop_engine = TRUE; break;
      case FV_TD_STOP_OFF:       Context -> stop_engine = FALSE; break;
      case FV_TD_UNDERLINED_ON:  Context -> style |= FSF_UNDERLINED; break;
      case FV_TD_UNDERLINED_OFF: Context -> style &= ~FSF_UNDERLINED; break;
   }

   return s;
}
//+
///td_create_lines
STATIC int32 td_create_lines(struct FeelinClass *Class,FObject Obj,STRPTR Text,FTDContext *context)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   STRPTR s;

//   FTDLine *line; /* (FTDLine *)(LOD -> LineList.Tail)*/;
//   F_Log(0,"Line 0x%08lx ?= 0x%08lx",context -> line,line);

   if ((s = Text) != NULL)
   {
      while (*s)
      {

         /* if 'context -> line' we  need  to  create  a  new  line  either
         because none has been created so far (first pass) or because a new
         line is requested */

         if (!context -> line)
         {
            if ((context -> line = F_NewP(CUD -> Pool,sizeof (FTDLine))) != NULL)
            {
               F_LinkTail(&LOD -> LineList,(FNode *) context -> line);

               context -> line -> align = context -> align;
               context -> line -> spacing = context -> spacing;
            }
            else return FALSE;
         }

         /* chunks are created when needed, and they are very specific e.g.
         some  text,  an image, a shortcut, an object... appropriate chunks
         are created as data is found.

         For example, text attributes are saved in the 'context', when  raw
         text  is  found  a  chunk 'text' is created and attributes copied.
         Thus, if only  text  attributes  are  defined  no  text  chunk  is
         created. */

         if (context -> stop_engine == FALSE)
         {
            while (*s == '<')
            {
               s = td_parse_tag(s,context);

               /* a line breaker as been found, probably a <hr> (separation
               line).  'context  ->  line'  is set to NULL to request a new
               line */

               if (!context -> line || context -> stop_engine) break;
            }
         }

         /* 'context -> line' is NULL because a line breaker has been found
         while  parsing  tags,  thus  there  is  no  text  chunk to create,
         continue from loop start */

         if (!context -> line) continue;

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

         if (*s == '\0')
         {
            break;
         }
         else if (*s == '\n')
         {
            s++;

            /* As explained before, I set 'context  ->  line'  to  NULL  to
            request a new line */

            context -> line = NULL;
         }
         else
         {

            /* At this point, there is raw  text.  A  text  chunk  must  be
            created to initiated.

            WARNING: Don't forget that the raw text can contain a  shortcut
            definition */

            if (context -> stop_shortcut == FALSE)
            {
               s = td_short_check(s,context);
               s = td_chunk_text_new(s,context,NULL);
               s = td_short_check(s,context);
            }
            else
            {
//               F_Log(0,"CHUNK line 0x%08lx ?= context.line 0x%08lx",line,context.line);

               s = td_chunk_text_new(s,context,NULL);
            }
         }
      }
   }
   return TRUE;
}
//+

/*** public ****************************************************************/

///td_create
void td_create(struct FeelinClass *Class,FObject Obj)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   FTDContext *context;

   if ((context = td_context_new(Class,Obj)) != NULL)
   {
      td_create_lines(Class,Obj,LOD -> Prep,context);
      td_create_lines(Class,Obj,LOD -> Text,context);

      LOD -> Shortcut = context -> shortcut;

      /*** still colors list from context ***/

      LOD -> ColorsList.Head = context -> ColorsList.Head;
      LOD -> ColorsList.Tail = context -> ColorsList.Tail;

      context -> ColorsList.Head = NULL;
      context -> ColorsList.Tail = NULL;

      if (FF_TD_CONTEXT_EXTRA_TOPLEFT & context -> flags)
      {
         LOD -> Flags |= FF_TD_EXTRA_TOPLEFT;
      }

      if (FF_TD_CONTEXT_EXTRA_BOTTOMRIGHT & context -> flags)
      {
         LOD -> Flags |= FF_TD_EXTRA_BOTTOMRIGHT;
      }

      td_context_dispose(context);
   }
}
//+

