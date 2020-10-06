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
	Configs myConfig;
	void okPressed();
	void cancelPressed();
	void anonChanged();
	double GetPartsCost(int invId);
	void CreateReport(String start, String end);
	void ExportQTF();
	
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
	Configs myConfig;
	void okPressed();
	void cancelPressed();
	void CustChanged();
	double GetPartsCost(int invId);
	void CreateReport(String start, String end);
	void ExportQTF();
	
	String taxQTF;
	String GetCustomerName(int custId);
public:
	typedef IncomeWindow CLASSNAME;
	IncomeWindow();
	void Paint(Draw& w) {
        w.DrawRect(GetSize(), Color(204, 255, 255)); // <= enter your background color here
     }
};

struct ProfitLossWindow : WithProfitLossWindowLayout<TopWindow> {
	Configs myConfig;
	void cancelPressed();
	void okPressed();
	void CreateReport(String start, String end);
	double GetPartsCost(int invId);
public:
	typedef ProfitLossWindow CLASSNAME;
	ProfitLossWindow();
	void Paint(Draw& w) {
		w.DrawRect(GetSize(), Color(204, 255, 255)); // <= enter your background color here
	}
};

#endif