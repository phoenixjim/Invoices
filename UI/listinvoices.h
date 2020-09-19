#ifndef _Invoices_listinvoices_h_
#define _Invoices_listinvoices_h_
#include <SqlCtrl/SqlCtrl.h>
#include <plugin/sqlite3/Sqlite3.h>

#define SCHEMADIALECT <plugin/sqlite3/Sqlite3Schema.h>
// #define MODEL "Invoices/Tables/Invoices.sch"
// #include <Sql/sch_header.h>

struct ListInvoicesWindow : WithListInvoicesWindowLayout<TopWindow> {
public:
	ListInvoicesWindow() {
	CtrlLayout(*this, "List Invoices");
};
	void Paint(Draw& w) {
        w.DrawRect(GetSize(), Color(204, 255, 255)); // <= enter your background color here
     }
};


#endif
