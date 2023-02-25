#ifndef _Invoices_converts_h_
#define _Invoices_converts_h_

#include <CtrlLib/CtrlLib.h>
#pragma foreign_keys = ON
#include <SqlCtrl/SqlCtrl.h>
using namespace Upp;
// #define LAYOUTFILE <Invoices/Invoices.lay>
// #include <CtrlCore/lay.h>

#include "Invoices/Tables/sqlincludes.h"

Convert& DateIntConvert();
Convert& ConvDouble();
Convert& ConvLineItem();
Convert& ConvCurrency();

double round(double d, int n);
double PercentFormat(double d);
String prnMoney( double money );
double taxMoney ( double money );

#endif
