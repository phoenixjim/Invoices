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
	sqlTaxReport.AddColumn ( AMTPAID, "Total", 80 ).SetConvert ( ConvDouble() ).SetDisplay ( StdRightDisplay() ).HeaderTab().AlignRight();
	
	dateStart.SetConvert ( DateIntConvert() );
	dateEnd.SetConvert ( DateIntConvert() );

	Date now = GetSysDate();
	dateStart.SetText( Format("%", FirstDayOfYear( now ) ).ToString() );
	dateEnd.SetText( Format("%", now ).ToString() );

 	SQL * Select(CUST_ID, CUSTNAME).From(CUSTOMERS);
 	while (SQL.Fetch())
 	{
 		cbCustomers.Add(SQL[CUST_ID], SQL[CUSTNAME]);
 	}
	
	ok << [=] { okPressed(); };
	cancel << [=] { cancelPressed(); };
	btnReport << [=] { CreateReport(dateStart.GetData().ToString(), dateEnd.GetData().ToString()); };
	voided.WhenAction << [=] { voidedChanged(); };

	dv___0.SetInk(TXTCOLOR);
	dv___1.SetInk(TXTCOLOR);
	dv___2.SetInk(TXTCOLOR);
	voided.Set(1).SetColor(TXTCOLOR);
}

double IncomeWindow::GetPartsCost ( int invId )
{
	SQL * Select ( SqlSum ( COST ) ).From ( PRODUCTS ).Where ( INVOICEID == invId );
	SQL.Fetch();
	double parts = IsNull(SQL[0]) ? 0.00 : round(SQL[0], 2);
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
	if (voided.Get()== 0) where = where && AMTPAID > 0.00;
	
	sql * SelectAll().From(INVOICES).Where(where);

	while ( sql.Fetch() )
	{
		sqlTaxReport.Add ( sql[INVOICENUMBER],
						   sql[DATEPAID],
						   GetCustomerName ( idNum ),
						   (double)sql[TAXABLESUB], //100.0,
						   (double)sql[NONTAXABLESUB], //100.0,
						   (double)sql[TAX], //100.0,
						   GetPartsCost ( sql[INVOICENUMBER] ),
						   (double)sql[AMTPAID]); //100.0);
	}
}

void IncomeWindow::ExportQTF() // LINUX ONLY - FIX THIS
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
		headertext << "Tax Report " << s << " to " << e << " for " << myConfig.data.companyname << " and customer: " << sqlTaxReport.Get( 0, CUSTNAME );
		
		taxQTF = "{{110:153:123:122:123:123:123@L [+70>* Inv NO.:: Date Paid:: Taxable:: Non-Taxable:: Sales Tax:: Total:: Parts Cost::@W ][+90> ";
		int rowcount = sqlTaxReport.GetCount();
		for ( int i = 0; i < rowcount; i++ )
		{
			taxQTF << sqlTaxReport.Get( i, INVOICENUMBER ) << ":: " << 
			::Format(Date( 1970, 1, 1) + sqlTaxReport.Get( i, DATEPAID ))  << ":: " << 
			prnMoney(sqlTaxReport.Get ( i, TAXABLESUB )) << ":: " << 
			prnMoney(sqlTaxReport.Get ( i, NONTAXABLESUB )) << ":: " << 
			prnMoney(sqlTaxReport.Get ( i, TAX )) << ":: " << 
			prnMoney(sqlTaxReport.Get ( i, AMTPAID )) << ":: " << 
			prnMoney(sqlTaxReport.Get ( i, COST ));
			if ((i % 2 == 0 )&& (i < rowcount - 1)) taxQTF << " ::@L ][+90> ";
			else if (i < rowcount - 1) taxQTF << " ::@W ][+90> ";
			else taxQTF << " ] }} {{110:153:205:123:132:123:123:123@L ";

			sumTaxable += (double)sqlTaxReport.Get ( i, TAXABLESUB );
			sumNontaxable += (double)sqlTaxReport.Get ( i, NONTAXABLESUB );
			sumTax += (double)sqlTaxReport.Get ( i, TAX );
			sumTotal += (double)sqlTaxReport.Get ( i, AMTPAID );
			sumParts += (double)sqlTaxReport.Get ( i, COST );
		}

		taxQTF << "[+70>* Taxable&[ " << prnMoney( sumTaxable) << " ] :: Non-Taxable&[ " << prnMoney( sumNontaxable);
		taxQTF << " ] :: Sales Tax&[ " << prnMoney( sumTax) <<  " ] :: COGS&[ " << prnMoney( sumParts);
		taxQTF << " ] :: Grand Total&[ " << prnMoney( sumTotal) << " ] :: ";

		taxQTF << " [ Fed/State Gross:: " << prnMoney( sumTotal - sumTax ) << " ] :: NET& " << prnMoney( sumTotal - sumTax - sumParts ) << " }}"; // was income1040 instead of sumTotal
			
		taxQTF << "[+80< &&Remember use 'Fed/State Gross' for income, parts cost separate on Schedule C.]";
		Report report;
		report.Header ( headertext ).Footer ( "Page $$P" );
		report << taxQTF;
		// report.Margins(400,400);
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

void IncomeWindow::voidedChanged()
{
	okPressed();
}
