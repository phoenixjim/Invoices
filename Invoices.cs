using System;
using JTSInvoices.Tables;
using MigraDoc.DocumentObjectModel;
using MigraDoc.Rendering;
using MigraDoc.DocumentObjectModel.Shapes;
using MigraDoc.DocumentObjectModel.Tables;
using System.Diagnostics;
using System.Data;
using System.IO;
// using System.Windows;
using System.ComponentModel;
// using System.Windows.Forms;

namespace JTSInvoices.DBAccess
{
    class Invoices
    {
        static globalParams gParams = new globalParams();
        public static transactionDAL tDAL = new transactionDAL();
        public static transactionsTable tBLL = new transactionsTable();
        public static DataTable tidata = new DataTable();
        public static transactionDetailDAL tiDAL = new transactionDetailDAL();
        public static CustomersDAL cDAL = new CustomersDAL();
        public static customersTable cBLL = new customersTable();

        private static Document document;
        static TextFrame addressFrame;
        static TextFrame infoFrame;
        static Table table;
        readonly static Color TableBorder = new Color(81, 125, 192);
        readonly static Color TableBlue = new Color(235, 240, 249);
        readonly static Color TableGray = new Color(242, 242, 242);
        public static bool isNumeric(string s)
        {
            return int.TryParse(s, out int n);
        }

