#include "ncbind/ncbind.hpp"
#include <libpsd.h>

static tjs_int getIntProp(iTJSDispatch2 *obj, const tjs_char *name)
{
	tTJSVariant var;
	obj->PropGet(0, name, NULL, &var, obj);
	return (tjs_int)var;
}

// オブジェクトに数値を格納
static void setIntProp(iTJSDispatch2 *obj, const tjs_char *name, int value)
{
	tTJSVariant var = value;
	obj->PropSet(TJS_MEMBERENSURE, name,  NULL, &var, obj);
}

// オブジェクトに文字列を格納
static void setStrProp(iTJSDispatch2 *obj, const tjs_char *name, ttstr &value)
{
	tTJSVariant var = value;
	obj->PropSet(TJS_MEMBERENSURE, name,  NULL, &var, obj);
}

static int convBlendMode(psd_blend_mode mode)
{
	switch (mode) {
	case psd_blend_mode_normal:			// 'norm' = normal
		return ltPsNormal;
	case psd_blend_mode_darken:			// 'dark' = darken
		return ltPsDarken;
	case psd_blend_mode_multiply:		// 'mul ' = multiply
		return ltPsMultiplicative;
	case psd_blend_mode_color_burn:		// 'idiv' = color burn
		return ltPsColorBurn;
	case psd_blend_mode_linear_burn:		// 'lbrn' = linear burn
		return ltPsSubtractive;
	case psd_blend_mode_lighten:			// 'lite' = lighten
		return ltPsLighten;
	case psd_blend_mode_screen:			// 'scrn' = screen
		return ltPsScreen;
	case psd_blend_mode_color_dodge:		// 'div ' = color dodge
		return ltPsColorDodge;
	case psd_blend_mode_linear_dodge:	// 'lddg' = linear dodge
		return ltPsAdditive;
	case psd_blend_mode_overlay:			// 'over' = overlay
		return ltPsOverlay;
	case psd_blend_mode_soft_light:		// 'sLit' = soft light
		return ltPsSoftLight;
	case psd_blend_mode_hard_light:		// 'hLit' = hard light
		return ltPsHardLight;
	case psd_blend_mode_difference:		// 'diff' = difference
		return ltPsDifference;
	case psd_blend_mode_exclusion:		// 'smud' = exclusion
		return ltPsExclusion;
	case psd_blend_mode_dissolve:		// 'diss' = dissolve
	case psd_blend_mode_vivid_light:		// 'vLit' = vivid light
	case psd_blend_mode_linear_light:	// 'lLit' = linear light
	case psd_blend_mode_pin_light:		// 'pLit' = pin light
	case psd_blend_mode_hard_mix:		// 'hMix' = hard mix
		// not supported;
		break;
	}
	return ltPsNormal;
}

class PSD {


	
protected:
	psd_context *context; //< 処理用コンテキスト
	psd_status status;    //< 最後の処理のステータス
public:
	/**
	 * コンストラクタ
	 */
	PSD() : context(NULL) {};

	/**
	 * デストラクタ
	 */
	~PSD() {
		if (context) {
			psd_image_free(context);
		}
	};

	/**
	 * PSD画像のロード
	 * @param filename ファイル名
	 * @return ロードに成功したら true
	 */
	bool load(const char *filename) {
		status = psd_image_load(&context, (char*)filename);
		return status == psd_status_done;
	}

#define INTGETTER(tag) int get_ ## tag(){ return context ? context->tag : -1; }

	INTGETTER(width);
	INTGETTER(height);
	INTGETTER(channels);
	INTGETTER(depth);
	INTGETTER(color_mode);
	INTGETTER(layer_count);

protected:

	/**
	 * レイヤ番号が適切かどうか判定
	 * @param no レイヤ番号
	 */
	void checkLayerNo(int no) {
		if (!context) {
			TVPThrowExceptionMessage(L"no data");
		}
		if (no < 0 || no > context->layer_count) {
			TVPThrowExceptionMessage(L"not such layer");
		}
	}

