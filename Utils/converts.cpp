#include "converts.h"

struct ConvDoubleCls : Convert
{
	virtual Value Format ( const Value &q ) const;
};

Value ConvDoubleCls::Format ( const Value &q ) const
	{
		return q.IsNull() ? Null : UPP::Format("%2!,n", q);
	}

Convert& ConvDouble()
{
	return Single<ConvDoubleCls>();
}

struct DateIntConvertCls : ConvertDate {
    virtual Value Format(const Value& q) const;
    virtual Value Scan(const Value& text) const;
    virtual int Filter(int chr) const;
};
// From sql borrow.cpp example
Value DateIntConvertCls::Format(const Value& q) const
{
    return IsNull(q) ? String() : ::Format(Date( 1970, 1, 1) + (int)q);
}
// Modified from UPP source
Value DateIntConvertCls::Scan(const Value& text) const
{
	const char *s;
	Date date;
	String txt = text;
	if(txt.IsEmpty()) return (Date) Null;
	s = StrToDate(date, txt);
	if(s)
		for(;;) {
			if(IsDigit(*s))
				break;
			if(*s == '\0')
				return date - Date(1970, 1, 1);
			s++;
		}
	return ErrorValue(t_("Invalid date !"));
}

int DateIntConvertCls::Filter(int chr) const
{
    return CharFilterDate(chr);
}

Convert& DateIntConvert()
{
    return Single<DateIntConvertCls>();
}

struct ConvLineItemCls : Convert
{
	virtual Value Scan ( const Value &q ) const;
	virtual Value Format (const Value &q) const;
};

Value ConvLineItemCls::Format ( const Value &q ) const
	{
	int idNum = q;
	// idNum ++;
	switch(idNum)
	{
		case 1:
			return "Service";
			break;
		case 2:
			return "Part";
			break;
		case 3:
			return "Refund";
			break;
		case 4:
			return "Tip";
			break;
		case 5:
			return "Note";
			break;
		default:
			return "Service";
	}
}

Value ConvLineItemCls::Scan(const Value &q) const
{
	String text = q;
	if(text.IsEmpty()) return Null;
	if (text.IsEqual("Service"))
		return 1;
	else if (text.IsEqual("Part"))
		return 2;
	else if (text.IsEqual("Refund"))
		return 3;
	else if (text.IsEqual("Tip"))
		return 4;
	else if (text.IsEqual("Note"))
		return 5;
	else return 1;
}

Convert& ConvLineItem()
{
	return Single<ConvLineItemCls>();
}

