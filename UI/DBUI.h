#ifndef _Invoices_DBUI_h_
#define _Invoices_DBUI_h_
#include <CtrlLib/CtrlLib.h>
#pragma foreign_keys = ON
#include <SqlCtrl/SqlCtrl.h>

#include "../Utils/configs.h"
using namespace Upp;

#define LAYOUTFILE <Invoices/Invoices.lay>
#include <CtrlCore/lay.h>

#include "Invoices/Tables/sqlincludes.h"
#include "../Utils/converts.h"
#include <Report/Report.h>

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
public:
    CustomersWindow();
	virtual void Paint(Draw& w) {
        w.DrawRect(GetSize(), Color(204, 255, 255)); // <= enter your background color here
    }
	void AddNewCustomer();
	void EditRow();
};
	
// CreateInvoice
struct CreateInvoiceWindow : WithCreateInvoiceWindowLayout<TopWindow> {
	Configs myConfig;
	long nextInvoice;
	int printInvoice;
	
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
	void PrintInvoice();
public:
	CreateInvoiceWindow();
	CreateInvoiceWindow(int invoice);
	void Paint(Draw& w) {
        w.DrawRect(GetSize(), Color(204, 255, 255)); // <= enter your background color here
     }
};

// ListInvoices
struct InvoicesWindow : WithInvoicesWindowLayout<TopWindow> {
	Configs myConfig;
	
	void btnPrintClicked();
	void btnApplyPaymentClicked();
	void btnEditClicked();
	void btnVoidClicked();
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
struct LineItemsWindow : public WithLineItemsWindowLayout<TopWindow> {
	void EditRow();
	void AddNewItem();
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
	void EditRow();
};

struct CompanyInfoWindow : WithCompanyInfoWindowLayout<TopWindow> {
	void Save();
	void Cancel();
public:
	IdCtrls ctrls;
	
	Configs myConfig;
	typedef CompanyInfoWindow CLASSNAME;
	CompanyInfoWindow();
	void Paint(Draw& w) {
        w.DrawRect(GetSize(), Color(204, 255, 255)); // <= enter your background color here
    }
};

#include "../Reports/Reports.h"

class Invoices : public WithInvoicesLayout<TopWindow> {
	Configs myConfig;
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
	String version = "1.0.0.1";
public:
	typedef Invoices CLASSNAME;
	
	Invoices();
	void Paint(Draw& w) {
		w.DrawRect(GetSize(), Color(204, 255, 255)); // <= enter your background color here
	}
	// Menu definition
	void		MainMenu(Bar& bar);

	void		TransactionsMenu(Bar& bar);
	void		ReportsMenu(Bar& bar);
	void		ManagementMenu(Bar& bar);
};

#endif

