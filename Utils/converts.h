#ifndef _Invoices_converts_h_
#define _Invoices_converts_h_

#include <CtrlLib/CtrlLib.h>
using namespace Upp;

Convert& DateIntConvert();
Convert& ConvDouble();
Convert& ConvLineItem();

double round(double d, int n);
double PercentFormat(double d);

#endif
