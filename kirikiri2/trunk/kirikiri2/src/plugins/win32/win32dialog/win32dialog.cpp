#include "ncbind.hpp"

// CreateDialogIndirect のテーブル書き出し用クラス
#include "dialog.hpp"

struct Header;
struct Items;

class WIN32Dialog {
public:
	// 型のalias
	typedef ncbPropAccessor PropT;
	typedef ncbDictionaryAccessor DictT;

	typedef tTJSVariant     VarT;
	typedef iTJSDispatch2   DspT;

	typedef DialogConfig::SizeT   SizeT;
	typedef DialogConfig::NameT   NameT;
	typedef DialogConfig::StringT StringT;
private:
	typedef ncbInstanceAdaptor<WIN32Dialog> SelfAdaptorT;
	typedef ncbInstanceAdaptor<Header>      HeadAdaptorT;
	typedef ncbInstanceAdaptor<Items>       ItemAdaptorT;

	HWND dialogHWnd;
	HICON icon;
	DspT *owner, *objthis, *callbacks;
	bool getCallbacks;
	BYTE *buf;
	BYTE *ref;
public:
	// constructor
	WIN32Dialog(DspT *_owner = 0)
		:	dialogHWnd(0),
			icon(0),
			owner(_owner),
			objthis(0),
			callbacks(0),
			getCallbacks(false),
			buf(0)
	{}

	// destructor
	virtual ~WIN32Dialog() {
		//TVPAddLog(TJS_W("# WIN32Dialog.finalize()"));
		if (callbacks) callbacks->Release();
		if (buf) TVP_free(buf);
	}
	BYTE* CreateBuffer(SizeT sz) {
		// 領域確保
		ref = buf = reinterpret_cast<BYTE*>(TVP_malloc(sz + 4));
		SizeT c = 0;
		DialogTemplate::Alignment(ref, c, 4);
		return ref;
	}

	// コールバック呼び出し
	LRESULT callback(tjs_char const *event, UINT msg, WPARAM wparam, LPARAM lparam) {
		DspT *obj = owner ? owner : objthis;
		if (!obj) return FALSE;

		// コールバックが存在するか
		tjs_uint32 hint = 0;
		tTJSVariant rslt;
		if (TJS_FAILED(obj->PropGet(TJS_MEMBERMUSTEXIST, event, &hint, &rslt, obj))) return FALSE;

		// 引数を生成してコールバックを呼ぶ
		rslt.Clear();
		tTJSVariant vmsg((tjs_int32)msg), vwp((tjs_int32)wparam), vlp((tjs_int64)lparam);
		tTJSVariant *params[] = { &vmsg, &vwp, &vlp };
		obj->FuncCall(0, event, 0, &rslt, 3, params, obj);

		return (rslt.AsInteger() != 0) ? TRUE : FALSE;
	}

	// テンプレート作成
	static tjs_error TJS_INTF_METHOD makeTemplate(VarT *result, tjs_int numparams, VarT **param, WIN32Dialog *self);

	bool IsValid() const { return dialogHWnd != 0; }
	void checkDialogValid() const {
		if (!IsValid()) TVPThrowExceptionMessage(TJS_W("Dialog not opened."));
	}
	// 各種 Item 操作関数
	HWND GetItemHWND(int id) const {
		checkDialogValid();
		HWND ret = GetDlgItem(dialogHWnd, id);
		if (!ret) ThrowLastError();
		return ret;
	}
	// for tjs
	ULONG GetItem(int id) const { return (ULONG)GetItemHWND(id); }

	int GetItemInt(int id) const {
		checkDialogValid();
		BOOL success = FALSE;
		int ret = (int)GetDlgItemInt(dialogHWnd, id, &success, TRUE);
		if (!success) ThrowLastError();
		return ret;
	}
	void SetItemInt(int id, int value) {
		checkDialogValid();
		if (!SetDlgItemInt(dialogHWnd, id, (UINT)value, TRUE)) ThrowLastError();
	}
	StringT GetItemText(int id) const {
		checkDialogValid();
		SizeT len = GetDlgItemTextW(dialogHWnd, id, 0, 0);
		tjs_char *p = new tjs_char[len+1];
		GetDlgItemTextW(dialogHWnd, id, p, len+1);
		StringT ret = p;
		delete[] p;
		return ret;
	}
	void SetItemText(int id, NameT string) {
		checkDialogValid();
		if (!SetDlgItemTextW(dialogHWnd, id, string)) ThrowLastError();
	}

	void SetItemEnabled(int id, bool en) {
		HWND wnd = GetItemHWND(id);
		EnableWindow(wnd, en ? TRUE : FALSE);
	}
	bool GetItemEnabled(int id) {
		HWND wnd = GetItemHWND(id);
		return IsWindowEnabled(wnd) != 0;
	}

