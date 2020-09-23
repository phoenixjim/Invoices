#ifndef __Invoices__Invoices_h_
#define __Invoices__Invoices_h_

#include <CtrlLib/CtrlLib.h>

using namespace Upp;

#include "customers.h"
#include "../configs.h"

// Just a quick info dialog showing currently selected file
struct ShowDBWindow : WithShowDBWindowLayout<TopWindow> {
	ShowDBWindow() { CtrlLayout(*this, "Show Current Database File"); }
};

class Invoices : public WithInvoicesLayout<TopWindow> {
	CustomersWindow	cwin;

	MenuBar		mainmenu;
	Configs		myConfig;
public:
	typedef Invoices CLASSNAME;
	Invoices();
	
	// Menu definition
	void		MainMenu(Bar& bar);

	// void		TransactionsMenu(Bar& bar);
	// void		ReportsMenu(Bar& bar);
	void		ManagementMenu(Bar& bar);
	
	// Keys for shortcuts
	// bool Key(dword key, int count) override;
};
#endif
