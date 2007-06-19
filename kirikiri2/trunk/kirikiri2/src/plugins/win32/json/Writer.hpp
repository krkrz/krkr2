/**
 * 出力処理用インターフェース
 */
class IWriter {
public:
	int indent;
	IWriter() {
		indent = 0;
	}
	virtual ~IWriter(){};
	virtual void write(const tjs_char *str) = 0;
	virtual void write(tjs_char ch) = 0;
	virtual void write(tTVReal) = 0;
	virtual void write(tTVInteger) = 0;

	inline void newline() {
		write((tjs_char)'\n');
		for (int i=0;i<indent;i++) {
			write((tjs_char)' ');
		}
	}

	inline void addIndent() {
		indent++;
		newline();
	}

	inline void delIndent() {
		indent--;
		newline();
	}
};

/**
 * 文字列出力
 */
class IStringWriter : public IWriter {

public:
	ttstr buf;
	/**
	 * コンストラクタ
	 */
	IStringWriter(){};

	virtual void write(const tjs_char *str) {
		buf += str;
	}

	virtual void write(tjs_char ch) {
		buf += ch;
	}

	virtual void write(tTVReal num) {
		tTJSVariantString *str = TJSRealToString(num);
		buf += str;
		str->Release();
	}

	virtual void write(tTVInteger num) {
		tTJSVariantString *str = TJSIntegerToString(num);
		buf += str;
		str->Release();
	}
};

/**
 * ファイル出力
 */
class IFileWriter : public IWriter {

	/// 出力バッファ
	ttstr buf;
	/// 出力ストリーム
	iTJSTextWriteStream *stream;

public:

	/**
	 * コンストラクタ
	 */
	IFileWriter(const tjs_char *filename) {
		stream = TVPCreateTextStreamForWrite(filename, TJS_W(""));
	}

	/**
	 * デストラクタ
	 */
	~IFileWriter() {
		if (stream) {
			if (buf.length() > 0) {
				stream->Write(buf);
				buf.Clear();
			}
			stream->Destruct();
		}
	}

	virtual void write(const tjs_char *str) {
		if (stream) {
			buf += str;
			if (buf.length() >= 1024) {
				stream->Write(buf);
				buf.Clear();
			}
		}
	}

	virtual void write(tjs_char ch) {
		buf += ch;
	}

	virtual void write(tTVReal num) {
		tTJSVariantString *str = TJSRealToString(num);
		buf += str;
		str->Release();
	}

	virtual void write(tTVInteger num) {
		tTJSVariantString *str = TJSIntegerToString(num);
		buf += str;
		str->Release();
	}
};
