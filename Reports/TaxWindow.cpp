#include "Reports.h"
#include <Report/Report.h>

TaxWindow::TaxWindow()
{
	CtrlLayoutOKCancel(*this, "Select date range for report:");
	
	sqlTaxReport.AddColumn(INVOICENUMBER, "Inv NO.", 100);
	sqlTaxReport.AddColumn(DATEPAID, "Date Paid", 200).SetConvert(DateIntConvert());
	sqlTaxReport.AddColumn(CUSTOMERID, "Cust. No.", 65); // OR CUSTNAME, IF NOT ANON
	sqlTaxReport.AddColumn(CUSTNAME, "Customer Name", 140).HeaderTab().Show(false);
	sqlTaxReport.AddColumn(TAXABLESUB, "Taxable", 150);
	sqlTaxReport.AddColumn(NONTAXABLESUB, "Non-Taxable", 150);
	sqlTaxReport.AddColumn(TAX, "Sales Tax", 150);
	sqlTaxReport.AddColumn(GRANDTOTAL, "Total", 150);
	sqlTaxReport.AddColumn(COST, "My Parts Cost", 150); // PARTS COST FOR THIS INVOICE
	dateStart.SetConvert(DateIntConvert());
	dateEnd.SetConvert(DateIntConvert());
	// Anonymous by default, hide names
	anon.Set(1);
	
	anon.WhenAction << THISBACK(anonChanged);
	ok << [=] { okPressed(); };
	cancel << [=] { cancelPressed(); };
}

double TaxWindow::GetPartsCost(int invId)
{
	SQL * Select(SqlSum(COST)).From(PRODUCTS).Where(INVOICEID == invId);
	SQL.Fetch();
	return SQL[0];
}

String TaxWindow::GetCustomerName(int custId)
{
	SQL * Select(CUSTNAME).From(CUSTOMERS).Where(CUST_ID == custId);
	SQL.Fetch();
	return SQL[0];
}

void TaxWindow::okPressed()
{
	if (IsNull(dateStart) || IsNull(dateEnd)) {
		return;
	}
	sqlTaxReport.Clear();
	Sql sql;
	String commandstring = "Select * from INVOICES Where DATEPAID Between " + dateStart.GetData().ToString() + " AND " + dateEnd.GetData().ToString();
	
	sql.Execute(commandstring);
	while (sql.Fetch())
	{
		sqlTaxReport.Add(sql[INVOICENUMBER],
			sql[DATEPAID],
			sql[CUSTOMERID],
			GetCustomerName(sql[CUSTOMERID]),
			sql[TAXABLESUB],
			sql[NONTAXABLESUB],
			sql[TAX],
			sql[GRANDTOTAL],
			GetPartsCost(sql[INVOICENUMBER]));
	}
	QtfReport(sqlTaxReport.AsQtf()); // + "&&" + sqlTaxReport.AsQtf(false, false)); for adding second table etc
}

void TaxWindow::cancelPressed()
{ // bye
	Close();
}

void TaxWindow::anonChanged()
{
	if (anon.Get() == 1) {
		// use cust Number
		 sqlTaxReport.HeaderTab(CUSTOMERID).Show(true); // .ColumnWidths("150 200 65 1 150 150 150 200 200");
		 sqlTaxReport.HeaderTab(CUSTNAME).Show(false);
		return;
	}
	else  {// use cust name
		 sqlTaxReport.HeaderTab(CUSTOMERID).Show(false); // .ColumnWidths("150 200 65 1 150 150 150 200 200");
		 sqlTaxReport.HeaderTab(CUSTNAME).Show(true);
		return;
	}
		
}

