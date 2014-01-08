#include "psdparse/psdfile.h"

// ncb.typeconv: cast: enum->int
NCB_TYPECONV_CAST_INTEGER(psd::LayerType);
NCB_TYPECONV_CAST_INTEGER(psd::BlendMode);

static int convBlendMode(psd::BlendMode mode)
{
	switch (mode) {
	case psd::BLEND_MODE_NORMAL:			// 'norm' = normal
		return ltPsNormal;
	case psd::BLEND_MODE_DARKEN:			// 'dark' = darken
		return ltPsDarken;
	case psd::BLEND_MODE_MULTIPLY:		// 'mul ' = multiply
		return ltPsMultiplicative;
	case psd::BLEND_MODE_COLOR_BURN:		// 'idiv' = color burn
		return ltPsColorBurn;
	case psd::BLEND_MODE_LINEAR_BURN:		// 'lbrn' = linear burn
		return ltPsSubtractive;
	case psd::BLEND_MODE_LIGHTEN:			// 'lite' = lighten
		return ltPsLighten;
	case psd::BLEND_MODE_SCREEN:			// 'scrn' = screen
		return ltPsScreen;
	case psd::BLEND_MODE_COLOR_DODGE:		// 'div ' = color dodge
		return ltPsColorDodge;
	case psd::BLEND_MODE_LINEAR_DODGE:	// 'lddg' = linear dodge
		return ltPsAdditive;
	case psd::BLEND_MODE_OVERLAY:			// 'over' = overlay
		return ltPsOverlay;
	case psd::BLEND_MODE_SOFT_LIGHT:		// 'sLit' = soft light
		return ltPsSoftLight;
	case psd::BLEND_MODE_HARD_LIGHT:		// 'hLit' = hard light
		return ltPsHardLight;
	case psd::BLEND_MODE_DIFFERENCE:		// 'diff' = difference
		return ltPsDifference;
	case psd::BLEND_MODE_EXCLUSION:		// 'smud' = exclusion
		return ltPsExclusion;
	case psd::BLEND_MODE_DISSOLVE:		// 'diss' = dissolve
	case psd::BLEND_MODE_VIVID_LIGHT:		// 'vLit' = vivid light
	case psd::BLEND_MODE_LINEAR_LIGHT:	// 'lLit' = linear light
	case psd::BLEND_MODE_PIN_LIGHT:		// 'pLit' = pin light
	case psd::BLEND_MODE_HARD_MIX:		// 'hMix' = hard mix
  case psd::BLEND_MODE_DARKER_COLOR:
  case psd::BLEND_MODE_LIGHTER_COLOR:
  case psd::BLEND_MODE_SUBTRACT:
  case psd::BLEND_MODE_DIVIDE:
		// not supported;
		break;
	}
	return ltPsNormal;
}

class PSD {

protected:
  psd::PSDFile psdFile;	// PSD データ

public:
	/**
	 * コンストラクタ
	 */
  PSD() {}; 

	/**
	 * デストラクタ
	 */
	~PSD() {};

	/**
	 * PSD画像のロード
	 * @param filename ファイル名
	 * @return ロードに成功したら true
	 */
	bool load(const char *filename) {
    char native_filename[2048];
    ttstr filename_ttstr(filename);
    filename_ttstr = TVPGetPlacedPath(filename_ttstr);
    if (filename_ttstr.length()) {
      if (!wcschr(filename_ttstr.c_str(), '>')) {
        // ローカルパス化
        TVPGetLocalName(filename_ttstr);
        filename_ttstr.ToNarrowStr(native_filename, 2048);
      } else {
        // アーカイブ内部なのでアクセスできない
        TVPThrowExceptionMessage(L"psd: cannot access to archived file");
        return false;
      }
      psdFile.load(native_filename);
    } else {
      // ネイティブパスが直接渡されたとみなす
      psdFile.load(filename);
    }
    return psdFile.isLoaded;
	}

#define INTGETTER(tag) int get_ ## tag(){ return psdFile.isLoaded ? psdFile.header.tag : -1; }

