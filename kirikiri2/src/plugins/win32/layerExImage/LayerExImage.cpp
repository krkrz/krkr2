
/** original code header */
/*
 * 07/08/2001 v1.00 - Davide Pizzolato - www.xdp.it
 * CxImage version 5.99c 17/Oct/2004
 */

#include "LayerExImage.h"

void
NI_LayerExImage::lut(BYTE* pLut)
{
	BYTE *src = (BYTE*)_buffer;
	for (int i=0; i < _height ; i++){
		BYTE *p = src;
		for (int j=0; j< _width; j++) {
			*p++ = pLut[*p]; // B
			*p++ = pLut[*p]; // G
			*p++ = pLut[*p]; // R
			p++;             // A
		}
		src += _pitch;
	}
}

/**
 * 明度とコントラスト
 * @param brightness 明度 -255 ～ 255, 負数の場合は暗くなる
 * @param contrast コントラスト -100 ～100, 0 の場合変化しない
 */
void
NI_LayerExImage::light(int brightness, int contrast)
{
	float c = (100 + contrast)/100.0f;
	brightness +=128;
	BYTE cTable[256];
	for (int i=0;i<256;i++)	{
		cTable[i] = (BYTE)max(0,min(255,(int)((i-128)*c + brightness)));
	}
	lut(cTable);
}


////////////////////////////////////////////////////////////////////////////////
#define  HSLMAX   255	/* H,L, and S vary over 0-HSLMAX */
#define  RGBMAX   255   /* R,G, and B vary over 0-RGBMAX */
                        /* HSLMAX BEST IF DIVISIBLE BY 6 */
                        /* RGBMAX, HSLMAX must each fit in a BYTE. */
/* Hue is undefined if Saturation is 0 (grey-scale) */
/* This value determines where the Hue scrollbar is */
/* initially set for achromatic colors */
#define HSLUNDEFINED (HSLMAX*2/3)
////////////////////////////////////////////////////////////////////////////////

static RGBQUAD
RGBtoHSL(RGBQUAD lRGBColor)
{
	BYTE R,G,B;					/* input RGB values */
	BYTE H,L,S;					/* output HSL values */
	BYTE cMax,cMin;				/* max and min RGB values */
	WORD Rdelta,Gdelta,Bdelta;	/* intermediate value: % of spread from max*/

	R = lRGBColor.rgbRed;	/* get R, G, and B out of DWORD */
	G = lRGBColor.rgbGreen;
	B = lRGBColor.rgbBlue;

	cMax = max( max(R,G), B);	/* calculate lightness */
	cMin = min( min(R,G), B);
	L = (BYTE)((((cMax+cMin)*HSLMAX)+RGBMAX)/(2*RGBMAX));

	if (cMax==cMin){			/* r=g=b --> achromatic case */
		S = 0;					/* saturation */
		H = HSLUNDEFINED;		/* hue */
	} else {					/* chromatic case */
		if (L <= (HSLMAX/2))	/* saturation */
			S = (BYTE)((((cMax-cMin)*HSLMAX)+((cMax+cMin)/2))/(cMax+cMin));
		else
			S = (BYTE)((((cMax-cMin)*HSLMAX)+((2*RGBMAX-cMax-cMin)/2))/(2*RGBMAX-cMax-cMin));
		/* hue */
		Rdelta = (WORD)((((cMax-R)*(HSLMAX/6)) + ((cMax-cMin)/2) ) / (cMax-cMin));
		Gdelta = (WORD)((((cMax-G)*(HSLMAX/6)) + ((cMax-cMin)/2) ) / (cMax-cMin));
		Bdelta = (WORD)((((cMax-B)*(HSLMAX/6)) + ((cMax-cMin)/2) ) / (cMax-cMin));

		if (R == cMax)
			H = (BYTE)(Bdelta - Gdelta);
		else if (G == cMax)
			H = (BYTE)((HSLMAX/3) + Rdelta - Bdelta);
		else /* B == cMax */
			H = (BYTE)(((2*HSLMAX)/3) + Gdelta - Rdelta);

//		if (H < 0) H += HSLMAX;     //always false
		if (H > HSLMAX) H -= HSLMAX;
	}
	RGBQUAD hsl={L,S,H,0};
	return hsl;
}