        public static void CreateInvoice(int Invoice)
        {
            // now need to verify that this invoice exists...
            tBLL = tDAL.DisplayTransactionByInvoice(Invoice);
            if (tBLL.invoice_number.ToString() != Invoice.ToString())
            {
                Console.WriteLine("No such Invoice: " + tBLL.invoice_number.ToString());
                return;
            }
            tidata = tiDAL.DisplayTransactionDetailByInvoice(tBLL.invoice_number);
            cBLL = cDAL.SearchDealerCustomerForTransaction(tBLL.customer_id.ToString());

            // Create the document using MigraDoc.
            document = CreateDocument();
            document.UseCmykColor = true;
            // Create a renderer for PDF that uses Unicode font encoding.
            var pdfRenderer = new PdfDocumentRenderer(true);

            // Set the MigraDoc document.
            pdfRenderer.Document = document;

            // Create the PDF document.
            pdfRenderer.RenderDocument();

            // Save the PDF document...
            DateTime dt = new DateTime(tBLL.transaction_date);
            var filename = Path.Combine(gParams.OutputFolder + "\\Invoice-" + tBLL.invoice_number.ToString() + "-" + cBLL.name + "-" + dt.ToString("MMddyyyy") + ".pdf");

            pdfRenderer.Save(filename);
            // ...and start a viewer.
            try
            {
                Process.Start(filename);
            }
            catch (Win32Exception ex)
            {
                System.Windows.Forms.MessageBox.Show(ex.Message);
            }

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
            style.Font.Size = 9;

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
            section.PageSetup.PageFormat = PageFormat.Letter;
            section.PageSetup.TopMargin = "2cm";
            section.PageSetup.BottomMargin = "2cm";
            /*
            // Put a logo in the header
            // both images too dark on laser, need better solution for "logo"
            Image image = section.Headers.Primary.AddImage("businesscard.jpg");
            // OR Image image = section.Headers.Primary.AddImage("newbusinesscard.png");
            image.Height = "4cm";
            image.LockAspectRatio = true;
            image.RelativeVertical = RelativeVertical.Line;
            image.RelativeHorizontal = RelativeHorizontal.Margin;
            image.Top = ShapePosition.Center;
            image.Left = ShapePosition.Center;
            image.WrapFormat.Style = WrapStyle.Through;
            */
            Paragraph paragraph = section.Headers.Primary.AddParagraph();
            paragraph.AddText(gParams.CompanyName);
            if (tBLL.status == 3)
            {
                FormattedText paidText = paragraph.AddFormattedText("\tPaid in Full, Thank you!");
                paidText.Color = Colors.Firebrick;
            }
            paragraph.Format.Font.Size = 14;
            paragraph.Format.Alignment = ParagraphAlignment.Left;

            // Create footer
            paragraph = section.Footers.Primary.AddParagraph();
            // Add the notes paragraph
            paragraph.Format.Borders.Color = TableBorder;
            paragraph.Format.Shading.Color = TableGray;

            Unit contentWidth = section.PageSetup.PageWidth - section.PageSetup.LeftMargin - section.PageSetup.RightMargin;
            Unit indentSize = (contentWidth) / 2.0;
            paragraph.Format.LeftIndent = indentSize;
            paragraph.Format.RightIndent = indentSize;

            paragraph.Format.Alignment = ParagraphAlignment.Center;
            paragraph.AddLineBreak();
            FormattedText thanks = paragraph.AddFormattedText("Thank you for your business!");
            thanks.Color = Colors.DarkBlue;
            paragraph.AddLineBreak();
            paragraph.AddLineBreak();
            string companyLine = gParams.CompanyName + " · " + gParams.CompanyCity + ", " + gParams.CompanyState + " · " + gParams.CompanyZip + " · " + gParams.CompanyPhone + " · " + gParams.CompanyEmail;
            paragraph.AddText(companyLine);
            paragraph.AddLineBreak();
            paragraph.AddLineBreak();
            paragraph.Format.Font.Size = 8;

            // Create the text frame for the address
            addressFrame = section.AddTextFrame();
            addressFrame.Height = "3.0cm";
            addressFrame.Width = "7.0cm";
            addressFrame.Left = ShapePosition.Left;
            addressFrame.RelativeHorizontal = RelativeHorizontal.Margin;
            addressFrame.Top = "6.0cm";
            addressFrame.RelativeVertical = RelativeVertical.Page;

            // Create a text frame for the invoice info
            infoFrame = section.AddTextFrame();
            infoFrame.Height = "3.0cm";
            infoFrame.Width = "6cm";
            infoFrame.Left = ShapePosition.Right;
            infoFrame.RelativeHorizontal = RelativeHorizontal.Margin;
            infoFrame.Top = "5.5cm";
            infoFrame.RelativeVertical = RelativeVertical.Page;

            paragraph = infoFrame.AddParagraph();
            paragraph.Format.Alignment = ParagraphAlignment.Justify;

            paragraph.Format.Font.Name = "Verdana";
            paragraph.Format.Font.Size = 10;
            // paragraph.Format.SpaceAfter = 3;
            paragraph.Style = "Reference";
            paragraph.AddText("INVOICE:\t\t" + tBLL.invoice_number.ToString("D6"));
            paragraph.AddLineBreak();
            DateTime td = new DateTime(tBLL.transaction_date);
            // DateTime dd = new DateTime(tBLL.due_date);
            paragraph.AddText("Invoice-date:\t\t" + td.ToString("MM/dd/yyyy"));
            paragraph.AddLineBreak();
            // paragraph.AddText("Due-date:    \t\t" + dd.ToString("MM/dd/yyyy"));
            paragraph.AddLineBreak();
            paragraph.AddLineBreak();
            paragraph.AddText("Terms:\t\t");
            FormattedText formattedText = paragraph.AddFormattedText(tBLL.terms.ToString());
            formattedText.Size = 8;

            // To move table down
            paragraph = section.AddParagraph();
            paragraph.Format.SpaceBefore = "8.5cm";

            // Create the item table
            table = section.AddTable();
            table.Style = "Table";
            table.Borders.Color = TableBorder;
            table.Borders.Width = 0.25;
            table.Borders.Left.Width = 0.5;
            table.Borders.Right.Width = 0.5;
            table.Rows.LeftIndent = 0;

            // Before you can add a row, you must define the columns
            Column column = table.AddColumn("1cm");
            column.Format.Alignment = ParagraphAlignment.Center;

            column = table.AddColumn("2.5cm");
            column.Format.Alignment = ParagraphAlignment.Right;

            column = table.AddColumn("3cm");
            column.Format.Alignment = ParagraphAlignment.Right;

            column = table.AddColumn("3.5cm");
            column.Format.Alignment = ParagraphAlignment.Right;

            column = table.AddColumn("2cm");
            column.Format.Alignment = ParagraphAlignment.Center;

            column = table.AddColumn("4.5cm");
            column.Format.Alignment = ParagraphAlignment.Right;

            // Create the header of the table
            Row row = table.AddRow();
            row.HeadingFormat = true;
            row.Format.Alignment = ParagraphAlignment.Center;
            row.Format.Font.Bold = true;
            row.Shading.Color = TableBlue;
            row.Cells[0].AddParagraph("Item");
            row.Cells[0].Format.Font.Bold = false;
            row.Cells[0].Format.Alignment = ParagraphAlignment.Left;
            row.Cells[0].VerticalAlignment = VerticalAlignment.Bottom;
            row.Cells[0].MergeDown = 1;
            row.Cells[1].AddParagraph("Name");
            row.Cells[1].Format.Alignment = ParagraphAlignment.Left;
            row.Cells[2].AddParagraph("Description");
            row.Cells[2].Format.Alignment = ParagraphAlignment.Left;
            row.Cells[2].MergeRight = 3;
            row.Cells[5].AddParagraph("Total");
            row.Cells[5].Format.Alignment = ParagraphAlignment.Left;
            row.Cells[5].VerticalAlignment = VerticalAlignment.Bottom;
            row.Cells[5].MergeDown = 1;

            row = table.AddRow();
            row.HeadingFormat = true;
            row.Format.Alignment = ParagraphAlignment.Center;
            row.Format.Font.Bold = true;
            row.Shading.Color = TableBlue;
            row.Cells[1].AddParagraph("Item Price");
            row.Cells[1].Format.Alignment = ParagraphAlignment.Left;
            row.Cells[1].MergeRight = 1;
            row.Cells[3].AddParagraph("Quantity");
            row.Cells[3].Format.Alignment = ParagraphAlignment.Left;
            row.Cells[4].AddParagraph("Tax");
            row.Cells[4].Format.Alignment = ParagraphAlignment.Left;

            table.SetEdge(0, 0, 6, 2, Edge.Box, BorderStyle.Single, 0.75, Color.Empty);
        }
        static void FillContent()
        {
            double taxrate = (double)gParams.taxrate;
            // Customer info
            Paragraph paragraph = addressFrame.AddParagraph();
            paragraph.AddText(cBLL.name.ToString());
            paragraph.AddLineBreak();
            paragraph.AddText(cBLL.address.ToString());
            paragraph.AddLineBreak();
            paragraph.AddText(cBLL.city.ToString() + " " + cBLL.state.ToString() + ", " + cBLL.zip.ToString());
            paragraph.AddLineBreak();
            paragraph.AddText(cBLL.contact.ToString());
            paragraph.AddLineBreak();
            paragraph.AddText(cBLL.email.ToString());

            // Iterate the invoice items
            double totalExtendedPrice = 0;
            double nontaxableExtendedPrice = 0;
            double salestaxtotal = 0;

            int i = 1;
            foreach (DataRow dr in tidata.Rows)
            {
                double quantity = double.Parse(dr["qty"].ToString());
                double price = double.Parse(dr["price"].ToString());
                double total = double.Parse(dr["total"].ToString());

                // Each item fills two rows
                Row row1 = table.AddRow();
                Row row2 = table.AddRow();
                row1.TopPadding = 1.5;
                row1.Cells[0].Shading.Color = TableGray;
                row1.Cells[0].VerticalAlignment = VerticalAlignment.Center;
                row1.Cells[0].MergeDown = 1;
                row1.Cells[2].Format.Alignment = ParagraphAlignment.Left;
                row1.Cells[2].MergeRight = 2;
                row1.Cells[5].Shading.Color = TableGray;
                row1.Cells[5].MergeDown = 1;
                row2.Cells[1].MergeRight = 1;

                row1.Cells[0].AddParagraph(i.ToString());
                paragraph = row1.Cells[1].AddParagraph();
                paragraph.AddFormattedText(dr["product_name"].ToString(), TextFormat.Bold);
                paragraph = row1.Cells[2].AddParagraph();
                paragraph.AddFormattedText(dr["description"].ToString(), TextFormat.Bold);
                row2.Cells[1].AddParagraph("$ " + price.ToString("0.00"));
                row2.Cells[3].AddParagraph(quantity.ToString());
                double salestax = taxrate * double.Parse(dr["isTaxable"].ToString()) * double.Parse(cBLL.taxable.ToString()) * quantity * price;
                row2.Cells[4].AddParagraph("$ " + salestax.ToString("0.00"));
                total = price * quantity + salestax;
                row1.Cells[5].AddParagraph("$ " + total.ToString("0.00"));
                row1.Cells[5].VerticalAlignment = VerticalAlignment.Bottom;
                if (dr["isTaxable"].ToString() == "1") totalExtendedPrice += price * quantity;
                else nontaxableExtendedPrice += price * quantity;

                salestaxtotal += salestax;

                table.SetEdge(0, table.Rows.Count - 2, 6, 2, Edge.Box, BorderStyle.Single, 0.75);
            }

            // Add an invisible row as a space line to the table
            Row row = table.AddRow();
            row.Borders.Visible = false;

            // Add the taxable price row
            row = table.AddRow();
            row.Cells[0].Borders.Visible = false;
            row.Cells[0].AddParagraph("Taxable Subtotal");
            row.Cells[0].Format.Font.Bold = true;
            row.Cells[0].Format.Alignment = ParagraphAlignment.Right;
            row.Cells[0].MergeRight = 4;
            row.Cells[5].AddParagraph("$ " + totalExtendedPrice.ToString("0.00"));

            // Add the Non-Taxable row
            row = table.AddRow();
            row.Cells[0].Borders.Visible = false;
            row.Cells[0].AddParagraph("Nontaxable Subtotal");
            row.Cells[0].Format.Font.Bold = true;
            row.Cells[0].Format.Alignment = ParagraphAlignment.Right;
            row.Cells[0].MergeRight = 4;
            row.Cells[5].AddParagraph("$ " + nontaxableExtendedPrice.ToString("0.00"));

            // Add the Sales Tax row
            row = table.AddRow();
            row.Cells[0].Borders.Visible = false;
            row.Cells[0].AddParagraph("Sales Tax");
            row.Cells[5].AddParagraph("$ " + salestaxtotal.ToString("0.00"));
            row.Cells[0].Format.Font.Bold = true;
            row.Cells[0].Format.Alignment = ParagraphAlignment.Right;
            row.Cells[0].MergeRight = 4;

            // Add the total due row
            row = table.AddRow();
            row.Cells[0].AddParagraph("Invoice Total");
            row.Cells[0].Borders.Visible = false;
            row.Cells[0].Format.Font.Bold = true;
            row.Cells[0].Format.Alignment = ParagraphAlignment.Right;
            row.Cells[0].MergeRight = 4;
            totalExtendedPrice = nontaxableExtendedPrice + totalExtendedPrice + salestaxtotal;
            row.Cells[5].AddParagraph("$ " + totalExtendedPrice.ToString("0.00"));

            // Add the Amount Paid row
            // decimal AmtPaid = 0;
            row = table.AddRow();
            row.Cells[0].AddParagraph("Amount Paid");
            row.Cells[0].Borders.Visible = false;
            row.Cells[0].Format.Font.Bold = true;
            row.Cells[0].Format.Alignment = ParagraphAlignment.Right;
            row.Cells[0].MergeRight = 4;
            decimal AmtPaid = tBLL.amtPaid;
            row.Cells[5].AddParagraph("$ " + AmtPaid.ToString("0.00"));

            // Add the remaining Balance Due row
            decimal BalRemaining = tBLL.grandTotal - AmtPaid;
            row = table.AddRow();
            row.Cells[0].AddParagraph("Remaining Balance");
            row.Cells[0].Borders.Visible = false;
            row.Cells[0].Format.Font.Bold = true;
            row.Cells[0].Format.Alignment = ParagraphAlignment.Right;
            row.Cells[0].MergeRight = 4;
            row.Cells[5].AddParagraph("$ " + BalRemaining.ToString("0.00"));

            // Set the borders of the specified cell range
            table.SetEdge(5, table.Rows.Count - 4, 1, 4, Edge.Box, BorderStyle.Single, 0.75);
        }

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

            /*
             // To move table down
             paragraph = section.AddParagraph();
             paragraph.Format.SpaceBefore = "7cm";

            */

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

            /*
             // To move table down
             paragraph = section.AddParagraph();
             paragraph.Format.SpaceBefore = "7cm";

            */

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

            /*
             // To move table down
             paragraph = section.AddParagraph();
             paragraph.Format.SpaceBefore = "7cm";

            */

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

}
