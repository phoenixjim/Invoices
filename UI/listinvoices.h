#ifndef _Invoices_listinvoices_h_
#define _Invoices_listinvoices_h_
#include <SqlCtrl/SqlCtrl.h>
#include <plugin/sqlite3/Sqlite3.h>

#define SCHEMADIALECT <plugin/sqlite3/Sqlite3Schema.h>
// #define MODEL "Invoices/Tables/Invoices.sch"
// #include <Sql/sch_header.h>

struct ListInvoicesWindow : WithListInvoicesWindowLayout<TopWindow> {
	String DBFile;
	String	configfile = ConfigFile();
	String	cfg;

	FileSel selectdbwin;
	void btnPrintClicked();
	void btnApplyPaymentClicked();
	void btnEditClicked();
	void btnVoidClicked();
	void btnPaidInFullClicked();
	void btnFixDateClicked();
	void btnByPaidClicked();
	void btnByBalanceDueClicked();
	void btnByDatesClicked();
	void btnByCustomerClicked();
	void btnByVoidedClicked();
	
public:
	ListInvoicesWindow();
	virtual void Paint(Draw& w) {
        w.DrawRect(GetSize(), Color(204, 255, 255)); // <= enter your background color here
    }
	String SelectDB();
};

/*
public partial class frmTransactions : Form
    {
        globalParams gParams = new globalParams();
        public frmTransactions()
        {
            InitializeComponent();
             SQLiteConnection conn = new SQLiteConnection(gParams.myConnection);
            {
                try
                {
                    string query = "select id, name from tbl_customers";
                    SQLiteDataAdapter da = new SQLiteDataAdapter(query, conn);
                    conn.Open();
                    DataSet ds = new DataSet();
                    da.Fill(ds, "tbl_customers");
                    cbCustomer.DataSource = ds.Tables["tbl_customers"];
                    cbCustomer.DisplayMember = "name";
                    cbCustomer.ValueMember = "id";
                    cbCustomer.SelectedIndex = 4;
                }
                catch (Exception ex)
                {
                    // write exception info to log or anything else
                    MessageBox.Show("Error occured! " + ex.StackTrace);
                }
                
            }
        }

        transactionDAL tdal = new transactionDAL();
        transactionDetailDAL tidal = new transactionDetailDAL();

        // customersDAL cDAL = new customersDAL();
        private void pictureBoxClose_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void frmTransactions_Load(object sender, EventArgs e)
        {
            //Dispay all the transactions
            DataTable dt = tdal.DisplayAllTransactions();
            dgvTransactions.DataSource = dt;
            dgvTransactions.Sort(dgvTransactions.Columns[0], System.ComponentModel.ListSortDirection.Descending);
        }
        private void btnAll_Click(object sender, EventArgs e)
        {
            //Dispay all the transactions
            DataTable dt = tdal.DisplayAllTransactions();
            dgvTransactions.DataSource = dt;
            dgvTransactions.Sort(dgvTransactions.Columns[0], System.ComponentModel.ListSortDirection.Descending);
        }

        private void dgvTransactions_CellFormatting(object sender, DataGridViewCellFormattingEventArgs e)
        {
            // If the column is the DATED column, check the
            // value.
            if ((e.ColumnIndex == 3) || (e.ColumnIndex == 10))
            {
                ShortFormDateFormat(e);
            }
            else if (5 <= e.ColumnIndex && e.ColumnIndex <= 9)
                this.dgvTransactions.Columns[e.ColumnIndex].DefaultCellStyle.Format = "c"; ;
        }
        private static void ShortFormDateFormat(DataGridViewCellFormattingEventArgs formatting)
        {
            if (formatting.Value != null)
            {
                try
                {
                    DateTime theDate = new DateTime((long)formatting.Value);
                    String dateString = theDate.ToString("MM/dd/yyyy");
                    formatting.Value = dateString;
                    formatting.FormattingApplied = true;
                }
                catch (FormatException)
                {
                    // Set to false in case there are other handlers interested trying to
                    // format this DataGridViewCellFormattingEventArgs instance.
                    formatting.FormattingApplied = false;
                }
            }
        }
        private void btnShowDates_Click(object sender, EventArgs e)
        {
            DateTime start, end;
            start = dtStart.Value;
            end = dtEnd.Value;
            DataTable dt = tdal.DisplayTransactionByDateRange(start.Ticks, end.Ticks);
            dgvTransactions.DataSource = dt;
            dgvTransactions.Sort(dgvTransactions.Columns[0], System.ComponentModel.ListSortDirection.Descending);
        }

        private void btnShowCustomer_Click(object sender, EventArgs e)
        {
            if (cbCustomer.SelectedIndex >= 0)
            {
                 DataTable dt = tdal.DisplayTransactionByCustomer(cbCustomer.SelectedValue.ToString());
                dgvTransactions.DataSource = dt;
                dgvTransactions.Sort(dgvTransactions.Columns[0], System.ComponentModel.ListSortDirection.Descending);
            }
        }

        private void btnShowVoided_Click(object sender, EventArgs e)
        {
            DataTable dt = tdal.DisplayVoidedTransactions();
            dgvTransactions.DataSource = dt;
        }

        transactionsTable tBLL = new transactionsTable();

        private void btnApplyPayment_Click(object sender, EventArgs e)
        {
            if (dgvTransactions.SelectedRows.Count != 1)
                return;
            tBLL = tdal.DisplayTransactionByInvoice(Convert.ToInt32(dgvTransactions.SelectedRows[0].Cells[1].Value.ToString()));
            // customersDAL cDAL = new customersDAL();
            // int taxable = cDAL.GetTaxableFromName(tBLL.customer_id.ToString());
            if (ctxtPayment.Text != "" && decimal.Parse(ctxtPayment.Text) > 0) tBLL.amtPaid = decimal.Parse(ctxtPayment.Text);
            else return;
            if (tBLL.amtPaid > 0)
            {
                tBLL.datePaid = tBLL.transaction_date; // DateTime.Today.Ticks;
                tBLL.status = (tBLL.grandTotal.Equals(tBLL.amtPaid) ? (int)status.paidinfull : (int)status.partialpayment);
            }

            tdal.Update(tBLL);
            DataTable dt = tdal.DisplayAllTransactions();
            dgvTransactions.DataSource = dt;
            dgvTransactions.Sort(dgvTransactions.Columns[0], System.ComponentModel.ListSortDirection.Descending);
        }

        private void dgvTransactions_DataBindingComplete(object sender, DataGridViewBindingCompleteEventArgs e)
        {
            dgvTransactions.AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.DisplayedCells;
            dgvTransactions.Columns[5].AutoSizeMode = DataGridViewAutoSizeColumnMode.Fill;
        }

        private void btnVoid_Click(object sender, EventArgs e)
        {
            if (dgvTransactions.SelectedRows.Count != 1)
                return;
            tBLL = tdal.DisplayTransactionByInvoice(Convert.ToInt32(dgvTransactions.SelectedRows[0].Cells[1].Value.ToString()));
            tBLL.status = (int)status.voided;
            tBLL.amtPaid = tBLL.grandTotal = tBLL.nonTaxableSub = tBLL.taxableSub = tBLL.tax = 0;
            tidal.VoidTransactionItemsByInvoice(Convert.ToInt32(dgvTransactions.SelectedRows[0].Cells[1].Value.ToString()));
            tBLL.invoice_number = -1;
            tdal.Update(tBLL);
            DataTable dt = tdal.DisplayAllTransactions();
            dgvTransactions.DataSource = dt;
            dgvTransactions.Sort(dgvTransactions.Columns[0], System.ComponentModel.ListSortDirection.Descending);
        }
        private void dgvTransactions_RowHeaderMouseClick(object sender, DataGridViewCellMouseEventArgs e)
        {
            if (dgvTransactions.SelectedRows.Count < 1)
                return;
            tBLL = tdal.DisplayTransactionByInvoice(Convert.ToInt32(dgvTransactions.SelectedRows[0].Cells[1].Value.ToString()));
        }
        private void txtPaid_Click(object sender, EventArgs e)
        {
            DataTable dt = tdal.DisplayPaidTransactions(); // include partial payments
            dgvTransactions.DataSource = dt;
            dgvTransactions.Sort(dgvTransactions.Columns[0], System.ComponentModel.ListSortDirection.Descending);
        }

        private void txtBalanceDue_Click(object sender, EventArgs e)
        {
            
            DataTable dt = tdal.DisplayTransactionsWithBalance(); // include partial payments
            dgvTransactions.DataSource = dt;
            dgvTransactions.Sort(dgvTransactions.Columns[0], System.ComponentModel.ListSortDirection.Descending);
        }

        private void btnPrintSelected_Click(object sender, EventArgs e)
        {
            if (dgvTransactions.SelectedRows.Count < 1)
            {
                MessageBox.Show("Please select the invoice(s) first, thank you!");
                return;
            }
            for (int i = 0; i < dgvTransactions.SelectedRows.Count; i++)
            {
                Invoices.CreateInvoice(int.Parse(dgvTransactions.SelectedRows[i].Cells[1].Value.ToString()));
            }
        }

        private void btnIntegrity_Click(object sender, EventArgs e)
        {
            this.Hide();
        }

        private void btnPaidInFull_Click(object sender, EventArgs e)
        {
            if (dgvTransactions.SelectedRows.Count != 1)
                return;
            tBLL = tdal.DisplayTransactionByInvoice(Convert.ToInt32(dgvTransactions.SelectedRows[0].Cells[1].Value.ToString()));
            tBLL.status = (int)status.paidinfull;
            tdal.Update(tBLL);
            DataTable dt = tdal.DisplayAllTransactions();
            dgvTransactions.DataSource = dt;
            dgvTransactions.Sort(dgvTransactions.Columns[0], System.ComponentModel.ListSortDirection.Descending);
        }

        private void btnFixDate_Click(object sender, EventArgs e)
        {
            if (dgvTransactions.SelectedRows.Count != 1)
            {
                MessageBox.Show("Must select exactly 1 line.");
                return;
            }
            tBLL = tdal.DisplayTransactionByInvoice(Convert.ToInt32(dgvTransactions.SelectedRows[0].Cells[1].Value.ToString()));
            tBLL.transaction_date = dtpFixDate.Value.Ticks;
            tdal.Update(tBLL);
            DataTable dt = tdal.DisplayAllTransactions();
            dgvTransactions.DataSource = dt;
            dgvTransactions.Sort(dgvTransactions.Columns[0], System.ComponentModel.ListSortDirection.Descending);
        }

        private void btnEdit_Click(object sender, EventArgs e)
        {
            if (dgvTransactions.SelectedRows.Count != 1)
            {
                MessageBox.Show("Must select exactly 1 line.");
                return;
            }
            
            int status = tdal.getStatus(Convert.ToInt32(dgvTransactions.SelectedRows[0].Cells[1].Value.ToString()));
            if (status != 0)
            {
                MessageBox.Show("Can't Edit invoice " + dgvTransactions.SelectedRows[0].Cells[1].Value.ToString() + "\nIt has been voided or paid partially or fully");
                return;
            }
            frmEditInvoice editInvoice = new frmEditInvoice(Convert.ToInt32(dgvTransactions.SelectedRows[0].Cells[1].Value.ToString()));
            editInvoice.Show();
            this.Close();
        }
    }
	
*/
#endif
