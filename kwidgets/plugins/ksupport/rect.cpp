//
// $Id$
//


#include <vector>
#include <algorithm>
#include "ncbind.hpp"


/**------------------------------
 * 矩形
 ------------------------------*/
class KRect
{
public:
  tjs_real left, top, right, bottom;

  /**------------------------------
   * コンストラクタ
   *
   * @param l 左端
   * @param t 上端
   * @param w 幅
   * @param h 高さ
   ------------------------------*/
  KRect(tjs_real l, tjs_real t, tjs_real w, tjs_real h) {
    left = l;
    top = t;
    right = w + l;
    bottom = h + t;
  }

  /**------------------------------
   * 矩形が有効かどうか判定する
   *
   * @return 矩形が有効かどうか
   ------------------------------*/
  bool getIsValid(void) const  {
    return (left <= right && top <= bottom);
  }

  /*------------------------------
   * 左
   ------------------------------*/
  tjs_real getLeft(void) const {
    return left;
  }

  void setLeft(tjs_real v) {
    left = v;
  }

  /*------------------------------
   * 右
   ------------------------------*/
  tjs_real getRight(void) const {
    return right;
  }

  void setRight(tjs_real v) {
    right = v;
  }

  /*------------------------------
   * 上
   ------------------------------*/
  tjs_real getTop(void) const {
    return top;
  }

  void setTop(tjs_real v) {
    top = v;
  }
  
  /*------------------------------
   * 下
   ------------------------------*/
  tjs_real getBottom(void) const {
    return bottom;
  }

  void setBottom(tjs_real v) {
    bottom = v;
  }

  /**------------------------------
   * 幅
   *
   * @return 矩形の幅
   ------------------------------*/
  tjs_real getWidth(void) const {
    return right - left;
  }

  void setWidth(tjs_real v) {
    right = left + v;
  }

  /**------------------------------
   * 高さ
   *
   * @return 矩形の高さ
   ------------------------------*/
  tjs_real getHeight(void) const {
    return bottom - top;
  }
  void setHeight(tjs_real v) {
    bottom = top + v;
  }

  /**------------------------------
   * 矩形の複製
   *
   * @return 自分自身を複製した矩形
   ------------------------------*/
  KRect *dup(void) const {
    return new KRect(left, top, getWidth(), getHeight());
  }

  /**------------------------------
   * 矩形の割り当て
   * 
   * @param rect 割り当て元
   * 
   * rectの内容を自分自身にコピーする
   ------------------------------*/
  void assign(const KRect *rect) {
    left = rect->left;
    top = rect->top;
    right = rect->right;
    bottom = rect->bottom;
  }

  /**------------------------------
   * 交差判定
   *
   * @param rect 矩形
   * @return 交差しているか？
   *
   * rectと自分自身に重なりがあるか判定する
   ------------------------------*/
  bool intersects(const KRect *rect) const {
    return ! (rect->right <= left
              || right <= rect->left
              || rect->bottom <= top
              || bottom <= rect->top);
  }

  /**------------------------------
   * 包含判定
   *
   * @param x X座標
   * @param y Y座標
   * @return 包含しているか？
   *
   * 矩形が座標(X,Y)を包含しているか判定する
   ------------------------------*/
  bool contains(tjs_real x, tjs_real y) const {
    return left <=x && x < right && top <= y && y <= bottom;
  }

  /**------------------------------
   * 包含演算
   *
   * @param rect 矩形
   *
   * rectと自分自身の両方を含む最大の矩形を、自分自身に割り当て直す
   ------------------------------*/
  void include(const KRect *rect) {
    left = left < rect->left ? left : rect->left;
    top = top < rect->top ? top : rect->top;
    right = right > rect->right ? right : rect->right;
    bottom = bottom > rect->bottom ? bottom : rect->bottom;
  }

  /**------------------------------
   * 交差演算
   *
   * @param rect 矩形
   *
   * rectと自分自身の重なる部分を、自分自身に割り当て直す
   ------------------------------*/
  void intersectWith(const KRect *rect) {
    left = left > rect->left ? left : rect->left;
    top = top > rect->top ? top : rect->top;
    right = right < rect->right ? right : rect->right;
    bottom = bottom < rect->bottom ? bottom : rect->bottom;
  }

  /**------------------------------
   * 包含演算
   *
   * @param rect 矩形
   *
   * rectと自分自身の共通矩形を、自分自身に割り当て直す
   ------------------------------*/
  void unionWith(const KRect *rect) {
    left = left < rect->left ? left : rect->left;
    top = top < rect->top ? top : rect->top;
    right = right > rect->right ? right : rect->right;
    bottom = bottom > rect->bottom ? bottom : rect->bottom;
  }
};