	void SetPos(int x, int y) {
		if (dialogHWnd) SetWindowPos(dialogHWnd, 0, x, y, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
	}
	long GetWidth() const {
		if (!dialogHWnd) return 0;
		RECT r;
		GetWindowRect(dialogHWnd, &r);
		return r.right - r.left;
	}
	long GetHeight() const {
		if (!dialogHWnd) return 0;
		RECT r;
		GetWindowRect(dialogHWnd, &r);
		return r.bottom - r.top;
	}

	static tjs_error TJS_INTF_METHOD sendItemMessage(VarT *result, tjs_int numparams, VarT **param, WIN32Dialog *self) {
		if (numparams < 2) return TJS_E_BADPARAMCOUNT;
		int id   = (int)param[0]->AsInteger();
		UINT msg = (UINT)param[1]->AsInteger();
		WPARAM wp = (numparams > 2) ? (WPARAM)param[2]->AsInteger() : 0;
		LPARAM lp = (numparams > 3) ? ((param[3]->Type() == tvtString) ? (LPARAM)param[3]->GetString() : (LPARAM)param[3]->AsInteger()) : 0;
		*result = self->_sendItemMessage(id, msg, wp, lp);
		return  TJS_S_OK;
	}
protected:
	LRESULT _sendItemMessage(int id, UINT msg, WPARAM wparam, LPARAM lparam) {
		checkDialogValid();
		return SendDlgItemMessage(dialogHWnd, id, msg, wparam, lparam);
	}

public:
	VarT GetBaseUnits() const {
		VarT var;
		DictT dict;
		if (dict.IsValid()) {
			LONG units = GetDialogBaseUnits();
			dict.SetValue(TJS_W("h"), LOWORD(units));
			dict.SetValue(TJS_W("v"), HIWORD(units));
			var = dict;
		}
		return var;
	}
	VarT MapRect(VarT in) const {
		checkDialogValid();
		VarT var;
		PropT::DefsT::Tag<LONG> tagInt;
		PropT from(in);
		DictT to;
		if (from.IsValid() && to.IsValid()) {
			RECT rect;
			rect.left   = from.GetValue(TJS_W("left"),   tagInt);
			rect.top    = from.GetValue(TJS_W("top"),    tagInt);
			rect.right  = from.GetValue(TJS_W("right"),  tagInt);
			rect.bottom = from.GetValue(TJS_W("bottom"), tagInt);

			if (!MapDialogRect(dialogHWnd, &rect)) ThrowLastError();
			to.SetValue(TJS_W("left"),   rect.left);
			to.SetValue(TJS_W("top"),    rect.top);
			to.SetValue(TJS_W("right"),  rect.right);
			to.SetValue(TJS_W("bottom"), rect.bottom);
			var = to;
		}
		return var;
	}

	// open (tjs raw callback)
	static tjs_error TJS_INTF_METHOD open(VarT *result, tjs_int numparams, VarT **param, DspT *objthis) {
		WIN32Dialog *self = SelfAdaptorT::GetNativeInstance(objthis); 
		if (!self) return TJS_E_NATIVECLASSCRASH;
		self->objthis = objthis;
		*result = self->_open(numparams > 0 ? *(param[0]) : VarT());
		return TJS_S_OK;
	}

	void close(DWORD id) {
		if (dialogHWnd) EndDialog(dialogHWnd, id);
		dialogHWnd = 0;
	}

	// stubs
	virtual bool onInit(   long msg, long wp, long lp) { return false; }
	virtual bool onCommand(long msg, long wp, long lp) { return false; }

protected:
	// ダイアログを開く
	int _open(VarT win) {
		HWND hwnd = 0;
		if (win.Type() == tvtObject) {
			DspT *obj = win.AsObjectNoAddRef();
			VarT val;
			obj->PropGet(0, TJS_W("HWND"), NULL, &val, obj);
			hwnd = (HWND)((tjs_int)(val));
			HINSTANCE hinst = GetModuleHandle(0);
			icon = LoadIcon(hinst, IDI_APPLICATION);
		}
		int ret = DialogBoxIndirectParam(GetModuleHandle(0), (LPCDLGTEMPLATE)ref, hwnd, (DLGPROC)DlgProc, (LPARAM)this);
		if (ret == -1) ThrowLastError();
		return ret;
	}
	// GetLastErrorのエラーメッセージを取得して投げる
	static inline void ThrowLastError() {
		LPVOID lpMessageBuffer;
		FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
					   NULL, GetLastError(),
					   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					   (LPWSTR)&lpMessageBuffer, 0, NULL);
		ttstr tmp((tjs_char*)lpMessageBuffer);
		LocalFree(lpMessageBuffer);
		TVPThrowExceptionMessage(tmp.c_str());
	}

public:
	// ダイアログプロシージャ
	static LRESULT CALLBACK DlgProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
		WIN32Dialog *inst;
		switch (msg) {
		case WM_INITDIALOG:
			SetWindowLong(hwnd, DWL_USER, (LONG)lparam);
			inst = (WIN32Dialog *)lparam;
			if (inst) {
				inst->dialogHWnd = hwnd;
				if (inst->icon) SendMessage(hwnd, WM_SETICON, 0, (LPARAM)inst->icon);
				return inst->callback(TJS_W("onInit"),    msg, wparam, lparam);
			}
			break;
		case WM_COMMAND:
			if ((inst = (WIN32Dialog *)GetWindowLong(hwnd, DWL_USER)) != 0)
				return inst->callback(TJS_W("onCommand"), msg, wparam, lparam);
			break;
		}
		return FALSE;
	}

	// テンプレート値書き出し用
	template <typename T>
	static void SetValue(PropT *prop, NameT name, T *out) {
		tjs_uint32 hint = 0;
		if (prop->HasValue(name, &hint))
			*out = static_cast<T>(prop->GetValue(name, PropT::DefsT::Tag<tjs_int>(), 0, &hint));
	}
	static void SetSzOrd(PropT *prop, NameT name, DialogTemplate::sz_Or_Ord *out) {
		tjs_uint32 hint = 0;
		tTJSVariantType type;
		if (prop->HasValue(name, &hint, &type)) {
			if (type == tvtString) out->SetString(  prop->GetValue(name, PropT::DefsT::Tag<NameT>(),   0, &hint));
			else                   out->SetID((WORD)prop->GetValue(name, PropT::DefsT::Tag<tjs_int>(), 0, &hint));
		}
	}
	static void SetString(PropT *prop, NameT name, StringT *out) {
		tjs_uint32 hint = 0;
		if (prop->HasValue(name, &hint))
			*out = prop->GetValue(name, PropT::DefsT::Tag<NameT>(), 0, &hint);
	}
};


