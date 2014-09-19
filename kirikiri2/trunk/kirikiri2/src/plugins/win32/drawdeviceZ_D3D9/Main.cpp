#include <windows.h>
#include "tp_stub.h"
#include "simplebinder.hpp"

#include "DrawDevice.h" // on fakeFiles/DrawDevice.h -> ../drawdevice/BasicDrawDevice.h

////////////////////////////////////////////////////////////////
// DrawDeviceの仕様違いを吸収
class myTVPDrawDevice : public tTVPDrawDevice {
protected:
	tTVPRect ClipRect; //!< クリッピング矩形
public:
	virtual void TJS_INTF_METHOD SetDestRectangle(const tTVPRect & rect) {
		ClipRect = rect;
		tTVPDrawDevice::SetDestRectangle(rect);
	}
	/** same as tTVPDrawDevice::SetHintText
	virtual void TJS_INTF_METHOD SetHintText(iTVPLayerManager * manager, const ttstr & text) {
		iTVPLayerManager * primary_manager = GetLayerManagerAt(PrimaryLayerManagerIndex);
		if(primary_manager && primary_manager == manager) Window->SetHintText(text);
	}
	 */

	// dummy (for krkrZ draw device interface)
	virtual void TJS_INTF_METHOD SetClipRectangle(const tTVPRect & rect) {}
	virtual void TJS_INTF_METHOD OnTouchDown( tjs_real x, tjs_real y, tjs_real cx, tjs_real cy, tjs_uint32 id ) {}
	virtual void TJS_INTF_METHOD OnTouchUp( tjs_real x, tjs_real y, tjs_real cx, tjs_real cy, tjs_uint32 id ) {}
	virtual void TJS_INTF_METHOD OnTouchMove( tjs_real x, tjs_real y, tjs_real cx, tjs_real cy, tjs_uint32 id ) {}
	virtual void TJS_INTF_METHOD OnTouchScaling( tjs_real startdist, tjs_real curdist, tjs_real cx, tjs_real cy, tjs_int flag ) {}
	virtual void TJS_INTF_METHOD OnTouchRotate( tjs_real startangle, tjs_real curangle, tjs_real dist, tjs_real cx, tjs_real cy, tjs_int flag ) {}
	virtual void TJS_INTF_METHOD OnMultiTouch() {}
	virtual void TJS_INTF_METHOD OnDisplayRotate( tjs_int orientation, tjs_int rotate, tjs_int bpp, tjs_int width, tjs_int height ) {}
	virtual void TJS_INTF_METHOD SetHintText(iTVPLayerManager * manager, iTJSDispatch2* sender, const ttstr & text) {}
	virtual bool TJS_INTF_METHOD SwitchToFullScreen( HWND window, tjs_uint w, tjs_uint h, tjs_uint bpp, tjs_uint color, bool changeresolution ) { return false; }
	virtual void TJS_INTF_METHOD RevertFromFullScreen( HWND window, tjs_uint w, tjs_uint h, tjs_uint bpp, tjs_uint color ) {}
	virtual bool TJS_INTF_METHOD WaitForVBlank( tjs_int* in_vblank, tjs_int* delayed ) { return false; }
};
// [XXX] fake wrapper
#define tTVPDrawDevice myTVPDrawDevice
#include "DrawDeviceZ/BasicDrawDevice.h"
#undef tTVPDrawDevice

////////////////////////////////////////////////////////////////
// TJS空間にDrawDeviceクラスを登録

class DrawDeviceZ : public tTJSNI_BasicDrawDevice {
public:
	static tjs_error CreateNew(iTJSDispatch2 *objthis, DrawDeviceZ* &inst, tjs_int num, tTJSVariant **args) {
		inst = new DrawDeviceZ();
		if (inst) inst->Construct(num, args, objthis);
		return TJS_S_OK;
	}
	static void Destroy(DrawDeviceZ* inst) {
		if (inst) inst->Invalidate();
		delete inst;
	}

	tjs_error GetInterface(tTJSVariant *result) const {
		if (result) *result = reinterpret_cast<tjs_int64>(GetDevice());
		return TJS_S_OK;
	}

