#ifndef _Invoices_converts_h_
#define _Invoices_converts_h_

#include <CtrlLib/CtrlLib.h>
#pragma foreign_keys = ON
#include <SqlCtrl/SqlCtrl.h>
using namespace Upp;

Convert& DateIntConvert();
Convert& ConvDouble();
Convert& ConvLineItem();

double round(double d, int n);
double PercentFormat(double d);

#endif