// ダイアログヘッダ設定クラス
struct Header : public DialogHeader {
	Header() : DialogHeader() {}

	void store(tTJSVariant elmobj) {
		ncbPropAccessor elm(elmobj);
		WIN32Dialog::SetValue( &elm, TJS_W("helpID"),      &helpID);
		WIN32Dialog::SetValue( &elm, TJS_W("exStyle"),     &exStyle);
		WIN32Dialog::SetValue( &elm, TJS_W("style"),       &style);
		WIN32Dialog::SetValue( &elm, TJS_W("x"),           &x);
		WIN32Dialog::SetValue( &elm, TJS_W("y"),           &y);
		WIN32Dialog::SetValue( &elm, TJS_W("cx"),          &cx);
		WIN32Dialog::SetValue( &elm, TJS_W("cy"),          &cy);

		WIN32Dialog::SetSzOrd( &elm, TJS_W("menu"),        &menu);
		WIN32Dialog::SetSzOrd( &elm, TJS_W("windowClass"), &windowClass);

		WIN32Dialog::SetString(&elm, TJS_W("title"),       &title);

		WIN32Dialog::SetValue( &elm, TJS_W("pointSize"),   &pointSize);
		WIN32Dialog::SetValue( &elm, TJS_W("weight"),      &weight);
		WIN32Dialog::SetValue( &elm, TJS_W("italic"),      &italic);
		WIN32Dialog::SetValue( &elm, TJS_W("charset"),     &charset);

		WIN32Dialog::SetString(&elm, TJS_W("typeFace"),    &typeFace);
	}

	void set_dlgItems(WORD n) {        dlgItems = n; }
	WORD get_dlgItems() const { return dlgItems;     }
};

struct Items : public DialogItems {
	Items() : DialogItems() {}

	void store(tTJSVariant elmobj) {
		ncbPropAccessor elm(elmobj);
		WIN32Dialog::SetValue( &elm, TJS_W("helpID"),      &helpID);
		WIN32Dialog::SetValue( &elm, TJS_W("exStyle"),     &exStyle);
		WIN32Dialog::SetValue( &elm, TJS_W("style"),       &style);
		WIN32Dialog::SetValue( &elm, TJS_W("x"),           &x);
		WIN32Dialog::SetValue( &elm, TJS_W("y"),           &y);
		WIN32Dialog::SetValue( &elm, TJS_W("cx"),          &cx);
		WIN32Dialog::SetValue( &elm, TJS_W("cy"),          &cy);
		WIN32Dialog::SetValue( &elm, TJS_W("id"),          &id);

		WIN32Dialog::SetSzOrd( &elm, TJS_W("windowClass"), &windowClass);
		WIN32Dialog::SetSzOrd( &elm, TJS_W("title"),       &title);

		// extraCount/Data は未実装につき指定できない
		//WIN32Dialog::SetValue( &elm, TJS_W("extraCount"),  &extraCount);
	}
};

// テンプレート作成
tjs_error TJS_INTF_METHOD WIN32Dialog::makeTemplate(VarT *result, tjs_int numparams, VarT **param, WIN32Dialog *self)
{
	if (numparams < 1) return TJS_E_BADPARAMCOUNT;

	// Header/Itemsインスタンスの取得とサイズの計算
	BYTE *p = 0;
	Header *head = HeadAdaptorT::GetNativeInstance(param[0]->AsObjectNoAddRef(), true);
	SizeT sz = 0;
	head->Write(p, sz);

	Items *item, **items = new Items*[numparams-1];
	for (int i = 1; i < numparams; i++) {
		items[i-1] = item = ItemAdaptorT::GetNativeInstance(param[i]->AsObjectNoAddRef(), true);
		item->Write(p, sz);
	}
	p = self->CreateBuffer(sz + 2);
	ZeroMemory(p, sz);
	sz = 0;
	head->Write(p, sz);
	for (int i = 1; i < numparams; i++)
		items[i-1]->Write(p, sz);

	delete[] items;
	return TJS_S_OK;
}



