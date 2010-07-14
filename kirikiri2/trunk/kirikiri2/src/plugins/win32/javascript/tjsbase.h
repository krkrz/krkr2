#ifndef __TJSBASE_H__
#define __TJSBASE_H__

class TJSBase {
public:
	enum Type {
		TYPE_OBJECT,
		TYPE_INSTANCE
	};
	TJSBase(Type type) : type(type) {}
	virtual ~TJSBase() {};
	bool isType(Type type) { return this->type == type; }
protected:
	Type type;
};

#endif
