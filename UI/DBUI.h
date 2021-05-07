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
	Note,
	Weekly,
	Daily
};
/*
enum Status
	{
	Void = 0,
	Unpaid,
	PaidInFull
};
*/
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
	int pInvoice;
	
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
	void MarkAsPaid();
	
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
	void btnVoidClicked();
	void btnFixDateClicked();
	void btnByPaidClicked();
	void btnByBalanceDueClicked();
	void btnByDatesClicked();
	void btnByCustomerClicked();
	void btnByVoidedClicked();
	void btnAllClicked();
		
public:
	InvoicesWindow();
	virtual void Paint(Draw& w) {
        w.DrawRect(GetSize(), Color(204, 255, 255)); // <= enter your background color here
    }
};

// ListLineItems
struct LineItemsWindow : public WithLineItemsWindowLayout<TopWindow> {
	Configs myConfig;
	void EditRow();
	void AddNewItem();
	void CalcInvoiceTotal(long invoice);
	double CalcItemTotal(int itemnumber);
	void DeleteItem();
	
public:
	LineItemsWindow();
	void Paint(Draw& w) {
        w.DrawRect(GetSize(), Color(204, 255, 255)); // <= enter your background color here
     }
};

// Products
struct ProductsWindow : WithProductsWindowLayout<TopWindow> {
public:
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
	CompanyInfoWindow();
	void Paint(Draw& w) {
        w.DrawRect(GetSize(), Color(204, 255, 255)); // <= enter your background color here
    }
};

#include "../Reports/Reports.h"

class Invoices : public WithInvoicesLayout<TopWindow> {
	Configs myConfig;
	
	CustomersWindow	custwin;
	ProductsWindow prodwin;
	CreateInvoiceWindow createinvoicewin;
	TaxWindow taxreportwin;
	IncomeWindow incomewin;
	ProfitLossWindow profitwin;
	CompanyInfoWindow setcompanywin;
	InvoicesWindow invoiceswin;
	LineItemsWindow listlineitemswin;
	
	MenuBar		mainmenu;
	// added 'save and mark paid' on create invoice window
	String version = "1.2.2.1";
public:
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

