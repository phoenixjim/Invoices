#include "Reports.h"
#include <Report/Report.h>

TaxWindow::TaxWindow()
{
	CtrlLayoutOKCancel ( *this, "Select date range for report:" );

	sqlTaxReport.AddColumn ( INVOICENUMBER, "Inv NO.", 100 );
	sqlTaxReport.AddColumn ( DATEPAID, "Date Paid", 100 ).SetConvert ( DateIntConvert() );
	sqlTaxReport.AddColumn ( CUSTOMERID, "Cust. No.", 65 ); // OR CUSTNAME, IF NOT ANON
	sqlTaxReport.AddColumn ( CUSTNAME, "Customer Name", 140 ).HeaderTab().Show ( false );
	sqlTaxReport.AddColumn ( TAXABLESUB, "Taxable", 80 ).SetConvert ( ConvDouble() ).SetDisplay ( StdRightDisplay() ).HeaderTab().AlignRight(); //
	sqlTaxReport.AddColumn ( NONTAXABLESUB, "Non-Taxable", 80 ).SetConvert ( ConvDouble() ).SetDisplay ( StdRightDisplay() ).HeaderTab().AlignRight();
	sqlTaxReport.AddColumn ( TAX, "Sales Tax", 80 ).SetConvert ( ConvDouble() ).SetDisplay ( StdRightDisplay() ).HeaderTab().AlignRight(); // .SetConvert ( ConvDouble() )
	sqlTaxReport.AddColumn ( COST, "My Parts Cost", 80 ).SetConvert ( ConvDouble() ).SetDisplay ( StdRightDisplay() ).HeaderTab().AlignRight(); // .SetConvert ( ConvDouble() ) PARTS COST FOR THIS INVOICE
	sqlTaxReport.AddColumn ( AMTPAID, "Total", 80 ).SetConvert ( ConvDouble() ).SetDisplay ( StdRightDisplay() ).HeaderTab().AlignRight(); // .SetConvert ( ConvDouble() )
	
	dateStart.SetConvert ( DateIntConvert() );
	dateEnd.SetConvert ( DateIntConvert() );
	
	Date now = GetSysDate();
	dateStart.SetText( Format("%", FirstDayOfYear( now ) ).ToString() );
	dateEnd.SetText( Format("%", now ).ToString() );
	// Anonymous by default, hide names
	anon.Set ( 1 );

	anon.WhenAction << [=] { anonChanged(); };
	ok << [=] { okPressed();
			  };
	cancel << [=] { cancelPressed();
				  };
	btnReport << [=] { CreateReport(dateStart.GetData().ToString(), dateEnd.GetData().ToString());
					 };
	lStart.SetInk(TXTCOLOR);
	lEnd.SetInk(TXTCOLOR);
	anon.SetColor(TXTCOLOR);
}

