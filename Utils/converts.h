#ifndef _Invoices_converts_h_
#define _Invoices_converts_h_
#include "lookups.h"

#include <CtrlLib/CtrlLib.h>
#pragma foreign_keys = ON
#include <SqlCtrl/SqlCtrl.h>
using namespace Upp;
// #define LAYOUTFILE <Invoices/Invoices.lay>
// #include <CtrlCore/lay.h>

#include "Invoices/Tables/sqlincludes.h"

Convert& DateIntConvert();
Convert& ConvDouble();
Convert& ConvDbl();
Convert& ConvRate();
Convert& ConvLineItem();
Convert& ConvCounty();
Convert& ConvCurrency();

// From sergeynikitin @ https://www.ultimatepp.org/forums/index.php?t=usrinfo&id=538&
LookupHdr(TYPES,TYPENUM,TYPENAME);
LookupHdr(COUNTIES,COUNTY_NUM,COUNTY_NAME);

#endif