	INTGETTER(width);
	INTGETTER(height);
	INTGETTER(channels);
	INTGETTER(depth);
  int get_color_mode()  { return psdFile.isLoaded ? psdFile.header.mode : -1; }
  int get_layer_count() { return psdFile.isLoaded ? (int)psdFile.layerList.size() : -1; }

protected:

	/**
	 * レイヤ番号が適切かどうか判定
	 * @param no レイヤ番号
	 */
	void checkLayerNo(int no) {
		if (!psdFile.isLoaded) {
			TVPThrowExceptionMessage(L"no data");
		}
		if (no < 0 || no >= get_layer_count()) {
			TVPThrowExceptionMessage(L"not such layer");
		}
	}

	/**
	 * 名前の取得
	 * @param name 名前文字列（ユニコード)
	 * @len 長さ
	 */
  ttstr layname(psd::LayerInfo &lay) {
		ttstr ret;
		if (!lay.layerNameUnicode.empty()) {
			ret = ttstr(lay.layerNameUnicode.c_str()); 
		} else {
			ret = ttstr(lay.layerName.c_str());
		}
		return ret;
	}

public:
	/**
	 * レイヤ種別の取得
	 * @param no レイヤ番号
	 * @return レイヤ種別
	 */
	int getLayerType(int no) {
		checkLayerNo(no);
    return (int)psdFile.layerList[no].layerType;
	}

	/**
	 * レイヤ名称の取得
	 * @param no レイヤ番号
	 * @return レイヤ種別
	 */
	ttstr getLayerName(int no) {
		checkLayerNo(no);
    return layname(psdFile.layerList[no]);
	}
	
	/**
	 * レイヤ情報の取得
	 * @param no レイヤ番号
	 * @return レイヤ情報が格納された辞書
	 */
	tTJSVariant getLayerInfo(int no) {
		checkLayerNo(no);
    psd::LayerInfo &lay = psdFile.layerList[no];
		tTJSVariant result;	
		ncbDictionaryAccessor dict;
		if (dict.IsValid()) {
#define SETPROP(dict, obj, prop) dict.SetValue(L ## #prop, obj.prop)
			SETPROP(dict, lay, top);
			SETPROP(dict, lay, left);
			SETPROP(dict, lay, bottom);
			SETPROP(dict, lay, right);
			SETPROP(dict, lay, width);
			SETPROP(dict, lay, height);
			SETPROP(dict, lay, opacity);
      dict.SetValue(L"type",       convBlendMode(lay.blendMode));
      dict.SetValue(L"layer_type", lay.layerType);
      dict.SetValue(L"blend_mode", lay.blendMode);
      dict.SetValue(L"visible",    lay.isVisible());
			dict.SetValue(L"name",       layname(lay));

			// additional information
			SETPROP(dict, lay, clipping);
      dict.SetValue(L"layer_id", lay.layerId);
      dict.SetValue(L"obsolete", lay.isObsolete());
      dict.SetValue(L"transparency_protected", lay.isTransparencyProtected());
      dict.SetValue(L"pixel_data_irrelevant",  lay.isPixelDataIrrelevant());

      // レイヤーカンプ
      if (lay.layerComps.size() > 0) {
        ncbDictionaryAccessor compDict;
        if (compDict.IsValid()) {
          for (std::map<int, psd::LayerCompInfo>::iterator it = lay.layerComps.begin();
               it != lay.layerComps.end(); it++)	{
            ncbDictionaryAccessor tmp;
            if (tmp.IsValid()) {
              psd::LayerCompInfo &comp = it->second;
              tmp.SetValue(L"id",         comp.id);
              tmp.SetValue(L"offset_x",   comp.offsetX);
              tmp.SetValue(L"offset_y",   comp.offsetY);
              tmp.SetValue(L"enable",     comp.isEnabled);
              compDict.SetValue((tjs_int32)comp.id, tmp.GetDispatch());
            }
          }
          dict.SetValue(L"layer_comp", compDict.GetDispatch());
        }
      }
      
      // SETPROP(dict, lay, adjustment_valid); // 調整レイヤーかどうか？レイヤタイプで判別可能
      // SETPROP(dict, lay, fill_opacity);
      // SETPROP(dict, lay, layer_name_id);
      // SETPROP(dict, lay, layer_version);
      // SETPROP(dict, lay, blend_clipped);
      // SETPROP(dict, lay, blend_interior);
      // SETPROP(dict, lay, knockout);
      // SETPROP(dict, lay, transparency); // lspf(protection)のもの
      // SETPROP(dict, lay, composite);
      // SETPROP(dict, lay, position_respectively);
      // SETPROP(dict, lay, sheet_color);
      // SETPROP(dict, lay, reference_point_x); // 塗りつぶしレイヤ（パターン）のオフセット
      // SETPROP(dict, lay, reference_point_y); // 塗りつぶしレイヤ（パターン）のオフセット
      // SETPROP(dict, lay, transparency_shapes_layer);
      // SETPROP(dict, lay, layer_mask_hides_effects);
      // SETPROP(dict, lay, vector_mask_hides_effects);
      // SETPROP(dict, lay, divider_type);
      // SETPROP(dict, lay, divider_blend_mode);

			// group layer はスクリプト側では layer_id 参照で引くようにする
			if (lay.parent != NULL)
				dict.SetValue(L"group_layer_id", lay.parent->layerId);

			result = dict;
		}

		return result;
	}
	
