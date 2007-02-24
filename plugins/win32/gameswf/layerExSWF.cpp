#include "layerExSWF.hpp"

/**
 * コンストラクタ
 */
layerExSWF::layerExSWF(DispatchT obj) : layerExCairo(obj)
{
}

/**
 * デストラクタ
 */
layerExSWF::~layerExSWF()
{
}

void
layerExSWF::drawSWF(SWFMovie *swf)
{
	swf->draw(cairo);
}
