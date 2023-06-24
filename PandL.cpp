#include "Reports/Reports.h"
#include <Report/Report.h>

PandLWindow::PandLWindow()
{
	CtrlLayoutOKCancel(*this, "Profit & Loss"); 
	
	/*

	btnReport << [=] { CreateReport ( dateStart.GetData().ToString(), dateEnd.GetData().ToString() ); };
	WhenClose = [=] { cleanup(); };

	lStart.SetInk(TXTCOLOR);
	lEnd.SetInk(TXTCOLOR);
	lBetween.SetInk(TXTCOLOR);
	*/

	ok << [=] { okPressed(); };
	cancel << [=] { cancelPressed(); };
	Date thisdate = GetSysDate();
	startDate.SetConvert(DateIntConvert());
	endDate.SetConvert(DateIntConvert());
	startDate.SetText( Format("%", FirstDayOfYear( thisdate ) ).ToString() );
	endDate.SetText( Format("%", thisdate ).ToString() );
	
	nameEdt.SetText( myConfig.data.companyname.ToString() ) ;
	startDate.SetFocus();

	startDate.WhenAction << [=] { refreshAll(); };
	endDate.WhenAction << [=] { refreshAll(); };
	rentDbl.WhenAction << [=] { refreshAll(); };
	utilitiesDbl.WhenAction << [=] { refreshAll(); };
	suppliesDbl.WhenAction << [=] { refreshAll(); };
	phoneDbl.WhenAction << [=] { refreshAll(); };
	travelDbl.WhenAction << [=] { refreshAll(); };
	taxesDbl.WhenAction << [=] { refreshAll(); };
	otherDbl.WhenAction << [=] { refreshAll(); };

	salesRevDbl.SetConvert( ConvDouble() ).SetText(  "0.00" );
	rentDbl.SetConvert( ConvDouble() ).SetText(  "0.00" );
	utilitiesDbl.SetConvert( ConvDouble() ).SetText(  "0.00" );
	suppliesDbl.SetConvert( ConvDouble() ).SetText(  "0.00" );
	phoneDbl.SetConvert( ConvDouble() ).SetText(  "0.00" );
	travelDbl.SetConvert( ConvDouble() ).SetText( "0.00" );
	taxesDbl.SetConvert( ConvDouble() ).SetText( "0.00" );
	otherDbl.SetConvert( ConvDouble() ).SetText( "0.00" );
	cogsDbl.SetConvert( ConvDouble() );
	totalDbl.SetConvert( ConvDouble() );
	refreshAll();
}

void PandLWindow::okPressed() {
	CreateReport(startDate.GetData().ToString(), endDate.GetData().ToString());
}

void PandLWindow::cancelPressed() {
	Close();
}