/*
public DataTable GetAllTransactionsByDateRange(long start, long end)
        {
            //Create SQL Connection
            SQLiteConnection conn = new SQLiteConnection(gParams.myConnection);

            //Create a DataTable
            DataTable dt = new DataTable();

            try
            {
                //Write SQL Query
                string sql = "SELECT * FROM tbl_transactions WHERE datePaid BETWEEN @start AND @end";
                
                //SQL Command to Execute Query
                SQLiteCommand cmd = new SQLiteCommand(sql, conn);
                //SQLiteDataAdapter to hold the data from database
                SQLiteDataAdapter adapter = new SQLiteDataAdapter(cmd);
                adapter.SelectCommand.Parameters.AddWithValue("@start", start.ToString());
                adapter.SelectCommand.Parameters.AddWithValue("@end", end.ToString());

                //Open DAtabase Connection
                conn.Open();
                adapter.Fill(dt);
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
            }
            finally
            {
                conn.Close();
            }

            return dt;
        }
        public DataTable GetAllTransactionsByDateRangebyCustomer(long start, long end, string customer)
        {
            //Create SQL Connection
            SQLiteConnection conn = new SQLiteConnection(gParams.myConnection);

            //Create a DataTable
            DataTable dt = new DataTable();

            try
            {
                //Write SQL Query
                string sql = "SELECT * FROM tbl_transactions WHERE customer_id = @customer AND datePaid BETWEEN @start AND @end";

                //SQL Command to Execute Query
                SQLiteCommand cmd = new SQLiteCommand(sql, conn);
                //SQLiteDataAdapter to hold the data from database
                SQLiteDataAdapter adapter = new SQLiteDataAdapter(cmd);
                adapter.SelectCommand.Parameters.AddWithValue("@customer", customer);
                adapter.SelectCommand.Parameters.AddWithValue("@start", start);
                adapter.SelectCommand.Parameters.AddWithValue("@end", end);

                //Open DAtabase Connection
                conn.Open();
                adapter.Fill(dt);
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
            }
            finally
            {
                conn.Close();
            }

            return dt;
        }
class TaxReports
    {
        #region Global variables
        static globalParams gParams = new globalParams();

        public static transactionDAL tDAL = new transactionDAL();
        public static DataTable tdata = new DataTable(); // invoices

        public static DataTable pdata = new DataTable(); // products
        public static productsDAL pDAL = new productsDAL();

        public static CustomersDAL cDAL = new CustomersDAL();
        public static DataTable cdata = new DataTable(); // customer list

        static DateTime startdate, enddate;

        private static Document document; // adjust the following for landscape mode table report
        static Table table;
        readonly static Color TableBorder = new Color(81, 125, 192);
        readonly static Color TableBlue = new Color(235, 240, 249);
        readonly static Color TableGray = new Color(242, 242, 242);
        #endregion
        #region Helper functions to verify arguments are valid
        public static bool isLong(string s)
        {
            return long.TryParse(s, out long n);
        }
        public interface IRange<T>
        {
            T Start { get; }
            T End { get; }
            bool Includes(T value);
            bool Includes(IRange<T> range);
        }
        public class DateRange : IRange<DateTime>
        {
            public DateRange(DateTime start, DateTime end)
            {
                Start = start;
                End = end;
            }

            public DateTime Start { get; private set; }
            public DateTime End { get; private set; }

            public bool Includes(DateTime value)
            {
                return (Start <= value) && (value <= End);
            }

            public bool Includes(IRange<DateTime> range)
            {
                return (Start <= range.Start) && (range.End <= End);
            }
        }
        #endregion
        public static void TaxReport(long start, long finish, string customer="0")
        {
            #region Verify Dates
            DateTime begin = new DateTime(2010, 1, 1, 0, 0, 0, 0);
            DateTime end = new DateTime(2040, 12, 31, 23, 59, 59, 999);
            DateRange ok = new DateRange(begin, end);

            DateTime first = new DateTime(start);
            DateTime last = new DateTime(finish);
            DateRange check = new DateRange(first, last);

            if (!ok.Includes(check))
            {
                System.Windows.Forms.MessageBox.Show("Date out of range:\n\t" + Process.GetCurrentProcess().ProcessName + " [long startdate] [long enddate]\nDates must fall between 2010 and 2040.");
                return;
            }

            startdate = first;
            enddate = last;

            #endregion
            #region Initialize date by Range
            // tdata = tDAL.testGetAllTransactionsByDateRange(startdate.Ticks, enddate.Ticks); // Until data is fixed, use this
            if (customer == "0") tdata = tDAL.GetAllTransactionsByDateRange(startdate.Ticks, enddate.Ticks);
            else tdata = tDAL.GetAllTransactionsByDateRangebyCustomer(startdate.Ticks, enddate.Ticks, customer);

            if (tdata.Rows.Count < 1)
            {
                System.Windows.Forms.MessageBox.Show("Invalid Date Range given");
                return;
            }
            // cdata = cDAL.GetAllCustomers();
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

            var filename = "\\Report-" + startdate.ToString("MMddyyyy") + "-" + enddate.ToString("MMddyyyy") + ".pdf";

            pdfRenderer.Save(Path.Combine(gParams.OutputFolder + filename));
            // ...and start a viewer.
            try
            {
                Process.Start(Path.Combine(gParams.OutputFolder + filename));
            }
            catch (Win32Exception ex)
            {
                System.Windows.Forms.MessageBox.Show(ex.Message);
            }
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
            paragraph.AddText("Report for dates from " + startdate.ToString("MM-dd-yyyy") + " to " + enddate.ToString("MM-dd-yyyy"));
            paragraph.AddLineBreak();
            paragraph.Format.Font.Size = 9;
            paragraph.Format.Alignment = ParagraphAlignment.Center;

            section.PageSetup.PageFormat = PageFormat.Letter;
            section.PageSetup.Orientation = Orientation.Landscape;

            section.PageSetup.TopMargin = "2cm";
            section.PageSetup.BottomMargin = "1.5cm";
            section.PageSetup.LeftMargin = "1.8cm";
            section.PageSetup.RightMargin = "1.8cm";

            Paragraph footerRight1 = section.Footers.Primary.AddParagraph();
            footerRight1.AddText("Report for dates from " + startdate.ToString("MM-dd-yyyy") + " to " + enddate.ToString("MM-dd-yyyy") + "Page ");
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
            #region size columns Inv#, Date (Paid), Customer name, taxablesub, nontaxablesub, sales tax, total paid, parts cost, status
            // Columns: 
            Column column = table.AddColumn("2.2cm"); // Invoice Number
            column.Format.Alignment = ParagraphAlignment.Center;

            column = table.AddColumn("2.5cm"); // Date Paid
            column.Format.Alignment = ParagraphAlignment.Center;

            column = table.AddColumn("4.5cm"); // Customer name
            column.Format.Alignment = ParagraphAlignment.Center;

            column = table.AddColumn("2.25cm"); // taxable subtotal
            column.Format.Alignment = ParagraphAlignment.Center;

            column = table.AddColumn("3cm"); // nontaxable subtotal
            column.Format.Alignment = ParagraphAlignment.Center;

            column = table.AddColumn("2.5cm"); // sales tax charged
            column.Format.Alignment = ParagraphAlignment.Center;

            column = table.AddColumn("2.5cm"); // total paid
            column.Format.Alignment = ParagraphAlignment.Center;

            column = table.AddColumn("2.5cm"); // parts cost
            column.Format.Alignment = ParagraphAlignment.Center;

            column = table.AddColumn("2.5cm"); // invoice status (void, partially or fully paid)
            column.Format.Alignment = ParagraphAlignment.Center;
            #endregion

            #region Create the header of the table
            Row row = table.AddRow();
            row.HeadingFormat = true;
            row.Format.Alignment = ParagraphAlignment.Center;
            row.Format.Font.Bold = true;
            row.Shading.Color = TableBlue;
            row.Cells[0].AddParagraph("Invoice No.");
            // row.Cells[0].Format.Font.Bold = false;
            row.Cells[0].Format.Alignment = ParagraphAlignment.Left;
            row.Cells[0].VerticalAlignment = VerticalAlignment.Bottom;
            // row.Cells[0].MergeDown = 1;
            row.Cells[1].AddParagraph("Date Paid");
            row.Cells[1].Format.Alignment = ParagraphAlignment.Left;
            row.Cells[2].AddParagraph("Customer Name");
            row.Cells[2].Format.Alignment = ParagraphAlignment.Left;
            row.Cells[3].AddParagraph("Taxable");
            row.Cells[3].Format.Alignment = ParagraphAlignment.Right;
            row.Cells[4].AddParagraph("Non Taxable");
            row.Cells[4].Format.Alignment = ParagraphAlignment.Right;
            row.Cells[5].AddParagraph("Sales Tax");
            row.Cells[5].Format.Alignment = ParagraphAlignment.Right;
            row.Cells[6].AddParagraph("Total Paid");
            row.Cells[6].Format.Alignment = ParagraphAlignment.Right;
            row.Cells[7].AddParagraph("Parts Cost");
            row.Cells[7].Format.Alignment = ParagraphAlignment.Right;
            row.Cells[8].AddParagraph("Status");
            row.Cells[8].Format.Alignment = ParagraphAlignment.Left;
            #endregion
            table.SetEdge(0, 0, 9, 1, Edge.Box, BorderStyle.Single, 0.75, TableBorder); // Color.Empty);

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
        static void FillContent()
        {
            double taxrate = (double)gParams.taxrate;
            double nonTaxableSub = 0;
            double TaxableSub = 0;
            double collectedTax = 0;
            double partsCost = 0;
            double totalReceived = 0;
            Paragraph paragraph;
            foreach (DataRow dr in tdata.Rows)
            {
                #region Row Setup
                Row row1 = table.AddRow();
                row1.Cells[0].Shading.Color = TableGray;
                row1.Cells[0].VerticalAlignment = VerticalAlignment.Center;
                row1.Cells[2].Format.Alignment = ParagraphAlignment.Left;
                row1.Cells[3].Format.Alignment = ParagraphAlignment.Right;
                row1.Cells[4].Format.Alignment = ParagraphAlignment.Right;
                row1.Cells[5].Format.Alignment = ParagraphAlignment.Right;
                row1.Cells[6].Format.Alignment = ParagraphAlignment.Right;
                row1.Cells[7].Format.Alignment = ParagraphAlignment.Right;
                #endregion
                row1.Cells[0].AddParagraph(dr["invoice_number"].ToString());
                paragraph = row1.Cells[1].AddParagraph(); // date paid
                DateTime td = new DateTime(long.Parse(dr["datePaid"].ToString()));
                paragraph.AddFormattedText(td.ToString("MM-dd-yyyy"));
                cdata = cDAL.Search(int.Parse(dr["customer_id"].ToString()));
                // Customer name then taxable, nontaxable, tax and totalpaid, parts cost and status
                paragraph = row1.Cells[2].AddParagraph();
                paragraph.AddText(cdata.Rows[0]["name"].ToString());
                paragraph = row1.Cells[3].AddParagraph();
                paragraph.AddText(double.Parse(dr["taxableSub"].ToString()).ToString("$ 0.00"));

                paragraph = row1.Cells[4].AddParagraph();
                paragraph.AddText(double.Parse(dr["nonTaxableSub"].ToString()).ToString("$ 0.00"));
                paragraph = row1.Cells[5].AddParagraph();
                paragraph.AddText(double.Parse(dr["tax"].ToString()).ToString("$ 0.00"));
                paragraph = row1.Cells[6].AddParagraph();
                paragraph.AddText(double.Parse(dr["amtPaid"].ToString()).ToString("$ 0.00"));
                // need all parts for this invoice if any for number 7, then total cost of them... Function!!!
                paragraph = row1.Cells[7].AddParagraph();
                double totalparts = getPartsTotal(int.Parse(dr["invoice_number"].ToString()));
                paragraph.AddText(totalparts.ToString("$ 0.00"));
                paragraph = row1.Cells[8].AddParagraph();
                switch (dr["status"].ToString())
                {
                    case "0": // unpaid
                        paragraph.AddText("Unpaid");
                        break;
                    case "1": // void
                        paragraph.AddText("Void");
                        break;
                    case "2": // void
                        paragraph.AddText("Partially Paid");
                        break;
                    case "3": // void
                        paragraph.AddText("Paid in Full");
                        break;
                }
                if (int.Parse(dr["status"].ToString()) > 1)
                {
                    // calculating totals
                    TaxableSub += (double.Parse(dr["amtPaid"].ToString()) == 0) ? 0 :double.Parse(dr["taxableSub"].ToString());
                    nonTaxableSub += (double.Parse(dr["amtPaid"].ToString()) == 0) ? 0 : double.Parse(dr["nonTaxableSub"].ToString());
                    collectedTax += (double.Parse(dr["amtPaid"].ToString()) == 0) ? 0 : double.Parse(dr["tax"].ToString());
                    partsCost += totalparts;
                    totalReceived += double.Parse(dr["amtPaid"].ToString());
                }
            }

            // Add an invisible row as a space line to the table
            Row row = table.AddRow();
            row.Borders.Visible = false;

            // One more row for totals
            row = table.AddRow();
            row.Shading.Color = TableBlue;
            row.Format.Font.Bold = true;
            paragraph = row.Cells[2].AddParagraph(); // Totals:
            paragraph.AddText("Totals:");
            paragraph = row.Cells[3].AddParagraph(); // taxable
            paragraph.AddText(TaxableSub.ToString("$ 0.00"));

            paragraph = row.Cells[4].AddParagraph(); // non taxable
            paragraph.AddText(nonTaxableSub.ToString("$ 0.00"));
            paragraph = row.Cells[5].AddParagraph(); // sales tax
            paragraph.AddText(collectedTax.ToString("$ 0.00"));
            paragraph = row.Cells[6].AddParagraph(); // amt received
            paragraph.AddText(totalReceived.ToString("$ 0.00"));
            // need all parts for this invoice if any for number 7, then total cost of them... Function!!!
            paragraph = row.Cells[7].AddParagraph(); // total parts cost of doing business deduction
            paragraph.AddText(partsCost.ToString("$ 0.00"));

            row = table.AddRow();
            row.Borders.Visible = false;

            row = table.AddRow();
            row.Cells[0].Borders.Visible = false;
            row.Cells[4].Borders.Visible = false;
            row.Cells[5].Borders.Visible = false;
            row.Cells[6].Borders.Visible = false;
            row.Cells[7].Borders.Visible = false;
            row.Cells[8].Borders.Visible = false;

            row.Cells[1].MergeRight = 1;
            row.Cells[1].AddParagraph("Total Taxable Income:");
            row.Cells[1].Format.Font.Bold = true;
            row.Cells[1].Format.Alignment = ParagraphAlignment.Right;
            row.Cells[3].AddParagraph("$ " + (totalReceived - partsCost - collectedTax).ToString("0.00"));
        }

        public static void AnonTaxReport(long start, long finish, string customer = "0")
        {
            #region Verify Dates
            DateTime begin = new DateTime(2010, 1, 1, 0, 0, 0, 0);
            DateTime end = new DateTime(2040, 12, 31, 23, 59, 59, 999);
            DateRange ok = new DateRange(begin, end);

            DateTime first = new DateTime(start);
            DateTime last = new DateTime(finish);
            DateRange check = new DateRange(first, last);

            if (!ok.Includes(check))
            {
                System.Windows.Forms.MessageBox.Show("Date out of range:\n\t" + Process.GetCurrentProcess().ProcessName + " [long startdate] [long enddate]\nDates must fall between 2010 and 2040.");
                return;
            }

            startdate = first;
            enddate = last;

            #endregion
            #region Initialize date by Range
            // tdata = tDAL.testGetAllTransactionsByDateRange(startdate.Ticks, enddate.Ticks); // Until data is fixed, use this
            if (customer == "0") tdata = tDAL.GetAllTransactionsByDateRange(startdate.Ticks, enddate.Ticks);
            else tdata = tDAL.GetAllTransactionsByDateRangebyCustomer(startdate.Ticks, enddate.Ticks, customer);

            if (tdata.Rows.Count < 1)
            {
                System.Windows.Forms.MessageBox.Show("Invalid Date Range given");
                return;
            }
            // cdata = cDAL.GetAllCustomers();
            #endregion
            #region Create and show PDF
            document = AnonCreateDocument();
            document.UseCmykColor = true;
            // Create a renderer for PDF that uses Unicode font encoding.
            var pdfRenderer = new PdfDocumentRenderer(true);

            // Set the MigraDoc document.
            pdfRenderer.Document = document;

            // Create the PDF document.
            pdfRenderer.RenderDocument();

            // Save the PDF document...

            var filename = "\\Report-" + startdate.ToString("MMddyyyy") + "-" + enddate.ToString("MMddyyyy") + ".pdf";

            pdfRenderer.Save(gParams.OutputFolder + filename);
            // ...and start a viewer.
            try
            {
                Process.Start(gParams.OutputFolder + filename);
            }
            catch (Win32Exception ex)
            {
                System.Windows.Forms.MessageBox.Show(ex.Message);
            }
            #endregion
        }
        static Document AnonCreateDocument()
        {
            // Create a new MigraDoc document.
            document = new Document();

            // document.Styles[StyleNames.Normal].Font.Name = "Lucida Sans";
            DefineStyles();
            AnonCreatePage();
            AnonFillContent();
            return document;
        }
        static void AnonCreatePage()
        {
            // Each MigraDoc document needs at least one section.
            Section section = document.AddSection();
            // Create footer
            Paragraph paragraph = section.Headers.Primary.AddParagraph();
            paragraph.AddText("Report for dates from " + startdate.ToString("MM-dd-yyyy") + " to " + enddate.ToString("MM-dd-yyyy"));
            paragraph.AddLineBreak();
            paragraph.Format.Font.Size = 9;
            paragraph.Format.Alignment = ParagraphAlignment.Center;

            section.PageSetup.PageFormat = PageFormat.Letter;
            section.PageSetup.Orientation = Orientation.Landscape;

            section.PageSetup.TopMargin = "2cm";
            section.PageSetup.BottomMargin = "1.5cm";
            section.PageSetup.LeftMargin = "1.8cm";
            section.PageSetup.RightMargin = "1.8cm";

            Paragraph footerRight1 = section.Footers.Primary.AddParagraph();
            footerRight1.AddText("Report for dates from " + startdate.ToString("MM-dd-yyyy") + " to " + enddate.ToString("MM-dd-yyyy") + "Page ");
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
            #region size columns Inv#, Date (Paid), Customer name, taxablesub, nontaxablesub, sales tax, total paid, parts cost, status
            // Columns: 
            Column column = table.AddColumn("2.2cm"); // Invoice Number
            column.Format.Alignment = ParagraphAlignment.Center;

            column = table.AddColumn("2.5cm"); // Date Paid
            column.Format.Alignment = ParagraphAlignment.Center;

            column = table.AddColumn("2.5cm"); // Customer Number
            column.Format.Alignment = ParagraphAlignment.Center;

            column = table.AddColumn("2.25cm"); // taxable subtotal
            column.Format.Alignment = ParagraphAlignment.Center;

            column = table.AddColumn("3cm"); // nontaxable subtotal
            column.Format.Alignment = ParagraphAlignment.Center;

            column = table.AddColumn("2.5cm"); // sales tax charged
            column.Format.Alignment = ParagraphAlignment.Center;

            column = table.AddColumn("2.5cm"); // total paid
            column.Format.Alignment = ParagraphAlignment.Center;

            column = table.AddColumn("2.5cm"); // parts cost
            column.Format.Alignment = ParagraphAlignment.Center;

            column = table.AddColumn("2.5cm"); // invoice status (void, partially or fully paid)
            column.Format.Alignment = ParagraphAlignment.Center;
            #endregion

            #region Create the header of the table
            Row row = table.AddRow();
            row.HeadingFormat = true;
            row.Format.Alignment = ParagraphAlignment.Center;
            row.Format.Font.Bold = true;
            row.Shading.Color = TableBlue;
            row.Cells[0].AddParagraph("Invoice No.");
            // row.Cells[0].Format.Font.Bold = false;
            row.Cells[0].Format.Alignment = ParagraphAlignment.Left;
            row.Cells[0].VerticalAlignment = VerticalAlignment.Bottom;
            // row.Cells[0].MergeDown = 1;
            row.Cells[1].AddParagraph("Date Paid");
            row.Cells[1].Format.Alignment = ParagraphAlignment.Left;
            row.Cells[2].AddParagraph("Customer #");
            row.Cells[2].Format.Alignment = ParagraphAlignment.Left;
            row.Cells[3].AddParagraph("Taxable");
            row.Cells[3].Format.Alignment = ParagraphAlignment.Right;
            row.Cells[4].AddParagraph("Non Taxable");
            row.Cells[4].Format.Alignment = ParagraphAlignment.Right;
            row.Cells[5].AddParagraph("Sales Tax");
            row.Cells[5].Format.Alignment = ParagraphAlignment.Right;
            row.Cells[6].AddParagraph("Total Paid");
            row.Cells[6].Format.Alignment = ParagraphAlignment.Right;
            row.Cells[7].AddParagraph("Parts Cost");
            row.Cells[7].Format.Alignment = ParagraphAlignment.Right;
            row.Cells[8].AddParagraph("Status");
            row.Cells[8].Format.Alignment = ParagraphAlignment.Left;
            #endregion
            table.SetEdge(0, 0, 9, 1, Edge.Box, BorderStyle.Single, 0.75, TableBorder); // Color.Empty);

        }

        static void AnonFillContent()
        {
            double taxrate = (double)gParams.taxrate;
            double nonTaxableSub = 0;
            double TaxableSub = 0;
            double collectedTax = 0;
            double partsCost = 0;
            double totalReceived = 0;
            Paragraph paragraph;
            foreach (DataRow dr in tdata.Rows)
            {
                #region Row Setup
                Row row1 = table.AddRow();
                row1.Cells[0].Shading.Color = TableGray;
                row1.Cells[0].VerticalAlignment = VerticalAlignment.Center;
                row1.Cells[2].Format.Alignment = ParagraphAlignment.Left;
                row1.Cells[3].Format.Alignment = ParagraphAlignment.Right;
                row1.Cells[4].Format.Alignment = ParagraphAlignment.Right;
                row1.Cells[5].Format.Alignment = ParagraphAlignment.Right;
                row1.Cells[6].Format.Alignment = ParagraphAlignment.Right;
                row1.Cells[7].Format.Alignment = ParagraphAlignment.Right;
                #endregion
                row1.Cells[0].AddParagraph(dr["invoice_number"].ToString());
                paragraph = row1.Cells[1].AddParagraph(); // date paid
                DateTime td = new DateTime(long.Parse(dr["datePaid"].ToString()));
                paragraph.AddFormattedText(td.ToString("MM-dd-yyyy"));
                // cdata = cDAL.Search(int.Parse(dr["customer_id"].ToString()));
                // Customer name then taxable, nontaxable, tax and totalpaid, parts cost and status
                paragraph = row1.Cells[2].AddParagraph();
                paragraph.AddText(dr["customer_id"].ToString());

                paragraph = row1.Cells[3].AddParagraph();
                paragraph.AddText(double.Parse(dr["taxableSub"].ToString()).ToString("$ 0.00"));

                paragraph = row1.Cells[4].AddParagraph();
                paragraph.AddText(double.Parse(dr["nonTaxableSub"].ToString()).ToString("$ 0.00"));
                paragraph = row1.Cells[5].AddParagraph();
                paragraph.AddText(double.Parse(dr["tax"].ToString()).ToString("$ 0.00"));
                paragraph = row1.Cells[6].AddParagraph();
                paragraph.AddText(double.Parse(dr["amtPaid"].ToString()).ToString("$ 0.00"));
                // need all parts for this invoice if any for number 7, then total cost of them... Function!!!
                paragraph = row1.Cells[7].AddParagraph();
                double totalparts = getPartsTotal(int.Parse(dr["invoice_number"].ToString()));
                paragraph.AddText(totalparts.ToString("$ 0.00"));
                paragraph = row1.Cells[8].AddParagraph();
                switch (dr["status"].ToString())
                {
                    case "0": // unpaid
                        paragraph.AddText("Unpaid");
                        break;
                    case "1": // void
                        paragraph.AddText("Void");
                        break;
                    case "2": // void
                        paragraph.AddText("Partially Paid");
                        break;
                    case "3": // void
                        paragraph.AddText("Paid in Full");
                        break;
                }
                if (int.Parse(dr["status"].ToString()) > 1)
                {
                    // calculating totals
                    TaxableSub += (double.Parse(dr["amtPaid"].ToString()) == 0) ? 0 : double.Parse(dr["taxableSub"].ToString());
                    nonTaxableSub += (double.Parse(dr["amtPaid"].ToString()) == 0) ? 0 : double.Parse(dr["nonTaxableSub"].ToString());
                    collectedTax += (double.Parse(dr["amtPaid"].ToString()) == 0) ? 0 : double.Parse(dr["tax"].ToString());
                    partsCost += totalparts;
                    totalReceived += double.Parse(dr["amtPaid"].ToString());
                }
            }

            // Add an invisible row as a space line to the table
            Row row = table.AddRow();
            row.Borders.Visible = false;

            // One more row for totals
            row = table.AddRow();
            row.Shading.Color = TableBlue;
            row.Format.Font.Bold = true;
            paragraph = row.Cells[2].AddParagraph(); // Totals:
            paragraph.AddText("Totals:");
            paragraph = row.Cells[3].AddParagraph(); // taxable
            paragraph.AddText(TaxableSub.ToString("$ 0.00"));

            paragraph = row.Cells[4].AddParagraph(); // non taxable
            paragraph.AddText(nonTaxableSub.ToString("$ 0.00"));
            paragraph = row.Cells[5].AddParagraph(); // sales tax
            paragraph.AddText(collectedTax.ToString("$ 0.00"));
            paragraph = row.Cells[6].AddParagraph(); // amt received
            paragraph.AddText(totalReceived.ToString("$ 0.00"));
            // need all parts for this invoice if any for number 7, then total cost of them... Function!!!
            paragraph = row.Cells[7].AddParagraph(); // total parts cost of doing business deduction
            paragraph.AddText(partsCost.ToString("$ 0.00"));

            row = table.AddRow();
            row.Borders.Visible = false;

            row = table.AddRow();
            row.Cells[0].Borders.Visible = false;
            row.Cells[4].Borders.Visible = false;
            row.Cells[5].Borders.Visible = false;
            row.Cells[6].Borders.Visible = false;
            row.Cells[7].Borders.Visible = false;
            row.Cells[8].Borders.Visible = false;

            row.Cells[1].MergeRight = 1;
            row.Cells[1].AddParagraph("Total Taxable Income:");
            row.Cells[1].Format.Font.Bold = true;
            row.Cells[1].Format.Alignment = ParagraphAlignment.Right;
            row.Cells[3].AddParagraph("$ " + (totalReceived - partsCost - collectedTax).ToString("0.00"));
        }
    }
*/