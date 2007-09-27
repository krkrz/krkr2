#if defined(__WIN32) // mingw
/* NOP */
#else // POSIX
# include "../Posix/SignalTranslator.cpp"
#endif
