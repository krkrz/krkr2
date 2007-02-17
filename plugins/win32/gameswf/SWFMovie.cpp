#include "SWFMovie.hpp"

#include "base/utility.h"
#include "base/container.h"
#include "base/tu_file.h"
#include "base/tu_types.h"

SWFMovie::SWFMovie()
{
}

SWFMovie::~SWFMovie()
{
	if (m) {
	}
	if (md) {
	}
}

void
SWFMovie::load(const char *name)
{

}

void
SWFMovie::setFrame(int frame)
{
	if (!m) {
		return;
	}
}