	/**
	 * レイヤデータの読み出し
	 * @param layer 読み出し先レイヤ
	 * @param no レイヤ番号
	 */
	void getLayerData(tTJSVariant layer, int no) {
		if (!layer.AsObjectNoAddRef()->IsInstanceOf(0, 0, 0, L"Layer", NULL)) {
			TVPThrowExceptionMessage(L"not layer");
		}
		checkLayerNo(no);

    psd::LayerInfo &lay = psdFile.layerList[no];
    if (lay.layerType != psd::LAYER_TYPE_NORMAL) {
			TVPThrowExceptionMessage(L"invalid layer type");
		}

		int width  = lay.width;
		int height = lay.height;

		if (width <= 0 || height <= 0) {
			// サイズ０のレイヤはロードできない
			return;
		}

		ncbPropAccessor obj(layer);
		SETPROP(obj, lay, left);
		SETPROP(obj, lay, top);
		SETPROP(obj, lay, opacity);
		obj.SetValue(L"width",  width);
		obj.SetValue(L"height", height);
		obj.SetValue(L"type",   convBlendMode(lay.blendMode));
    obj.SetValue(L"visible", lay.isVisible());
		obj.SetValue(L"imageLeft",  0);
		obj.SetValue(L"imageTop",   0);
		obj.SetValue(L"imageWidth",  width);
		obj.SetValue(L"imageHeight", height);
		obj.SetValue(L"name", layname(lay));

		// 画像データのコピー
    unsigned char *buffer = (unsigned char*)obj.GetValue(L"mainImageBufferForWrite", ncbTypedefs::Tag<tjs_int>());
    int pitch = obj.GetValue(L"mainImageBufferPitch", ncbTypedefs::Tag<tjs_int>());
    psdFile.getLayerImage(lay, buffer, psd::BGRA_LE, pitch, psd::IMAGE_MODE_MASKEDIMAGE);
	}

