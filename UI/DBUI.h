#ifndef _Invoices_customers_h_
#define _Invoices_customers_h_
#include <CtrlLib/CtrlLib.h>
#pragma foreign_keys = ON
#include <SqlCtrl/SqlCtrl.h>
using namespace Upp;

#define LAYOUTFILE <Invoices/UI/Invoices.lay>
#include <CtrlCore/lay.h>

#include <Invoices/UI/sqlincludes.h>
#include "converts.h"
#include "../configs.h"
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
	double round(double d, int n);
	void DeleteRow();
public:
	CreateInvoiceWindow();
	void Paint(Draw& w) {
        w.DrawRect(GetSize(), Color(204, 255, 255)); // <= enter your background color here
     }
};

struct TaxWindow : WithTaxWindowLayout<TopWindow> {
	void okPressed();
	void cancelPressed();
	void anonChanged();
	public:
		typedef TaxWindow CLASSNAME;
		TaxWindow();
		void Paint(Draw& w) {
        w.DrawRect(GetSize(), Color(204, 255, 255)); // <= enter your background color here
     }
};

struct IncomeWindow : WithIncomeByCustomerLayout<TopWindow> {
	public:
		typedef IncomeWindow CLASSNAME;
		IncomeWindow();
		void Paint(Draw& w) {
        w.DrawRect(GetSize(), Color(204, 255, 255)); // <= enter your background color here
     }
};

struct ProfitLossWindow : WithProfitLossWindowLayout<TopWindow> {
	public:
		typedef ProfitLossWindow CLASSNAME;
		ProfitLossWindow();
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

class Invoices : public WithInvoicesLayout<TopWindow> {
	CustomersWindow	cwin;
	ProductsWindow prodwin;
	CreateInvoiceWindow createinvoicewin;
	TaxWindow taxreportwin;
	IncomeWindow incomewin;
	ProfitLossWindow profitwin;
		
	MenuBar		mainmenu;

public:
	CompanyInfoWindow setcompanywin;
	InvoicesWindow invoiceswin;
	LineItemsWindow listlineitemswin;
	
	typedef Invoices CLASSNAME;
	Configs myConfig;

	Invoices();
	
	// Menu definition
	void		MainMenu(Bar& bar);

	void		TransactionsMenu(Bar& bar);
	void		ReportsMenu(Bar& bar);
	void		ManagementMenu(Bar& bar);
	
	void		UpdateTables();
};

#endif

