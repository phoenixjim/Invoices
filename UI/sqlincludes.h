#ifndef _Invoices_sqlincludes_h_
#define _Invoices_sqlincludes_h_
#include <SqlCtrl/SqlCtrl.h>

#include <plugin/sqlite3/Sqlite3.h>

using namespace Upp;
#define LAYOUTFILE <Invoices/UI/Invoices.lay>
#include <CtrlCore/lay.h>

#define SCHEMADIALECT <plugin/sqlite3/Sqlite3Schema.h> // <-- Mirek informed needed in header for sql schema use
#define MODEL "Invoices/Tables/Invoices.sch"
#include <Sql/sch_header.h>
#undef MODEL

#endif