	/**
	 * スライスデータの読み出し
	 * @return スライス情報辞書 %[ top, left, bottom, right, slices:[ %[ id, group_id, left, top, bottom, right ], ... ] ]
	 *         スライス情報がない場合は void を返す
	 */
	tTJSVariant getSlices() {
		if (!psdFile.isLoaded) TVPThrowExceptionMessage(L"no data");
		tTJSVariant result;	
		ncbDictionaryAccessor dict;
		ncbArrayAccessor arr;
    if (psdFile.slice.isEnabled) {
			if (dict.IsValid()) {
        psd::SliceResource &sr = psdFile.slice;
				dict.SetValue(L"top",    sr.boundingTop);
				dict.SetValue(L"left",   sr.boundingLeft);
				dict.SetValue(L"bottom", sr.boundingBottom);
				dict.SetValue(L"right",  sr.boundingRight);
				dict.SetValue(L"name",   ttstr(sr.groupName.c_str()));
				if (arr.IsValid()) {
					for (int i = 0; i < (int)sr.slices.size(); i++) {
						ncbDictionaryAccessor tmp;
						if (tmp.IsValid()) {
              psd::SliceItem &item = sr.slices[i];
              tmp.SetValue(L"id",      	item.id);
              tmp.SetValue(L"group_id", item.groupId);
              tmp.SetValue(L"origin",   item.origin);
              tmp.SetValue(L"type",     item.type);
              tmp.SetValue(L"left",     item.left);
              tmp.SetValue(L"top",      item.top);
              tmp.SetValue(L"right",    item.right);
              tmp.SetValue(L"bottom",   item.bottom);
              tmp.SetValue(L"color",    ((item.colorA<<24) | (item.colorR<<16) | (item.colorG<<8) | item.colorB));
              tmp.SetValue(L"cell_text_is_html",    item.isCellTextHtml);
              tmp.SetValue(L"horizontal_alignment", item.horizontalAlign);
              tmp.SetValue(L"vertical_alignment",   item.verticalAlign);
              tmp.SetValue(L"associated_layer_id",	item.associatedLayerId);
              tmp.SetValue(L"name",      ttstr(item.name.c_str()));
              tmp.SetValue(L"url",       ttstr(item.url.c_str()));
              tmp.SetValue(L"target",    ttstr(item.target.c_str()));
              tmp.SetValue(L"message",   ttstr(item.message.c_str()));
              tmp.SetValue(L"alt_tag",   ttstr(item.altTag.c_str()));
              tmp.SetValue(L"cell_text", ttstr(item.cellText.c_str()));
              arr.SetValue((tjs_int32)i, tmp.GetDispatch());
						}
					}
					dict.SetValue(L"slices", arr.GetDispatch());
				}
				result = dict;
			}
		}
		return result;
	}

	/**
	 * ガイドデータの読み出し
	 * @return ガイド情報辞書 %[ vertical:[ x1, x2, ... ], horizontal:[ y1, y2, ... ] ]
	 *         ガイド情報がない場合は void を返す
	 */
	tTJSVariant getGuides() {
		if (!psdFile.isLoaded) TVPThrowExceptionMessage(L"no data");
		tTJSVariant result;	
		ncbDictionaryAccessor dict;
		ncbArrayAccessor vert, horz;
    if (psdFile.gridGuide.isEnabled) {
      psd::GridGuideResource gg = psdFile.gridGuide;
			if (dict.IsValid() && vert.IsValid() && horz.IsValid()) {
				dict.SetValue(L"horz_grid",  gg.horizontalGrid);
				dict.SetValue(L"vert_grid",  gg.verticalGrid);
				dict.SetValue(L"vertical",   vert.GetDispatch());
				dict.SetValue(L"horizontal", horz.GetDispatch());
				for (int i = 0, v = 0, h = 0; i < (int)gg.guides.size(); i++) {
					if (gg.guides[i].direction == 0) {
						vert.SetValue(v++, gg.guides[i].location);
					} else {
						horz.SetValue(h++, gg.guides[i].location);
					}
				}
				result = dict;
			}
		}
		return result;
	}

