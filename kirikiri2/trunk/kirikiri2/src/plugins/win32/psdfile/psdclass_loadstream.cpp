#include "psdclass.h"

#ifndef LOAD_MEMORY

#include "psdparse/psdparse.h"

/**
 * Stream 用の Iterator
 */
class PSDIterator : public std::iterator<std::random_access_iterator_tag, const unsigned char>
{
public:

	typedef size_t diff_t;

	PSDIterator() : _psd(0), _pos(0), _size(0) {
		count++;
	}

	/**
	 * コンストラクタ
	 * @param srream 参照ストリーム
	 * @param begin true なら冒頭、false なら末尾
	 */
	PSDIterator(PSD* psd, bool begin) : _psd(psd), _pos(0), _size(0) {
		_size = psd->mStreamSize;
		_pos = begin ? 0 : _size;
		count++;
	}

	// コピーコンストラクタ
	PSDIterator(const PSDIterator& o) {
		_psd = o._psd;
		_pos = o._pos;
		_size = o._size;
		count++;
	}

	// 代入演算子
	PSDIterator &operator=(const PSDIterator &o) {
		_psd  = o._psd;
		_pos  = o._pos;
		_size = o._size;
		return *this;
	}
	
	// デストラクタ
	virtual ~PSDIterator() {
		count--;
	}

	// イテレータを進める(前置)
	PSDIterator& operator++() {
		_pos++;
		if (_pos > _size) _pos = _size;
		return *this;
	}

	// イテレータを進める（後置)
	PSDIterator operator++(int) {
		PSDIterator ret = *this;
		_pos++;
		if (_pos > _size) _pos = _size;
		return ret;
	}

	PSDIterator& operator+=(diff_t n) {
		_pos += n;
		if (_pos > _size) _pos = _size;
		return *this;
	}

	PSDIterator operator+(diff_t n) {
		PSDIterator ret = *this;
		ret += n;
		return ret;
	}
	
	// イテレータを戻す(前置)
	PSDIterator& operator--() {
		_pos--;
		if (_pos < 0) _pos = 0;
		return *this;
	}

	// イテレータを戻す（後置)
	PSDIterator operator--(int) {
		PSDIterator ret = *this;
		_pos--;
		if (_pos < 0) _pos = 0;
		return ret;
	}

	PSDIterator& operator-=(diff_t n) {
		_pos -= n;
		if (_pos < 0) _pos = 0;
		return *this;
	}

	PSDIterator operator-(diff_t n) {
		PSDIterator ret = *this;
		ret -= n;
		return ret;
	}

	// 読み取り
	const unsigned char& operator*() {
		_psd->getStreamValue(_pos, value);
		return value;
	}

	// 差分
	diff_t operator-(const PSDIterator& b) const {
		return (diff_t)(_pos - b._pos);
	}
	
	// イテレータの一致判定
	bool operator==(const PSDIterator& o) const {
		return o._pos == _pos;
	}
	// イテレータの一致判定
	bool operator!=(const PSDIterator& o) const {
		return o._pos != _pos;
	}

	bool operator<(const PSDIterator& o) const {
		return _pos < o._pos;
	}
	bool operator>(const PSDIterator& o) const {
		return _pos > o._pos;
	}
	bool operator<=(const PSDIterator& o) const {
		return _pos <= o._pos;
	}
	bool operator>=(const PSDIterator& o) const {
		return _pos >= o._pos;
	}
	

	
private:
	PSD *_psd;
	tTVInteger _size; //< ストリームサイズ保持用
	tTVInteger _pos;  //< 参照位置
	unsigned char value;
	static int count;
};

int PSDIterator::count = 0;

void
PSD::clearStream()
{
	if (pStream) {
		pStream->Release();
		pStream = 0;
	}
	mStreamSize = 0;
}

bool
PSD::loadStream(const ttstr &filename)
{
	clearData();

	// ストリームのまま
	isLoaded = false;
	pStream = TVPCreateIStream(filename, TJS_BS_READ);
	if (pStream) {

		STATSTG stat;
		pStream->Stat(&stat, STATFLAG_NONAME);
		mStreamSize = stat.cbSize.QuadPart;

		PSDIterator begin(this, true);
		PSDIterator end(this, false);
		psd::Parser<PSDIterator> parser(*this);
		bool r = parse(begin , end,  parser);
		if (r && begin == end) {
			dprint("succeeded\n");
			isLoaded = processParsed();
		}
		if (!isLoaded) {
			clearData();
		}
	}
	return isLoaded;	
}

void
PSD::getStreamValue(const tTVInteger &pos, unsigned char &value)
{
	if (pos >=0 && pos < mStreamSize) {
		LARGE_INTEGER n;
		n.QuadPart = pos;
		pStream->Seek(n, STREAM_SEEK_SET, 0);
		ULONG read;
		pStream->Read(&value, 1, &read);
	} else {
		value = 0;
	}
}

#endif
