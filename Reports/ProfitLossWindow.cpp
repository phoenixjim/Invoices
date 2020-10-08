#include "Reports.h"
#include <Report/Report.h>

ProfitLossWindow::ProfitLossWindow()
{
	CtrlLayoutOKCancel(*this, "Profit / Loss Report"); 
	
	sqlTaxReport.AddColumn("Accounts", 50);
	sqlTaxReport.AddColumn("Current", 50);
	sqlTaxReport.AddColumn("Previous", 50);
	sqlTaxReport.AddColumn("Change", 50);
	sqlTaxReport.AddColumn("Percent", 50);
	
	dateStart.SetConvert(DateIntConvert());
	dateEnd.SetConvert(DateIntConvert());
	
	ok << [=] { okPressed(); };
	cancel << [=] { cancelPressed(); };
	btnReport << [=] { CreateReport ( dateStart.GetData().ToString(), dateEnd.GetData().ToString() ); };
}

void ProfitLossWindow::cancelPressed()
{
	Close();
}

void ProfitLossWindow::okPressed()
{	// generate table
	// get needed data:
	if (IsNull(dateStart) || IsNull(dateEnd)) return;
	
	prevDateStart = Date(1969, 1, 1) + StrInt(~dateStart.GetData());
	prevDateEnd = Date(1969, 1, 1) + StrInt(~dateEnd.GetData());

	prevStart = IntStr(prevDateStart.Get() - Date(1970, 1, 1).Get());
	prevEnd   = IntStr(prevDateEnd.Get() - Date(1970, 1, 1).Get());
	
	// This years info:
	Sql sql;
	SqlBool where;
	where = Between(DATEPAID, dateStart.GetData().ToString(), dateEnd.GetData().ToString());
	
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
	oldwhere = Between(DATEPAID, prevStart, prevEnd );

	oldsql * SelectAll().From(INVOICES).Where(oldwhere);

	while ( oldsql.Fetch() )
	{
		thenTaxable += (double) oldsql[TAXABLESUB];
		thenNontaxable += (double) oldsql[NONTAXABLESUB];
		thenParts += round((double)GetPartsCost(oldsql[INVOICENUMBER]), 2);
	}
	
	double thenGross = thenTaxable + thenNontaxable;
	double thenNet = thenGross - thenParts;
	
	double partsChange = -(round(nowParts - thenParts, 2));
	double partsPercent = PercentFormat(partsChange / thenParts);
	
	double grossChange = nowGross - thenGross;
	double grossPercent = PercentFormat(grossChange / thenGross);
	
	double netChange = nowNet - thenNet;
	double netPercent = PercentFormat(netChange / thenNet);
	
	sqlTaxReport.Add("Gross income:", nowGross, thenGross, grossChange, grossPercent);
	sqlTaxReport.Add("Cost of Goods:", nowParts, thenParts, partsChange, partsPercent);
	sqlTaxReport.Add("Net Profit:", nowNet, thenNet, netChange, netPercent);
}

void ProfitLossWindow::CreateReport( String start, String end)
{	// generate report, customize header with date info
	if (sqlTaxReport.GetCount() < 1) return;
	String plQTF;
	String s = ::Format(Date( 1970, 1, 1) + StrInt(start));
	String e = ::Format(Date( 1970, 1, 1) + StrInt(end));
	prevDateStart = Date(1969, 1, 1) + StrInt(~dateStart.GetData());
	prevDateEnd = Date(1969, 1, 1) + StrInt(~dateEnd.GetData());
	
	String header = "[+40< Profit / Loss between " << s << " to " << e << " and " << prevDateStart << " to " << prevDateEnd << " for " << myConfig.data.companyname << "]";

	// "[ [ {{2000:2000:2000:2000:2000@L [= ]:: [= ]:: [= ]:: [= ]:: [= ]}}]]"
	plQTF = "[ [+60* {{2000:2000:2000:2000:2000@L  [< Categories:]:: [= ";
	plQTF << s << " to " << e << "]:: [= " << prevDateStart << " to " << prevDateEnd << "]:: [= Change:]:: [= Percent:]}}]&";
	for (int i = 0; i < sqlTaxReport.GetCount(); i++)
	{
		plQTF << "&[ [+60 {{2000:2000:2000:2000:2000@L [< " << sqlTaxReport.Get(i, 0) << "]:: [> ";
			plQTF << sqlTaxReport.Get(i, 1) << "]:: [> " << sqlTaxReport.Get(i, 2) << "]:: [> " << sqlTaxReport.Get(i, 3) <<
			"]:: [> " << sqlTaxReport.Get(i, 4) << "]}}]";
	}

	Report profit;
	profit.Header(header);
	profit << plQTF;
	Perform( profit );
}

double ProfitLossWindow::GetPartsCost ( int invId )
{
	SQL * Select ( SqlSum ( COST ) ).From ( PRODUCTS ).Where ( INVOICEID == invId );
	SQL.Fetch();
	double parts = IsNull(SQL[0]) ? 0.00 : round(SQL[0], 2);
	return parts;
}


