#ifndef _Invoices_createinvoice_h_
#define _Invoices_createinvoice_h_
#include <SqlCtrl/SqlCtrl.h>
#include <plugin/sqlite3/Sqlite3.h>

#define SCHEMADIALECT <plugin/sqlite3/Sqlite3Schema.h>
// #define MODEL "Invoices/Tables/Invoices.sch"
// #include <Sql/sch_header.h>

struct CreateInvoiceWindow : WithCreateInvoiceWindowLayout<TopWindow> {
public:
	CreateInvoiceWindow() { CtrlLayout(*this, "Create Invoice"); }
	void Paint(Draw& w) {
        w.DrawRect(GetSize(), Color(204, 255, 255)); // <= enter your background color here
     }
};

/*
	public partial class frmCreateInvoice : Form
    {
        globalParams gParams = new globalParams();
        CustomersDAL dcDAL = new CustomersDAL();
        // productsDAL pDAL = new productsDAL();

        transactionDAL tDAL = new transactionDAL();
        transactionDetailDAL tdDAL = new transactionDetailDAL();

        DataTable transactionDT = new DataTable();
        int detailId = 0, currentinvoice = 0;
        public enum pName
        { Service=1,
        Part,
        Tip,
        Refund
        };
        public frmCreateInvoice()
        {
            InitializeComponent();
            
            gParams.lastinvoice = tDAL.MaxInvoiceNumber();
            currentinvoice = gParams.lastinvoice + 1;
            txtInvNumber.Text = (gParams.lastinvoice + 1).ToString();
            txtNonTaxable.Text = String.Format("{0:2}", "0");
            txtSubTotal.Text = String.Format("{0:2}", "0");
            txtSalesTax.Text = String.Format("{0:2}", "0");
            txtGrandTotal.Text = String.Format("{0:2}", "0");
            txtTaxRate.Text = Convert.ToString(gParams.taxrate);
            cbProduct.DataSource = Enum.GetValues(typeof(pName));
            // btnUpdate.Visible = false;
            btnSave.Visible = true;
            btnUpdateRow.Visible = false;
            btnDelete.Visible = true;

            using SQLiteConnection conn = new SQLiteConnection(gParams.myConnection);
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
        private void pictureBoxClose_Click(object sender, EventArgs e)
        {
            this.Hide();
        }

        private void frmCreateInvoice_Load(object sender, EventArgs e)
        {
            try {
                // specify Columns for our TransactionDataTable if null
                transactionDT.Columns.Add("Product Name");
                transactionDT.Columns.Add("Description");
                transactionDT.Columns.Add("Price");
                transactionDT.Columns.Add("Quantity");
                transactionDT.Columns.Add("Total");
                transactionDT.Columns.Add("IsTaxable");

                //Show in DAta Grid View
                dgvAddedProducts.DataSource = transactionDT;

                dtpBillDate.Format = DateTimePickerFormat.Custom;
                dtpBillDate.CustomFormat = "dddd MM/dd/yyyy";
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message + " Load Error!");
            }
        }

        private void btnAdd_Click(object sender, EventArgs e)
        {
            //Get Product Name, Description, Price and Qty customer wants to buy
            if (cbProduct.SelectedIndex == -1)
            {
                MessageBox.Show("Must select product type first.");
                return;
            }
            string productName = cbProduct.Items[cbProduct.SelectedIndex].ToString();
            string description = txtDescription.Text;
            decimal Price = decimal.Parse(ctxtPrice.Text);
            decimal Qty = decimal.Parse(TxtQty.Text);

            decimal Total = Price * Qty; //Total=RatexQty
            int Taxable = (chkProductTaxable.Checked ? 1 : 0);

            //Display the Subtotal in textbox
            //Get the subtotal value from textbox
            decimal subTotal, nonTaxable, tax, grandtotal, taxrate;
            subTotal = decimal.Parse(txtSubTotal.Text);
            nonTaxable = decimal.Parse(txtNonTaxable.Text);
            // tax = decimal.Parse(txtSalesTax.Text);
            taxrate = decimal.Parse(txtTaxRate.Text);

            if (chkProductTaxable.Checked && chkTaxable.Checked)
            {
                subTotal += Price * Qty;
                tax = (subTotal * taxrate);
            }
            else if (chkTaxable.Checked)
            {
                tax = (subTotal * taxrate);
                nonTaxable += Price * Qty;
            }
            else
            {
                tax = 0;
                nonTaxable += Price * Qty;
            }
            tax = decimal.Round(tax, 2);

            //Add product to the dAta Grid View
            transactionDT.Rows.Add(productName,description,Price,Qty,Total,Taxable);
            // dgvAddedProducts.Rows.Add(productName, description, Price, Qty, Total, Taxable);

            //Display the Subtotal in textbox, based on customer taxable status
            if (chkTaxable.Checked)
            {
                txtNonTaxable.Text = nonTaxable.ToString();
                txtSubTotal.Text = subTotal.ToString();
            }
            else
            {
                txtSubTotal.Text = "0";
                txtNonTaxable.Text = (nonTaxable + subTotal).ToString();
            }
            txtSalesTax.Text = tax.ToString();
            
            grandtotal = decimal.Round(nonTaxable+subTotal+tax, 2);

            txtGrandTotal.Text = grandtotal.ToString();
            // calculate_Totals();

            //Clear the Textboxes
            // txtProductName.Text = "";
            cbProduct.SelectedIndex = 0;
            txtDescription.Text = "";
            ctxtPrice.Text = "0.00";
            TxtQty.Text = "";
            chkProductTaxable.Checked = true;
        }
        private void btnDelete_Click(object sender, EventArgs e)
        {
            Int32 selectedRowCount =
                    dgvAddedProducts.Rows.GetRowCount(DataGridViewElementStates.Selected);
            if (selectedRowCount > 0)
            {
                decimal Total, subTotal, subNonTaxable, Tax, GrandTotal;
                int taxable;

                Total = Convert.ToDecimal(dgvAddedProducts.SelectedRows[0].Cells["Total"].Value.ToString());
                // MessageBox.Show(dgvAddedProducts.SelectedRows[0].Cells[5].Value.ToString());
                taxable = Convert.ToInt32(dgvAddedProducts.SelectedRows[0].Cells[5].Value.ToString());
                subTotal = Convert.ToDecimal(txtSubTotal.Text);
                subNonTaxable = Convert.ToDecimal(txtNonTaxable.Text);
                Tax = Convert.ToDecimal(txtSalesTax.Text);
                if (Convert.ToBoolean(taxable) && chkTaxable.Checked)
                {
                    subTotal -= Total;
                    Tax -= Total * Convert.ToDecimal(txtTaxRate.Text);
                }
                else
                    subNonTaxable -= Total;
                GrandTotal = subNonTaxable + subTotal + Tax;
                txtSubTotal.Text = subTotal.ToString();
                txtNonTaxable.Text = subNonTaxable.ToString();
                txtSalesTax.Text = Tax.ToString();
                txtGrandTotal.Text = GrandTotal.ToString();

                dgvAddedProducts.Rows.RemoveAt(dgvAddedProducts.SelectedRows[0].Index);
            }
                
        }

        private void btnSave_Click(object sender, EventArgs e)
        {
            //Get the Values from PurchaseSales Form First
            transactionsTable transaction = new transactionsTable();

            //Get the ID of Customer Here
            //Lets get name of the customer first
            if (cbCustomer.SelectedIndex < 0)
            {
                MessageBox.Show("Must select a Customer!");
                return;
            }
            // string deaCustName = "Frank Shannon"; // FROM combobox
            // customersBLL dc = // dcDAL.GetDeaCustIDFromName(deaCustName);

            // DETAILS for transaction
            transaction.invoice_number = Convert.ToInt32(txtInvNumber.Text);
            transaction.customer_id = Convert.ToInt32(cbCustomer.SelectedValue.ToString()); // dc.id;
            transaction.transaction_date = dtpBillDate.Value.Date.Ticks;
            // transaction.due_date = dtpDueDate.Value.Date.Ticks;
            transaction.terms = txtTerms.Text;
            transaction.nonTaxableSub = decimal.Parse(txtNonTaxable.Text);
            transaction.taxableSub = decimal.Parse(txtSubTotal.Text);
            transaction.tax = decimal.Parse(txtSalesTax.Text);
            transaction.grandTotal = decimal.Parse(txtGrandTotal.Text);
            transaction.amtPaid = 0;
            transaction.datePaid = 0;
            transaction.status = (int)status.notpaid;


            // transaction.added_by = u.id;
            transaction.transactionDetails = transactionDT;

            //Lets Create a Boolean Variable and set its value to false
            bool success = false;

            //Actual Code to Insert Transaction And Transaction Details
            //Create aboolean value and insert transaction 
            bool w = tDAL.Insert_Transaction(transaction);

            //Use for loop to insert Transaction Details
            for (int i = 0; i < transactionDT.Rows.Count; i++)
            {
                //Get all the details of the product
                transactionItemsTable transactionDetail = new transactionItemsTable();

                transactionDetail.product_name = transactionDT.Rows[i][0].ToString();
                transactionDetail.description = transactionDT.Rows[i][1].ToString();
                transactionDetail.price = Math.Round(decimal.Parse(transactionDT.Rows[i][2].ToString()), 2);
                transactionDetail.qty = decimal.Parse(transactionDT.Rows[i][3].ToString());
                transactionDetail.total = Math.Round(decimal.Parse(transactionDT.Rows[i][4].ToString()), 2);
                transactionDetail.invoice_id = Convert.ToInt32(txtInvNumber.Text);
                transactionDetail.isTaxable = Convert.ToInt32(transactionDT.Rows[i]["isTaxable"].ToString());

                //Insert Transaction Details inside the database
                bool y = tdDAL.InsertTransactionDetail(transactionDetail);
                success = w && y;
            }

            if (success == true)
            {
                gParams.lastinvoice = Convert.ToInt32(txtInvNumber.Text);
                gParams.saveParams();
            }
            else
            {
                //Transaction Failed
                MessageBox.Show("Transaction Failed");
                this.Close();
            }
            if (MessageBox.Show("Print this invoice?", "Print", MessageBoxButtons.YesNo, MessageBoxIcon.Question) == DialogResult.Yes)
            {
                // Print to PDF :)
                // Process.Start("JTS_Invoice_pdf.exe", gParams.lastinvoice.ToString());
                Invoices.CreateInvoice(gParams.lastinvoice);
            }
            this.Close();
        }

        private void cbCustomer_SelectedIndexChanged(object sender, EventArgs e)
        {
            string value = cbCustomer.SelectedValue.ToString();
            int taxable = dcDAL.GetTaxableFromName(value); // .Items[cbCustomer.SelectedIndex].DisplayMemeber.ToString()) ;
            chkTaxable.Checked = Convert.ToBoolean(taxable);
            
        }

        private void cbProduct_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (cbProduct.SelectedIndex > 0)
            {
                string prodName = cbProduct.SelectedValue.ToString();
                switch (prodName)
                {
                    case "Tip":
                        chkProductTaxable.Checked = false;
                        break;
                    case "Refund":
                        chkProductTaxable.Checked = false;
                        break;
                    case "Service":
                        chkProductTaxable.Checked = true;
                        break;
                    case "Part":
                        chkProductTaxable.Checked = true;
                        break;
                }
            }
        }

        private void frmCreateInvoice_FormClosed(object sender, FormClosedEventArgs e)
        {
            gParams.saveParams();
        }

        private void dgvAddedProducts_CellFormatting(object sender, DataGridViewCellFormattingEventArgs e)
        {
            if (3 == e.ColumnIndex || e.ColumnIndex == 5)
                this.dgvAddedProducts.Columns[e.ColumnIndex].DefaultCellStyle.Format = "c"; ;
        }

        private void btnUpdate_Click(object sender, EventArgs e)
        {
            //Get the Values from PurchaseSales Form First
            transactionsTable transaction = new transactionsTable();

            //Get the ID of Customer Here
            //Lets get name of the customer first
            if (cbCustomer.SelectedIndex < 0)
            {
                MessageBox.Show("Must select a Customer!");
                return;
            }
            // string deaCustName = "Frank Shannon"; // FROM combobox
            // customersBLL dc = // dcDAL.GetDeaCustIDFromName(deaCustName);

            // DETAILS for transaction
            transaction.invoice_number = Convert.ToInt32(txtInvNumber.Text);
            transaction.customer_id = Convert.ToInt32(cbCustomer.SelectedValue.ToString()); // dc.id;
            transaction.transaction_date = dtpBillDate.Value.Date.Ticks;
            // transaction.due_date = dtpDueDate.Value.Date.Ticks;
            transaction.terms = txtTerms.Text;
            transaction.nonTaxableSub = decimal.Parse(txtNonTaxable.Text);
            transaction.taxableSub = decimal.Parse(txtSubTotal.Text);
            transaction.tax = decimal.Parse(txtSalesTax.Text);
            transaction.grandTotal = decimal.Parse(txtGrandTotal.Text);
            transaction.amtPaid = 0;
            transaction.datePaid = 0;
            transaction.status = (int)status.notpaid;


            // transaction.added_by = u.id;
            transaction.transactionDetails = transactionDT;

            //Lets Create a Boolean Variable and set its value to false
            bool success = false;

            //Actual Code to Insert Transaction And Transaction Details
            //Create aboolean value and insert transaction 
            bool w = tDAL.Update(transaction);

            //Use for loop to insert Transaction Details
            for (int i = 0; i < transactionDT.Rows.Count; i++)
            {
                //Get all the details of the product
                transactionItemsTable transactionDetail = new transactionItemsTable();

                transactionDetail.product_name = transactionDT.Rows[i][1].ToString();
                transactionDetail.description = transactionDT.Rows[i][2].ToString();
                transactionDetail.price = Math.Round(decimal.Parse(transactionDT.Rows[i][3].ToString()), 2);
                transactionDetail.qty = decimal.Parse(transactionDT.Rows[i][4].ToString());
                transactionDetail.total = Math.Round(decimal.Parse(transactionDT.Rows[i][5].ToString()), 2);
                // transactionDetail.description = txtDescription.Text;
                transactionDetail.invoice_id = Convert.ToInt32(txtInvNumber.Text);
                // MessageBox.Show(transactionDT.Rows[i]["Taxable"].ToString());
                transactionDetail.isTaxable = Convert.ToInt32(transactionDT.Rows[i]["isTaxable"].ToString());

                //Insert Transaction Details inside the database
                bool y = tdDAL.InsertTransactionDetail(transactionDetail);
                success = w && y;
            }

            if (success == false)
            {
                //Transaction Failed
                MessageBox.Show("Transaction Failed");
                this.Close();
            }
            if (MessageBox.Show("Print this invoice?", "Print", MessageBoxButtons.YesNo, MessageBoxIcon.Question) == DialogResult.Yes)
            {
                // Print to PDF :)
                // Process.Start("JTS_Invoice_pdf.exe", gParams.lastinvoice.ToString());
                Invoices.CreateInvoice(gParams.lastinvoice);
            }
            this.Close();
        }

        private void dgvAddedProducts_RowHeaderMouseDoubleClick(object sender, DataGridViewCellMouseEventArgs e)
        {
            detailId = e.RowIndex;
            DataGridViewRow row = dgvAddedProducts.Rows[detailId];

            cbProduct.SelectedIndex = (int)Enum.Parse(typeof(pName), row.Cells["product_name"].Value.ToString().Trim()) - 1;
            txtDescription.Text = row.Cells["description"].Value.ToString();
            TxtQty.Text = row.Cells["qty"].Value.ToString();
            ctxtPrice.Text = row.Cells["price"].Value.ToString();
            chkTaxable.Checked = row.Cells["isTaxable"].Value.ToString() == "1";
        }

        private void btnDeductTax_Click(object sender, EventArgs e)
        {
            if (ctxtPrice.Text == null || ctxtPrice.Text == "" || TxtQty.Text != "1")
            {
                MessageBox.Show("Must have price first, single items only please.");
                return;
            }
            double Price = Math.Round(double.Parse(ctxtPrice.Text) / 1.08, 2);
            ctxtPrice.Text = Price.ToString();
        }

        private void btnUpdateRow_Click(object sender, EventArgs e)
        {
            DataGridViewRow newDataRow = dgvAddedProducts.Rows[detailId];
            newDataRow.Cells[1].Value = cbProduct.Text;
            newDataRow.Cells[2].Value = txtDescription.Text;
            newDataRow.Cells[3].Value = ctxtPrice.Text;
            newDataRow.Cells[4].Value = TxtQty.Text;
            newDataRow.Cells[5].Value = (long.Parse(ctxtPrice.Text) * long.Parse(TxtQty.Text)).ToString();
            newDataRow.Cells[7].Value = chkTaxable.Checked ? 1 : 0;

            decimal Price = decimal.Parse(ctxtPrice.Text);
            decimal Qty = decimal.Parse(TxtQty.Text);
        }
    }*/

#
#endif
