#include "Reports.h"
#include <Report/Report.h>

YearToYearWindow::YearToYearWindow()
{
	CtrlLayoutOKCancel(*this, "Year by Year Report"); 
	MinimizeBox();
	
	sqlTaxReport.AddColumn("Accounts", 50);
	sqlTaxReport.AddColumn("Current", 50).SetConvert(ConvDouble());
	sqlTaxReport.AddColumn("Previous", 50).SetConvert(ConvDouble());
	sqlTaxReport.AddColumn("Change", 50).SetConvert(ConvDouble());
	sqlTaxReport.AddColumn("Percent", 50);
	
	dateYear.SetText("1");
	Date thisdate = GetSysDate();
	dateYear.Max((int)thisdate.year - 2018);
	dateStart.SetConvert(DateIntConvert());
	dateEnd.SetConvert(DateIntConvert());
	
	dateStart.SetText( Format("%", FirstDayOfYear( thisdate ) ).ToString() );
	dateEnd.SetText( Format("%", thisdate ).ToString() );
	
	ok << [=] { okPressed(); };
	cancel << [=] { cancelPressed(); };
	btnReport << [=] { CreateReport ( dateStart.GetData().ToString(), dateEnd.GetData().ToString() ); };
	WhenClose = [=] { cleanup(); };

	lStart.SetInk(TXTCOLOR);
	lEnd.SetInk(TXTCOLOR);
	lBetween.SetInk(TXTCOLOR);
}
void YearToYearWindow::cleanup()
{
	dateYear.SetText("1");
	sqlTaxReport.Clear();
	dateStart.Clear();
	dateEnd.Clear();
	Close();
}
void YearToYearWindow::cancelPressed()
{
	cleanup();
}

void YearToYearWindow::okPressed()
{	// generate table
	// get needed data:
	double nowTaxable = 0.0, nowNontaxable = 0.0, nowParts = 0.0,
		thenTaxable = 0.0, thenNontaxable = 0.0, thenParts = 0.0,
		thenGross = 0.0, thenNet = 0.0, partsChange = 0.0, partsPercent = 0.0,
		grossChange = 0.0, grossPercent = 0.0, netChange = 0.0, netPercent = 0.0;
	
	if (IsNull(dateStart) || IsNull(dateEnd)) return;
	int yearIndex = (dateYear.IsNotNull() ? StrInt(dateYear.GetData().ToString()) : 1);
	prevDateStart = Date(1970-yearIndex, 1, 1) + StrInt(~dateStart.GetData());
	prevDateEnd = Date(1970-yearIndex, 1, 1) + StrInt(~dateEnd.GetData());

	prevStart = IntStr(prevDateStart.Get() - Date(1970, 1, 1).Get());
	prevEnd   = IntStr(prevDateEnd.Get() - Date(1970, 1, 1).Get());
	
	// This years info:
	Sql sql;
	SqlBool where;
	where = Between(DATEPAID, dateStart.GetData().ToString(), dateEnd.GetData().ToString()) && STATUS > 0;
	
	sqlTaxReport.Clear();

	sql * SelectAll().From(INVOICES).Where(where);

	while ( sql.Fetch() )
	{
		nowTaxable += (double) sql[TAXABLESUB];
		nowNontaxable += (double) sql[NONTAXABLESUB];
		nowParts += round((double)GetPartsCost(sql[INVOICENUMBER]), 2);
	}

	double nowGross = nowTaxable + nowNontaxable;
	double nowNet = nowGross - nowParts;
	
	// Last Years info:
	Sql oldsql;
	SqlBool oldwhere;
	oldwhere = Between(DATEPAID, prevStart, prevEnd ) && STATUS > 0;

	oldsql * SelectAll().From(INVOICES).Where(oldwhere);

	while ( oldsql.Fetch() )
	{
		thenTaxable += (double) oldsql[TAXABLESUB];
		thenNontaxable += (double) oldsql[NONTAXABLESUB];
		thenParts += round((double)GetPartsCost(oldsql[INVOICENUMBER]), 2);
	}
	
	thenGross = thenTaxable + thenNontaxable;
	thenNet = thenGross - thenParts;
	
	partsChange = round(nowParts - thenParts, 2);
	partsPercent = PercentFormat(partsChange / thenParts);
	
	grossChange = round(nowGross - thenGross,2);
	grossPercent = PercentFormat(grossChange / thenGross);
	
	netChange = round(nowNet - thenNet,2);
	netPercent = PercentFormat(netChange / thenNet);
	
	sqlTaxReport.Add("Gross income:", nowGross, thenGross, grossChange, grossPercent);
	sqlTaxReport.Add("Cost of Goods:", nowParts, thenParts, partsChange, partsPercent);
	sqlTaxReport.Add("Net Profit:", nowNet, thenNet, netChange, netPercent);
}

void YearToYearWindow::CreateReport( String start, String end)
{	// generate report, customize header with date info
	if (sqlTaxReport.GetCount() < 1) return;
	String plQTF;
	String s = ::Format(Date( 1970, 1, 1) + StrInt(start));
	String e = ::Format(Date( 1970, 1, 1) + StrInt(end));
	int yearIndex = (dateYear.IsNotNull() ? StrInt(dateYear.GetData().ToString()) : 1);
	prevDateStart = Date(1970-yearIndex, 1, 1) + StrInt(~dateStart.GetData());
	prevDateEnd = Date(1970-yearIndex, 1, 1) + StrInt(~dateEnd.GetData());
	
	String header = "[+40< Profit / Loss between " << s << " to " << e << " and " << prevDateStart << " to " << prevDateEnd << " for " << myConfig.data.companyname << "]";

	// "[ [ {{2000:2000:2000:2000:2000@L [= ]:: [= ]:: [= ]:: [= ]:: [= ]}}]]"
	plQTF = "[ [+60* {{2000:2000:2000:2000:2000@L  [< Categories:]:: [= ";
	plQTF << s << " to " << e << "]:: [= " << prevDateStart << " to " << prevDateEnd << "]:: [= Change:]:: [= Percent:]}}]&";
	for (int i = 0; i < sqlTaxReport.GetCount(); i++)
	{
		plQTF << "&[ [+60 {{2000:2000:2000:2000:2000@L [< " << sqlTaxReport.Get(i, 0) << "]:: [> ";
			plQTF << prnMoney(sqlTaxReport.Get(i, 1)) << "]:: [> " << prnMoney(sqlTaxReport.Get(i, 2)) << "]:: [> " << prnMoney(sqlTaxReport.Get(i, 3)) <<
			"]:: [> " << sqlTaxReport.Get(i, 4) << "]}}]";
	}

	Report profit;
	profit.Header(header);
	profit << plQTF;
	Perform( profit );
}
