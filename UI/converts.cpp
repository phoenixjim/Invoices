#include "Invoices/UI/converts.h"

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

/*
Value CountryConvert::Format(const Value& q) const
{
	if(q.IsNull()) return Null;
	static VectorMap<int, String> country;
	static Time lastcleartime;
	if(GetSysTime()-lastcleartime > 600){
		country.Clear();
		lastcleartime=GetSysTime();
	}
	if(country.Find(q) > 0)
		return country.Get(q, String());
	else {
		Sql sql;
		sql * Select(COU_ID, COU_NAME).From(COUNTRY_T).Where(COU_ID==q);
		if(sql.Fetch()) {
			int sid = sql[COU_ID];
			//if(country.Find(sid) < 0)
			country.Add(sid, sql[COU_NAME]);
			return country.Get(q, String());
		}else
			return (String)("");

	}
}
*/