	/**
	 * 合成結果の取得。取得領域は画像全体サイズ内におさまってる必要があります
   * 注意：PSDファイル自体に合成済み画像が存在しない場合は取得に失敗します
   *
	 * @param layer 格納先レイヤ(width,heightサイズに調整される)
	 * @return 取得に成功したら true
	 */
  bool getBlend(tTJSVariant layer) {
		if (!layer.AsObjectNoAddRef()->IsInstanceOf(0, 0, 0, L"Layer", NULL)) {
			TVPThrowExceptionMessage(L"not layer");
		}

		// 合成結果を生成
    if (psdFile.imageData) {

			// 格納先を調整
			ncbPropAccessor obj(layer);
			obj.SetValue(L"width",  get_width());
			obj.SetValue(L"height", get_height());
			obj.SetValue(L"imageLeft",  0);
			obj.SetValue(L"imageTop",   0);
			obj.SetValue(L"imageWidth",  get_width());
			obj.SetValue(L"imageHeight", get_height());

      // 画像データのコピー
      unsigned char *buffer = (unsigned char*)obj.GetValue(L"mainImageBufferForWrite", ncbTypedefs::Tag<tjs_int>());
      int pitch = obj.GetValue(L"mainImageBufferPitch", ncbTypedefs::Tag<tjs_int>());
      psdFile.getMergedImage(buffer, psd::BGRA_LE, pitch);

			return true;
		}

		return false;
	}

	/**
	 * レイヤーカンプ
	 */
	tTJSVariant getLayerComp() {
		if (!psdFile.isLoaded) TVPThrowExceptionMessage(L"no data");
		tTJSVariant result;
    ncbDictionaryAccessor dict;
		ncbArrayAccessor arr;
    int compNum = psdFile.layerComps.size();
    if (compNum > 0) {
      if (dict.IsValid()) {
        dict.SetValue(L"last_applied_id", psdFile.lastAppliedCompId);
        if (arr.IsValid()) {
          for (int i = 0; i < compNum; i++) {
            ncbDictionaryAccessor tmp;
            if (tmp.IsValid()) {
              psd::LayerComp &comp = psdFile.layerComps[i];
              tmp.SetValue(L"id",      	        comp.id);
              tmp.SetValue(L"record_visibility", comp.isRecordVisibility);
              tmp.SetValue(L"record_position",   comp.isRecordPosition);
              tmp.SetValue(L"record_appearance", comp.isRecordAppearance);
              tmp.SetValue(L"name",             ttstr(comp.name.c_str()));
              tmp.SetValue(L"comment",          ttstr(comp.comment.c_str()));
              arr.SetValue((tjs_int32)i,        tmp.GetDispatch());
            }
          }
          dict.SetValue(L"comps", arr.GetDispatch());
        }
        result = dict;
      }
    }
		return result;
	}
};

