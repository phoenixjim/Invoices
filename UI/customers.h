#ifndef _Invoices_customers_h_
#define _Invoices_customers_h_
#include <Invoices/UI/sqlincludes.h>
#include "../configs.h"

struct CustomersWindow : WithCustomersWindowLayout<TopWindow> {
	Configs	myConfig;
public:
    CustomersWindow();
	virtual void Paint(Draw& w) {
        w.DrawRect(GetSize(), Color(204, 255, 255)); // <= enter your background color here
    }
    void btnAddCustomerClick();
	void btnUpdateCustomerClick();
	void btnSearchCustomerClick();
	void EditRow();

	// void FakeStub();
};
	

#endif
