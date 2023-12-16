#include "utils.h"

double GetPartsCost ( int invId )
{
	SQL * Select ( SqlSum ( COST ) ).From ( PRODUCTS ).Where ( INVOICEID == invId );
	SQL.Fetch();
	double parts = IsNull(SQL[0]) ? 0.00 : round(SQL[0], 2);
	return parts;
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
