#ifdef main
    #undef main
#endif

extern "C" int ObjectiveMain( int, char*[] );

int
main( int argc, char* argv[] )
{
    return ObjectiveMain( argc, argv );
}
