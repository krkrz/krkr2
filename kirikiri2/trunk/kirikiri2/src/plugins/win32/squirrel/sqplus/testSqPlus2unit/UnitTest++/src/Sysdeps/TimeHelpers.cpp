#if defined(__WIN32) // mingw
# include "../Win32/TimeHelpers.cpp"
#else // POSIX
# include "../Posix/TimeHelpers.cpp"
#endif