void PandLWindow::CreateReport(String start, String end)
{
	if (!(startDate.IsNotNull() && endDate.IsNotNull())) return;
	
	String plQTF;
	String s = ::Format(Date( 1970, 1, 1) + StrInt(start));
	String e = ::Format(Date( 1970, 1, 1) + StrInt(end));
	
	Report profit;
	String header = "{{4878:243:4879f0;g0; [ CUSTOMER NAME ]:: [ ]:: [ STATEMENT REPORT  ]:: [*4 Jim`'s Tech Services ]:: :: [ {{4597:804:4599 [* STARTING DATE ] :: [ to ]:: [* ENDING DATE ]:: [ " << s << " ]:: [ ]:: [ " << e << " ]  }} }}" ; 
	// outer table opened for revenue and expenses
	plQTF = "{{4878:243:4879f0;g0; [ [* REVENUE ]]:: [* ]:: [ [* EXPENSES ]]::";
	plQTF << " [ {{10000f0;g0; [ [2 Includes deductions for returns `& discounts ] ]:: ";
	plQTF << "[ {{5000:5000 [ [2 Sales Revenue ] ]:: [< `$ 0.00 ] :: [2 Other Revenue ] :: [< `$ ";
	
	double nowTaxable = 0.0, nowNontaxable = 0.0, nowParts = 0.0;
	double rent = 0.00, utilities = 0.00, supplies = 0.00, phone = 0.00, expenses = 0.00, travel = 0.00;
	double taxes = 0.00, other = 0.00, total = 0.00, netIncome = 0.00, debits = 0.00;
	
	Sql sql;
	SqlBool where;
	where = Between(DATEPAID, startDate.GetData().ToString(), endDate.GetData().ToString()) && STATUS > 0;
	
	sql * SelectAll().From(INVOICES).Where(where);

	while ( sql.Fetch() )
	{
		nowTaxable += (double) sql[TAXABLESUB];
		nowNontaxable += (double) sql[NONTAXABLESUB];
		nowParts += round((double)GetPartsCost(sql[INVOICENUMBER]), 2);
	}
	plQTF << DblStr( nowTaxable + nowNontaxable ) << " ]:: [ [c$1 Gross Revenue] ]:: [ `$ ";
	plQTF << DblStr( nowTaxable + nowNontaxable ) << " ]}}&][  ]}}&][  &][  &]";
	plQTF << "[ {{10000f0;g0; &[ [* COST OF GOODS SOLD ] ]:: [ {{5000:5000 [ [c$1 Total COGS]  ]:: [ `$ ";
	plQTF << DblStr( nowParts ) << " ]}}&][  &][ &] [ &] [c$l>*  Gross Profit ]:: [ ";
	plQTF << "{{5000:4000 [ [/2 Gross Revenue minus COGS ] ]:: [ `$ ";
	plQTF << DblStr( nowTaxable + nowNontaxable - nowParts ) << "] }} ][  ]:: [ ]}} ]:: [ ]:: [ ";

	// otherRevDbl.SetConvert( ConvDouble() ).SetText( DblStr( nowTaxable + nowNontaxable ) );
	// cogsDbl.SetText( DblStr( nowParts ) ) ;

	rent = StrDbl( rentDbl.GetData().ToString() );
	utilities = StrDbl( utilitiesDbl.GetData().ToString() );
	supplies = StrDbl( suppliesDbl.GetData().ToString() );
	phone = StrDbl( phoneDbl.GetData().ToString() );
	travel = StrDbl( travelDbl.GetData().ToString() );
	taxes = StrDbl( taxesDbl.GetData().ToString() );
	other = StrDbl( otherDbl.GetData().ToString() );

	debits = rent + utilities + supplies + expenses + travel + taxes + other + phone;
	
	plQTF << "{{5000:5000 [ Rent ]:: [ `$ " << DblStr ( rent );
	plQTF << "] :: [ Utilities ]:: [ `$ " << DblStr ( utilities );
	plQTF << "] :: [ Office Supplies ]:: [ `$ " << DblStr ( supplies );
	plQTF << "] :: [ Internet `& Phone ]:: [ `$ " << DblStr ( phone );
	plQTF << "] :: [ Travel ]:: [ `$ " << DblStr ( travel );
	plQTF << "] :: [ Taxes ]:: [ `$ " << DblStr ( taxes );
	plQTF << "] :: [ Other Expenses ]:: [ `$ " << DblStr ( other );
	plQTF << "] :: [*c$1 Total Expenses ] :: [ `$ " << DblStr ( debits );
	plQTF << "] }}"; // Debits table closed
	
	total = nowTaxable + nowNontaxable - debits - nowParts;
	// Begin gross profit table
	plQTF << " [ &][ & ] [ &] [ &] ";
	plQTF << " [ {{10000f0;g0; [c$1* Net Income ] :: [ ";
	plQTF << "{{5454:4546 [/2 Gross profit minus total expenses ] :: [ `$ ";
	plQTF << DblStr ( total ) << " ] }} ] }} ]";
	plQTF << " }}"; // close outer table.

	profit.Header(header);
	profit << plQTF;
	Perform( profit );
	
}

double PandLWindow::GetPartsCost ( int invId )
{
	SQL * Select ( SqlSum ( COST ) ).From ( PRODUCTS ).Where ( INVOICEID == invId );
	SQL.Fetch();
	double parts = IsNull(SQL[0]) ? 0.00 : round(SQL[0], 2);
	return parts;
}

void PandLWindow::refreshAll()
{
	
	double nowTaxable = 0.0, nowNontaxable = 0.0, nowParts = 0.0;
	double rent = 0.00, utilities = 0.00, supplies = 0.00, travel = 0.00;
	double phone = 0.00, taxes = 0.00, other = 0.00, total = 0.00, netIncome = 0.00, debits = 0.00;
	
	Sql sql;
	SqlBool where;
	where = Between(DATEPAID, startDate.GetData().ToString(), endDate.GetData().ToString()) && STATUS > 0;
	
	sql * SelectAll().From(INVOICES).Where(where);

	while ( sql.Fetch() )
	{
		nowTaxable += (double) sql[TAXABLESUB];
		nowNontaxable += (double) sql[NONTAXABLESUB];
		nowParts += round((double)GetPartsCost(sql[INVOICENUMBER]), 2);
	}
	otherRevDbl.SetConvert( ConvDouble() ).SetText( DblStr( nowTaxable + nowNontaxable ) );
	cogsDbl.SetText( DblStr( nowParts ) ) ;

	rent = StrDbl( rentDbl.GetData().ToString() );
	utilities = StrDbl( utilitiesDbl.GetData().ToString() );
	supplies = StrDbl( suppliesDbl.GetData().ToString() );
	phone = StrDbl( phoneDbl.GetData().ToString() );
	travel = StrDbl( travelDbl.GetData().ToString() );
	taxes = StrDbl( taxesDbl.GetData().ToString() );
	other = StrDbl( otherDbl.GetData().ToString() );

	debits = rent + utilities + supplies + phone + travel + taxes + other;

	grossDbl.SetConvert( ConvDouble() ).SetText( DblStr( nowTaxable + nowNontaxable - nowParts ) );
	totalDbl.SetText( DblStr( debits ) );
	netIncomeDbl.SetConvert( ConvDouble() ).SetText( DblStr( nowTaxable + nowNontaxable - nowParts - debits ) );
}