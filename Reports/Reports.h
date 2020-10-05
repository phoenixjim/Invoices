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
#include "../Utils/configs.h"

struct TaxWindow : WithTaxWindowLayout<TopWindow> {
	void okPressed();
	void cancelPressed();
	void anonChanged();
	double GetPartsCost(int invId);
	void CreateReport(String start, String end);
	
	String taxQTF;
	String GetCustomerName(int custId);
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

#endif