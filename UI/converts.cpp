#include "Invoices/UI/converts.h"

struct ConvDoubleCls : Convert
{
	virtual Value Format ( const Value &q ) const;
};

Value ConvDoubleCls::Format ( const Value &q ) const
	{
		return q.IsNull() ? Null : UPP::Format ( "%.2f", q );
	}

Convert& ConvDouble()
{
	return Single<ConvDoubleCls>();
}

struct DateIntConvertCls : Convert {
    virtual Value Format(const Value& q) const;
    virtual Value Scan(const Value& text) const;
    virtual int Filter(int chr) const;
};

Value DateIntConvertCls::Format(const Value& q) const
{
    return IsNull(q) ? String() : ::Format(Date( 1970, 1, 1) + (int)q);
}

Value DateIntConvertCls::Scan(const Value& text) const
{
    String txt = text;
    if(IsNull(txt))
        return Null;
    txt.TrimLast(6);
    Date d;
    if(StrToDate(d, txt))
        return d - Date(1970, 1, 1);
    return ErrorValue("Invalid date!");
}

int DateIntConvertCls::Filter(int chr) const
{
    return CharFilterDate(chr);
}

Convert& DateIntConvert()
{
    return Single<DateIntConvertCls>();
}

