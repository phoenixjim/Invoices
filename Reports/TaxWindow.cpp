#include "Reports.h"

TaxWindow::TaxWindow()
{
	CtrlLayoutOKCancel(*this, "Select date range for report:");
	
	sqlTaxReport.AddColumn(INVOICENUMBER, "Inv NO.", 100);
	sqlTaxReport.AddColumn(DATEPAID, "Date Paid", 200).SetConvert(DateIntConvert());
	sqlTaxReport.AddColumn(CUSTOMERID, "Cust. No.", 65); // OR CUSTNAME, IF NOT ANON
	sqlTaxReport.AddColumn(CUSTNAME, "Customer Name", 140);
	sqlTaxReport.AddColumn(TAXABLESUB, "Taxable", 150);
	sqlTaxReport.AddColumn(NONTAXABLESUB, "Non-Taxable", 150);
	sqlTaxReport.AddColumn(TAX, "Sales Tax", 150);
	sqlTaxReport.AddColumn(GRANDTOTAL, "Total", 150);
	sqlTaxReport.AddColumn(COST, "My Parts Cost", 150); // PARTS COST FOR THIS INVOICE
	anon.Set(1);
	anon.WhenAction << THISBACK(anonChanged);
	ok << [=] { okPressed(); };
	cancel << [=] { cancelPressed(); };
}

void TaxWindow::okPressed()
{
	PromptOK(__func__);
}

void TaxWindow::cancelPressed()
{ // bye
	Close();
}

void TaxWindow::anonChanged()
{
	if (anon.Get() == 1) {
		// use cust Number
		 sqlTaxReport.HeaderTab(2).Show(true); // .ColumnWidths("150 200 65 1 150 150 150 200 200");
		 sqlTaxReport.HeaderTab(3).Show(false);
		return;
	}
	else  {// use cust name
		 sqlTaxReport.HeaderTab(2).Show(false); // .ColumnWidths("150 200 65 1 150 150 150 200 200");
		 sqlTaxReport.HeaderTab(3).Show(true);
		return;
	}
		
}
