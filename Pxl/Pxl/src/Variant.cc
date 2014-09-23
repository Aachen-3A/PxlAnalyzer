//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2009-2012 Martin Erdmann   -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------

#include "Pxl/Pxl/interface/pxl/core/Variant.hh"
#include "Pxl/Pxl/interface/pxl/core/Tokenizer.hh"

#include <algorithm>

namespace pxl
{

Variant::Variant() :
		type(TYPE_NONE)
{
}

Variant::~Variant()
{
	clear();
}

Variant::Variant(const Variant& a) :
		type(TYPE_NONE)
{
	copy(a);
}

Variant::Variant(const char *s)
{
	data.__String = new std::string(s);
	type = TYPE_STRING;
}

Variant::Variant(const Serializable &a)
{
	data.__Serializable = a.clone();
	type = TYPE_SERIALIZABLE;
}

Variant::Variant(const Serializable *a)
{
	data.__Serializable = a->clone();
	type = TYPE_SERIALIZABLE;
}

Variant::Variant(const std::vector<Variant> &a)
{
	data.__vec = new std::vector<Variant>(a);
	type = TYPE_VECTOR;
}

void Variant::clear()
{
	if (type == TYPE_STRING)
	{
		safe_delete(data.__String);
	}
	else if (type == TYPE_BASIC3VECTOR)
	{
		safe_delete(data.__Basic3Vector);
	}
	else if (type == TYPE_LORENTZVECTOR)
	{
		safe_delete(data.__LorentzVector);
	}
	else if (type == TYPE_SERIALIZABLE)
	{
		safe_delete(data.__Serializable);
	}
	else if (type == TYPE_VECTOR)
	{
		safe_delete(data.__vec);
	}
	type = TYPE_NONE;
}

void Variant::check(const Type t) const
{
	if (type != t)
		throw bad_conversion(type, t);
}

void Variant::check(const Type t)
{
	if (type == TYPE_NONE)
	{
		memset(&data, 0, sizeof(data));
		switch (t)
		{
		case TYPE_STRING:
			data.__String = new std::string;
			break;
		case TYPE_BASIC3VECTOR:
			data.__Basic3Vector = new Basic3Vector;
			break;
		case TYPE_LORENTZVECTOR:
			data.__LorentzVector = new LorentzVector;
			break;
		case TYPE_VECTOR:
			data.__vec = new vector_t;
			break;
		default:
			break;
		}
		type = t;
	}
	else if (type != t)
	{
		throw bad_conversion(type, t);
	}
}

const std::type_info& Variant::getTypeInfo() const
{
	if (type == TYPE_BOOL)
	{
		const std::type_info &ti = typeid(data.__Bool);
		return ti;
	}
	else if (type == TYPE_CHAR)
	{
		const std::type_info &ti = typeid(data.__Char);
		return ti;
	}
	else if (type == TYPE_UCHAR)
	{
		const std::type_info &ti = typeid(data.__UChar);
		return ti;
	}
	else if (type == TYPE_INT16)
	{
		const std::type_info &ti = typeid(data.__Int16);
		return ti;
	}
	else if (type == TYPE_UINT16)
	{
		const std::type_info &ti = typeid(data.__UInt16);
		return ti;
	}
	else if (type == TYPE_INT32)
	{
		const std::type_info &ti = typeid(data.__Int32);
		return ti;
	}
	else if (type == TYPE_UINT32)
	{
		const std::type_info &ti = typeid(data.__UInt32);
		return ti;
	}
	else if (type == TYPE_INT64)
	{
		const std::type_info &ti = typeid(data.__Int64);
		return ti;
	}
	else if (type == TYPE_UINT64)
	{
		const std::type_info &ti = typeid(data.__UInt64);
		return ti;
	}
	else if (type == TYPE_FLOAT)
	{
		const std::type_info &ti = typeid(data.__Float);
		return ti;
	}
	else if (type == TYPE_DOUBLE)
	{
		const std::type_info &ti = typeid(data.__Double);
		return ti;
	}
	else if (type == TYPE_STRING)
	{
		const std::type_info &ti = typeid(*data.__String);
		return ti;
	}
	else if (type == TYPE_SERIALIZABLE)
	{
		const std::type_info &ti = typeid(data.__Serializable);
		return ti;
	}
	else if (type == TYPE_BASIC3VECTOR)
	{
		const std::type_info &ti = typeid(data.__Basic3Vector);
		return ti;
	}
	else if (type == TYPE_LORENTZVECTOR)
	{
		const std::type_info &ti = typeid(data.__LorentzVector);
		return ti;
	}
	else if (type == TYPE_VECTOR)
	{
		const std::type_info &ti = typeid(*data.__vec);
		return ti;
	}
	else
	{
		const std::type_info &ti = typeid(0);
		return ti;
	}

}

const char *Variant::getTypeName(Type type)
{
	if (type == TYPE_NONE)
	{
		return "none";
	}
	else if (type == TYPE_BOOL)
	{
		return "bool";
	}
	else if (type == TYPE_CHAR)
	{
		return "char";
	}
	else if (type == TYPE_UCHAR)
	{
		return "uchar";
	}
	else if (type == TYPE_INT16)
	{
		return "int16";
	}
	else if (type == TYPE_UINT16)
	{
		return "uint16";
	}
	else if (type == TYPE_INT32)
	{
		return "int32";
	}
	else if (type == TYPE_UINT32)
	{
		return "uint32";
	}
	else if (type == TYPE_INT64)
	{
		return "int64";
	}
	else if (type == TYPE_UINT64)
	{
		return "uint64";
	}
	else if (type == TYPE_FLOAT)
	{
		return "float";
	}
	else if (type == TYPE_DOUBLE)
	{
		return "double";
	}
	else if (type == TYPE_SERIALIZABLE)
	{
		return "Serializable";
	}
	else if (type == TYPE_BASIC3VECTOR)
	{
		return "Basic3Vector";
	}
	else if (type == TYPE_LORENTZVECTOR)
	{
		return "LorentzVector";
	}
	else if (type == TYPE_STRING)
	{
		return "string";
	}
	else if (type == TYPE_VECTOR)
	{
		return "vector";
	}
	else
	{
		return "unknown";
	}
}

Variant::Type Variant::toType(const std::string &name)
{
	if (name == "none")
	{
		return TYPE_NONE;
	}
	else if (name == "bool")
	{
		return TYPE_BOOL;
	}
	else if (name == "char")
	{
		return TYPE_CHAR;
	}
	else if (name == "uchar")
	{
		return TYPE_UCHAR;
	}
	else if (name == "int16")
	{
		return TYPE_INT16;
	}
	else if (name == "uint16")
	{
		return TYPE_UINT16;
	}
	else if (name == "int32")
	{
		return TYPE_INT32;
	}
	else if (name == "uint32")
	{
		return TYPE_UINT32;
	}
	else if (name == "int64")
	{
		return TYPE_INT64;
	}
	else if (name == "uint64")
	{
		return TYPE_UINT64;
	}
	else if (name == "float")
	{
		return TYPE_FLOAT;
	}
	else if (name == "double")
	{
		return TYPE_DOUBLE;
	}
	else if (name == "Serializable")
	{
		return TYPE_SERIALIZABLE;
	}
	else if (name == "Basic3Vector")
	{
		return TYPE_BASIC3VECTOR;
	}
	else if (name == "LorentzVector")
	{
		return TYPE_LORENTZVECTOR;
	}
	else if (name == "string")
	{
		return TYPE_STRING;
	}
	else if (name == "vector")
	{
		return TYPE_STRING;
	}
	else
	{
		return TYPE_NONE;
	}
}

bool Variant::operator ==(const Variant &a) const
{
	if (type != a.type)
		return false;
	if (type == TYPE_BOOL)
	{
		return (data.__Bool == a.data.__Bool);
	}
	else if (type == TYPE_CHAR)
	{
		return (data.__Char == a.data.__Char);
	}
	else if (type == TYPE_UCHAR)
	{
		return (data.__UChar == a.data.__UChar);
	}
	else if (type == TYPE_INT16)
	{
		return (data.__Int16 == a.data.__Int16);
	}
	else if (type == TYPE_UINT16)
	{
		return (data.__UInt16 == a.data.__UInt16);
	}
	else if (type == TYPE_INT32)
	{
		return (data.__Int32 == a.data.__Int32);
	}
	else if (type == TYPE_UINT32)
	{
		return (data.__UInt32 == a.data.__UInt32);
	}
	else if (type == TYPE_INT64)
	{
		return (data.__Int64 == a.data.__Int64);
	}
	else if (type == TYPE_UINT64)
	{
		return (data.__UInt64 == a.data.__UInt64);
	}
	else if (type == TYPE_FLOAT)
	{
		return (data.__Float == a.data.__Float);
	}
	else if (type == TYPE_DOUBLE)
	{
		return (data.__Double == a.data.__Double);
	}
	else if (type == TYPE_SERIALIZABLE)
	{
		return (data.__Serializable == a.data.__Serializable);
	}
	else if (type == TYPE_BASIC3VECTOR)
	{
		return ((*data.__Basic3Vector) == (*a.data.__Basic3Vector));
	}
	else if (type == TYPE_STRING)
	{
		return (*data.__String == *a.data.__String);
	}
	else if (type == TYPE_VECTOR)
	{
		return (*data.__vec == *a.data.__vec);
	}
	else
	{
		throw std::runtime_error("compare operator not implemented");
	}
}

std::string Variant::toString() const
{
	if (type == TYPE_STRING)
		return *data.__String;

	std::stringstream sstr;
	if (type == TYPE_BOOL)
	{
		sstr << data.__Bool;
	}
	else if (type == TYPE_CHAR)
	{
		sstr << data.__Char;
	}
	else if (type == TYPE_UCHAR)
	{
		sstr << data.__UChar;
	}
	else if (type == TYPE_INT16)
	{
		sstr << data.__Int16;
	}
	else if (type == TYPE_UINT16)
	{
		sstr << data.__UInt16;
	}
	else if (type == TYPE_INT32)
	{
		sstr << data.__Int32;
	}
	else if (type == TYPE_UINT32)
	{
		sstr << data.__UInt32;
	}
	else if (type == TYPE_INT64)
	{
		sstr << data.__Int64;
	}
	else if (type == TYPE_UINT64)
	{
		sstr << data.__UInt64;
	}
	else if (type == TYPE_FLOAT)
	{
		sstr << data.__Float;
	}
	else if (type == TYPE_DOUBLE)
	{
		sstr << data.__Double;
	}
	else if (type == TYPE_BASIC3VECTOR)
	{
		sstr << data.__Basic3Vector->getX() << " " << data.__Basic3Vector->getY()
				<< " " << data.__Basic3Vector->getZ();
	}
	else if (type == TYPE_LORENTZVECTOR)
	{
		sstr << data.__LorentzVector->getX() << " "
				<< data.__LorentzVector->getY() << " "
				<< data.__LorentzVector->getZ() << " "
				<< data.__LorentzVector->getE();
	}
	else if (type == TYPE_VECTOR)
	{
		sstr << *data.__vec;
	}

	return sstr.str();
}

Variant Variant::fromString(const std::string &str, Type type)
{
	std::stringstream sstr(str);
	switch (type)
	{
	case TYPE_BOOL:
	{
		std::string upperstr(str);
		std::transform(upperstr.begin(), upperstr.end(), upperstr.begin(),
				(int(*)(int))toupper);if
(		upperstr == "YES")
		return Variant(true);
		else if (upperstr == "NO")
		return Variant(false);
		if (upperstr == "TRUE")
			return Variant(true);
		else if (upperstr == "FALSE")
			return Variant(false);
		if (upperstr == "1")
			return Variant(true);
		else if (upperstr == "0")
			return Variant(false);
		throw bad_conversion(type, TYPE_BOOL);
	}
	case TYPE_CHAR:
	{
		char c;
		sstr >> c;
		return Variant(c);
	}
	case TYPE_UCHAR:
	{
		unsigned char c;
		sstr >> c;
		return Variant(c);
	}
	case TYPE_INT16:
	{
		int16_t c;
		sstr >> c;
		return Variant(c);
	}
	case TYPE_UINT16:
	{
		uint16_t c;
		sstr >> c;
		return Variant(c);
	}
	case TYPE_INT32:
	{
		int32_t c;
		sstr >> c;
		return Variant(c);
	}
	case TYPE_UINT32:
	{
		uint32_t c;
		sstr >> c;
		return Variant(c);
	}
	case TYPE_INT64:
	{
		int64_t c;
		sstr >> c;
		return Variant(c);
	}
	case TYPE_UINT64:
	{
		uint64_t c;
		sstr >> c;
		return Variant(c);
	}
	case TYPE_FLOAT:
	{
		float c;
		sstr >> c;
		return Variant(c);
	}
	case TYPE_DOUBLE:
	{
		double c;
		sstr >> c;
		return Variant(c);
	}
	case TYPE_BASIC3VECTOR:
	{
		double d;
		Basic3Vector v;
		sstr >> d;
		v.setX(d);
		sstr >> d;
		v.setY(d);
		sstr >> d;
		v.setZ(d);
		return Variant(v);
	}
	case TYPE_LORENTZVECTOR:
	{
		double d;
		LorentzVector v;
		sstr >> d;
		v.setX(d);
		sstr >> d;
		v.setY(d);
		sstr >> d;
		v.setZ(d);
		sstr >> d;
		v.setE(d);
		return Variant(v);
	}
	case TYPE_STRING:
	{
		return Variant(str);
	}
	case TYPE_VECTOR:
	{
		std::vector<Variant> stringVectorValue;
		Tokenizer tok;
		tok.setText(str);
		tok.setCharType('[', Tokenizer::WHITESPACE);
		tok.setCharType(']', Tokenizer::WHITESPACE);
		tok.setCharType('(', Tokenizer::WHITESPACE);
		tok.setCharType(')', Tokenizer::WHITESPACE);
		tok.setCharType('\"', Tokenizer::LITERAL);
		tok.setCharType('\'', Tokenizer::LITERAL);
		tok.setCharType(',', Tokenizer::DELIM);
		while (tok.hasNext())
		{
			std::string s = tok.next();
			stringVectorValue.push_back(s);
		}
		return Variant(stringVectorValue);
	}
	default:
		throw std::runtime_error("pxl::Variant::fromString: unknown type");
	}

}

bool Variant::operator !=(const Variant &a) const
{
	if (type != a.type)
		return true;

	switch (type)
	{
	case TYPE_BOOL:
		return (data.__Bool != a.data.__Bool);
	case TYPE_CHAR:
		return (data.__Char != a.data.__Char);
	case TYPE_UCHAR:
		return (data.__UChar != a.data.__UChar);
	case TYPE_INT16:
		return (data.__Int16 != a.data.__Int16);
	case TYPE_UINT16:
		return (data.__UInt16 == a.data.__UInt16);
	case TYPE_INT32:
		return (data.__Int32 == a.data.__Int32);
	case TYPE_UINT32:
		return (data.__UInt32 == a.data.__UInt32);
	case TYPE_INT64:
		return (data.__Int64 == a.data.__Int64);
	case TYPE_UINT64:
		return (data.__UInt64 == a.data.__UInt64);
	case TYPE_FLOAT:
		return (data.__Float == a.data.__Float);
	case TYPE_DOUBLE:
		return (data.__Double == a.data.__Double);
	case TYPE_SERIALIZABLE:
		return (data.__Serializable == a.data.__Serializable);
	case TYPE_BASIC3VECTOR:
		return ((*data.__Basic3Vector) == (*a.data.__Basic3Vector));
	case TYPE_STRING:
		return (*data.__String == *a.data.__String);
	case TYPE_VECTOR:
		return (*data.__vec != *a.data.__vec);
	default:
		throw std::runtime_error("compare operator not implemented");
	}
}

void Variant::serialize(const OutputStream &out) const
{
	out.write((char) type);
	switch (type)
	{
	case TYPE_BOOL:
		out.write(data.__Bool);
		break;
	case TYPE_CHAR:
		out.write(data.__Char);
		break;
	case TYPE_UCHAR:
		out.write(data.__UChar);
		break;
	case TYPE_INT16:
		out.write(data.__Int16);
		break;
	case TYPE_UINT16:
		out.write(data.__UInt16);
		break;
	case TYPE_INT32:
		out.write(data.__Int32);
		break;
	case TYPE_UINT32:
		out.write(data.__UInt32);
		break;
	case TYPE_INT64:
		out.write(data.__Int64);
		break;
	case TYPE_UINT64:
		out.write(data.__UInt64);
		break;
	case TYPE_FLOAT:
		out.writeFloat(data.__Float);
		break;
	case TYPE_DOUBLE:
		out.writeDouble(data.__Double);
		break;
	case TYPE_STRING:
		out.writeString(*data.__String);
		break;
	case TYPE_BASIC3VECTOR:
		data.__Basic3Vector->serialize(out);
		break;
	case TYPE_LORENTZVECTOR:
		data.__LorentzVector->serialize(out);
		break;
	case TYPE_VECTOR:
	{
		uint32_t s = data.__vec->size();
		out.write(s);
		for (size_t i = 0; i < s; i++)
			data.__vec->at(i).serialize(out);
		break;
	}
	default:
		break;
	}
}

void Variant::deserialize(const InputStream &in)
{
	unsigned char t;
	in.read(t);
	check(static_cast<Type>(t));

	switch (type)
	{
	case TYPE_BOOL:
		in.readBool(data.__Bool);
		break;
	case TYPE_CHAR:
		in.read(data.__Char);
		break;
	case TYPE_UCHAR:
		in.read(data.__UChar);
		break;
	case TYPE_INT16:
		in.read(data.__Int16);
		break;
	case TYPE_UINT16:
		in.read(data.__UInt16);
		break;
	case TYPE_INT32:
		in.read(data.__Int32);
		break;
	case TYPE_UINT32:
		in.read(data.__UInt32);
		break;
	case TYPE_INT64:
		in.read(data.__Int64);
		break;
	case TYPE_UINT64:
		in.read(data.__UInt64);
		break;
	case TYPE_FLOAT:
		in.readFloat(data.__Float);
		break;
	case TYPE_DOUBLE:
		in.readDouble(data.__Double);
		break;
	case TYPE_STRING:
		in.readString(*data.__String);
		break;
	case TYPE_BASIC3VECTOR:
		data.__Basic3Vector->deserialize(in);
		break;
	case TYPE_LORENTZVECTOR:
		data.__LorentzVector->deserialize(in);
		break;
	case TYPE_VECTOR:
	{
		uint32_t s = 0;
		in.read(s);
		data.__vec->resize(s);
		for (size_t i = 0; i < s; i++)
			data.__vec->at(i).deserialize(in);
		break;
	}
	default:
		break;
	}
}

void Variant::copy(const Variant &a)
{
	Type t = a.type;
	if (t == TYPE_BOOL)
	{
		operator =(a.data.__Bool);
	}
	else if (t == TYPE_CHAR)
	{
		operator =(a.data.__Char);
	}
	else if (t == TYPE_UCHAR)
	{
		operator =(a.data.__UChar);
	}
	else if (t == TYPE_INT16)
	{
		operator =(a.data.__Int16);
	}
	else if (t == TYPE_UINT16)
	{
		operator =(a.data.__UInt16);
	}
	else if (t == TYPE_INT32)
	{
		operator =(a.data.__Int32);
	}
	else if (t == TYPE_UINT32)
	{
		operator =(a.data.__UInt32);
	}
	else if (t == TYPE_INT64)
	{
		operator =(a.data.__Int64);
	}
	else if (t == TYPE_UINT64)
	{
		operator =(a.data.__UInt64);
	}
	else if (t == TYPE_FLOAT)
	{
		operator =(a.data.__Float);
	}
	else if (t == TYPE_DOUBLE)
	{
		operator =(a.data.__Double);
	}
	else if (t == TYPE_STRING)
	{
		operator =(*a.data.__String);
	}
	else if (t == TYPE_SERIALIZABLE)
	{
		operator =(a.data.__Serializable);
	}
	else if (t == TYPE_BASIC3VECTOR)
	{
		operator =(*a.data.__Basic3Vector);
	}
	else if (t == TYPE_LORENTZVECTOR)
	{
		operator =(*a.data.__LorentzVector);
	}
	else if (t == TYPE_VECTOR)
	{
		operator =(*a.data.__vec);
	}
	else
	{
		type = TYPE_NONE;
	}
}

bool Variant::toBool() const
{
	switch (type)
	{
	case TYPE_BOOL:
		return data.__Bool;
		break;
	case TYPE_CHAR:
		return data.__Char != 0;
		break;
	case TYPE_UCHAR:
		return data.__UChar != 0;
		break;
	case TYPE_INT16:
		return data.__Int16 != 0;
		break;
	case TYPE_UINT16:
		return data.__UInt16 != 0;
		break;
	case TYPE_INT32:
		return data.__Int32 != 0;
		break;
	case TYPE_UINT32:
		return data.__UInt32 != 0;
		break;
	case TYPE_INT64:
		return data.__Int64 != 0;
		break;
	case TYPE_UINT64:
		return data.__UInt64 != 0;
		break;
	case TYPE_SERIALIZABLE:
		return data.__Serializable != 0;
		break;
	case TYPE_STRING:
	{
		std::string upperstr(*data.__String);
		std::transform(upperstr.begin(), upperstr.end(), upperstr.begin(),
				(int(*)(int))toupper);if
(		upperstr == "YES")
		return true;
		else if (upperstr == "NO")
		return false;
		if (upperstr == "TRUE")
			return true;
		else if (upperstr == "FALSE")
			return false;
		if (upperstr == "1")
			return true;
		else if (upperstr == "0")
			return false;
		else
			throw bad_conversion(type, TYPE_BOOL);
	}
		break;
	case TYPE_VECTOR:
		return data.__vec->size() != 0;
		break;
	case TYPE_FLOAT:
	case TYPE_DOUBLE:
	case TYPE_BASIC3VECTOR:
	case TYPE_LORENTZVECTOR:
	case TYPE_NONE:
		throw bad_conversion(type, TYPE_BOOL);
		break;
	}
	return false;
}

#define INT_CASE(from_var, from_type, to_type, to) \
	case Variant::from_type:\
		if (data.__##from_var < std::numeric_limits<to>::min() || data.__##from_var > std::numeric_limits<to>::max())\
			throw bad_conversion(type, to_type);\
		else\
			return static_cast<to>(data.__##from_var);\
		break;\

#define INT_FUNCTION(to_type, fun, to) \
to Variant::fun() const { \
	switch (type) { \
	case Variant::TYPE_BOOL: \
		return data.__Bool ? 1 : 0; \
		break; \
	INT_CASE(Char, TYPE_CHAR, to_type, to) \
	INT_CASE(UChar, TYPE_UCHAR, to_type, to) \
	INT_CASE(Int16, TYPE_INT16, to_type, to) \
	INT_CASE(UInt16, TYPE_UINT16, to_type, to) \
	INT_CASE(Int32, TYPE_INT32, to_type, to) \
	INT_CASE(UInt32, TYPE_UINT32, to_type, to) \
	INT_CASE(Int64, TYPE_INT64, to_type, to) \
	INT_CASE(UInt64, TYPE_UINT64, to_type, to) \
	INT_CASE(Float, TYPE_FLOAT, to_type, to) \
	INT_CASE(Double, TYPE_DOUBLE, to_type, to) \
	case Variant::TYPE_STRING: \
		{ \
		long l = atol(data.__String->c_str()); \
		if (l < std::numeric_limits<to>::min() || l > std::numeric_limits<to>::max()) \
			throw bad_conversion(type, to_type); \
		else \
			return l; \
		} \
		break; \
	case Variant::TYPE_SERIALIZABLE: \
	case Variant::TYPE_BASIC3VECTOR: \
	case Variant::TYPE_LORENTZVECTOR: \
	case Variant::TYPE_VECTOR: \
	case Variant::TYPE_NONE: \
		throw bad_conversion(type, TYPE_INT16); \
		break;\
	}\
	return 0;\
}

INT_FUNCTION( TYPE_CHAR, toChar, char)
INT_FUNCTION( TYPE_UCHAR, toUChar, unsigned char)
INT_FUNCTION( TYPE_INT16, toInt16, int16_t)
INT_FUNCTION( TYPE_UINT16, toUInt16, uint16_t)
INT_FUNCTION( TYPE_INT32, toInt32, int32_t)
INT_FUNCTION( TYPE_UINT32, toUInt32, uint32_t)
INT_FUNCTION( TYPE_INT64, toInt64, int64_t)
INT_FUNCTION( TYPE_UINT64, toUInt64, uint64_t)

PXL_DLL_EXPORT std::ostream& operator <<(std::ostream& os, const Variant &v)
{
	switch (v.getType())
	{
	case Variant::TYPE_BOOL:
		os << v.asBool();
		break;
	case Variant::TYPE_CHAR:
		os << v.asChar();
		break;
	case Variant::TYPE_UCHAR:
		os << v.asUChar();
		break;
	case Variant::TYPE_INT16:
		os << v.asInt16();
		break;
	case Variant::TYPE_UINT16:
		os << v.asUInt16();
		break;
	case Variant::TYPE_INT32:
		os << v.asInt32();
		break;
	case Variant::TYPE_UINT32:
		os << v.asUInt32();
		break;
	case Variant::TYPE_INT64:
		os << v.asInt64();
		break;
	case Variant::TYPE_UINT64:
		os << v.asUInt64();
		break;
	case Variant::TYPE_FLOAT:
		os << v.asFloat();
		break;
	case Variant::TYPE_DOUBLE:
		os << v.asDouble();
		break;
	case Variant::TYPE_STRING:
		os << v.asString();
		break;
	case Variant::TYPE_SERIALIZABLE:
		v.asSerializable().print(0, os);
		break;
	case Variant::TYPE_BASIC3VECTOR:
		os << v.asBasic3Vector();
		break;
	case Variant::TYPE_VECTOR:
	{
		const std::vector<Variant> &vec = v.asVector();
		os << "(";
		for (size_t i = 0; i < vec.size(); i++)
		{
			if (i != 0)
				os << ", ";
			os << vec[i];
		}
		os << ")";
		break;
	}
	default:
		break;
	}
	return os;
}

// std::vector
bool Variant::isVector() const
{
	return (type == TYPE_VECTOR);
}

Variant::operator std::vector<Variant> &()
{
	check(TYPE_VECTOR);
	return *data.__vec;
}

Variant::operator const std::vector<Variant> &() const
{
	check(TYPE_VECTOR);
	return *data.__vec;
}

std::vector<Variant> &Variant::asVector()
{
	check(TYPE_VECTOR);
	return *data.__vec;
}

const std::vector<Variant> &Variant::asVector() const
{
	check(TYPE_VECTOR);
	return *data.__vec;
}

Variant Variant::fromVector(const std::vector<Variant> &s)
{
	return Variant(s);
}

Variant &Variant::operator =(const std::vector<Variant> &a)
{
	if (type != TYPE_VECTOR)
	{
		clear();
	}
	data.__vec = new std::vector<Variant>(a);
	type = TYPE_VECTOR;
	return *this;
}

bool Variant::operator ==(const std::vector<Variant> &a) const
{
	if (type != TYPE_VECTOR)
		return false;
	return *data.__vec == a;
}

bool Variant::operator !=(const std::vector<Variant> &a) const
{
	check(TYPE_VECTOR);
	return *data.__vec != a;
}

Variant &Variant::operator[](size_t i)
{
	check(TYPE_VECTOR);
	return (*data.__vec)[i];
}

const Variant &Variant::operator[](size_t i) const
{
	check(TYPE_VECTOR);
	return (*data.__vec)[i];
}

void Variant::resize(size_t i)
{
	check(TYPE_VECTOR);
	return data.__vec->resize(i);
}

float Variant::toFloat() const
{
	if (type == TYPE_CHAR)
	{
		return static_cast<float>(data.__Char);
	}
	else if (type == TYPE_UCHAR)
	{
		return static_cast<float>(data.__UChar);
	}
	else if (type == TYPE_INT16)
	{
		return static_cast<float>(data.__Int16);
	}
	else if (type == TYPE_UINT16)
	{
		return static_cast<float>(data.__UInt16);
	}
	else if (type == TYPE_INT32)
	{
		return static_cast<float>(data.__Int32);
	}
	else if (type == TYPE_UINT32)
	{
		return static_cast<float>(data.__UInt32);
	}
	else if (type == TYPE_INT64)
	{
		return static_cast<float>(data.__Int64);
	}
	else if (type == TYPE_UINT64)
	{
		return static_cast<float>(data.__UInt64);
	}
	else if (type == TYPE_FLOAT)
	{
		return static_cast<float>(data.__Float);
	}
	else if (type == TYPE_DOUBLE)
	{
		return static_cast<float>(data.__Double);
	}
	else if (type == TYPE_STRING)
	{
		return static_cast<float>(std::atof(data.__String->c_str()));
	}
	else if (type == TYPE_BOOL)
	{
		return data.__Bool ? 1.0f : 0.0f;
	}
	else
	{
		return 0.0;
	}
}

double Variant::toDouble() const
{
	if (type == TYPE_CHAR)
	{
		return static_cast<double>(data.__Char);
	}
	else if (type == TYPE_UCHAR)
	{
		return static_cast<double>(data.__UChar);
	}
	else if (type == TYPE_INT16)
	{
		return static_cast<double>(data.__Int16);
	}
	else if (type == TYPE_UINT16)
	{
		return static_cast<double>(data.__UInt16);
	}
	else if (type == TYPE_INT32)
	{
		return static_cast<double>(data.__Int32);
	}
	else if (type == TYPE_UINT32)
	{
		return static_cast<double>(data.__UInt32);
	}
	else if (type == TYPE_INT64)
	{
		return static_cast<double>(data.__Int64);
	}
	else if (type == TYPE_UINT64)
	{
		return static_cast<double>(data.__UInt64);
	}
	else if (type == TYPE_FLOAT)
	{
		return static_cast<double>(data.__Float);
	}
	else if (type == TYPE_DOUBLE)
	{
		return static_cast<double>(data.__Double);
	}
	else if (type == TYPE_STRING)
	{
		return std::atof(data.__String->c_str());
	}
	else if (type == TYPE_BOOL)
	{
		return data.__Bool ? 1.0 : 0.0;
	}
	else
	{
		return 0.0;
	}
}

} // namespace pxl
