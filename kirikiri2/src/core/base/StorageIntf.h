//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000-2005  W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// Universal Storage System
//---------------------------------------------------------------------------
#ifndef StorageIntfH
#define StorageIntfH

#include "tjsNative.h"
#include "tjsHashSearch.h"
#include <vector>

//---------------------------------------------------------------------------
// archive delimiter
//---------------------------------------------------------------------------
extern tjs_char  TVPArchiveDelimiter; //  = '>';



//---------------------------------------------------------------------------
// utilities
//---------------------------------------------------------------------------
ttstr TVPStringFromBMPUnicode(const tjs_uint16 *src, tjs_int maxlen = -1);
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// tTVPArchive base archive class
//---------------------------------------------------------------------------
class tTVPArchive
{
private:
	tjs_uint RefCount;

public:
	//-- constructor
	tTVPArchive(const ttstr & name)
		{ ArchiveName = name; Init = false; RefCount = 1; }
	virtual ~tTVPArchive() { ; }

	//-- AddRef and Release
	void AddRef() { RefCount++; }
	void Release() { if(RefCount == 1) delete this; else RefCount--; }

	//-- must be implemented by delivered class
	virtual tjs_uint GetCount() = 0;
	virtual ttstr GetName(tjs_uint idx) = 0;

	virtual tTJSBinaryStream * CreateStreamByIndex(tjs_uint idx) = 0;

	//-- others, implemented in this class
private:

	tTJSHashTable<ttstr, tjs_uint, tTJSHashFunc<ttstr>, 1024> Hash;
	bool Init;
	ttstr ArchiveName;

public:
	static void NormalizeInArchiveStorageName(ttstr & name);

private:
	void AddToHash();
public:
	tTJSBinaryStream * CreateStream(const ttstr & name);
	bool IsExistent(const ttstr & name);
};
//---------------------------------------------------------------------------




/*[*/
//---------------------------------------------------------------------------
// iTVPStorageMedia
//---------------------------------------------------------------------------
/*
	abstract class for managing media ( like file: http: etc.)
*/
/*]*/
#if 0
/*[*/
	// for plug-in
class tTJSBinaryStream;
/*]*/
#endif
/*[*/
//---------------------------------------------------------------------------
class iTVPStorageLister // callback class for GetListAt
{
public:
	virtual void Add(const ttstr &file) = 0;
};
//---------------------------------------------------------------------------
class iTVPStorageMedia
{
public:
	virtual void AddRef() = 0;
	virtual void Release() = 0;

	virtual ttstr GetName() = 0;
		// returns media name like "file", "http" etc.

//	virtual ttstr IsCaseSensitive() = 0;
		// returns whether this media is case sensitive or not

	virtual void NormalizeDomainName(ttstr &name) = 0;
		// normalize domain name according with the media's rule

	virtual void NormalizePathName(ttstr &name) = 0;
		// normalize path name according with the media's rule

	// "name" below is normalized but does not contain media, eg.
	// not "media://domain/path" but "domain/path"

	virtual bool CheckExistentStorage(const ttstr &name) = 0;
		// check file existence

	virtual tTJSBinaryStream * Open(const ttstr & name, tjs_uint32 flags) = 0;
		// open a storage and return a tTJSBinaryStream instance.
		// name does not contain in-archive storage name but
		// is normalized.

	virtual void GetListAt(const ttstr &name, iTVPStorageLister * lister) = 0;
		// list files at given place

	virtual ttstr GetLocallyAccessibleName(const ttstr &name) = 0;
		// basically the same as above,
		// check wether given name is easily accessible from local OS filesystem.
		// if true, returns local OS native name. otherwise returns an empty string.
};
//---------------------------------------------------------------------------
/*]*/



//---------------------------------------------------------------------------
// must be implemented in each platform
//---------------------------------------------------------------------------
extern tTVPArchive * TVPOpenArchive(const ttstr & name);
	// open archive and return tTVPArchive instance.

TJS_EXP_FUNC_DEF(ttstr, TVPGetTemporaryName, ());
	// retrieve file name to store temporary data ( must be unique, local name )

TJS_EXP_FUNC_DEF(ttstr, TVPGetAppPath, ());
	// retrieve program path, in normalized storage name

void TVPPreNormalizeStorageName(ttstr &name);
		// called by TVPNormalizeStorageName before it process the storage name.
		// user may pass the OS's native filename to the TVP storage system,
		// so that this function must convert it to the TVP storage name rules.

iTVPStorageMedia * TVPCreateFileMedia();
	// create basic default "file:" storage media