NCB_REGISTER_SUBCLASS(Header) {
	Constructor();
	Method(TJS_W("store"), &Class::store);
	Property(TJS_W("dlgItems"), &Class::get_dlgItems, &Class::set_dlgItems);
}
NCB_REGISTER_SUBCLASS(Items) {
	Constructor();
	Method(TJS_W("store"), &Class::store);
}

#define ENUM(n) Variant(#n, (long)n, 0)

NCB_REGISTER_CLASS(WIN32Dialog) {
	NCB_SUBCLASS(Header, Header);
	NCB_SUBCLASS(Items,  Items);

	Constructor<iTJSDispatch2*>(0);

	RawCallback(TJS_W("makeTemplate"), &Class::makeTemplate, 0);
	RawCallback(TJS_W("open"), &Class::open, 0);
	Method(TJS_W("close"),     &Class::close);

	Method(TJS_W("getItem"),         &Class::GetItem);
	Method(TJS_W("setItemInt"),      &Class::SetItemInt);
	Method(TJS_W("getItemInt"),      &Class::GetItemInt);
	Method(TJS_W("setItemText"),     &Class::SetItemText);
	Method(TJS_W("getItemText"),     &Class::GetItemText);
	Method(TJS_W("setItemEnabled"),  &Class::SetItemEnabled);
	Method(TJS_W("getItemEnabled"),  &Class::GetItemEnabled);

	RawCallback(TJS_W("sendItemMessage"), &Class::sendItemMessage, 0);

	Method(TJS_W("getBaseUnits"),    &Class::GetBaseUnits);
	Method(TJS_W("mapRect"),         &Class::MapRect);

	Method(TJS_W("setPos"),          &Class::SetPos);
	Property(TJS_W("width"),         &Class::GetWidth,  (int)0);
	Property(TJS_W("height"),        &Class::GetHeight, (int)0);

	Method(TJS_W("onInit"),    &Class::onInit);
	Method(TJS_W("onCommand"), &Class::onCommand);

	// 定数定義

	// Window Styles
	ENUM(WS_OVERLAPPED);
	ENUM(WS_POPUP);
	ENUM(WS_CHILD);
	ENUM(WS_MINIMIZE);
	ENUM(WS_VISIBLE);
	ENUM(WS_DISABLED);
	ENUM(WS_CLIPSIBLINGS);
	ENUM(WS_CLIPCHILDREN);
	ENUM(WS_MAXIMIZE);
	ENUM(WS_CAPTION);
	ENUM(WS_BORDER);
	ENUM(WS_DLGFRAME);
	ENUM(WS_VSCROLL);
	ENUM(WS_HSCROLL);
	ENUM(WS_SYSMENU);
	ENUM(WS_THICKFRAME);
	ENUM(WS_GROUP);
	ENUM(WS_TABSTOP);

	ENUM(WS_MINIMIZEBOX);
	ENUM(WS_MAXIMIZEBOX);

	ENUM(WS_TILED);
	ENUM(WS_ICONIC);
	ENUM(WS_SIZEBOX);
	ENUM(WS_TILEDWINDOW);

	ENUM(WS_OVERLAPPEDWINDOW);
	ENUM(WS_POPUPWINDOW);
	ENUM(WS_CHILDWINDOW);

	ENUM(WS_EX_DLGMODALFRAME);
	ENUM(WS_EX_NOPARENTNOTIFY);
	ENUM(WS_EX_TOPMOST);
	ENUM(WS_EX_ACCEPTFILES);
	ENUM(WS_EX_TRANSPARENT);
	ENUM(WS_EX_MDICHILD);
	ENUM(WS_EX_TOOLWINDOW);
	ENUM(WS_EX_WINDOWEDGE);
	ENUM(WS_EX_CLIENTEDGE);
	ENUM(WS_EX_CONTEXTHELP);
	ENUM(WS_EX_RIGHT);
	ENUM(WS_EX_LEFT);
	ENUM(WS_EX_RTLREADING);
	ENUM(WS_EX_LTRREADING);
	ENUM(WS_EX_LEFTSCROLLBAR);
	ENUM(WS_EX_RIGHTSCROLLBAR);
	ENUM(WS_EX_CONTROLPARENT);
	ENUM(WS_EX_STATICEDGE);
	ENUM(WS_EX_APPWINDOW);
	ENUM(WS_EX_OVERLAPPEDWINDOW);
	ENUM(WS_EX_PALETTEWINDOW);
#ifdef   WS_EX_LAYERED
	ENUM(WS_EX_LAYERED);
#endif
	ENUM(WS_EX_NOINHERITLAYOUT);
	ENUM(WS_EX_LAYOUTRTL);
#ifdef   WS_EX_COMPOSITED
	ENUM(WS_EX_COMPOSITED);
#endif
#ifdef   WS_EX_NOACTIVATE
	ENUM(WS_EX_NOACTIVATE);
#endif

	// Dialog Box Command IDs
	ENUM(IDOK);
	ENUM(IDCANCEL);
	ENUM(IDABORT);
	ENUM(IDRETRY);
	ENUM(IDIGNORE);
	ENUM(IDYES);
	ENUM(IDNO);
	ENUM(IDCLOSE);
	ENUM(IDHELP);
#ifdef   IDTRYAGAIN
	ENUM(IDTRYAGAIN);
#endif
#ifdef   IDCONTINUE
	ENUM(IDCONTINUE);
#endif
#ifdef   IDTIMEOUT
	ENUM(IDTIMEOUT);
#endif

	// Edit Control Styles
	ENUM(ES_LEFT);
	ENUM(ES_CENTER);
	ENUM(ES_RIGHT);
	ENUM(ES_MULTILINE);
	ENUM(ES_UPPERCASE);
	ENUM(ES_LOWERCASE);
	ENUM(ES_PASSWORD);
	ENUM(ES_AUTOVSCROLL);
	ENUM(ES_AUTOHSCROLL);
	ENUM(ES_NOHIDESEL);
	ENUM(ES_OEMCONVERT);
	ENUM(ES_READONLY);
	ENUM(ES_WANTRETURN);
	ENUM(ES_NUMBER);

	// Edit Control Notification Codes
	ENUM(EN_SETFOCUS);
	ENUM(EN_KILLFOCUS);
	ENUM(EN_CHANGE);
	ENUM(EN_UPDATE);
	ENUM(EN_ERRSPACE);
	ENUM(EN_MAXTEXT);
	ENUM(EN_HSCROLL);
	ENUM(EN_VSCROLL);
#ifdef   EN_ALIGN_LTR_EC
	ENUM(EN_ALIGN_LTR_EC);
#endif
#ifdef   EN_ALIGN_RTL_EC
	ENUM(EN_ALIGN_RTL_EC);
#endif

	// Edit control EM_SETMARGIN parameters
	ENUM(EC_LEFTMARGIN);
	ENUM(EC_RIGHTMARGIN);
	ENUM(EC_USEFONTINFO);
	// wParam of EM_GET/SETIMESTATUS
#ifdef   EMSIS_COMPOSITIONSTRING
	ENUM(EMSIS_COMPOSITIONSTRING);
#endif
	// lParam for EMSIS_COMPOSITIONSTRING
#ifdef   EIMES_GETCOMPSTRATONCE
	ENUM(EIMES_GETCOMPSTRATONCE);
#endif
#ifdef   EIMES_CANCELCOMPSTRINFOCUS
	ENUM(EIMES_CANCELCOMPSTRINFOCUS);
#endif
#ifdef   EIMES_COMPLETECOMPSTRKILLFOCUS
	ENUM(EIMES_COMPLETECOMPSTRKILLFOCUS);
#endif

	// Edit Control Messages
	ENUM(EM_GETSEL);
	ENUM(EM_SETSEL);
	ENUM(EM_GETRECT);
	ENUM(EM_SETRECT);
	ENUM(EM_SETRECTNP);
	ENUM(EM_SCROLL);
	ENUM(EM_LINESCROLL);
	ENUM(EM_SCROLLCARET);
	ENUM(EM_GETMODIFY);
	ENUM(EM_SETMODIFY);
	ENUM(EM_GETLINECOUNT);
	ENUM(EM_LINEINDEX);
	ENUM(EM_SETHANDLE);
	ENUM(EM_GETHANDLE);
	ENUM(EM_GETTHUMB);
	ENUM(EM_LINELENGTH);
	ENUM(EM_REPLACESEL);
	ENUM(EM_GETLINE);
	ENUM(EM_LIMITTEXT);
	ENUM(EM_CANUNDO);
	ENUM(EM_UNDO);
	ENUM(EM_FMTLINES);
	ENUM(EM_LINEFROMCHAR);
	ENUM(EM_SETTABSTOPS);
	ENUM(EM_SETPASSWORDCHAR);
	ENUM(EM_EMPTYUNDOBUFFER);
	ENUM(EM_GETFIRSTVISIBLELINE);
	ENUM(EM_SETREADONLY);
	ENUM(EM_SETWORDBREAKPROC);
	ENUM(EM_GETWORDBREAKPROC);
	ENUM(EM_GETPASSWORDCHAR);
	ENUM(EM_SETMARGINS);
	ENUM(EM_GETMARGINS);
	ENUM(EM_SETLIMITTEXT);
	ENUM(EM_GETLIMITTEXT);
	ENUM(EM_POSFROMCHAR);
	ENUM(EM_CHARFROMPOS);
#ifdef   EM_SETIMESTATUS
	ENUM(EM_SETIMESTATUS);
#endif
#ifdef   EM_GETIMESTATUS
	ENUM(EM_GETIMESTATUS);
#endif

	// EDITWORDBREAKPROC code values
	ENUM(WB_LEFT);
	ENUM(WB_RIGHT);
	ENUM(WB_ISDELIMITER);

	// Button Control Styles
	ENUM(BS_PUSHBUTTON);
	ENUM(BS_DEFPUSHBUTTON);
	ENUM(BS_CHECKBOX);
	ENUM(BS_AUTOCHECKBOX);
	ENUM(BS_RADIOBUTTON);
	ENUM(BS_3STATE);
	ENUM(BS_AUTO3STATE);
	ENUM(BS_GROUPBOX);
	ENUM(BS_USERBUTTON);
	ENUM(BS_AUTORADIOBUTTON);
#ifdef   BS_PUSHBOX
	ENUM(BS_PUSHBOX);
#endif
	ENUM(BS_OWNERDRAW);
#ifdef   BS_TYPEMASK
	ENUM(BS_TYPEMASK);
#endif
	ENUM(BS_LEFTTEXT);
	ENUM(BS_TEXT);
	ENUM(BS_ICON);
	ENUM(BS_BITMAP);
	ENUM(BS_LEFT);
	ENUM(BS_RIGHT);
	ENUM(BS_CENTER);
	ENUM(BS_TOP);
	ENUM(BS_BOTTOM);
	ENUM(BS_VCENTER);
	ENUM(BS_PUSHLIKE);
	ENUM(BS_MULTILINE);
	ENUM(BS_NOTIFY);
	ENUM(BS_FLAT);
	ENUM(BS_RIGHTBUTTON);

	// User Button Notification Codes
	ENUM(BN_CLICKED);
	ENUM(BN_PAINT);
	ENUM(BN_HILITE);
	ENUM(BN_UNHILITE);
	ENUM(BN_DISABLE);
	ENUM(BN_DOUBLECLICKED);
	ENUM(BN_PUSHED);
	ENUM(BN_UNPUSHED);
	ENUM(BN_DBLCLK);
	ENUM(BN_SETFOCUS);
	ENUM(BN_KILLFOCUS);

	// Button Control Messages
	ENUM(BM_GETCHECK);
	ENUM(BM_SETCHECK);
	ENUM(BM_GETSTATE);
	ENUM(BM_SETSTATE);
	ENUM(BM_SETSTYLE);
	ENUM(BM_CLICK);
	ENUM(BM_GETIMAGE);
	ENUM(BM_SETIMAGE);
#ifdef   BM_SETDONTCLICK
	ENUM(BM_SETDONTCLICK);
#endif
	ENUM(BST_UNCHECKED);
	ENUM(BST_CHECKED);
	ENUM(BST_INDETERMINATE);
	ENUM(BST_PUSHED);
	ENUM(BST_FOCUS);

	// Static Control Constants
	ENUM(SS_LEFT);
	ENUM(SS_CENTER);
	ENUM(SS_RIGHT);
	ENUM(SS_ICON);
	ENUM(SS_BLACKRECT);
	ENUM(SS_GRAYRECT);
	ENUM(SS_WHITERECT);
	ENUM(SS_BLACKFRAME);
	ENUM(SS_GRAYFRAME);
	ENUM(SS_WHITEFRAME);
	ENUM(SS_USERITEM);
	ENUM(SS_SIMPLE);
	ENUM(SS_LEFTNOWORDWRAP);
	ENUM(SS_OWNERDRAW);
	ENUM(SS_BITMAP);
	ENUM(SS_ENHMETAFILE);
	ENUM(SS_ETCHEDHORZ);
	ENUM(SS_ETCHEDVERT);
	ENUM(SS_ETCHEDFRAME);
	ENUM(SS_TYPEMASK);
#ifdef   SS_REALSIZECONTROL
	ENUM(SS_REALSIZECONTROL);
#endif
	ENUM(SS_NOPREFIX);
	ENUM(SS_NOTIFY);
	ENUM(SS_CENTERIMAGE);
	ENUM(SS_RIGHTJUST);
	ENUM(SS_REALSIZEIMAGE);
	ENUM(SS_SUNKEN);
#ifdef   SS_EDITCONTROL
	ENUM(SS_EDITCONTROL);
#endif
	ENUM(SS_ENDELLIPSIS);
	ENUM(SS_PATHELLIPSIS);
	ENUM(SS_WORDELLIPSIS);
	ENUM(SS_ELLIPSISMASK);

	// Static Control Mesages
	ENUM(STM_SETICON);
	ENUM(STM_GETICON);
	ENUM(STM_SETIMAGE);
	ENUM(STM_GETIMAGE);
	ENUM(STN_CLICKED);
	ENUM(STN_DBLCLK);
	ENUM(STN_ENABLE);
	ENUM(STN_DISABLE);
#ifdef   STM_MSGMAX
	ENUM(STM_MSGMAX);
#endif

	// Dialog Styles
	ENUM(DS_ABSALIGN);
	ENUM(DS_SYSMODAL);
	ENUM(DS_LOCALEDIT);
	ENUM(DS_SETFONT);
	ENUM(DS_MODALFRAME);
	ENUM(DS_NOIDLEMSG);
	ENUM(DS_SETFOREGROUND);
	ENUM(DS_3DLOOK);
	ENUM(DS_FIXEDSYS);
	ENUM(DS_NOFAILCREATE);
	ENUM(DS_CONTROL);
	ENUM(DS_CENTER);
	ENUM(DS_CENTERMOUSE);
	ENUM(DS_CONTEXTHELP);
	ENUM(DS_SHELLFONT);
#ifdef   DS_USEPIXELS
	ENUM(DS_USEPIXELS);
#endif

	ENUM(DM_GETDEFID);
	ENUM(DM_SETDEFID);
	ENUM(DM_REPOSITION);
	// Returned in HIWORD() of DM_GETDEFID result if msg is supported
	ENUM(DC_HASDEFID);

	// Dialog Codes
	ENUM(DLGC_WANTARROWS);
	ENUM(DLGC_WANTTAB);
	ENUM(DLGC_WANTALLKEYS);
	ENUM(DLGC_WANTMESSAGE);
	ENUM(DLGC_HASSETSEL);
	ENUM(DLGC_DEFPUSHBUTTON);
	ENUM(DLGC_UNDEFPUSHBUTTON);
	ENUM(DLGC_RADIOBUTTON);
	ENUM(DLGC_WANTCHARS);
	ENUM(DLGC_STATIC);
	ENUM(DLGC_BUTTON);

	// Listbox Return Values
	ENUM(LB_CTLCODE);
	ENUM(LB_OKAY);
	ENUM(LB_ERR);
	ENUM(LB_ERRSPACE);

	// Listbox Notification Codes
	ENUM(LBN_ERRSPACE);
	ENUM(LBN_SELCHANGE);
	ENUM(LBN_DBLCLK);
	ENUM(LBN_SELCANCEL);
	ENUM(LBN_SETFOCUS);
	ENUM(LBN_KILLFOCUS);

	ENUM(LB_ADDSTRING);
	ENUM(LB_INSERTSTRING);
	ENUM(LB_DELETESTRING);
	ENUM(LB_SELITEMRANGEEX);
	ENUM(LB_RESETCONTENT);
	ENUM(LB_SETSEL);
	ENUM(LB_SETCURSEL);
	ENUM(LB_GETSEL);
	ENUM(LB_GETCURSEL);
	ENUM(LB_GETTEXT);
	ENUM(LB_GETTEXTLEN);
	ENUM(LB_GETCOUNT);
	ENUM(LB_SELECTSTRING);
	ENUM(LB_DIR);
	ENUM(LB_GETTOPINDEX);
	ENUM(LB_FINDSTRING);
	ENUM(LB_GETSELCOUNT);
	ENUM(LB_GETSELITEMS);
	ENUM(LB_SETTABSTOPS);
	ENUM(LB_GETHORIZONTALEXTENT);
	ENUM(LB_SETHORIZONTALEXTENT);
	ENUM(LB_SETCOLUMNWIDTH);
	ENUM(LB_ADDFILE);
	ENUM(LB_SETTOPINDEX);
	ENUM(LB_GETITEMRECT);
	ENUM(LB_GETITEMDATA);
	ENUM(LB_SETITEMDATA);
	ENUM(LB_SELITEMRANGE);
	ENUM(LB_SETANCHORINDEX);
	ENUM(LB_GETANCHORINDEX);
	ENUM(LB_SETCARETINDEX);
	ENUM(LB_GETCARETINDEX);
	ENUM(LB_SETITEMHEIGHT);
	ENUM(LB_GETITEMHEIGHT);
	ENUM(LB_FINDSTRINGEXACT);
	ENUM(LB_SETLOCALE);
	ENUM(LB_GETLOCALE);
	ENUM(LB_SETCOUNT);
	ENUM(LB_INITSTORAGE);
	ENUM(LB_ITEMFROMPOINT);
#ifdef   LB_MULTIPLEADDSTRING
	ENUM(LB_MULTIPLEADDSTRING);
#endif
#ifdef   LB_GETLISTBOXINFO
	ENUM(LB_GETLISTBOXINFO);
#endif
#ifdef   LB_MSGMAX
	ENUM(LB_MSGMAX);
#endif

	// Listbox Styles
	ENUM(LBS_NOTIFY);
	ENUM(LBS_SORT);
	ENUM(LBS_NOREDRAW);
	ENUM(LBS_MULTIPLESEL);
	ENUM(LBS_OWNERDRAWFIXED);
	ENUM(LBS_OWNERDRAWVARIABLE);
	ENUM(LBS_HASSTRINGS);
	ENUM(LBS_USETABSTOPS);
	ENUM(LBS_NOINTEGRALHEIGHT);
	ENUM(LBS_MULTICOLUMN);
	ENUM(LBS_WANTKEYBOARDINPUT);
	ENUM(LBS_EXTENDEDSEL);
	ENUM(LBS_DISABLENOSCROLL);
	ENUM(LBS_NODATA);
	ENUM(LBS_NOSEL);
#ifdef   LBS_COMBOBOX
	ENUM(LBS_COMBOBOX);
#endif
	ENUM(LBS_STANDARD);

	// Combo Box return Values
	ENUM(CB_OKAY);
	ENUM(CB_ERR);
	ENUM(CB_ERRSPACE);

	// Combo Box Notification Codes
	ENUM(CBN_ERRSPACE);
	ENUM(CBN_SELCHANGE);
	ENUM(CBN_DBLCLK);
	ENUM(CBN_SETFOCUS);
	ENUM(CBN_KILLFOCUS);
	ENUM(CBN_EDITCHANGE);
	ENUM(CBN_EDITUPDATE);
	ENUM(CBN_DROPDOWN);
	ENUM(CBN_CLOSEUP);
	ENUM(CBN_SELENDOK);
	ENUM(CBN_SELENDCANCEL);

	// Combo Box styles
	ENUM(CBS_SIMPLE);
	ENUM(CBS_DROPDOWN);
	ENUM(CBS_DROPDOWNLIST);
	ENUM(CBS_OWNERDRAWFIXED);
	ENUM(CBS_OWNERDRAWVARIABLE);
	ENUM(CBS_AUTOHSCROLL);
	ENUM(CBS_OEMCONVERT);
	ENUM(CBS_SORT);
	ENUM(CBS_HASSTRINGS);
	ENUM(CBS_NOINTEGRALHEIGHT);
	ENUM(CBS_DISABLENOSCROLL);
	ENUM(CBS_UPPERCASE);
	ENUM(CBS_LOWERCASE);

	// Combo Box messages
	ENUM(CB_GETEDITSEL);
	ENUM(CB_LIMITTEXT);
	ENUM(CB_SETEDITSEL);
	ENUM(CB_ADDSTRING);
	ENUM(CB_DELETESTRING);
	ENUM(CB_DIR);
	ENUM(CB_GETCOUNT);
	ENUM(CB_GETCURSEL);
	ENUM(CB_GETLBTEXT);
	ENUM(CB_GETLBTEXTLEN);
	ENUM(CB_INSERTSTRING);
	ENUM(CB_RESETCONTENT);
	ENUM(CB_FINDSTRING);
	ENUM(CB_SELECTSTRING);
	ENUM(CB_SETCURSEL);
	ENUM(CB_SHOWDROPDOWN);
	ENUM(CB_GETITEMDATA);
	ENUM(CB_SETITEMDATA);
	ENUM(CB_GETDROPPEDCONTROLRECT);
	ENUM(CB_SETITEMHEIGHT);
	ENUM(CB_GETITEMHEIGHT);
	ENUM(CB_SETEXTENDEDUI);
	ENUM(CB_GETEXTENDEDUI);
	ENUM(CB_GETDROPPEDSTATE);
	ENUM(CB_FINDSTRINGEXACT);
	ENUM(CB_SETLOCALE);
	ENUM(CB_GETLOCALE);
	ENUM(CB_GETTOPINDEX);
	ENUM(CB_SETTOPINDEX);
	ENUM(CB_GETHORIZONTALEXTENT);
	ENUM(CB_SETHORIZONTALEXTENT);
	ENUM(CB_GETDROPPEDWIDTH);
	ENUM(CB_SETDROPPEDWIDTH);
	ENUM(CB_INITSTORAGE);
#ifdef   CB_MULTIPLEADDSTRING
	ENUM(CB_MULTIPLEADDSTRING);
#endif
#ifdef   CB_GETCOMBOBOXINFO
	ENUM(CB_GETCOMBOBOXINFO);
#endif
#ifdef   CB_MSGMAX
	ENUM(CB_MSGMAX);
#endif

	// Scroll Bar Styles
	ENUM(SBS_HORZ);
	ENUM(SBS_VERT);
	ENUM(SBS_TOPALIGN);
	ENUM(SBS_LEFTALIGN);
	ENUM(SBS_BOTTOMALIGN);
	ENUM(SBS_RIGHTALIGN);
	ENUM(SBS_SIZEBOXTOPLEFTALIGN);
	ENUM(SBS_SIZEBOXBOTTOMRIGHTALIGN);
	ENUM(SBS_SIZEBOX);
	ENUM(SBS_SIZEGRIP);

	// Scroll bar messages
	ENUM(SBM_SETPOS);
	ENUM(SBM_GETPOS);
	ENUM(SBM_SETRANGE);
	ENUM(SBM_SETRANGEREDRAW);
	ENUM(SBM_GETRANGE);
	ENUM(SBM_ENABLE_ARROWS);
	ENUM(SBM_SETSCROLLINFO);
	ENUM(SBM_GETSCROLLINFO);
#ifdef   SBM_GETSCROLLBARINFO
	ENUM(SBM_GETSCROLLBARINFO);
#endif
	ENUM(SIF_RANGE);
	ENUM(SIF_PAGE);
	ENUM(SIF_POS);
	ENUM(SIF_DISABLENOSCROLL);
	ENUM(SIF_TRACKPOS);
	ENUM(SIF_ALL);

	// Font Weights
	ENUM(FW_DONTCARE);
	ENUM(FW_THIN);
	ENUM(FW_EXTRALIGHT);
	ENUM(FW_LIGHT);
	ENUM(FW_NORMAL);
	ENUM(FW_MEDIUM);
	ENUM(FW_SEMIBOLD);
	ENUM(FW_BOLD);
	ENUM(FW_EXTRABOLD);
	ENUM(FW_HEAVY);

	// Control classes
	Variant(TJS_W("BUTTON"),    0x0080, 0);
	Variant(TJS_W("EDIT"),      0x0081, 0);
	Variant(TJS_W("STATIC"),    0x0082, 0);
	Variant(TJS_W("LISTBOX"),   0x0083, 0);
	Variant(TJS_W("SCROLLBAR"), 0x0084, 0);
	Variant(TJS_W("COMBOBOX"),  0x0085, 0);
}

