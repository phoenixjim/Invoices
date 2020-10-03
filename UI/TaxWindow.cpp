#include "DBUI.h"

TaxWindow::TaxWindow()
{
	CtrlLayoutOKCancel(*this, "Select date range for report:");
	
	sqlTaxReport.AddColumn(INVOICENUMBER, "Inv NO.");
	sqlTaxReport.AddColumn(DATEPAID, "Date Paid").SetConvert(DateIntConvert());
	sqlTaxReport.AddColumn(CUSTOMERID, "Cust. No."); // OR CUSTNAME, IF NOT ANON
	// sqlTaxReport.AddColumn(CUSTNAME, "Customer Name");
	sqlTaxReport.AddColumn(TAXABLESUB, "Taxable");
	sqlTaxReport.AddColumn(NONTAXABLESUB, "Non-Taxable");
	sqlTaxReport.AddColumn(TAX, "Sales Tax");
	sqlTaxReport.AddColumn(GRANDTOTAL, "Total");
	sqlTaxReport.AddColumn(COST, "My Parts Cost"); // PARTS COST FOR THIS INVOICE

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
	PromptOK(__func__);
	if (anon.Get() == 1)
		// use cust Number
		// sqlTaxReport.
		return;
	else // use cust name
		return;
		
}
