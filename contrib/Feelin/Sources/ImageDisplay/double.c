#include "Private.h"
                  
typedef struct in_Double            FImage;

///id_double_create
STATIC F_CODE_CREATE_DOUBLE(id_double_create)
{
   FImage *image;

   if ((image = F_NewP(CUD -> Pool,sizeof (FImage))))
   {
      image -> Data[0] = id_image_new(RSpec,&image -> Methods[0]);
      image -> Data[1] = id_image_new(SSpec,&image -> Methods[1]);
      
      if (!image -> Data[0] || !image -> Data[1])
      {
         if (image -> Methods[0])
         {
            id_image_dispose(image -> Methods[0],image -> Data[0]);
         }

         if (image -> Methods[1])
         {
            id_image_dispose(image -> Methods[1],image -> Data[1]);
         }

         F_Dispose(image);

         image = NULL;
      }
   }

   return image;
}
//+
///id_double_delete
STATIC F_CODE_DELETE(id_double_delete)
{
   uint32 i;
   
   for (i = 0 ; i < 2 ; i++)
   {
      if (image -> Methods)
      {
         id_image_dispose(image -> Methods[i],image -> Data[i]);
      }
   }
}
//+
///id_double_setup
STATIC F_CODE_SETUP(id_double_setup)
{
   uint32 i;

   for (i = 0 ; i < 2 ; i++)
   {
      if (image -> Methods[i] && image -> Methods[i] -> setup)
      {
         image -> Methods[i] -> setup(image -> Data[i],Class,Render);
      }
   }
   return TRUE;
}
//+
///id_double_cleanup
STATIC F_CODE_CLEANUP(id_double_cleanup)
{
   uint32 i;

   for (i = 0 ; i < 2 ; i++)
   {
      if (image -> Methods[i] && image -> Methods[i] -> cleanup)
      {
         image -> Methods[i] -> cleanup(image -> Data[i],Class,Render);
      }
   }
}
//+

struct in_CodeTable id_double_table =
{
   (in_Code_Create *)   &id_double_create,
   (in_Code_Delete *)   &id_double_delete,
   (in_Code_Setup *)    &id_double_setup,
   (in_Code_Cleanup *)  &id_double_cleanup,
   NULL,
   NULL
};
