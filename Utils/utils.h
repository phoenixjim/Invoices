#ifndef _Invoices_utils_h_
#define _Invoices_utils_h_
#include <CtrlLib/CtrlLib.h>

using namespace Upp;
#include "../Tables/sqlincludes.h"

double round(double d, int n);
double PercentFormat(double d);
String prnMoney( double money );
String prnQty( double qty );
double taxMoney ( double money );
double getTaxRate(int custId);
double getCountyTaxRate( int county );
int getCustFromInvoice(int invoiceId);
double GetPartsCost(int invId);


#endif