/*
class Profits
    {
            #region Initialize date by Range
            tdata = tDAL.GetAllTransactionsByDateRange(startdate.Ticks, enddate.Ticks);
            prevdata = tDAL.GetAllTransactionsByDateRange(prevStart.Ticks, prevEnd.Ticks);

            if (tdata.Rows.Count < 1 || prevdata.Rows.Count < 1)
            {
                System.Windows.Forms.MessageBox.Show("Invalid Date Range given " + tdata.Rows.Count.ToString() + " " + pdata.Rows.Count.ToString());
                return;
            }
            #endregion
            #region Create and show PDF
            document = CreateDocument();
            document.UseCmykColor = true;
            // Create a renderer for PDF that uses Unicode font encoding.
            var pdfRenderer = new PdfDocumentRenderer(true);

            // Set the MigraDoc document.
            pdfRenderer.Document = document;

            // Create the PDF document.
            pdfRenderer.RenderDocument();

            // Save the PDF document...

            var filename = Path.Combine(gParams.OutputFolder + "\\Report-" + startdate.ToString("MMddyyyy") + "-" + enddate.ToString("MMddyyyy") + "_vs._" + prevStart.ToString("MMddyyyy") + "-" + prevEnd.ToString("MMddyyyy") + ".pdf");

            pdfRenderer.Save(filename);
            // ...and start a viewer.
            Process.Start(filename);
            #endregion
        }
        static void CreatePage()
        {
            // Each MigraDoc document needs at least one section.
            Section section = document.AddSection();
            // Create footer
            Paragraph paragraph = section.Headers.Primary.AddParagraph();
            paragraph.AddText("Profit and Loss Report - " + startdate.ToString("MMddyyyy") + "-" + enddate.ToString("MMddyyyy") + "_vs._" + prevStart.ToString("MMddyyyy") + "-" + prevEnd.ToString("MMddyyyy"));
            paragraph.AddLineBreak();
            paragraph.Format.Font.Size = 9;
            paragraph.Format.Alignment = ParagraphAlignment.Center;

            section.PageSetup.PageFormat = PageFormat.Letter;
            section.PageSetup.Orientation = Orientation.Portrait;

            section.PageSetup.TopMargin = "2cm";
            section.PageSetup.BottomMargin = "1.5cm";
            section.PageSetup.LeftMargin = "1.8cm";
            section.PageSetup.RightMargin = "1.8cm";

            Paragraph footerRight1 = section.Footers.Primary.AddParagraph();
            footerRight1.AddText("Profit and Loss Report - " + startdate.ToString("MMddyyyy") + "-" + enddate.ToString("MMddyyyy") + "_vs._" + prevStart.ToString("MMddyyyy") + "-" + prevEnd.ToString("MMddyyyy") + " Page ");
            footerRight1.AddPageField();
            footerRight1.AddText(" of ");
            footerRight1.AddNumPagesField();
            footerRight1.AddLineBreak();
            footerRight1.Format.Font.Size = 8;
            footerRight1.Format.Alignment = ParagraphAlignment.Right;

            table = section.AddTable();
            table.Style = "Table";
            table.Borders.Color = TableBorder;
            table.Borders.Width = 0.25;
            table.Borders.Left.Width = 0.5;
            table.Borders.Right.Width = 0.5;
            table.Rows.LeftIndent = 0;
            #region size columns Accounts, current date range, previous date range, Change, %
            // Columns: 
            Column column = table.AddColumn("6.5cm"); // Invoice Number
            column.Format.Alignment = ParagraphAlignment.Center;

            column = table.AddColumn("3cm"); // Date Paid
            column.Format.Alignment = ParagraphAlignment.Center;

            column = table.AddColumn("3cm"); // Customer name
            column.Format.Alignment = ParagraphAlignment.Center;

            column = table.AddColumn("3cm"); // taxable subtotal
            column.Format.Alignment = ParagraphAlignment.Center;

            column = table.AddColumn("2.5cm"); // nontaxable subtotal
            column.Format.Alignment = ParagraphAlignment.Center;

            #endregion

            #region Create the header of the table
            string current, previous;
            current = startdate.ToString("MM-dd-yyyy") + " to\n" + enddate.ToString("MM-dd-yyyy");
            previous = prevStart.ToString("MM-dd-yyyy") + " to\n" + prevEnd.ToString("MM-dd-yyyy");

            Row row = table.AddRow();
            row.HeadingFormat = true;
            row.Format.Alignment = ParagraphAlignment.Center;
            row.Format.Font.Bold = true;
            row.Shading.Color = TableBlue;
            row.Cells[0].AddParagraph("Accounts");
            // row.Cells[0].Format.Font.Bold = false;
            row.Cells[0].Format.Alignment = ParagraphAlignment.Left;
            row.Cells[0].VerticalAlignment = VerticalAlignment.Bottom;
            // row.Cells[0].MergeDown = 1;
            row.Cells[1].AddParagraph(current);
            row.Cells[1].Format.Alignment = ParagraphAlignment.Left;
            row.Cells[2].AddParagraph(previous);
            row.Cells[2].Format.Alignment = ParagraphAlignment.Left;
            row.Cells[3].AddParagraph("Change");
            row.Cells[3].Format.Alignment = ParagraphAlignment.Right;
            row.Cells[4].AddParagraph("%");
            row.Cells[4].Format.Alignment = ParagraphAlignment.Center;
            #endregion
            table.SetEdge(0, 0, 5, 1, Edge.Box, BorderStyle.Single, 0.75, TableBorder); // Color.Empty);

        }
        static void FillContent()
        {
            double oldGross = 0;
            double newGross = 0;
            double oldNet = 0;
            double newNet = 0;
            double netChange = 0;
            double netPercent = 0;
            double grossChange = 0;
            double grossPercent = 0;

            double tax = 0;
            double amtPaid = 0;
            double partsCost = 0;
            double oldPartsCost = 0;
            foreach (DataRow dr in tdata.Rows)
            {
                amtPaid += double.Parse(dr["amtPaid"].ToString());
                partsCost += (double.Parse(dr["amtPaid"].ToString()) == 0) ? 0 : getPartsTotal(int.Parse(dr["invoice_number"].ToString()));
                tax += (double.Parse(dr["amtPaid"].ToString()) == 0) ? 0 :double.Parse(dr["tax"].ToString());
            }
            newGross = amtPaid - tax;
            newNet = newGross - partsCost;
            tax = 0;
            amtPaid = 0;
            foreach (DataRow dr2 in prevdata.Rows)
            {
                amtPaid += double.Parse(dr2["amtPaid"].ToString());
                oldPartsCost += (double.Parse(dr2["amtPaid"].ToString()) == 0) ? 0 : getPartsTotal(int.Parse(dr2["invoice_number"].ToString()));
                tax += (double.Parse(dr2["amtPaid"].ToString()) == 0) ? 0 : double.Parse(dr2["tax"].ToString());
            }
            oldGross = amtPaid - tax;
            oldNet = oldGross - oldPartsCost;
            grossChange = newGross - oldGross;
            grossPercent = grossChange / oldGross;
            netChange = newNet - oldNet;
            netPercent = netChange / newNet;

            Row row = table.AddRow();
            row.Borders.Visible = false;

            Paragraph paragraph;
            row = table.AddRow();
            row.Shading.Color = TableGray;
            row.Format.Font.Bold = false;
            paragraph = row.Cells[0].AddParagraph(); // Totals:
            paragraph.AddText("Total Income");
            paragraph = row.Cells[1].AddParagraph(); // taxable
            paragraph.AddText(newGross.ToString("$ 0.00"));
            paragraph = row.Cells[2].AddParagraph(); // taxable
            paragraph.AddText(oldGross.ToString("$ 0.00"));
            paragraph = row.Cells[3].AddParagraph(); // taxable
            paragraph.AddText(grossChange.ToString("$ 0.00"));
            paragraph = row.Cells[4].AddParagraph(); // taxable
            paragraph.AddText(grossPercent.ToString("0.00 %"));

            row = table.AddRow();
            row.Shading.Color = TableBlue;
            row.Format.Font.Bold = false;
            paragraph = row.Cells[0].AddParagraph(); // Totals:
            paragraph.AddText("Total Cost of goods sold");
            paragraph = row.Cells[1].AddParagraph(); // taxable
            paragraph.AddText(partsCost.ToString("$ 0.00"));
            paragraph = row.Cells[2].AddParagraph(); // taxable
            paragraph.AddText(oldPartsCost.ToString("$ 0.00"));
            paragraph = row.Cells[3].AddParagraph(); // taxable
            paragraph.AddText((partsCost-oldPartsCost).ToString("$ 0.00"));
            paragraph = row.Cells[4].AddParagraph(); // taxable
            double partsChange = oldPartsCost == 0 ? 0 : ((partsCost - oldPartsCost) / oldPartsCost);
            paragraph.AddText(partsChange.ToString("0.00 %"));

            row = table.AddRow();
            row.Shading.Color = TableBlue;
            row.Format.Font.Size = 12;
            row.Format.Font.Bold = true;
            paragraph = row.Cells[0].AddParagraph(); // Totals:
            paragraph.AddText("Net Profit");
            paragraph = row.Cells[1].AddParagraph(); // taxable
            paragraph.AddText(newNet.ToString("$ 0.00"));
            paragraph = row.Cells[2].AddParagraph(); // taxable
            paragraph.AddText(oldNet.ToString("$ 0.00"));
            paragraph = row.Cells[3].AddParagraph(); // taxable
            paragraph.AddText(netChange.ToString("$ 0.00"));
            paragraph = row.Cells[4].AddParagraph(); // taxable
            paragraph.AddText(netPercent.ToString("0.00 %"));
        }
        static double getPartsTotal(int Invoice)
        {
            double total = 0;
            pdata = pDAL.GetProductsFromInvoice(Invoice);
            if (pdata.Rows.Count >= 1)
                foreach (DataRow dr in pdata.Rows)
                {
                    total += double.Parse(dr["cost"].ToString());
                }
            return total;
        }
    }
    */