#include "Reports.h"
#include <Report/Report.h>

IncomeWindow::IncomeWindow() {
	CtrlLayoutOKCancel ( *this, "Select date range and Customer for report:" );

	sqlTaxReport.AddColumn ( INVOICENUMBER, "Inv NO.", 100 );
	sqlTaxReport.AddColumn ( DATEPAID, "Date Paid", 100 ).SetConvert ( DateIntConvert() );
	sqlTaxReport.AddColumn ( CUSTNAME, "Customer Name", 140 );
	sqlTaxReport.AddColumn ( TAXABLESUB, "Taxable", 80 ).SetConvert ( ConvDouble() ).SetDisplay ( StdRightDisplay() ).HeaderTab().AlignRight();
	sqlTaxReport.AddColumn ( NONTAXABLESUB, "Non-Taxable", 80 ).SetConvert ( ConvDouble() ).SetDisplay ( StdRightDisplay() ).HeaderTab().AlignRight();
	sqlTaxReport.AddColumn ( TAX, "Sales Tax", 80 ).SetConvert ( ConvDouble() ).SetDisplay ( StdRightDisplay() ).HeaderTab().AlignRight();
	sqlTaxReport.AddColumn ( COST, "My Parts Cost", 80 ).SetConvert ( ConvDouble() ).SetDisplay ( StdRightDisplay() ).HeaderTab().AlignRight(); // PARTS COST FOR THIS INVOICE
	sqlTaxReport.AddColumn ( GRANDTOTAL, "Total", 80 ).SetConvert ( ConvDouble() ).SetDisplay ( StdRightDisplay() ).HeaderTab().AlignRight();
	
	dateStart.SetConvert ( DateIntConvert() );
	dateEnd.SetConvert ( DateIntConvert() );

 	SQL * Select(CUST_ID, CUSTNAME).From(CUSTOMERS);
 	while (SQL.Fetch())
 	{
 		cbCustomers.Add(SQL[CUST_ID], SQL[CUSTNAME]);
 	}
	
	ok << [=] { okPressed(); };
	cancel << [=] { cancelPressed(); };
	btnReport << [=] { CreateReport(dateStart.GetData().ToString(), dateEnd.GetData().ToString()); };
	btnExport.Hide(); // only for testing
	btnExport << [=] { ExportQTF(); };
}

double IncomeWindow::GetPartsCost ( int invId )
{
	SQL * Select ( SqlSum ( COST ) ).From ( PRODUCTS ).Where ( INVOICEID == invId );
	SQL.Fetch();
	double parts;
	parts = IsNull(SQL[0]) ? 0.00 : round(SQL[0], 2);
	return parts;
}

String IncomeWindow::GetCustomerName ( int custId )
{
	SQL * Select ( CUSTNAME ).From ( CUSTOMERS ).Where ( CUST_ID == custId );
	SQL.Fetch();
	return SQL[0];
}

void IncomeWindow::okPressed()
{
	if ( IsNull ( dateStart ) || IsNull ( dateEnd ) || IsNull ( cbCustomers ) )
	{
		return;
	}

	int idNum = 1;
	idNum += cbCustomers.GetIndex();
	if (IsNull(idNum) || idNum < 1)
		return;
	sqlTaxReport.Clear();
	
	Sql sql;
	SqlBool where;
	where = Between(DATEPAID, dateStart.GetData().ToString(), dateEnd.GetData().ToString());
	where = where && CUSTOMERID == idNum && STATUS > 1;
	
	sql * SelectAll().From(INVOICES).Where(where);

	while ( sql.Fetch() )
	{
		sqlTaxReport.Add ( sql[INVOICENUMBER],
						   sql[DATEPAID],
						   GetCustomerName ( idNum ),
						   round ( sql[TAXABLESUB], 2 ),
						   round ( sql[NONTAXABLESUB], 2 ),
						   round ( sql[TAX], 2 ),
						   GetPartsCost ( sql[INVOICENUMBER] ),
						   round ( sql[GRANDTOTAL], 2 ));
	}
}