static float
HueToRGB(float n1,float n2, float hue)
{
	//<F. Livraghi> fixed implementation for HSL2RGB routine
	float rValue;

	if (hue > 360)
		hue = hue - 360;
	else if (hue < 0)
		hue = hue + 360;

	if (hue < 60)
		rValue = n1 + (n2-n1)*hue/60.0f;
	else if (hue < 180)
		rValue = n2;
	else if (hue < 240)
		rValue = n1+(n2-n1)*(240-hue)/60;
	else
		rValue = n1;

	return rValue;
}

static RGBQUAD
HSLtoRGB(RGBQUAD lHSLColor)
{ 
	//<F. Livraghi> fixed implementation for HSL2RGB routine
	float h,s,l;
	float m1,m2;
	BYTE r,g,b;

	h = (float)lHSLColor.rgbRed * 360.0f/255.0f;
	s = (float)lHSLColor.rgbGreen/255.0f;
	l = (float)lHSLColor.rgbBlue/255.0f;

	if (l <= 0.5)	m2 = l * (1+s);
	else			m2 = l + s - l*s;

	m1 = 2 * l - m2;

	if (s == 0) {
		r=g=b=(BYTE)(l*255.0f);
	} else {
		r = (BYTE)(HueToRGB(m1,m2,h+120) * 255.0f);
		g = (BYTE)(HueToRGB(m1,m2,h) * 255.0f);
		b = (BYTE)(HueToRGB(m1,m2,h-120) * 255.0f);
	}

	RGBQUAD rgb = {b,g,r,0};
	return rgb;
}

/**
 * 色相と彩度
 * @param hue 色相
 * @param sat 彩度
 * @param blend ブレンド 0 (効果なし) ～ 1 (full effect)
 */
void
NI_LayerExImage::colorize(int hue, int sat, double blend)
{
	if (blend < 0.0f) blend = 0.0f;
	if (blend > 1.0f) blend = 1.0f;
	int a0 = (int)(256*blend);
	int a1 = 256 - a0;

	bool bFullBlend = false;
	if (blend > 0.999f)	bFullBlend = true;
	
	RGBQUAD color,hsl;

	BYTE *src = (BYTE*)_buffer;
	for (int y=0; y<_height; y++){
 		BYTE *p = src;
		for (int x=0; x<_width; x++){
			color.rgbBlue = p[0];
			color.rgbGreen= p[1];
			color.rgbRed  = p[2];
			if (bFullBlend){
				color = RGBtoHSL(color);
				color.rgbRed=hue;
				color.rgbGreen=sat;
				color = HSLtoRGB(color);
			} else {
				hsl = RGBtoHSL(color);
				hsl.rgbRed=hue;
				hsl.rgbGreen=sat;
				hsl = HSLtoRGB(hsl);
				//BlendPixelColor(x,y,hsl,blend);
				//color.rgbRed = (BYTE)(hsl.rgbRed * blend + color.rgbRed * (1.0f - blend));
				//color.rgbBlue = (BYTE)(hsl.rgbBlue * blend + color.rgbBlue * (1.0f - blend));
				//color.rgbGreen = (BYTE)(hsl.rgbGreen * blend + color.rgbGreen * (1.0f - blend));
				color.rgbRed = (BYTE)((hsl.rgbRed * a0 + color.rgbRed * a1)>>8);
				color.rgbBlue = (BYTE)((hsl.rgbBlue * a0 + color.rgbBlue * a1)>>8);
				color.rgbGreen = (BYTE)((hsl.rgbGreen * a0 + color.rgbGreen * a1)>>8);
			}
			*p++ = color.rgbBlue;  // B
			*p++ = color.rgbGreen; // G
			*p++ = color.rgbRed;   // R
			p++;                   // A
		}
		src += _pitch;
	}
}

/**
 * ノイズ追加
 * @param level ノイズレベル 0 (no noise) ～ 255 (lot of noise).
 */
void
NI_LayerExImage::noise(int level)
{
	BYTE *src = (BYTE*)_buffer;
	for (int y=0; y<_height; y++){
		BYTE *p = src;
		for (int x=0; x<_width; x++){
			int n = (int)((rand()/(float)RAND_MAX - 0.5)*level);
			*p++ = (BYTE)max(0,min(255,(int)(*p + n)));
			n = (int)((rand()/(float)RAND_MAX - 0.5)*level);
			*p++ = (BYTE)max(0,min(255,(int)(*p + n)));
			n = (int)((rand()/(float)RAND_MAX - 0.5)*level);
			*p++ = (BYTE)max(0,min(255,(int)(*p + n)));
			p++;
		}
		src += _pitch;
	}
}
