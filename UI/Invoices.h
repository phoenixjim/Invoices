#ifndef __Invoices__Invoices_h_
#define __Invoices__Invoices_h_

#include <CtrlLib/CtrlLib.h>


using namespace Upp;
#include "Invoices/Tables/sqlinclude.h"
#define LAYOUTFILE <Invoices/UI/Invoices.lay>
#include <CtrlCore/lay.h>

#include "customers.h"
#include "products.h"
#include "createinvoice.h"
#include "listinvoices.h"
#include "listlineitems.h"
#include "taxmenu.h"
#include "taxbycustomer.h"
#include "incomebycustomer.h"
#include "profitloss.h"
#include "setcompany.h"
Convert& DateIntConvert();
Convert& ConvDouble();

// this should be a simple file browser function - can do this here
// struct SelectDBWindow : WithSelectDBWindowLayout<TopWindow> {
// 	SelectDBWindow() { CtrlLayout(*this, "Select Database File"); }
// };
// Just a quick info dialog showing currently selected file
struct ShowDBWindow : WithShowDBWindowLayout<TopWindow> {
	ShowDBWindow() { CtrlLayout(*this, "Show Current Database File"); }
};
// Simple directory chooser function
// struct SelectOutputDirWindow : WithSelectOutputDirWindowLayout<TopWindow> {
// 	SelectOutputDirWindow() { CtrlLayout(*this, "Set Output Directory"); }
// };

class Invoices : public WithInvoicesLayout<TopWindow> {
	ProductsWindow prodwin;
	CustomersWindow cwin;
	CreateInvoiceWindow createinvoicewin;
	ListInvoicesWindow listinvoiceswin;
	ListLineItemsWindow listlineitemswin;
	TaxMenuWindow taxreportwin;
	TaxByCustomerWindow taxbycustomerwin;
	IncomeByCustomerWindow incomewin;
	ProfitLossWindow profitwin;
	SetCompanyWindow setcompanywin;
	ShowDBWindow showdbwin;

	MenuBar		 mainmenu;
	
public:
	typedef Invoices CLASSNAME;
	Invoices();
	
	// Menu definition
	void MainMenu(Bar& bar);

	void TransactionsMenu(Bar& bar);
	void ReportsMenu(Bar& bar);
	void ManagementMenu(Bar& bar);
	
String GetOutputDirectory();
String SelectDB();

String	DBFile;
String OutputDirectory;
String configfile = ConfigFile();
String cfg;

FileSel selectdbwin;
FileSel selectodirwin;

	// Keys for shortcuts
	// bool Key(dword key, int count) override;
};
#endif
