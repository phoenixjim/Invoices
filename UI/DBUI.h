#ifndef _Invoices_customers_h_
#define _Invoices_customers_h_
#include <CtrlLib/CtrlLib.h>
#include <SqlCtrl/SqlCtrl.h>
using namespace Upp;

#define LAYOUTFILE <Invoices/UI/Invoices.lay>
#include <CtrlCore/lay.h>

#include <Invoices/UI/sqlincludes.h>
#include "converts.h"
#include "../configs.h"
//Customers
struct CustomersWindow : WithCustomersWindowLayout<TopWindow> {
	Configs	myConfig;
	// EditString	name;
	
public:
    CustomersWindow();
	virtual void Paint(Draw& w) {
        w.DrawRect(GetSize(), Color(204, 255, 255)); // <= enter your background color here
    }
	void AddNewCustomer();
	// void btnUpdateCustomerClick();
	// void btnSearchCustomerClick();
	void EditRow();

	// void FakeStub();
};
	
// ListInvoices
struct InvoicesWindow : WithInvoicesWindowLayout<TopWindow> {
	Configs myConfig;
	
	void btnPrintClicked();
	void btnApplyPaymentClicked();
	void btnEditClicked();
	void btnVoidClicked();
	void btnPaidInFullClicked();
	void btnFixDateClicked();
	void btnByPaidClicked();
	void btnByBalanceDueClicked();
	void btnByDatesClicked();
	void btnByCustomerClicked();
	void btnByVoidedClicked();
	
public:
	typedef InvoicesWindow CLASSNAME;
	InvoicesWindow();
	virtual void Paint(Draw& w) {
        w.DrawRect(GetSize(), Color(204, 255, 255)); // <= enter your background color here
    }
};

// ListLineItems
struct LineItemsWindow : WithLineItemsWindowLayout<TopWindow> {
public:
	typedef LineItemsWindow CLASSNAME;
	LineItemsWindow();
	void Paint(Draw& w) {
        w.DrawRect(GetSize(), Color(204, 255, 255)); // <= enter your background color here
     }
};

// Products
struct ProductsWindow : WithProductsWindowLayout<TopWindow> {
public:
	typedef ProductsWindow CLASSNAME;
	ProductsWindow();
	SqlCtrls ctrls;

	virtual void Paint(Draw& w) {
        w.DrawRect(GetSize(), Color(204, 255, 255)); // <= enter your background color here
    }
    void AddNewProduct();
	// void btnUpdateProductClick();
	// void btnShowAllProductClick();
	// void btnProductRangeClick();
	void EditRow();
	
	// String GetOutputDirectory();
	String SelectDB();
};

// CreateInvoice
struct CreateInvoiceWindow : WithCreateInvoiceWindowLayout<TopWindow> {
public:
	CreateInvoiceWindow() { CtrlLayout(*this, "Create Invoice"); }
	void Paint(Draw& w) {
        w.DrawRect(GetSize(), Color(204, 255, 255)); // <= enter your background color here
     }
};


#endif
