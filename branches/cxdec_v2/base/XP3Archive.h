//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000-2004  W.Dee <dee@kikyou.info>

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// XP3 virtual file system support
//---------------------------------------------------------------------------
#ifndef XP3ArchiveH
#define XP3ArchiveH


#include "StorageIntf.h"



/*[*/
//---------------------------------------------------------------------------
// Extraction filter related
//---------------------------------------------------------------------------
#pragma pack(push, 4)
struct tTVPXP3ExtractionFilterInfo
{
	const tjs_uint SizeOfSelf; // structure size of tTVPXP3ExtractionFilterInfo itself
	const tjs_uint64 Offset; // offset of the buffer data in uncompressed stream position
	void * Buffer; // target data buffer
	const tjs_uint BufferSize; // buffer size in bytes pointed by "Buffer"
	const ttstr & FileName; // In-archive filename (normalized)
	const ttstr & ArchiveName; // Archive filename (may be normalized)

	tTVPXP3ExtractionFilterInfo(tjs_uint64 offset, void *buffer,
		tjs_uint buffersize, const ttstr &filename, const ttstr & archivename) :
			Offset(offset), Buffer(buffer), BufferSize(buffersize),
			FileName(filename), ArchiveName(archivename),
			SizeOfSelf(sizeof(tTVPXP3ExtractionFilterInfo)) {;}
};
#pragma pack(pop)

typedef void (TJS_INTF_METHOD * tTVPXP3ArchiveExtractionFilter)(
	tTVPXP3ExtractionFilterInfo *info);


/*]*/
//---------------------------------------------------------------------------
TJS_EXP_FUNC_DEF(void, TVPSetXP3ArchiveExtractionFilter, (tTVPXP3ArchiveExtractionFilter filter));
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
// tTVPXP3Archive  : XP3 ( TVP's native archive format ) Implmentation
//---------------------------------------------------------------------------
extern bool TVPIsXP3Archive(const ttstr &name); // check XP3 archive
extern void TVPClearXP3SegmentCache(); // clear XP3 segment cache
//---------------------------------------------------------------------------
struct tTVPXP3ArchiveSegment
{
	tjs_uint64 Start;  // start position in archive storage
	tjs_uint64 Offset; // offset in in-archive storage (in uncompressed offset)
	tjs_uint64 OrgSize; // original segment (uncompressed) size
	tjs_uint64 ArcSize; // in-archive segment (compressed) size
	bool IsCompressed; // is compressed ?
};
//---------------------------------------------------------------------------
class tTVPXP3Archive : public tTVPArchive
{
	ttstr Name;

	struct tArchiveItem
	{
		ttstr Name;
		tjs_uint64 OrgSize; // original ( uncompressed ) size
		tjs_uint64 ArcSize; // in-archive size
		std::vector<tTVPXP3ArchiveSegment> Segments;
	};

	tjs_int Count;

	std::vector<tArchiveItem> ItemVector;
public:
	tTVPXP3Archive(const ttstr & name);
	~tTVPXP3Archive();

	tjs_uint GetCount() { return Count; }
	const ttstr & GetName(tjs_uint idx) const { return ItemVector[idx].Name; }
	ttstr GetName(tjs_uint idx) { return ItemVector[idx].Name; }

	const ttstr & GetName() const { return Name; }

	tTJSBinaryStream * CreateStreamByIndex(tjs_uint idx);

private:
	static bool FindChunk(const tjs_uint8 *data, const tjs_uint8 * name,
		tjs_uint &start, tjs_uint &size);
	static tjs_int16 ReadI16FromMem(const tjs_uint8 *mem);
	static tjs_int32 ReadI32FromMem(const tjs_uint8 *mem);
	static tjs_int64 ReadI64FromMem(const tjs_uint8 *mem);
};
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
// tTVPXP2ArchiveStream  : XP3 In-Archive Stream Implmentation
//---------------------------------------------------------------------------
class tTVPSegmentData;
class tTVPXP3ArchiveStream : public tTJSBinaryStream
{
	tTVPXP3Archive * Owner;

	tjs_int StorageIndex; // index in archive

	std::vector<tTVPXP3ArchiveSegment> * Segments;
	tTJSBinaryStream * Stream;
	tjs_uint64 OrgSize; // original storage size

	tjs_int CurSegmentNum;
	tTVPXP3ArchiveSegment *CurSegment;
		// currently opened segment ( NULL for not opened )

	tjs_int LastOpenedSegmentNum;

	tjs_uint64 CurPos; // current position in absolute file position

	tjs_uint64 SegmentRemain; // remain bytes in current segment
	tjs_uint64 SegmentPos; // offset from current segment's start

	tTVPSegmentData *SegmentData; // uncompressed segment data

	bool SegmentOpened;

public:
	tTVPXP3ArchiveStream(tTVPXP3Archive *owner, tjs_int storageindex,
		std::vector<tTVPXP3ArchiveSegment> *segments, tTJSBinaryStream *stream,
			tjs_uint64 orgsize);
	~tTVPXP3ArchiveStream();

private:
	void EnsureSegment(); // ensure accessing to current segment
	void SeekToPosition(tjs_uint64 pos); // open segment at 'pos' and seek
	bool OpenNextSegment();


public:
	tjs_uint64 TJS_INTF_METHOD Seek(tjs_int64 offset, tjs_int whence);
	tjs_uint TJS_INTF_METHOD Read(void *buffer, tjs_uint read_size);
	tjs_uint TJS_INTF_METHOD Write(const void *buffer, tjs_uint write_size);
	tjs_uint64 TJS_INTF_METHOD GetSize();

};
//---------------------------------------------------------------------------





#endif