NCB_REGISTER_CLASS(KRect)
{
  Constructor<tjs_real, tjs_real, tjs_real, tjs_real>(0);

  NCB_METHOD(dup);
  NCB_METHOD(assign);
  NCB_METHOD(intersects);
  NCB_METHOD(contains);
  NCB_METHOD(include);
  NCB_METHOD(intersectWith);
  NCB_METHOD(unionWith);

  NCB_PROPERTY_RO(isValid, getIsValid);
  NCB_PROPERTY(left, getLeft, setLeft);
  NCB_PROPERTY(right, getRight, setRight);
  NCB_PROPERTY(top, getTop, setTop);
  NCB_PROPERTY(bottom, getBottom, setBottom);
  NCB_PROPERTY(width, getWidth, setWidth);
  NCB_PROPERTY(height, getHeight, setHeight);
};


/**----------------------------------------------------------------------
 * 領域
 ----------------------------------------------------------------------*/
class KRegion
{
public:
  std::vector<KRect> rectList;

  /**------------------------------
   * コンストラクタ
   ------------------------------*/
  KRegion(void) {
  }

  /**------------------------------
   * 内部に含む矩形の数
   ------------------------------*/
  tjs_uint getRectCount(void) const {
    return tjs_uint(rectList.size());
  }

  /**------------------------------
   * 矩形を直接参照する
   * 
   * @param index インデックス
   * @return 矩形
   ------------------------------*/
  KRect *rectAt(tjs_uint index) {
    return rectList[index].dup();
  }

  /**------------------------------
   * 領域は空かどうか判定する
   *
   * @return 領域は空か？
   ------------------------------*/
  bool getEmpty(void) const {
    return rectList.empty();
  }

  /**------------------------------
   * 領域を空にする
   ------------------------------*/
  void clear(void) {
    rectList.clear();
  }

  /**------------------------------
   * 領域を設定する
   *
   * @param r KRectもしくはKRegionオブジェクト
   ------------------------------*/
  void assign(tTJSVariant r) {
    KRect *rect = ncbInstanceAdaptor<KRect>::AdaptorT::GetNativeInstance(r.AsObjectNoAddRef());
    if (rect) {
      assignRect(rect);
      return;
    }
    KRegion *region = ncbInstanceAdaptor<KRegion>::AdaptorT::GetNativeInstance(r.AsObjectNoAddRef());
    if (region) {
      assignRegion(region);
      return;
    }
    TVPThrowExceptionMessage(L"invalid argumen for function");
  }

  /**------------------------------
   * 領域を拡張する
   *
   * @param r KRectもしくはKRegionオブジェクト
   *
   * 指定した範囲を領域につけ加える
   ------------------------------*/
  void include(tTJSVariant r) {
    KRect *rect = ncbInstanceAdaptor<KRect>::AdaptorT::GetNativeInstance(r.AsObjectNoAddRef());
    if (rect) {
      includeRect(rect);
      return;
    }
    KRegion *region = ncbInstanceAdaptor<KRegion>::AdaptorT::GetNativeInstance(r.AsObjectNoAddRef());
    if (region) {
      includeRegion(region);
      return;
    }
    TVPThrowExceptionMessage(L"invalid argumen for function");
  }

  /**------------------------------
   * 領域を削減する
   *
   * @param r KRectもしくはKRegionオブジェクト
   *
   * 指定した範囲を領域から取り除く
   ------------------------------*/
  void exclude(tTJSVariant r) {
    KRect *rect = ncbInstanceAdaptor<KRect>::AdaptorT::GetNativeInstance(r.AsObjectNoAddRef());
    if (rect) {
      excludeRect(rect);
      return;
    }
    KRegion *region = ncbInstanceAdaptor<KRegion>::AdaptorT::GetNativeInstance(r.AsObjectNoAddRef());
    if (region) {
      excludeRegion(region);
      return;
    }
    TVPThrowExceptionMessage(L"invalid argumen for function");
  }

  /**------------------------------
   * 交差演算
   * 
   * @param r KRectもしくはKRegionオブジェクト
   *
   * 指定した範囲をが領域の共通部分だけを残して残りを削除する
   ------------------------------*/
  void intersectWith(tTJSVariant r) {
    KRect *rect = ncbInstanceAdaptor<KRect>::AdaptorT::GetNativeInstance(r.AsObjectNoAddRef());
    if (rect) {
      intersectWithRect(rect);
      return;
    }
    KRegion *region = ncbInstanceAdaptor<KRegion>::AdaptorT::GetNativeInstance(r.AsObjectNoAddRef());
    if (region) {
      intersectWithRegion(region);
      return;
    }
    TVPThrowExceptionMessage(L"invalid argumen for function");
  }

