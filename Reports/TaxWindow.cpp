#include "Reports.h"
#include <Report/Report.h>

TaxWindow::TaxWindow()
{
	CtrlLayoutOKCancel ( *this, "Select date range for report:" );
	MinimizeBox();

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
	voided.WhenAction << [=] { voidedChanged(); };
	noCust.WhenAction << [=] { noCustChanged(); };
	
	ok << [=] { okPressed();
			  };
	cancel << [=] { cancelPressed();
				  };
	btnReport << [=] { 
		if ( SalesTax.Get() == 0 ) CreateReport(dateStart.GetData().ToString(), dateEnd.GetData().ToString());
		else CreateSalesTaxReport( dateStart.GetData().ToString(), dateEnd.GetData().ToString() );
	};
	
	btnSalesTax << [=] { CreateSalesTaxReport( dateStart.GetData().ToString(), dateEnd.GetData().ToString() ); };
	
	lStart.SetInk(TXTCOLOR);
	lEnd.SetInk(TXTCOLOR);
	anon.SetColor(TXTCOLOR);
	noCust.SetColor(TXTCOLOR);
	SalesTax.SetColor(TXTCOLOR);	
	voided.Set(0).SetColor(TXTCOLOR);
}

void TaxWindow::CreateSalesTaxReport(String start, String end)
{
	if (!(dateStart.IsNotNull() && dateEnd.IsNotNull())) return;
	
	String plQTF;
	String s = ::Format(Date( 1970, 1, 1) + StrInt(start));
	String e = ::Format(Date( 1970, 1, 1) + StrInt(end));
	
	Report salesTax;
	String header = "{{4878:243:4879f0;g0; [ COMPANY NAME ]:: [ ]:: [ SALES TAX REPORT  ]:: [*4 " << myConfig.data.companyname 
		<< " ]:: :: [ {{4597:804:4599 [* STARTING DATE ] :: [ ]:: [* ENDING DATE ]:: [ " << s << " ]:: [ ]:: [ " << e << " ]  }} }}"; 

	// outer table opened for revenue and sales tax collected
	// plQTF = "{{4878:243:4879f0;g0; [ [* REVENUE ]]:: [* ]:: [ [* SALES TAX COLLECTED ]]:: }}";
	double nowTaxable = 0.0, nowNontaxable = 0.0, nowTax = 0.0, calcTax = 0.0;
	
	plQTF = "{{5000:500:5000f0;g0 [* REVENUE{{5000:5000 [* TAXABLE ]:: [ ";
	
	Sql sql;
	SqlBool where;
	where = Between(DATEPAID, dateStart.GetData().ToString(), dateEnd.GetData().ToString()) && STATUS > 0;
	
	sql * SelectAll().From(INVOICES).Where(where);

	while ( sql.Fetch() )
	{
		nowTaxable += (double) sql[TAXABLESUB];
		nowNontaxable += (double) sql[NONTAXABLESUB];
		nowTax += (double) sql[TAX];
	}
	calcTax = nowTaxable * 0.08;
	
	plQTF << prnMoney( nowTaxable ) << " ] }} {{5000:5000 [* TAX EXEMPT ]:: [ ";
	plQTF << prnMoney( nowNontaxable ) << " ] }} {{5000:5000 [ NET ]:: [ ";
	plQTF << prnMoney( nowTaxable + nowNontaxable ) << " ] }} ]:: [  ]:: [* SALES TAX COLLECTED {{5000:5000 [* TOTAL ]:: [ ";
	plQTF << prnMoney( ( nowTax > calcTax ) ? nowTax : calcTax ) << " ] }} {{5000:5000 [* CHARGED ]:: [ ";
	plQTF << prnMoney( nowTax ) << " ] }}  {{5000:5000 [ Calculated ]:: [ ";
	plQTF << prnMoney( calcTax ) << " ] }} }}";

	salesTax.Header(header);
	salesTax << plQTF;
	Perform( salesTax );
	
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
	if (voided.Get()== 0) where = where && AMTPAID > 0.00;

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
	if(sqlTaxReport.GetCount() > 0) {
		double sumTaxable = 0.0, sumNontaxable = 0.0, sumTax = 0.0, sumTotal = 0.0,
			   sumParts = 0.0;
		String headertext;
		int rowcount = sqlTaxReport.GetCount();
		int custID;

		String s = ::Format(Date(1970, 1, 1) + StrInt(start));
		String e = ::Format(Date(1970, 1, 1) + StrInt(end));
		headertext << "Tax Report " << s << " to " << e << " for " << myConfig.data.companyname;

		if(SalesTax.Get() == 0) {
			if(noCust.Get() == 1)
				taxQTF = "{{110:171:0:0:145:145:145:145:145@L [+60>* Inv NO.:: Date Paid:: "
				         "Cust NO.:: Customer Name:: Taxable:: Non-Taxable:: Sales Tax:: Parts "
				         "Cost:: Grand Total::@W ][+90> ";

			else {
				if(anon.Get() == 1)
					taxQTF = "{{110:171:75:0:135:135:135:135:135@L [+60>* Inv NO.:: Date "
					         "Paid:: Cust NO.:: Customer Name:: Taxable:: Non-Taxable:: Sales "
					         "Tax:: Parts Cost:: Grand Total::@W ][+90> ";
				else
					taxQTF = "{{110:153:0:205:123:132:123:123:123@L [+60>* Inv NO.:: Date "
					         "Paid:: Cust NO.:: Customer Name:: Taxable:: Non-Taxable:: Sales "
					         "Tax:: Parts Cost:: Grand Total::@W ][+90> ";
			}
		}

		for(int i = 0; i < rowcount; i++) {

			if(SalesTax.Get() == 0) {
				taxQTF << sqlTaxReport.Get(i, INVOICENUMBER)
					   << ":: " << ::Format(Date(1970, 1, 1) + sqlTaxReport.Get(i, DATEPAID))
					   << ":: " << sqlTaxReport.Get(i, CUSTOMERID)
					   << ":: " << sqlTaxReport.Get(i, CUSTNAME)
					   << ":: " << prnMoney(sqlTaxReport.Get(i, TAXABLESUB))
					   << ":: " << prnMoney(sqlTaxReport.Get(i, NONTAXABLESUB))
					   << ":: " << prnMoney(sqlTaxReport.Get(i, TAX))
					   << ":: " << prnMoney(sqlTaxReport.Get(i, COST))
					   << ":: " << prnMoney(sqlTaxReport.Get(i, AMTPAID));

				if((i % 2 == 0) && (i < rowcount - 1))
					taxQTF << " ::@L ][+90> ";
				else if(i < rowcount - 1)
					taxQTF << " ::@W ][+90> ";
				else
					taxQTF << " :: ] }} {{110:153:205:123:132:123:123:123@L [+90>* ";

			sumTaxable += (double)sqlTaxReport.Get(i, TAXABLESUB);

			sumNontaxable += (double)sqlTaxReport.Get(i, NONTAXABLESUB);

			sumTax += (double)sqlTaxReport.Get(i, TAX);

			sumTotal += (double)sqlTaxReport.Get(i, AMTPAID);

			sumParts += (double)sqlTaxReport.Get(i, COST);
			}

			else {
				CreateSalesTaxReport( start, end );
				return;
			}

		}

		// if ( noCust.Get() == 1 )
		/* if(SalesTax.Get() == 1) {
			taxQTF << " {{110:153:140:123:123:162:123@1 ";
			taxQTF << " [+70>* Taxable&[ " << prnMoney(sumTaxable) << " ] :: Non-Taxable&[ "
				   << prnMoney(sumNontaxable);
			taxQTF << " ] :: Sales Tax&[ " << prnMoney(sumTax) << " ] :: COGS&[ "
				   << prnMoney(sumParts);
			taxQTF << " ] :: Grand Total&[ " << prnMoney(sumTotal) << " ] :: ";
		}

		else */
		 if(anon.Get() == 1) {
			taxQTF << " [+70>* Taxable&[ " << prnMoney(sumTaxable) << " ] :: Non-Taxable&[ "
				   << prnMoney(sumNontaxable);
			taxQTF << " ] :: Sales Tax&[ " << prnMoney(sumTax) << " ] :: COGS&[ "
				   << prnMoney(sumParts);
			taxQTF << " ] :: Grand Total&[ " << prnMoney(sumTotal) << " ] :: ";
		}

		else {
			taxQTF << "[+60>* Taxable&[ " << prnMoney(sumTaxable) << " ] :: Non-Taxable&[ "
				   << prnMoney(sumNontaxable);
			taxQTF << " ] :: Sales Tax&[ " << prnMoney(sumTax) << " ] :: COGS&[ "
				   << prnMoney(sumParts);
			taxQTF << " ] :: Grand Total&[ " << prnMoney(sumTotal) << " ] :: ";
		}

		taxQTF << " [ Fed/State Gross& " << prnMoney(sumTotal - sumTax) << " ] :: NET Profit& "
			   << prnMoney(sumTotal - sumTax - sumParts) << " }}";
		if(SalesTax.Get() == 0)
			taxQTF << "[+80< &&Remember use 'Fed/State Gross' for income, parts cost separate "
			          "on Schedule C.]";
		Report report;

		// report.SetStdFont ( SansSerif (12) );
		report.Header(headertext).NoFooter(); //.Footer ( "Page $$P of " ) ;
		report << taxQTF;
		// report.Margins(400,400);
		Perform(report);
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

void TaxWindow::noCustChanged()
{
	if (noCust.Get() == 1)
	{
		// use cust Number
		sqlTaxReport.HeaderTab ( CUSTOMERID ).Show ( false ); // .ColumnWidths("150 200 65 1 150 150 150 200 200");
		sqlTaxReport.HeaderTab ( CUSTNAME ).Show ( false );
		anon.Disable();
		return;
	}
	else
	{
		anon.Enable(true);
		anonChanged();
	}
}

void TaxWindow::voidedChanged()
{
	okPressed();
}