double TaxWindow::GetPartsCost ( int invId )
{
	SQL * Select ( SqlSum ( COST ) ).From ( PRODUCTS ).Where ( INVOICEID == invId );
	SQL.Fetch();
	double parts = IsNull(SQL[0]) ? 0.00 : round(SQL[0], 2);
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
	SqlBool where;
	where = Between(DATEPAID,  dateStart.GetData().ToString(), dateEnd.GetData().ToString());

	sql * SelectAll().From(INVOICES).Where(where);

	while ( sql.Fetch() )
	{
		sqlTaxReport.Add ( sql[INVOICENUMBER],
						   sql[DATEPAID],
						   sql[CUSTOMERID],
						   GetCustomerName ( sql[CUSTOMERID] ),
						   (double)sql[TAXABLESUB] ,
						   (double)sql[NONTAXABLESUB],
						   (double)sql[TAX],
						   GetPartsCost ( sql[INVOICENUMBER] ),
						   (double)sql[AMTPAID]);
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
		
		String s = ::Format(Date( 1970, 1, 1) + StrInt(start));
		String e = ::Format(Date( 1970, 1, 1) + StrInt(end));
		headertext << "Tax Report " << s << " to " << e << " for " << myConfig.data.companyname;
		if (anon.Get() == 1)
			taxQTF = "{{110:171:111:0:136:137:136:137:137@L [+60>* Inv NO.:: Date Paid:: Cust NO.:: Customer Name:: Taxable:: Non-Taxable:: Sales Tax:: Parts Cost:: Grand Total::@W ][+90> ";
		else taxQTF = "{{110:153:0:205:123:132:123:123:123@L [+60>* Inv NO.:: Date Paid:: Cust NO.:: Customer Name:: Taxable:: Non-Taxable:: Sales Tax:: Parts Cost:: Grand Total::@W ][+90> ";
		int rowcount = sqlTaxReport.GetCount();
		int custID;
		for ( int i = 0; i < rowcount; i++ )
		{

			taxQTF << sqlTaxReport.Get( i, INVOICENUMBER ) << ":: " << 
				::Format(Date( 1970, 1, 1) + sqlTaxReport.Get( i, DATEPAID ))  << ":: " << 
				sqlTaxReport.Get( i, CUSTOMERID )  << ":: " << 
				sqlTaxReport.Get( i, CUSTNAME )  << ":: " << 
				prnMoney(sqlTaxReport.Get ( i, TAXABLESUB )) << ":: " << 
				prnMoney(sqlTaxReport.Get ( i, NONTAXABLESUB )) << ":: " << 
				prnMoney(sqlTaxReport.Get ( i, TAX )) << ":: " << 
				prnMoney(sqlTaxReport.Get ( i, COST )) << ":: " << 
				prnMoney(sqlTaxReport.Get ( i, AMTPAID ));
			if ((i % 2 == 0 )&& (i < rowcount - 1)) taxQTF << " ::@L ][+90> ";
			else if (i < rowcount - 1) taxQTF << " ::@W ][+90> ";
			else taxQTF << " :: ] }} {{110:153:205:123:132:123:123:123@L ";

			sumTaxable += (double)sqlTaxReport.Get ( i, TAXABLESUB );
			sumNontaxable += (double)sqlTaxReport.Get ( i, NONTAXABLESUB );
			sumTax += (double)sqlTaxReport.Get ( i, TAX );
			sumTotal += (double)sqlTaxReport.Get ( i, AMTPAID );
			sumParts += (double)sqlTaxReport.Get ( i, COST );
		}
		// []&:: []&:: []&:: 
		if (anon.Get() == 1)
		{
			taxQTF << "[+70>* Taxable&[ " << prnMoney( sumTaxable) << " ] :: Non-Taxable&[ " << prnMoney( sumNontaxable);
			taxQTF << " ] :: Sales Tax&[ " << prnMoney( sumTax) <<  " ] :: COGS&[ " << prnMoney( sumParts);
			taxQTF << " ] :: Grand Total&[ " << prnMoney( sumTotal) << " ] :: ";
		}
		else
		{
			taxQTF << "[+60>* Taxable&[ " << prnMoney( sumTaxable) << " ] :: Non-Taxable&[ " << prnMoney( sumNontaxable);
			taxQTF << " ] :: Sales Tax&[ " << prnMoney( sumTax) <<  " ] :: COGS&[ " << prnMoney( sumParts);
			taxQTF << " ] :: Grand Total&[ " << prnMoney( sumTotal) << " ] :: ";
		}
		taxQTF << " :: [ Fed/State Income:: " << prnMoney( sumTotal - sumTax ) << " ] }}"; // was income1040 instead of sumTotal
			
		taxQTF << "[+80<@R &&Remember to deduct parts cost from income on Schedule C (Not done here) ]";
		Report report;
	
		// report.SetStdFont ( SansSerif (12) );
		report.Header ( headertext ).NoFooter(); //.Footer ( "Page $$P of " ) ;
		report << taxQTF;
		// report.Margins(400,400);
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

