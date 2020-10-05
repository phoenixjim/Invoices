#ifndef _Invoices_DBUI_h_
#define _Invoices_DBUI_h_
#include <CtrlLib/CtrlLib.h>
#pragma foreign_keys = ON
#include <SqlCtrl/SqlCtrl.h>
using namespace Upp;

#define LAYOUTFILE <Invoices/Invoices.lay>
#include <CtrlCore/lay.h>

#include "Invoices/Tables/sqlincludes.h"
#include "../Utils/converts.h"
#include "../Utils/configs.h"

enum pNames
        { 
        Service=1,
        Part,
        Gift,
        Refund,
        Note
        };
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
	// void Open(Ctrl* owner);
	
public:
	typedef InvoicesWindow CLASSNAME;
	InvoicesWindow();
	virtual void Paint(Draw& w) {
        w.DrawRect(GetSize(), Color(204, 255, 255)); // <= enter your background color here
    }
};

// ListLineItems
struct LineItemsWindow : public WithLineItemsWindowLayout<TopWindow> {
	void EditRow();
	void AddNewItem();
	// void Open(Ctrl* owner);
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
	Configs myConfig;
	long nextInvoice;

	double CalcItemTotal(int itemnumber);
	void CalcInvoiceTotal();
	void CustChanged();
	void ProdChanged();
	void AdjustPrice();
	void SaveInvoice();
	void CancelInvoice();
	void AddItem();
	void ClearItem();
	void DeleteRow();
public:
	CreateInvoiceWindow();
	void Paint(Draw& w) {
        w.DrawRect(GetSize(), Color(204, 255, 255)); // <= enter your background color here
     }
};

struct CompanyInfoWindow : WithCompanyInfoWindowLayout<TopWindow> {
	Configs myConfig;
	void Save();
	void Cancel();
	public:
		typedef CompanyInfoWindow CLASSNAME;
		CompanyInfoWindow();
		void Paint(Draw& w) {
        w.DrawRect(GetSize(), Color(204, 255, 255)); // <= enter your background color here
     }
};

#include "../Reports/Reports.h"

class Invoices : public WithInvoicesLayout<TopWindow> {
	CustomersWindow	cwin;
	ProductsWindow prodwin;
	CreateInvoiceWindow createinvoicewin;
	TaxWindow taxreportwin;
	IncomeWindow incomewin;
	ProfitLossWindow profitwin;
	CompanyInfoWindow setcompanywin;
	InvoicesWindow invoiceswin;
	LineItemsWindow listlineitemswin;
	
	MenuBar		mainmenu;

public:
	typedef Invoices CLASSNAME;
	Configs myConfig;

	Invoices();
	
	// Menu definition
	void		MainMenu(Bar& bar);

	void		TransactionsMenu(Bar& bar);
	void		ReportsMenu(Bar& bar);
	void		ManagementMenu(Bar& bar);
};

#endif

