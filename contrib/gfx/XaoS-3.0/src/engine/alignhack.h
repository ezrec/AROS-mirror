#if (defined(__GNUC__) && defined(__STDC__)) || defined(__MWERKS__)
    char dummy[((32 - sizeof (int) * 5 - ((int) (sizeof (number_t) + sizeof (int) - 1) / sizeof (int)) * sizeof (int)) < 0 ? 0 :
		  (32 - sizeof (int) * 5 - ((int) (sizeof (number_t) + sizeof (int) - 1) / sizeof (int)) * sizeof (int)))];	/*make it 32 byte aligned */
#endif
