#include "converts.h"

struct ConvDoubleCls : Convert
{
	virtual Value Format ( const Value &q ) const;
	virtual Value Scan ( const Value &q ) const;
};

Value ConvDoubleCls::Format ( const Value &q ) const // int to string, formatted
{
	// return q.IsNull() ? Null : UPP::Format("%2!,n", q);
	double currency;
	int money = q;
	if (money < 0) {
		currency = (double)abs(money) / 100.0;
		return UPP::Format("$(%2!,n)", currency);
	}
	else {
		currency = (double)money / 100.0;
		return (money == 0) ? "$0.00" : UPP::Format("$%2!,n", currency);
	}
}
	
Value ConvDoubleCls::Scan (const Value &q ) const // string to int
{
	String text = q;
	if (text[0] == '$') text.Remove(0);
	if (text[0] == '(') {
		text.Set(0,'-');
		text.TrimLast();
	}
	double currency = StrDbl(text.ToString());
	return (int) (currency * 100);
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

Value ConvLineItemCls::Format ( const Value &q ) const // use typenum to return correct string
	{
	int idNum = q;
	SQL * Select(TYPENUM, TYPENAME).From(TYPES);
	
	while (SQL.Fetch())
	{
		if (q == SQL[TYPENUM])
			return SQL[TYPENAME];
	}
	return "Service"; // if no type matching num
	
}

Value ConvLineItemCls::Scan(const Value &q) const // use typename to return type num
{
	String text = q;
	if(text.IsEmpty()) return Null;
	SQL * Select(TYPENUM, TYPENAME).From(TYPES);
	
	while (SQL.Fetch())
	{
		if (text.IsEqual(SQL[TYPENAME]))
			return SQL[TYPENUM];
	}
	return 1;
}

Convert& ConvLineItem()
{
	return Single<ConvLineItemCls>();
}

struct ConvCurrencyCls : Convert
{
	virtual Value Scan ( const Value &q ) const;
	virtual Value Format (const Value &q ) const;
};

Value ConvCurrencyCls::Format( const Value &q ) const // return formatted string %xx.xx
{
	double money = q;
	if (money < 0.00) {
		money = abs(money);
		return UPP::Format("$(%2!,n)", money);
	}
	else
		return UPP::Format("$%2!,n", money);
}

Value ConvCurrencyCls::Scan( const Value &q ) const // return unformatted int
{
	String text = q;
	if (text[0] == '$') text.Remove(0);
	if (text[0] == '(') {
		text.Set(0,'-');
		text.TrimLast();
	}
	double currency = StrDbl(text.ToString());
	return currency;
}

Convert& ConvCurrency()
{
	return Single<ConvCurrencyCls>();
}

String prnMoney( int money )
{
	double currency;
	if (money < 0) {
		currency = (double)abs(money) / 100.0;
		return UPP::Format("$(%2!,n)", currency);
	}
	else {
		currency = (double)money / 100.0;
		return (money == 0) ? "$0.00" : UPP::Format("$%2!,n", currency);
	}
}
double taxMoney ( int money )
{
	return ((double) money / 100.0);
}

double round(double d, int n) {
	double param, fractpart, intpart;
	int fixer = (int) (d * ipow10(n));
	param = (double) fixer;
	fractpart = modf (param , &intpart);

	// return floor(intpart + 0.5) / ipow10(n);
	return (intpart / ipow10(n));
}

double PercentFormat(double d) {
	return floor(d * ipow10(4) + 0.5) / ipow10(2);
}