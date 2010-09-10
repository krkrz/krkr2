@files=<*.*>;


foreach $file (@files)
{
	rename $file, lc $file;
}