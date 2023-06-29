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

//#define COLOR Color(204, 255, 255)
#define COLOR Color(102, 0, 102) // dark violet background, text should be white...
#define TXTCOLOR Color(255,255,255)
//Customers
class CustomersWindow : public WithCustomersWindowLayout<TopWindow> {
	Configs	myConfig;
public:
    CustomersWindow();
	virtual void Paint(Draw& w) {
        w.DrawRect(GetSize(), COLOR); // <= enter your background color here
    }
	void AddNewCustomer();
	void EditRow();
};
	
// CreateInvoice
class CreateInvoiceWindow : public WithCreateInvoiceWindowLayout<TopWindow> {
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
        w.DrawRect(GetSize(), COLOR); // <= enter your background color here
     }
};

// ListInvoices
class InvoicesWindow : public WithInvoicesWindowLayout<TopWindow> {
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
        w.DrawRect(GetSize(), COLOR); // <= enter your background color here
    }
};

// ListLineItems
class LineItemsWindow : public WithLineItemsWindowLayout<TopWindow> {
	Configs myConfig;
	void EditRow();
	void AddNewItem();
	void CalcInvoiceTotal(long invoice);
	double CalcItemTotal(int itemnumber);
	void DeleteItem();
	
public:
	LineItemsWindow();
	void Paint(Draw& w) {
        w.DrawRect(GetSize(), COLOR); // <= enter your background color here
     }
};

// Products
class ProductsWindow : public WithProductsWindowLayout<TopWindow> {
public:
	ProductsWindow();
	SqlCtrls ctrls;
	
	virtual void Paint(Draw& w) {
        w.DrawRect(GetSize(), COLOR); // <= enter your background color here
    }
    void AddNewProduct();
	void EditRow();
};

// Types
class TypesWindow : public WithTypesWindowLayout<TopWindow> {
public:
	TypesWindow();
	SqlCtrls ctrls;
	
	virtual void Paint(Draw& w) {
        w.DrawRect(GetSize(), COLOR); // <= enter your background color here
    }
    void AddNewType();
	void EditRow();
};

class CompanyInfoWindow : public WithCompanyInfoWindowLayout<TopWindow> {
	void Save();
	void Cancel();
public:
	IdCtrls ctrls;
	
	Configs myConfig;
	CompanyInfoWindow();
	void Paint(Draw& w) {
        w.DrawRect(GetSize(), COLOR); // <= enter your background color here
    }
};

#include "../Reports/Reports.h"


class Invoices : public WithInvoicesLayout<TopWindow> {
	Configs myConfig;
	
	CustomersWindow	custwin;
	ProductsWindow prodwin;
	TypesWindow typewin;
	CreateInvoiceWindow createinvoicewin;
	TaxWindow taxreportwin;
	IncomeWindow incomewin;
	YearToYearWindow year2yearwin;
	PandLWindow pandlwin;
	CompanyInfoWindow setcompanywin;
	InvoicesWindow invoiceswin;
	LineItemsWindow listlineitemswin;
	
	MenuBar		mainmenu;
	// added 'save and mark paid' on create invoice window
	String version = "2.1.0.3";
public:
	Invoices();
	void Paint(Draw& w) {
		w.DrawRect(GetSize(), COLOR); // <= enter your background color here
	}
	// Menu definition
	void		MainMenu(Bar& bar);

	void		FileMenu(Bar& bar);
	void		ReportsMenu(Bar& bar);
	void		ManagementMenu(Bar& bar);
};

#endif

