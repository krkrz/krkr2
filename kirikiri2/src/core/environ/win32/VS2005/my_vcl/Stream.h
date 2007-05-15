#pragma once
//----------------------------------------------------------------------------
class TStream
{
public:
	enum { soFromBeginning, soFromCurrent, soFromEnd };

	virtual ~TStream();

	virtual int Read(void*, unsigned int) = 0;
	virtual int Write(const void*, unsigned int) = 0;
	virtual void ReadBuffer(void * Buffer, int Count );
	virtual int WriteBuffer(const void*, unsigned int);

	PROPERTY_abstract(int, Size); // 純仮想
	PROPERTY_abstract(int, Position); // 純仮想
};


//----------------------------------------------------------------------------
class TFileStream : public TStream
{
private:
	wxInputStream  * m_wxInputStream;
	wxOutputStream * m_wxOutputStream;
public:
	TFileStream( const AnsiString&, Word mode );
	virtual ~TFileStream();

	virtual int Read(void*, unsigned int);
	virtual int Write(const void*, unsigned int);
	int CopyFrom(const TFileStream*, unsigned int);

	PROPERTY(int, Size); // 仮想
	PROPERTY(int, Position); // 仮想
};

