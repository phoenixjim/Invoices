#ifndef __Invoices__Invoices_h_
#define __Invoices__Invoices_h_

#include <CtrlLib/CtrlLib.h>

using namespace Upp;
#include <Invoices/UI/sqlincludes.h>
#include "DBUI.h"
#include "../configs.h"

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
	
	// Keys for shortcuts
	// bool Key(dword key, int count) override;
};
#endif