	/**
	 * 名前の取得
	 * @param name 名前文字列（ユニコード)
	 * @len 長さ
	 */
	ttstr layname(psd_layer_record *lay) {
		ttstr ret;
		if (lay->unicode_name_length > 0) {
			psd_ushort *name = lay->unicode_name;
			for (int i=0;i<lay->unicode_name_length;i++) {
				ret += (tjs_char)_byteswap_ushort(*name++);
			}
		} else {
			ret = ttstr((char*)lay->layer_name);
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
		psd_layer_record *lay = context->layer_records + no;
		return lay->layer_type;
	}

	/**
	 * レイヤ名称の取得
	 * @param no レイヤ番号
	 * @return レイヤ種別
	 */
	ttstr getLayerName(int no) {
		checkLayerNo(no);
		psd_layer_record *lay = context->layer_records + no;
		return layname(lay);
	}
	
	/**
	 * レイヤ情報の取得
	 * @param no レイヤ番号
	 * @return レイヤ情報が格納された辞書
	 */
	tTJSVariant getLayerInfo(int no) {
		checkLayerNo(no);
		psd_layer_record *lay = context->layer_records + no;
		tTJSVariant result;	
		iTJSDispatch2 *obj;
		if ((obj = TJSCreateDictionaryObject()) != NULL) {
			setIntProp(obj, L"layer_type", lay->layer_type);
			setIntProp(obj, L"top",        lay->top);
			setIntProp(obj, L"left",       lay->left);
			setIntProp(obj, L"bottom",     lay->bottom);
			setIntProp(obj, L"right",      lay->right);
			setIntProp(obj, L"width",      lay->width);
			setIntProp(obj, L"height",     lay->height);
			setIntProp(obj, L"blend_mode", lay->blend_mode);
			setIntProp(obj, L"opacity",    lay->opacity);
			setIntProp(obj, L"visible",    lay->visible);
			setStrProp(obj, L"name",       layname(lay));
			setIntProp(obj, L"type",       convBlendMode(lay->blend_mode));
			result = obj;
			obj->Release();
		}
		return result;
	}
	
	/**
	 * レイヤデータの読み出し
	 * @param layer 読み出し先レイヤ
	 * @param no レイヤ番号
	 */
	void getLayerData(tTJSVariant layer, int no) {
//		if (!layer.IsInstanceOf(L"Layer")) {
//			TVPThrowExceptionMessage(L"not layer");
//		}
		checkLayerNo(no);

		psd_layer_record *lay = context->layer_records + no;
		if (lay->layer_type != psd_layer_type_normal) {
			TVPThrowExceptionMessage(L"invalid layer type");
		}

		int width  = lay->width;
		int height = lay->height;

		if (width <= 0 || height <= 0) {
			// サイズ０のレイヤはロードできない
			return;
		}

		iTJSDispatch2 *obj = layer.AsObjectNoAddRef();
		setIntProp(obj, L"left", lay->left);
		setIntProp(obj, L"top",  lay->top);
		setIntProp(obj, L"width", width);
		setIntProp(obj, L"height", height);
		setIntProp(obj, L"type", convBlendMode(lay->blend_mode));
		setIntProp(obj, L"opacity", lay->opacity);
		setIntProp(obj, L"visible", lay->visible);
		setIntProp(obj, L"imageLeft",  0);
		setIntProp(obj, L"imageTop",   0);
		setIntProp(obj, L"imageWidth",  width);
		setIntProp(obj, L"imageHeight", height);
		setStrProp(obj, L"name", layname(lay));
		
		// 画像データのコピー
		psd_argb_color *src = lay->image_data;
		int srclen = width * 4;
		unsigned char *buffer = (unsigned char*)getIntProp(obj, L"mainImageBufferForWrite");
		int pitch = getIntProp(obj, L"mainImageBufferPitch");
		for (int y=0;y<height;y++) {
			memcpy(buffer, (unsigned char*)src, srclen);
			src    += width;
			buffer += pitch;
		}
	}

};

#define ENUM(n) Variant(#n + 4, (int)n)

NCB_REGISTER_CLASS(PSD) {

	Constructor();

	ENUM(psd_color_mode_bitmap);
	ENUM(psd_color_mode_grayscale);
	ENUM(psd_color_mode_indexed);
	ENUM(psd_color_mode_rgb);
	ENUM(psd_color_mode_cmyk);
	ENUM(psd_color_mode_multichannel);
	ENUM(psd_color_mode_duotone);
	ENUM(psd_color_mode_lab);

	ENUM(psd_layer_type_normal);
	ENUM(psd_layer_type_hidden);
	ENUM(psd_layer_type_folder);
	ENUM(psd_layer_type_solid_color);
	ENUM(psd_layer_type_gradient_fill);
	ENUM(psd_layer_type_pattern_fill);
	ENUM(psd_layer_type_levels);
	ENUM(psd_layer_type_curves);
	ENUM(psd_layer_type_brightness_contrast);
	ENUM(psd_layer_type_color_balance);
	ENUM(psd_layer_type_hue_saturation);
	ENUM(psd_layer_type_selective_color);
	ENUM(psd_layer_type_threshold);
	ENUM(psd_layer_type_invert);
	ENUM(psd_layer_type_posterize);
	ENUM(psd_layer_type_channel_mixer);
	ENUM(psd_layer_type_gradient_map);
	ENUM(psd_layer_type_photo_filter);

	ENUM(psd_blend_mode_normal);
	ENUM(psd_blend_mode_dissolve);
	ENUM(psd_blend_mode_darken);
	ENUM(psd_blend_mode_multiply);
	ENUM(psd_blend_mode_color_burn);
	ENUM(psd_blend_mode_linear_burn);
	ENUM(psd_blend_mode_lighten);
	ENUM(psd_blend_mode_screen);
	ENUM(psd_blend_mode_color_dodge);
	ENUM(psd_blend_mode_linear_dodge);
	ENUM(psd_blend_mode_overlay);
	ENUM(psd_blend_mode_soft_light);
	ENUM(psd_blend_mode_hard_light);
	ENUM(psd_blend_mode_vivid_light);
	ENUM(psd_blend_mode_linear_light);
	ENUM(psd_blend_mode_pin_light);
	ENUM(psd_blend_mode_hard_mix);
	ENUM(psd_blend_mode_difference);
	ENUM(psd_blend_mode_exclusion);
	ENUM(psd_blend_mode_hue);
	ENUM(psd_blend_mode_saturation);
	ENUM(psd_blend_mode_color);
	ENUM(psd_blend_mode_luminosity);
	ENUM(psd_blend_mode_pass_through);
	
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
};
