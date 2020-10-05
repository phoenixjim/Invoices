#include "Reports.h"

ProfitLossWindow::ProfitLossWindow()
{
	CtrlLayoutOKCancel(*this, "Profit / Loss Report"); 
	
	sqlTaxReport.AddColumn("Accounts", 100);
	sqlTaxReport.AddColumn("Current", 50);
	sqlTaxReport.AddColumn("Previous", 50);
	sqlTaxReport.AddColumn("Change", 50);
	sqlTaxReport.AddColumn("Percent", 50);
}

/*
class Profits
    {
        #region Globals for Profits
        static globalParams gParams = new globalParams();
        public static transactionDAL tDAL = new transactionDAL();
        public static DataTable tdata = new DataTable();
        public static DataTable prevdata = new DataTable();
        public static DateTime startdate, enddate, prevStart, prevEnd;
        public static DataTable pdata = new DataTable(); // products
        public static productsDAL pDAL = new productsDAL();

        private static Document document; // adjust the following for landscape mode table report
        static Table table;
        readonly static Color TableBorder = new Color(81, 125, 192);
        readonly static Color TableBlue = new Color(235, 240, 249);
        readonly static Color TableGray = new Color(242, 242, 242);
        #endregion
        public static void Profit(long start, long finish)
        {
            if (finish <= start)
            {
                System.Windows.MessageBox.Show("Dates aren't right!");
                return;
            }
            startdate = new DateTime(start);
            enddate = new DateTime(finish);
            prevStart = new DateTime(startdate.Year - 1, startdate.Month, startdate.Day);
            prevEnd = new DateTime(enddate.Year - 1, enddate.Month, enddate.Day);

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
        static Document CreateDocument()
        {
            // Create a new MigraDoc document.
            document = new Document();

            // document.Styles[StyleNames.Normal].Font.Name = "Lucida Sans";
            DefineStyles();
            CreatePage();
            FillContent();
            return document;
        }
        static void DefineStyles()
        {
            // Get the predefined style Normal.
            Style style = document.Styles["Normal"];
            // Because all styles are derived from Normal, the next line changes the 
            // font of the whole document. Or, more exactly, it changes the font of
            // all styles and paragraphs that do not redefine the font.
            style.Font.Name = "Verdana";

            style = document.Styles[StyleNames.Header];
            style.ParagraphFormat.AddTabStop("16cm", TabAlignment.Right);

            style = document.Styles[StyleNames.Footer];
            style.ParagraphFormat.AddTabStop("8cm", TabAlignment.Center);

            // Create a new style called Table based on style Normal
            style = document.Styles.AddStyle("Table", "Normal");
            style.Font.Name = "Verdana";
            // style.Font.Name = "Times New Roman";
            style.Font.Size = 8;

            // Create a new style called Reference based on style Normal
            style = document.Styles.AddStyle("Reference", "Normal");
            style.ParagraphFormat.SpaceBefore = "5mm";
            style.ParagraphFormat.SpaceAfter = "5mm";
            style.ParagraphFormat.TabStops.AddTabStop("16cm", TabAlignment.Right);
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