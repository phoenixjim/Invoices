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
	
	ok << [=] { okPressed();
				  };
	cancel << [=] { cancelPressed();
				  };
	btnReport << [=] { CreateReport(dateStart.GetData().ToString(), dateEnd.GetData().ToString());
					 };
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
	
	// NOTE: still no success here
	Sql sql;
	SqlBool where;
	where = Between(DATEPAID, dateStart.GetData().ToString(), dateEnd.GetData().ToString());
	where = where && CUSTOMERID == idNum;
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
		int strt, ending;
		
		String s = ::Format(Date( 1970, 1, 1) + StrInt(start));
		String e = ::Format(Date( 1970, 1, 1) + StrInt(end));
		headertext << "Tax Report " << s << " to " << e;
		for ( int i = 0; i < sqlTaxReport.GetCount(); i++ )
		{
			sumTaxable += ( double ) sqlTaxReport.Get ( i, TAXABLESUB );
			sumNontaxable += ( double ) sqlTaxReport.Get ( i, NONTAXABLESUB );
			sumTax += ( double ) sqlTaxReport.Get ( i, TAX );
			sumTotal += ( double ) sqlTaxReport.Get ( i, GRANDTOTAL );
			sumParts += (IsNull(sqlTaxReport.Get ( i, COST ))) ? 0.0 : ( double ) sqlTaxReport.Get ( i, COST );
		}

		taxQTF = "{{153:153:0:215:123:122:123:123:123 [C2>* :: :: :: :: Taxable:: Non-Taxable:: Sales Tax:: Parts Cost:: Grand Total:: :: Totals:: :: :: ";
		taxQTF << Format("%2!nl", sumTaxable) << ":: " << Format("%2!nl", sumNontaxable) << ":: " << Format("%2!nl", sumTax) << ":: " << Format("%2!nl", sumParts) <<":: " << Format("%2!nl", sumTotal) << "}}";
		Report report;
		report.SetStdFont ( SansSerif (12) );
		report.Landscape().Header ( headertext ).Footer ( "Page $$P" );
		report << sqlTaxReport.AsQtf() << taxQTF;

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