  /**------------------------------
   * 交差判定
   *
   * @param rect 矩形
   * @return 矩形と交差しているか？
   *
   * 矩形と交差しているかどうかを判定する
   ------------------------------*/
  bool intersects(const KRect * rect) const {
    for (tjs_uint i = 0; i < rectList.size(); i++)
      if (rectList[i].intersects(rect))
        return true;

    return false;
  }

  /**------------------------------
   * 包含判定
   *
   * @param x X座標
   * @param y Y座標
   * @return 座標を包含しているか？
   *
   * 座標(X,Y)を領域内に包含しているかどうかを判定する
   ------------------------------*/
  bool contains(tjs_real x, tjs_real y) const {
    for (tjs_uint i = 0; i < rectList.size(); i++)
      if (rectList[i].contains(x, y))
        return true;

    return false;
  }

  /*----------------------------------------------------------------------
   * 以下、内部処理
   ----------------------------------------------------------------------*/
  // 矩形割り当て
  void assignRect(const KRect *rect) {
    rectList.clear();
    rectList.push_back(*rect);
  }

  // 領域割り当て
  void assignRegion(const KRegion *region) {
    rectList = region->rectList;
  }

  // 矩形追加
  void includeRect(const KRect *rect) {
    KRegion r;
    r.assignRect(rect);
    r.excludeRegion(this);
    for (tjs_uint i = 0; i < r.rectList.size(); i++)
      rectList.push_back(r.rectList[i]);
  }

  // 領域追加
  void includeRegion(const KRegion *region) {
    if (region->getEmpty())
      return;
    
    KRegion r;
    r.assignRegion(region);
    r.excludeRegion(this);
    for (tjs_uint i = 0; i < r.rectList.size(); i++)
      rectList.push_back(r.rectList[i]);
  }

  // 矩形除去
  void excludeRect(const KRect *rect) {
    std::vector<KRect> newRectList;

    for (tjs_uint i = 0; i < rectList.size(); i++) {
      KRect &aRect = rectList[i];
      if (! aRect.intersects(rect)) {
        newRectList.push_back(aRect);
        continue;
      }
      if (rect->top > aRect.top) {
        newRectList.push_back(KRect(aRect.left, aRect.top, aRect.right - aRect.left, rect->top - aRect.top));
        aRect.top = rect->top;
      }
      if (rect->bottom < aRect.bottom) {
        newRectList.push_back(KRect(aRect.left, rect->bottom , aRect.right - aRect.left, aRect.bottom - rect->bottom));
        aRect.bottom = rect->bottom;
      }
      if (rect->left > aRect.left)
        newRectList.push_back(KRect(aRect.left, aRect.top, rect->left - aRect.left, aRect.bottom - aRect.top));
      if (rect->right < aRect.right)
        newRectList.push_back(KRect(rect->right, aRect.top, aRect.right - rect->right, aRect.bottom - aRect.top));
    }
    
    rectList.swap(newRectList);
  }

  // 領域除去
  void excludeRegion(const KRegion *region) {
    for (tjs_uint32 i = 0; i < region->rectList.size(); i++)
      excludeRect(&(region->rectList[i]));
  }
  
  // 矩形との交差
  void intersectWithRect(const KRect *rect) {
    std::vector<KRect> newRectList;

    for (tjs_uint i = 0; i < rectList.size(); i++) {
      KRect iRect(rectList[i]);
      iRect.intersectWith(rect);
      if (iRect.getIsValid())
        newRectList.push_back(iRect);
    }

    rectList.swap(newRectList);
  }

  // 領域との交差
  void intersectWithRegion(const KRegion *region) {
    std::vector<KRect> newRectList;

    for (tjs_uint i = 0; i < region->rectList.size(); i++)
      for (tjs_uint j = 0;j < rectList.size(); j++) {
        KRect iRect(region->rectList[i]);
        iRect.intersectWith(&rectList[j]);
        if (iRect.getIsValid())
          newRectList.push_back(iRect);
      }
    
    rectList.swap(newRectList);
  }
};

NCB_REGISTER_CLASS(KRegion)
{
  Constructor();

  NCB_METHOD(rectAt);
  NCB_METHOD(clear);
  NCB_METHOD(assign);
  NCB_METHOD(include);
  NCB_METHOD(exclude);
  NCB_METHOD(intersectWith);
  NCB_METHOD(intersects);
  NCB_METHOD(contains);

  NCB_PROPERTY_RO(rectCount, getRectCount);
  NCB_PROPERTY_RO(empty, getEmpty);
};

