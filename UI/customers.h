#ifndef _Invoices_customers_h_
#define _Invoices_customers_h_
#include <SqlCtrl/SqlCtrl.h>
#include <plugin/sqlite3/Sqlite3.h>

#define SCHEMADIALECT <plugin/sqlite3/Sqlite3Schema.h>
// #define MODEL "Invoices/Tables/Invoices.sch"
// #include <Sql/sch_header.h>
struct CustomersWindow : WithCustomersWindowLayout<TopWindow> {
	
public:
    CustomersWindow();
	void Paint(Draw& w) {
        w.DrawRect(GetSize(), Color(204, 255, 255)); // <= enter your background color here
     }
};
	

#endif