NCB_REGISTER_CLASS(PSD) {

	Constructor();

  Variant("color_mode_bitmap",              (int)psd::COLOR_MODE_BITMAP);
  Variant("color_mode_grayscale",           (int)psd::COLOR_MODE_GRAYSCALE);
  Variant("color_mode_indexed",             (int)psd::COLOR_MODE_INDEXED);
  Variant("color_mode_rgb",                 (int)psd::COLOR_MODE_RGB);
  Variant("color_mode_cmyk",                (int)psd::COLOR_MODE_CMYK);
  Variant("color_mode_multichannel",        (int)psd::COLOR_MODE_MULTICHANNEL);
  Variant("color_mode_duotone",             (int)psd::COLOR_MODE_DUOTONE);
  Variant("color_mode_lab",                 (int)psd::COLOR_MODE_LAB);
  
  Variant("blend_mode_normal",              (int)psd::BLEND_MODE_NORMAL);
  Variant("blend_mode_dissolve",            (int)psd::BLEND_MODE_DISSOLVE);
  Variant("blend_mode_darken",              (int)psd::BLEND_MODE_DARKEN);
  Variant("blend_mode_multiply",            (int)psd::BLEND_MODE_MULTIPLY);
  Variant("blend_mode_color_burn",          (int)psd::BLEND_MODE_COLOR_BURN);
  Variant("blend_mode_linear_burn",         (int)psd::BLEND_MODE_LINEAR_BURN);
  Variant("blend_mode_lighten",             (int)psd::BLEND_MODE_LIGHTEN);
  Variant("blend_mode_screen",              (int)psd::BLEND_MODE_SCREEN);
  Variant("blend_mode_color_dodge",         (int)psd::BLEND_MODE_COLOR_DODGE);
  Variant("blend_mode_linear_dodge",        (int)psd::BLEND_MODE_LINEAR_DODGE);
  Variant("blend_mode_overlay",             (int)psd::BLEND_MODE_OVERLAY);
  Variant("blend_mode_soft_light",          (int)psd::BLEND_MODE_SOFT_LIGHT);
  Variant("blend_mode_hard_light",          (int)psd::BLEND_MODE_HARD_LIGHT);
  Variant("blend_mode_vivid_light",         (int)psd::BLEND_MODE_VIVID_LIGHT);
  Variant("blend_mode_linear_light",        (int)psd::BLEND_MODE_LINEAR_LIGHT);
  Variant("blend_mode_pin_light",           (int)psd::BLEND_MODE_PIN_LIGHT);
  Variant("blend_mode_hard_mix",            (int)psd::BLEND_MODE_HARD_MIX);
  Variant("blend_mode_difference",          (int)psd::BLEND_MODE_DIFFERENCE);
  Variant("blend_mode_exclusion",           (int)psd::BLEND_MODE_EXCLUSION);
  Variant("blend_mode_hue",                 (int)psd::BLEND_MODE_HUE);
  Variant("blend_mode_saturation",          (int)psd::BLEND_MODE_SATURATION);
  Variant("blend_mode_color",               (int)psd::BLEND_MODE_COLOR);
  Variant("blend_mode_luminosity",          (int)psd::BLEND_MODE_LUMINOSITY);
  Variant("blend_mode_pass_through",        (int)psd::BLEND_MODE_PASS_THROUGH);

  // NOTE libpsd 非互換モード
  Variant("blend_mode_darker_color",        (int)psd::BLEND_MODE_DARKER_COLOR);
  Variant("blend_mode_lighter_color",       (int)psd::BLEND_MODE_LIGHTER_COLOR);
  Variant("blend_mode_subtract",            (int)psd::BLEND_MODE_SUBTRACT);
  Variant("blend_mode_divide",              (int)psd::BLEND_MODE_DIVIDE);
  

  // NOTE この定数はlibpsd互換ではありません(folderまでは互換)
  Variant("layer_type_normal",              (int)psd::LAYER_TYPE_NORMAL);
  Variant("layer_type_hidden",              (int)psd::LAYER_TYPE_HIDDEN);
  Variant("layer_type_folder",              (int)psd::LAYER_TYPE_FOLDER);
  Variant("layer_type_adjust",              (int)psd::LAYER_TYPE_ADJUST);
  Variant("layer_type_fill",                (int)psd::LAYER_TYPE_FILL);

	NCB_METHOD(load);

#define INTPROP(name) Property(TJS_W(# name), &Class::get_ ## name, NULL)

	INTPROP(width);
	INTPROP(height);
	INTPROP(channels);
	INTPROP(depth);
	INTPROP(color_mode);
	INTPROP(layer_count);

	NCB_METHOD(getLayerType);
	NCB_METHOD(getLayerName);
	NCB_METHOD(getLayerInfo);
	NCB_METHOD(getLayerData);

	NCB_METHOD(getSlices);
	NCB_METHOD(getGuides);
	NCB_METHOD(getBlend);
  NCB_METHOD(getLayerComp);
};
