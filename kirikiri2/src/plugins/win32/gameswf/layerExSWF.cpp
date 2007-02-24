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

/**
 * @param swf ムービー
 * @param advance 経過時間(ms)
 */
void
layerExSWF::drawSWF(SWFMovie *swf)
{
	swf->draw(cairo, reseted, _width, _height);
}