void IncomeWindow::ExportQTF()
{
	SaveFile("/home/james/Desktop/TaxReportAnon.qtf", sqlTaxReport.AsQtf());
}
void IncomeWindow::CreateReport(String start, String end)
{
	if ( sqlTaxReport.GetCount() > 0 )
	{
		double sumTaxable = 0.0, sumNontaxable = 0.0, sumTax = 0.0, sumTotal = 0.0, sumParts = 0.0;
		String headertext;
		
		String s = ::Format(Date( 1970, 1, 1) + StrInt(start));
		String e = ::Format(Date( 1970, 1, 1) + StrInt(end));
		headertext << "Tax Report " << s << " to " << e << " for " << myConfig.data.companyname << "and customer: " << sqlTaxReport.Get( 0, CUSTNAME );
		
		// {{153:153:0:215:123:122:123:123:123@L [* #Inv NO.#:: #Date Paid#:: #Cust. No.#:: #Customer Name#:: #Taxable#:: #Non-Taxable#:: #Sales Tax#:: #My Parts Cost#:: #Total#::@W ]
		// {{170:171:111:0:136:137:136:137:137@L
		taxQTF = "{{153:153:123:122:123:123:123@L [+60>* Inv NO.:: Date Paid:: Taxable:: Non-Taxable:: Sales Tax:: Total:: Parts Cost::@W ][+40> ";
		int rowcount = sqlTaxReport.GetCount();
		for ( int i = 0; i < rowcount; i++ )
		{
			taxQTF << sqlTaxReport.Get( i, INVOICENUMBER ) << ":: " << 
			::Format(Date( 1970, 1, 1) + sqlTaxReport.Get( i, DATEPAID ))  << ":: " << 
			Format("%2!nl", sqlTaxReport.Get ( i, TAXABLESUB )) << ":: " << 
			Format("%2!nl", sqlTaxReport.Get ( i, NONTAXABLESUB )) << ":: " << 
			Format("%2!nl", sqlTaxReport.Get ( i, TAX )) << ":: " << 
			Format("%2!nl", sqlTaxReport.Get ( i, GRANDTOTAL )) << ":: " << 
			Format("%2!nl", (IsNull(sqlTaxReport.Get ( i, COST ))) ? 0.0 : ( double ) sqlTaxReport.Get ( i, COST ));
			if ((i % 2 == 0 )&& (i < rowcount - 1)) taxQTF << " ::@L ][+40> ";
			else if (i < rowcount - 1) taxQTF << " ::@W ][+40> ";
			else taxQTF << ":: ]";

			sumTaxable += ( double ) sqlTaxReport.Get ( i, TAXABLESUB );
			sumNontaxable += ( double ) sqlTaxReport.Get ( i, NONTAXABLESUB );
			sumTax += ( double ) sqlTaxReport.Get ( i, TAX );
			sumTotal += ( double ) sqlTaxReport.Get ( i, GRANDTOTAL );
			sumParts += (IsNull(sqlTaxReport.Get ( i, COST ))) ? 0.0 : ( double ) sqlTaxReport.Get ( i, COST );
		}
		double income1040 = sumTaxable + sumNontaxable;
		taxQTF << "[+40>* :: :: Taxable:: Non-Taxable:: Sales Tax:: Grand Total:: Parts Cost:: ][+60>* Totals:: :: ";
		taxQTF << Format("%2!nl", round(sumTaxable)) << ":: " << Format("%2!nl", round(sumNontaxable)) << ":: " << 
			Format( "%2!nl", sumTax ) << ":: " << Format("%2!nl", sumTotal) << ":: " << Format("%2!nl", sumParts) <<  "][+60>* :: Fed/State Income :: :: :: :: " << 
			Format( "%2!nl", income1040 ) << ":: :: ] }}";
		taxQTF << "[+80< &&Remember parts cost was not deducted from income on last line.]";
		Report report;
		report.Header ( headertext ).Footer ( "Page $$P" );
		report << taxQTF;
		// QtfReport((Size)(216,279), taxQTF, "", true); // no difference
		Perform ( report );
	}
}

void IncomeWindow::cancelPressed()
{
	// bye
	Close();
}

void IncomeWindow::CustChanged()
{
	sqlTaxReport.Clear();
}

