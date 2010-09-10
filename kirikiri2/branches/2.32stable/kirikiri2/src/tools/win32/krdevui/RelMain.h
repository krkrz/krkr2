//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------

#ifndef RelMainH
#define RelMainH
//---------------------------------------------------------------------------
#define TVP_ST_UNKNOWN_SIZE ((tvp_uint64)(tvp_int64)-1)
#define TVP_ST_READ 0
#define TVP_ST_WRITE 1
#define TVP_ST_APPEND 2
#define TVP_ST_UPDATE 3
#define TVP_ST_ACCESS_MASK 0x0f

#define TVP_ST_SEEK_SET 0
#define TVP_ST_SEEK_CUR 1
#define TVP_ST_SEEK_END 2


//---------------------------------------------------------------------------
// tTVPStream base stream class
//---------------------------------------------------------------------------
class tTVPStream
{
private:
public:
	//-- constructor/destructor
	tTVPStream() {;}
	virtual ~tTVPStream() {;}

	//-- must implement
	virtual unsigned __int64 Seek(__int64 offset, int whence) = 0;
		/* if error, position is not changed */

	//-- optionally to implement
	virtual unsigned int Read(void *buffer, unsigned int read_size) { return 0; }
		/* returns actually read size */

	virtual unsigned int Write(const void *buffer, unsigned int write_size) { return 0; }
		/* returns actually written size */

	virtual void SetEndOfStorage(); // the default behavior is raising a exception
		/* if error, raises exception */

	//-- should implement for higher performance
	virtual unsigned __int64 GetSize()
	{
		unsigned __int64 orgpos = GetPosition();
		unsigned __int64 size = Seek(0, SEEK_END);
		Seek(orgpos, SEEK_SET);
		return size;
	}

	//-- below functions are utitlity functions. child classes should not
	//-- override these.
	unsigned __int64 GetPosition()
	{
		return Seek(0, SEEK_CUR);
	}

	unsigned __int64 SetPosition(unsigned __int64 pos)
	{
		return Seek(pos, SEEK_SET);
	}

	void ReadBuffer(void *buffer, unsigned int read_size);
	void WriteBuffer(const void *buffer, unsigned int write_size);
	/* ReadBuffer and WriteBuffer raise exception when specified size was not
	  read/witten. */

	void WriteInt64C(__int64 n);
	void WriteInt64(__int64 n);
	void WriteInt32(__int32 n);
	void WriteInt16(__int16 n);
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// XP3 filter related
//---------------------------------------------------------------------------
extern bool XP3EncDLLAvailable;
extern void (__stdcall * XP3ArchiveAttractFilter_v2_org)(
	unsigned __int32 hash,
	unsigned __int64 offset, void * buffer, long bufferlen);
extern void (__stdcall * XP3ArchiveAttractFilter_v2)(
	unsigned __int32 hash,
	unsigned __int64 offset, void * buffer, long bufferlen);
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// tTVPLocalFileStream
//---------------------------------------------------------------------------
class tTVPLocalFileStream : public tTVPStream
{
private:
	HANDLE Handle;
	char *FileName;
	bool UseEncryption;
	unsigned __int32 Salt;

public:
	tTVPLocalFileStream(const char *name,  unsigned int flag, bool useencryption = false,
		unsigned __int32 salt = 0);
	~tTVPLocalFileStream();

	unsigned __int64 Seek(__int64 offset, int whence);

	unsigned int Read(void *buffer, unsigned int read_size);
	unsigned int Write(const void *buffer, unsigned int write_size);

	void SetEndOfStorage();

	unsigned __int64 GetSize();

	HANDLE GetHandle() { return Handle; }
};
//---------------------------------------------------------------------------

#endif