	tjs_error Recreate(tTJSVariant *result) {
		GetDevice()->SetToRecreateDrawer();
		GetDevice()->EnsureDevice();
		return TJS_S_OK;
	}

};

bool Entry(bool link) {
	typedef DrawDeviceZ DDZ;
	return
		(SimpleBinder::BindUtil(link)
		 .Class   (TJS_W("DrawDeviceZ"), &DDZ::CreateNew, &DDZ::Destroy)
		 .Property(TJS_W("interface"),   &DDZ::GetInterface, 0)
		 .Function(TJS_W("recreate"),    &DDZ::Recreate)

		 .IsValid()) &&
			true;
}

bool onV2Link()   { return Entry(true);  }
bool onV2Unlink() { return Entry(false); }


////////////////////////////////////////////////////////////////
// [XXX] krkrZのdrawDeviceクラス実装をコンパイルできるように調整しつつinclude

#include "MsgIntf.h"

static IDirect3D9 *TVPDirect3D=NULL;

static IDirect3D9* (WINAPI * TVPDirect3DCreate)( UINT SDKVersion ) = NULL;

static HMODULE TVPDirect3DDLLHandle=NULL;

static tjs_int TVPGetDisplayColorFormat() { return 0; } // unused
static bool TJS_USERENTRY CatchBlockNoRethrow(void * data, const tTVPExceptionDesc & desc) { return false; } // for TVPDoTryBlcok

//---------------------------------------------------------------------------
static void TVPUnloadDirect3D();
static void TJS_USERENTRY TVPInitDirect3D(void *data=NULL)
{
	if(!TVPDirect3DDLLHandle)
	{
		// load d3d9.dll
		TVPAddLog( (const tjs_char*)TVPInfoDirect3D );
		TVPDirect3DDLLHandle = ::LoadLibraryW( L"d3d9.dll" );
		if(!TVPDirect3DDLLHandle)
			TVPThrowExceptionMessage(TVPCannotInitDirect3D, (const tjs_char*)TVPCannotLoadD3DDLL );
	}

	if(!TVPDirect3D)
	{
		try
		{
			// get Direct3DCreaet function
			TVPDirect3DCreate = (IDirect3D9*(WINAPI * )(UINT))GetProcAddress(TVPDirect3DDLLHandle, "Direct3DCreate9");
			if(!TVPDirect3DCreate)
				TVPThrowExceptionMessage(TVPCannotInitDirect3D, (const tjs_char*)TVPNotFoundDirect3DCreate );

			TVPDirect3D = TVPDirect3DCreate( D3D_SDK_VERSION );
			if( NULL == TVPDirect3D )
				TVPThrowExceptionMessage( TVPFaildToCreateDirect3D );
		}
		catch(...)
		{
			TVPUnloadDirect3D();
			throw;
		}
	}

	TVPGetDisplayColorFormat();
}
//---------------------------------------------------------------------------
static void TVPUninitDirect3D()
{
	// release Direct3D object ( DLL will not be released )
}
//---------------------------------------------------------------------------
static void TVPUnloadDirect3D()
{
	// release Direct3D object and /*release it's DLL */
	TVPUninitDirect3D();
	if(TVPDirect3D) TVPDirect3D->Release(), TVPDirect3D = NULL;

	TVPGetDisplayColorFormat();
}
//---------------------------------------------------------------------------
void TVPEnsureDirect3DObject()
{
	TVPDoTryBlock(&TVPInitDirect3D, &CatchBlockNoRethrow, NULL, NULL);
}
//---------------------------------------------------------------------------
IDirect3D9 * TVPGetDirect3DObjectNoAddRef()
{
	// retrieves IDirect3D9 interface
	return TVPDirect3D;
}

// WaitForVBlank は吉里吉里2では未使用インターフェースのため
inline DWORD myTimeGetTime() { return (DWORD)TVPGetTickCount(); }
#define timeGetTime myTimeGetTime

static tTJSNativeClass *classobj = NULL; // not used
#define TJS_NATIVE_CLASSID_NAME tTJSNC_BasicDrawDevice::ClassID
#define tTJSNativeClass(name) ::tTJSNativeClass()
#undef max
#include "DrawDeviceZ/BasicDrawDevice.cpp"
