typedef struct
{
    const char 	*name;
    const char 	*activateparam;
    boolean 	(*InitNetwork)(void);
    void 	(*CleanupNetwork)(void);
    void	(*NetCmd)(void);
} arosnetdriver_t;
