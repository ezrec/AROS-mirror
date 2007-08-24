
enum    {
    
        COLOR_TYPE_SCHEME = 1,                    // one of a color scheme entry
        COLOR_TYPE_PEN,                           // display's pen
        COLOR_TYPE_RGB                            // a 24bits color

        };

uint32 color_decode_spec(STRPTR Spec,uint32 *Data);