/*
extern void TVPPreNormalizeStorageName(ttstr &name);

extern tTJSBinaryStream * TVPOpenStream(const ttstr & name, tjs_uint32 flags);
	// open a storage and return a tTJSBinaryStream instance.
	// name does not contain in-archive storage name but
	// is normalized.

extern bool TVPCheckExistentStorage(const ttstr &name);
	// check file existence

extern void TVPGetStorageListAt(const ttstr &name, std::vector<ttstr> & list);

extern ttstr TVPGetMediaCurrent(const ttstr & name);
extern void TVPSetMediaCurrent(const ttstr & name, const ttstr & dir);

extern ttstr TVPGetNativeName(const ttstr &name);
	// retrieve OS native name

extern ttstr TVPGetLocallyAccessibleName(const ttstr &name);
	// check wether given name is easily accessible from local OS filesystem.
	// if true, returns local OS native name. otherwise returns an empty string.

*/
extern bool TVPRemoveFile(const ttstr &name);
	// remove local file ( "name" is a local *native* name )
	// this must not throw an exception ( return false if error )
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
// implementation in this unit
//---------------------------------------------------------------------------
TJS_EXP_FUNC_DEF(void, TVPRegisterStorageMedia, (iTVPStorageMedia *media));
	// register storage media
TJS_EXP_FUNC_DEF(void, TVPUnregisterStorageMedia, (iTVPStorageMedia *media));
	// remove storage media


extern tTJSBinaryStream * TVPCreateStream(const ttstr & name, tjs_uint32 flags = 0);
	// open "name" and return tTJSBinaryStream instance.
	// name will be local storage, network storage, in-archive storage, etc...

TJS_EXP_FUNC_DEF(bool, TVPIsExistentStorageNoSearch, (const ttstr &name));
	// if "name" is exists, return true. otherwise return false.
	// this does not search any auto search path.

TJS_EXP_FUNC_DEF(bool, TVPIsExistentStorageNoSearchNoNormalize, (const ttstr &name));

TJS_EXP_FUNC_DEF(ttstr, TVPNormalizeStorageName, (const ttstr & name));

TJS_EXP_FUNC_DEF(void, TVPSetCurrentDirectory, (const ttstr & name));
	// set system current directory.
	// directory must end with path delimiter '/',
	// or archive delimiter '>'.

TJS_EXP_FUNC_DEF(void, TVPGetLocalName, (ttstr &name));

ttstr TVPGetLocallyAccessibleName(const ttstr &name);


TJS_EXP_FUNC_DEF(ttstr, TVPExtractStorageExt, (const ttstr & name));
	// extract "name"'s extension and return it.


TJS_EXP_FUNC_DEF(ttstr, TVPExtractStorageName, (const ttstr & name));
	// extract "name"'s storage name ( excluding path ) and return it.

TJS_EXP_FUNC_DEF(ttstr, TVPExtractStoragePath, (const ttstr & name));
	// extract "name"'s path ( including last delimiter ) and return it.

TJS_EXP_FUNC_DEF(ttstr, TVPChopStorageExt, (const ttstr & name));
	// chop storage's extension and return it.
	// extensition delimiter '.' will not be held.


TJS_EXP_FUNC_DEF(void, TVPAddAutoPath, (const ttstr & name));
	// add given path to auto search path

TJS_EXP_FUNC_DEF(void, TVPRemoveAutoPath, (const ttstr &name));
	// remove given path from auto search path

TJS_EXP_FUNC_DEF(ttstr, TVPGetPlacedPath, (const ttstr & name));
	// search path and return the path which the "name" is placed.

extern ttstr TVPSearchPlacedPath(const ttstr & name);
	// the same as TVPGetPlacedPath, except for rising exception when the storage
	// is not found.

TJS_EXP_FUNC_DEF(bool, TVPIsExistentStorage, (const ttstr &name));
	// if "name" is exists, return true. otherwise return false.
	// this searches auto search path.

TJS_EXP_FUNC_DEF(void, TVPClearStorageCaches, ());
	// clear all internal storage related caches.


extern tjs_uint TVPSegmentCacheLimit; // XP3 segment cache limit, in bytes.

//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
// tTJSNC_Storages : TJS Storages class
//---------------------------------------------------------------------------
class tTJSNC_Storages : public tTJSNativeClass
{
	typedef tTJSNativeClass inherited;

public:
	tTJSNC_Storages();
	static tjs_uint32 ClassID;

protected:
	tTJSNativeInstance *CreateNativeInstance();
};
//---------------------------------------------------------------------------
extern tTJSNativeClass * TVPCreateNativeClass_Storages();
//---------------------------------------------------------------------------



#endif
