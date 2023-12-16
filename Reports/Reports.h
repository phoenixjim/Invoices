#ifndef _Reports_h_
#define _Reports_h_
#include <CtrlLib/CtrlLib.h>
#pragma foreign_keys = ON
#include <SqlCtrl/SqlCtrl.h>
using namespace Upp;

#define LAYOUTFILE <Invoices/Reports/Reports.lay>
#include <CtrlCore/lay.h>

#include "../Tables/sqlincludes.h"
#include "../Utils/converts.h"
#include "../Utils/utils.h"
#include "../Utils/configs.h"

#define COLOR Color(102, 0, 102) // dark violet background, text should be white...
#define TXTCOLOR Color(255,255,255)

struct TaxWindow : WithTaxWindowLayout<TopWindow> {
	Configs myConfig;
	void okPressed();
	void voidedChanged();
	void cancelPressed();
	void anonChanged();
	void noCustChanged();
	void CreateReport(String start, String end);
	void CreateSalesTaxReport(String start, String end);
	void ExportQTF();
	String getCountyNameFromNumber(int cnumber);
	int getCountyNumberFromInvoice(int invoiceId);
	String taxQTF;
	String GetCustomerName(int custId);
public:
	TaxWindow();
	void Paint(Draw& w) {
		w.DrawRect(GetSize(), COLOR); // <= enter your background color here
    }
};

struct IncomeWindow : WithIncomeByCustomerLayout<TopWindow> {
	Configs myConfig;
	void okPressed();
	void voidedChanged();
	void cancelPressed();
	void CustChanged();
	void CreateReport(String start, String end);
	void ExportQTF();
	
	String taxQTF;
	String GetCustomerName(int custId);
public:
	IncomeWindow();
	void Paint(Draw& w) {
        w.DrawRect(GetSize(), COLOR); // <= enter your background color here
     }
};

struct YearToYearWindow : WithYearToYearWindowLayout<TopWindow> {
	Configs myConfig;
	Date prevDateStart, prevDateEnd;
	String prevStart, prevEnd;

	void cleanup();
	void cancelPressed();
	void okPressed();
	void CreateReport(String start, String end);
public:
	YearToYearWindow();
	void Paint(Draw& w) {
		w.DrawRect(GetSize(), COLOR); // <= enter your background color here
	}
};

struct PandLWindow : public WithPandLWindowLayout<TopWindow> {
	Configs myConfig;
	void okPressed();
	void cancelPressed();
	void CreateReport(String start, String end);
	void refreshAll();
	public:
		PandLWindow();
	void Paint(Draw& w) {
		w.DrawRect(GetSize(), COLOR); // <= enter your background color here
	}
};

#endif