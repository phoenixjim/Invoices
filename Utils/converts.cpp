#include "converts.h"

struct ConvDoubleCls : Convert
{
	virtual Value Format ( const Value &q ) const;
	virtual Value Scan ( const Value &q ) const;
};

Value ConvDoubleCls::Format ( const Value &q ) const // double to string, formatted
{
	// return q.IsNull() ? Null : UPP::Format("%2!,n", q);
	double currency;
	double money = q;
	if (money < 0) {
		currency = (double)abs(money) ;
		return UPP::Format("$(%.2f)", currency);
	}
	else {
		currency = (double)money ;
		return (money == 0) ? "$0.00" : UPP::Format("$%.2f", currency);
	}
}
	
Value ConvDoubleCls::Scan (const Value &q ) const // string to double
{
	String text = q;
	if (text[0] == '$') text.Remove(0);
	if (text[0] == '(') {
		text.Set(0,'-');
		text.TrimLast();
	}
	double currency = StrDbl(text.ToString());
	return (double)(currency);
}

Convert& ConvDouble()
{
	return Single<ConvDoubleCls>();
}

struct ConvDblCls : Convert
{
	virtual Value Format ( const Value &q ) const;
	virtual Value Scan ( const Value &q ) const;
};

Value ConvDblCls::Format ( const Value &q ) const // double to string, formatted
{
	// return q.IsNull() ? Null : UPP::Format("%2!,n", q);
	double toreal;
	double real = q;
	if (real < 0) {
		toreal = (double)abs(real) ;
		return UPP::Format("(%.2f)", toreal);
	}
	else {
		toreal = (double)real ;
		return (real == 0) ? "0.00" : UPP::Format("%.2f", toreal);
	}
}
	
Value ConvDblCls::Scan (const Value &q ) const // string to double
{
	String text = q;
	if (text[0] == '(') {
		text.Set(0,'-');
		text.TrimLast();
	}
	double real = StrDbl(text.ToString());
	return (double)(real);
}

Convert& ConvDbl()
{
	return Single<ConvDblCls>();
}

struct ConvRateCls : Convert
{
	virtual Value Format ( const Value &q ) const;
	virtual Value Scan ( const Value &q ) const;
};

Value ConvRateCls::Format ( const Value &q ) const // double to string, formatted
{
	// return q.IsNull() ? Null : UPP::Format("%2!,n", q);
	double real = q;
	if (real < 0) {
		return UPP::Format("(%.2f) %%", 0.00);
	}
	else {
		return (real == 0.00) ? "0.00 %" : UPP::Format("%.2f %%", real);
	}
}
	
Value ConvRateCls::Scan (const Value &q ) const // string to double
{
	String text = q;
	if (text[text.GetCharCount() - 1] == '%')
		text.TrimLast(); // remove percent sign

	double real = StrDbl(text.ToString());
	return (double)(real);
}

Convert& ConvRate()
{
	return Single<ConvRateCls>();
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

String prnMoney( double money )
{
	double currency;
	if (money < 0) {
		currency = (double)abs(money) ;
		return UPP::Format("$(%.2f)", currency);
	}
	else {
		currency = (double)money ;
		return (money == 0) ? "$0.00" : UPP::Format("$%.2f", currency);
	}
}

String prnQty( double qty )
{
	if (qty < 0) {
		return UPP::Format("(%.2f)", 0.00);
	}
	else {
		return (qty == 0) ? "0.00" : UPP::Format("%.2f", qty);
	}
}

double taxMoney ( double money )
{
	return ((double) money );
}

double round(double d, int n) {
	double param, fractpart, intpart;
	int fixer = (int) (d * ipow10(n));
	param = (double) fixer;
	fractpart = modf (param , &intpart);

	return floor(intpart + 0.5) / ipow10(n);
	//return (intpart / ipow10(n));
}

double PercentFormat(double d) {
	return floor(d * ipow10(4) + 0.5) / ipow10(2);
}

struct ConvCountyCls : Convert
{
	virtual Value Scan ( const Value &q ) const;
	virtual Value Format (const Value &q) const;
};

Value ConvCountyCls::Format ( const Value &q ) const // use typenum to return correct string
	{
	int idNum = q;
	SQL * Select(COUNTY_NUM, COUNTY_NAME).From(COUNTIES);
	
	while (SQL.Fetch())
	{
		if (q == SQL[COUNTY_NUM])
			return SQL[COUNTY_NAME];
	}
	return "Columbia"; // if no type matching num
	
}

Value ConvCountyCls::Scan(const Value &q) const // use typename to return type num
{
	String text = q;
	if(text.IsEmpty()) return Null;
	SQL * Select(COUNTY_NUM, COUNTY_NAME).From(COUNTIES);
	
	while (SQL.Fetch())
	{
		if (text.IsEqual(SQL[COUNTY_NAME]))
			return SQL[COUNTY_NUM];
	}
	return 1;
}

Convert& ConvCounty()
{
	return Single<ConvCountyCls>();
}

double getTaxRate(int custId)
{
	Sql sqlCustomer;
 	sqlCustomer * Select(CTY_NUM).From(CUSTOMERS).Where( CUST_ID == custId );
 	sqlCustomer.Fetch();

	Sql sqlCounty;
	sqlCounty  * Select(COUNTY_RATE).From( COUNTIES ).Where( COUNTY_NUM == sqlCustomer[0]);
	sqlCounty.Fetch();
	
	return sqlCounty[0];
}

int getCustFromInvoice(int invoiceId)
{
	Sql sqlInvoice;
 	sqlInvoice * Select(CUSTOMERID).From(INVOICES).Where( INVOICENUMBER == invoiceId );
 	sqlInvoice.Fetch();
	int invNum = sqlInvoice[0];
	if (( invNum < 0 ) || (invNum > 1000)) return -1;
	return invNum;
}

double getCountyTaxRate( int county )
{
	Sql sqlCounty;
	sqlCounty  * Select(COUNTY_RATE).From( COUNTIES ).Where( COUNTY_NUM == county);
	sqlCounty.Fetch();
	
	// double taxrate = StrDbl(sqlCounty[COUNTY_RATE].ToString());
	return (double)sqlCounty[0]; // taxrate;	
}

LookupSrc(TYPES,TYPENUM,TYPENAME);
LookupSrc(COUNTIES,COUNTY_NUM,COUNTY_NAME);
