#include "Reports.h"
#include <Report/Report.h>

TaxWindow::TaxWindow()
{
	CtrlLayoutOKCancel ( *this, "Select date range for report:" );

	sqlTaxReport.AddColumn ( INVOICENUMBER, "Inv NO.", 100 );
	sqlTaxReport.AddColumn ( DATEPAID, "Date Paid", 100 ).SetConvert ( DateIntConvert() );
	sqlTaxReport.AddColumn ( CUSTOMERID, "Cust. No.", 65 ); // OR CUSTNAME, IF NOT ANON
	sqlTaxReport.AddColumn ( CUSTNAME, "Customer Name", 140 ).HeaderTab().Show ( false );
	sqlTaxReport.AddColumn ( TAXABLESUB, "Taxable", 80 ).SetConvert ( ConvDouble() ).SetDisplay ( StdRightDisplay() ).HeaderTab().AlignRight();
	sqlTaxReport.AddColumn ( NONTAXABLESUB, "Non-Taxable", 80 ).SetConvert ( ConvDouble() ).SetDisplay ( StdRightDisplay() ).HeaderTab().AlignRight();
	sqlTaxReport.AddColumn ( TAX, "Sales Tax", 80 ).SetConvert ( ConvDouble() ).SetDisplay ( StdRightDisplay() ).HeaderTab().AlignRight();
	sqlTaxReport.AddColumn ( COST, "My Parts Cost", 80 ).SetConvert ( ConvDouble() ).SetDisplay ( StdRightDisplay() ).HeaderTab().AlignRight(); // PARTS COST FOR THIS INVOICE
	sqlTaxReport.AddColumn ( GRANDTOTAL, "Total", 80 ).SetConvert ( ConvDouble() ).SetDisplay ( StdRightDisplay() ).HeaderTab().AlignRight();
	
	dateStart.SetConvert ( DateIntConvert() );
	dateEnd.SetConvert ( DateIntConvert() );
	// Anonymous by default, hide names
	anon.Set ( 1 );

	anon.WhenAction << THISBACK ( anonChanged );
	ok << [=] { okPressed();
			  };
	cancel << [=] { cancelPressed();
				  };
	btnReport << [=] { CreateReport(dateStart.GetData().ToString(), dateEnd.GetData().ToString());
					 };
	btnExport << [=] { ExportQTF(); };
}

double TaxWindow::GetPartsCost ( int invId )
{
	SQL * Select ( SqlSum ( COST ) ).From ( PRODUCTS ).Where ( INVOICEID == invId );
	SQL.Fetch();
	double parts;
	parts = IsNull(SQL[0]) ? 0.00 : round(SQL[0], 2);
	return parts;
}

String TaxWindow::GetCustomerName ( int custId )
{
	SQL * Select ( CUSTNAME ).From ( CUSTOMERS ).Where ( CUST_ID == custId );
	SQL.Fetch();
	return SQL[0];
}

void TaxWindow::okPressed()
{
	if ( IsNull ( dateStart ) || IsNull ( dateEnd ) )
	{
		return;
	}

	sqlTaxReport.Clear();
	Sql sql;
	String commandstring = "Select * from INVOICES Where DATEPAID Between " + dateStart.GetData().ToString() + " AND " + dateEnd.GetData().ToString();

	sql.Execute ( commandstring );

	while ( sql.Fetch() )
	{
		sqlTaxReport.Add ( sql[INVOICENUMBER],
						   sql[DATEPAID],
						   sql[CUSTOMERID],
						   GetCustomerName ( sql[CUSTOMERID] ),
						   round ( sql[TAXABLESUB], 2 ),
						   round ( sql[NONTAXABLESUB], 2 ),
						   round ( sql[TAX], 2 ),
						   GetPartsCost ( sql[INVOICENUMBER] ),
						   round ( sql[GRANDTOTAL], 2 ));
	}

}

void TaxWindow::ExportQTF()
{
	SaveFile("/home/james/Desktop/TaxReportAnon.qtf", sqlTaxReport.AsQtf());
}
void TaxWindow::CreateReport(String start, String end)
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

		if (anon.Get() == 1)
			taxQTF = "{{170:171:111:0:136:137:136:137:137 [C2>* :: :: :: :: Taxable:: Non-Taxable:: Sales Tax:: Parts Cost:: Grand Total:: :: Totals:: :: :: ";
		else taxQTF = "{{153:153:0:215:123:122:123:123:123 [C2>* :: :: :: :: Taxable:: Non-Taxable:: Sales Tax:: Parts Cost:: Grand Total:: :: Totals:: :: :: ";
		// taxQTF << ":: :: :: :: :: :: :: ::";
		taxQTF << Format("%2!nl", sumTaxable) << ":: " << Format("%2!nl", sumNontaxable) << ":: " << Format("%2!nl", sumTax) << ":: " << Format("%2!nl", sumParts) <<":: " << Format("%2!nl", sumTotal) << "}}";
		Report report;
		report.SetStdFont ( SansSerif (12) );
		report.Landscape().Header ( headertext ).Footer ( "Page $$P" );
		report << sqlTaxReport.AsQtf() << taxQTF;

		Perform ( report );
	}
}

void TaxWindow::cancelPressed()
{
	// bye
	Close();
}

void TaxWindow::anonChanged()
{
	if ( anon.Get() == 1 )
	{
		// use cust Number
		sqlTaxReport.HeaderTab ( CUSTOMERID ).Show ( true ); // .ColumnWidths("150 200 65 1 150 150 150 200 200");
		sqlTaxReport.HeaderTab ( CUSTNAME ).Show ( false );
		return;
	}

	else
	{
		// use cust name
		sqlTaxReport.HeaderTab ( CUSTOMERID ).Show ( false ); // .ColumnWidths("150 200 65 1 150 150 150 200 200");
		sqlTaxReport.HeaderTab ( CUSTNAME ).Show ( true );
		return;
	}

}

