#include "Invoices.h"
#include "listinvoices.h"
#include "../Tables/tables.h"

ListInvoicesWindow::ListInvoicesWindow()
{
	String DBFile;
	String configfile = ConfigFile();
	CtrlLayout ( *this, "List Invoices" );
	btnEdit << [=] { btnEditClicked();
				   };
	btnPrint << [=] { btnPrintClicked();
					};
	btnApplyPayment << [=] { btnApplyPaymentClicked();
						   };
	btnVoid << [=] { btnVoidClicked() ;
				   } ;
	btnPaidInFull << [=] { btnPaidInFullClicked();
						 };
	btnFixDate << [=] { btnFixDateClicked();
					  };
	btnByPaid << [=] { btnByPaidClicked();
					 };
	btnByBalanceDue << [=] { btnByBalanceDueClicked();
						   };
	btnByDates << [=] { btnByDatesClicked();
					  };
	btnByCustomer << [=] { btnByCustomerClicked();
						 };
	btnByVoided << [=] { btnByVoidedClicked();
					   };
	btnFake << [=] { btnVoidClicked();
				   }; // added to prevent invalid memory access error! Wierd...

	if ( FileExists ( configfile ) )
	{
		VectorMap<String, String> cfg = LoadIniFile ( configfile );
		DBFile = cfg.Get ( "DBFile", Null );
	}

	else
	{
		DBFile = SelectDB();
	}

	SQL;

	Sqlite3Session sqlite3;

	if ( !sqlite3.Open ( DBFile ) )
	{
		Exclamation ( "Can't create or open database file\n" );
		return;
	}

	SQL = sqlite3;

	SqlId Invoices ( "Invoices" ), id ( "id" ), invoiceNumber ( "invoiceNumber" ), customerId ( "customerId" ),
	transactionDate ( "transactionDate" ), terms ( "terms" ), nonTaxableSub ( "nonTaxableSub" ), taxableSub ( "taxableSub" ),
	tax ( "tax" ), grandTotal ( "grandTotal" ), amtPaid ( "amtPaid" ), datePaid ( "datePaid" ), status ( "status" );

	ListInvoicesArray.SetTable ( Invoices, id );

	// ListInvoicesArray.AddKey(id); See above line
	// ListInvoicesArray.Join(BOOK_ID, book); // joins id from other db to this id
	// ListInvoicesArray.AddColumn(name, "Name");  .SetConvert(DateIntConvert());
	ListInvoicesArray.AddColumn ( invoiceNumber, "Invoice#" );
	ListInvoicesArray.AddColumn ( customerId, "Cust id" );
	ListInvoicesArray.AddColumn ( transactionDate, "Transaction Date" ).SetConvert ( DateIntConvert() );
	ListInvoicesArray.AddColumn ( terms, "Terms" );
	ListInvoicesArray.AddColumn ( nonTaxableSub, "Non-Taxable Sub" ).SetConvert ( ConvDouble() );
	ListInvoicesArray.AddColumn ( taxableSub, "Taxable Sub" ).SetConvert ( ConvDouble() );
	ListInvoicesArray.AddColumn ( tax, "Tax" ).SetConvert ( ConvDouble() );
	ListInvoicesArray.AddColumn ( grandTotal, "Grand Total" ).SetConvert ( ConvDouble() );
	ListInvoicesArray.AddColumn ( amtPaid, "Amount Paid" ).SetConvert ( ConvDouble() );
	ListInvoicesArray.AddColumn ( datePaid, "Date Paid" ).SetConvert ( DateIntConvert() );
	ListInvoicesArray.AddColumn ( status, "Status" );
	// ListInvoicesArray.ColumnWidths("5 5 10 12 5 5 5 5 5 5");
	ListInvoicesArray.SetOrderBy ( invoiceNumber );

	Sql sql ( sqlite3 );
	sql.Execute ( "select * from Invoices" );


	while ( sql.Fetch() )
		ListInvoicesArray.Add ( sql );

	// ListInvoicesArray.WhenBar = THISBACK(BorrowMenu);
	// ListInvoicesArray.WhenLeftDouble = THISBACK(EditBorrow);
	// ListInvoicesArray.GoEndPostQuery();

};

void ListInvoicesWindow::btnPrintClicked()
{
	PromptOK ( __func__ );
}

void ListInvoicesWindow::btnApplyPaymentClicked()
{
	int SelectedCount = 0;
	int SelectedRow = -1;
	int RowCount = ListInvoicesArray.GetCount();

	if ( ListInvoicesArray.GetSelectCount() < 1) // Only works if multiselect is enabled on array
	{
		String message = "Must select a row!";
		PromptOK(message);
		return;
	}
	// now get data from selected row, and apply payment.

	/*
	tBLL = tdal.DisplayTransactionByInvoice(Convert.ToInt32(dgvTransactions.SelectedRows[0].Cells[1].Value.ToString()));

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
	*/
}

void ListInvoicesWindow::btnEditClicked()
{
	PromptOK ( __func__ );
}

void ListInvoicesWindow::btnVoidClicked()
{
	PromptOK ( __func__ );
}

void ListInvoicesWindow::btnPaidInFullClicked()
{
	PromptOK ( __func__ );
}

void ListInvoicesWindow::btnFixDateClicked()
{
	PromptOK ( __func__ );
}

void ListInvoicesWindow::btnByPaidClicked()
{
	PromptOK ( __func__ );
}

void ListInvoicesWindow::btnByBalanceDueClicked()
{
	PromptOK ( __func__ );
}

void ListInvoicesWindow::btnByDatesClicked()
{
	PromptOK ( __func__ );
}

void ListInvoicesWindow::btnByCustomerClicked()
{
	PromptOK ( __func__ );
}

void ListInvoicesWindow::btnByVoidedClicked()
{
	PromptOK ( __func__ );
}

String ListInvoicesWindow::SelectDB()
{
	selectdbwin.Type ( t_ ( "Invoices DB" ), "*.jts" );

	if ( !selectdbwin.ExecuteOpen ( t_ ( "Select DB File" ) ) )
	{
		return "";
	}

	/*
	from http://leonardoce.interfree.it/leowiki.html "simple configfile"
	*/
	cfg << "DBFile=" << selectdbwin.Get() << "\n";

	if ( !SaveFile ( ConfigFile(), cfg ) )
	{
		Exclamation ( "Error saving configuration!" );
	}

	return selectdbwin.Get();
}

/*
    class transactionDAL
    {
        globalParams gParams = new globalParams();
        //Create a connection string variable

        #region Insert Transaction Method
        public bool Insert_Transaction(transactionsTable t)
        {
            //Create a boolean value and set its default value to false
            bool isSuccess = false;
            //Create a SQLiteConnection first
            using (SQLiteConnection conn = new SQLiteConnection(gParams.myConnection))
            {
                try
                {
                    //Open Database Connection
                    conn.Open();

                    //SQL Query to Insert Transactions
                    // string sql = "INSERT INTO tbl_transactions ( invoice_number, customer_id, transaction_date, due_date, terms, nonTaxableSub, taxableSub, tax, grandTotal, amtPaid, datePaid, status ) VALUES (@invoice_number, @customer_id, @transaction_date, @due_date, @terms, @nonTaxableSub, @taxableSub, @tax, @grandTotal, @amtPaid, @datePaid, @status); select last_insert_rowid()";// SELECT @IDENTITY;";
                    string sql = "INSERT INTO tbl_transactions ( invoice_number, customer_id, transaction_date, terms, nonTaxableSub, taxableSub, tax, grandTotal, amtPaid, datePaid, status ) VALUES (@invoice_number, @customer_id, @transaction_date, @terms, @nonTaxableSub, @taxableSub, @tax, @grandTotal, @amtPaid, @datePaid, @status); select last_insert_rowid()";// SELECT @IDENTITY;";

                    //Sql Commandto pass the value in sql query
                    using (SQLiteCommand cmd = new SQLiteCommand(sql, conn))
                    {

                        //Passing the value to sql query using cmd
                        cmd.Parameters.AddWithValue("@invoice_number", t.invoice_number);
                        cmd.Parameters.AddWithValue("@customer_id", t.customer_id);
                        cmd.Parameters.AddWithValue("@transaction_date", t.transaction_date);
                        // cmd.Parameters.AddWithValue("@due_date", t.due_date);
                        cmd.Parameters.AddWithValue("@terms", t.terms);
                        cmd.Parameters.AddWithValue("@nonTaxableSub", t.nonTaxableSub);
                        cmd.Parameters.AddWithValue("@taxableSub", t.taxableSub);
                        cmd.Parameters.AddWithValue("@tax", t.tax);
                        cmd.Parameters.AddWithValue("@grandTotal", t.grandTotal);
                        cmd.Parameters.AddWithValue("@amtPaid", t.amtPaid);
                        cmd.Parameters.AddWithValue("@datePaid", t.datePaid);
                        cmd.Parameters.AddWithValue("@status", t.status);



                        //Execute the Query
                        // object o = cmd.ExecuteScalar();
                        int rows = cmd.ExecuteNonQuery();


                        //If the query is executed successfully then the value will not be null else it will be null
                        if (rows > 0) // (o!=null)
                        {
                            //Query Executed Successfully
                            isSuccess = true;
                        }
                        else
                        {
                            //failed to execute query
                            isSuccess = false;
                        }
                    }
                }
                catch (Exception ex)
                {
                    MessageBox.Show(ex.Message);
                }
                finally
                {
                    //Close the connection
                    conn.Close();
                }

                if (ConnectionState.Open == conn.State) conn.Close();
                return isSuccess;
            } // end using connection
        }
        #endregion
        #region METHOD TO DISPLAY ALL THE TRANSACTION
        public DataTable DisplayAllTransactions()
        {
            //SQLiteConnection First
            SQLiteConnection conn = new SQLiteConnection(gParams.myConnection);

            //Create a DAta Table to hold the datafrom database temporarily
            DataTable dt = new DataTable();

            try
            {
                //Write the SQL Query to Display all Transactions
                string sql = "SELECT * FROM tbl_transactions";

                //SQLiteCommand to Execute Query
                SQLiteCommand cmd = new SQLiteCommand(sql, conn);

                //SQLiteDataAdapter to Hold the data from database
                SQLiteDataAdapter adapter = new SQLiteDataAdapter(cmd);

                //Open DAtabase Connection
                conn.Open();

                adapter.Fill(dt);
            }
            catch(Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
            finally
            {
                if (ConnectionState.Open == conn.State) conn.Close();
            }

            return dt;
        }
        #endregion
        #region METHOD TO DISPLAY TRANSACTION BASED ON Date Range

        public DataTable DisplayTransactionByDateRange(long start, long end)
        {
            //Create SQL Connection
            SQLiteConnection conn = new SQLiteConnection(gParams.myConnection);

            //Create a DataTable
            DataTable dt = new DataTable();

            try
            {
                //Write SQL Query
                string sql = "SELECT * FROM tbl_transactions WHERE status > 1 AND datePaid BETWEEN @start AND @end";

                //SQL Command to Execute Query
                SQLiteCommand cmd = new SQLiteCommand(sql, conn);
                //SQLiteDataAdapter to hold the data from database
                SQLiteDataAdapter adapter = new SQLiteDataAdapter(cmd);
                adapter.SelectCommand.Parameters.AddWithValue("@start", start);
                adapter.SelectCommand.Parameters.AddWithValue("@end", end);

                //Open DAtabase Connection
                conn.Open();
                adapter.Fill(dt);
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
            finally
            {
                conn.Close();
            }

            return dt;
        }
        #endregion
        #region METHOD TO DISPLAY TRANSACTION BASED ON Customer ID

        public DataTable DisplayTransactionByCustomer(string customer)
        {
            //Create SQL Connection
            SQLiteConnection conn = new SQLiteConnection(gParams.myConnection);

            //Create a DataTable
            DataTable dt = new DataTable();

            try
            {
                //Write SQL Query
                string sql = "SELECT * FROM tbl_transactions WHERE customer_id=@customer";

                //SQL Command to Execute Query
                SQLiteCommand cmd = new SQLiteCommand(sql, conn);
                //SQLiteDataAdapter to hold the data from database
                SQLiteDataAdapter adapter = new SQLiteDataAdapter(cmd);
                adapter.SelectCommand.Parameters.AddWithValue("@customer", customer);

                //Open DAtabase Connection
                conn.Open();
                adapter.Fill(dt);
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
            finally
            {
                conn.Close();
            }

            return dt;
        }
        #endregion
        #region Method to Update transaction in Database
        public bool Update(transactionsTable p)
        {
            //create a boolean variable and set its initial value to false
            bool isSuccess = false;

            //Create SQL Connection for DAtabase
            SQLiteConnection conn = new SQLiteConnection(gParams.myConnection);

            try
            {
                //SQL Query to Update Data in dAtabase
                String sql = "UPDATE tbl_transactions SET customer_id=@customer_id, transaction_date=@transaction_date, nonTaxableSub=@nontaxablesub, taxableSub=@taxablesub, tax=@tax, grandTotal=@grandtotal, amtPaid=@amtPaid, datePaid=@datePaid, status=@status WHERE id=@id";

                //Create SQL Cmmand to pass the value to query
                SQLiteCommand cmd = new SQLiteCommand(sql, conn);
                //Passing the values using parameters and cmd
                cmd.Parameters.AddWithValue("@customer_id", p.customer_id);
                cmd.Parameters.AddWithValue("@nontaxablesub", p.nonTaxableSub);
                cmd.Parameters.AddWithValue("@tax", p.tax);
                cmd.Parameters.AddWithValue("@transaction_date", p.transaction_date);
                cmd.Parameters.AddWithValue("@taxablesub", p.taxableSub);
                cmd.Parameters.AddWithValue("@grandtotal", p.grandTotal);
                cmd.Parameters.AddWithValue("@amtPaid", p.amtPaid);
                cmd.Parameters.AddWithValue("@status", p.status);
                cmd.Parameters.AddWithValue("@datePaid", p.datePaid);
                cmd.Parameters.AddWithValue("@id", p.id);

                //Open the Database connection
                conn.Open();

                //Create Int Variable to check if the query is executed successfully or not
                int rows = cmd.ExecuteNonQuery();

                //if the query is executed successfully then the value of rows will be greater than 0 else it will be less than zero
                if (rows > 0)
                {
                    //Query ExecutedSuccessfully
                    isSuccess = true;
                }
                else
                {
                    //Failed to Execute Query
                    isSuccess = false;
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
            finally
            {
                conn.Close();
            }

            return isSuccess;
        }
        #endregion
        #region METHOD TO DISPLAY TRANSACTION BASED ON Invoice Number
        public transactionsTable DisplayTransactionByInvoice(int invoice)
        {
            transactionsTable p = new transactionsTable();
            //Create SQL Connection
            SQLiteConnection conn = new SQLiteConnection(gParams.myConnection);

            //Create a DataTable
            DataTable dt = new DataTable();

            try
            {
                //Write SQL Query
                string sql = "SELECT * FROM tbl_transactions WHERE invoice_number=@invoice";

                //SQL Command to Execute Query
                SQLiteCommand cmd = new SQLiteCommand(sql, conn);
                //SQLiteDataAdapter to hold the data from database
                SQLiteDataAdapter adapter = new SQLiteDataAdapter(cmd);
                adapter.SelectCommand.Parameters.AddWithValue("@invoice", invoice);

                //Open DAtabase Connection
                conn.Open();
                adapter.Fill(dt);
                if (dt.Rows.Count > 0)
                {
                    p.id = int.Parse(dt.Rows[0]["id"].ToString());
                    p.invoice_number = int.Parse(dt.Rows[0]["invoice_number"].ToString());
                    p.customer_id = int.Parse(dt.Rows[0]["customer_id"].ToString());
                    p.transaction_date = long.Parse(dt.Rows[0]["transaction_date"].ToString());
                    // p.due_date = long.Parse(dt.Rows[0]["due_date"].ToString());
                    p.terms = dt.Rows[0]["terms"].ToString();
                    p.nonTaxableSub = decimal.Parse(dt.Rows[0]["nonTaxableSub"].ToString());
                    p.taxableSub = decimal.Parse(dt.Rows[0]["taxableSub"].ToString());
                    p.tax = decimal.Parse(dt.Rows[0]["tax"].ToString());
                    p.grandTotal = decimal.Parse(dt.Rows[0]["grandTotal"].ToString());
                    p.amtPaid  = decimal.Parse(dt.Rows[0]["amtPaid"].ToString());
                    p.datePaid = long.Parse(dt.Rows[0]["datePaid"].ToString());
                    p.status = int.Parse(dt.Rows[0]["status"].ToString());
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
            finally
            {
                conn.Close();
            }

            return p;
        }
        public DataTable GetTransactionByInvoice(int invoice)
        {
            transactionsTable p = new transactionsTable();
            //Create SQL Connection
            SQLiteConnection conn = new SQLiteConnection(gParams.myConnection);

            //Create a DataTable
            DataTable dt = new DataTable();

            try
            {
                //Write SQL Query
                string sql = "SELECT * FROM tbl_transactions WHERE invoice_number=@invoice";

                //SQL Command to Execute Query
                SQLiteCommand cmd = new SQLiteCommand(sql, conn);
                //SQLiteDataAdapter to hold the data from database
                SQLiteDataAdapter adapter = new SQLiteDataAdapter(cmd);
                adapter.SelectCommand.Parameters.AddWithValue("@invoice", invoice);

                //Open DAtabase Connection
                conn.Open();
                adapter.Fill(dt);
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
            finally
            {
                conn.Close();
            }

            return dt;
        }
        #endregion
        #region METHOD TO DISPLAY TRANSACTIONS with BALANCE Remaining

        public DataTable DisplayTransactionsWithBalance()
        {
            //Create SQL Connection
            SQLiteConnection conn = new SQLiteConnection(gParams.myConnection);

            //Create a DataTable
            DataTable dt = new DataTable();

            try
            {
                //Write SQL Query
                string sql = "SELECT * FROM tbl_transactions WHERE status=0 OR status=2";

                //SQL Command to Execute Query
                SQLiteCommand cmd = new SQLiteCommand(sql, conn);
                //SQLiteDataAdapter to hold the data from database
                SQLiteDataAdapter adapter = new SQLiteDataAdapter(cmd);

                //Open DAtabase Connection
                conn.Open();
                adapter.Fill(dt);
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
            finally
            {
                conn.Close();
            }

            return dt;
        }
        #endregion
        #region METHOD TO DISPLAY PAID TRANSACTIONS (including partial)

        public DataTable DisplayPaidTransactions()
        {
            //Create SQL Connection
            SQLiteConnection conn = new SQLiteConnection(gParams.myConnection);

            //Create a DataTable
            DataTable dt = new DataTable();

            try
            {
                //Write SQL Query
                string sql = "SELECT * FROM tbl_transactions WHERE status > 1";

                //SQL Command to Execute Query
                SQLiteCommand cmd = new SQLiteCommand(sql, conn);
                //SQLiteDataAdapter to hold the data from database
                SQLiteDataAdapter adapter = new SQLiteDataAdapter(cmd);

                //Open DAtabase Connection
                conn.Open();
                adapter.Fill(dt);
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
            finally
            {
                conn.Close();
            }

            return dt;
        }
        #endregion
        #region METHOD TO DISPLAY VOIDED TRANSACTIONS

        public DataTable DisplayVoidedTransactions()
        {
            //Create SQL Connection
            SQLiteConnection conn = new SQLiteConnection(gParams.myConnection);

            //Create a DataTable
            DataTable dt = new DataTable();

            try
            {
                //Write SQL Query
                string sql = "SELECT * FROM tbl_transactions WHERE status=1";

                //SQL Command to Execute Query
                SQLiteCommand cmd = new SQLiteCommand(sql, conn);
                //SQLiteDataAdapter to hold the data from database
                SQLiteDataAdapter adapter = new SQLiteDataAdapter(cmd);

                //Open DAtabase Connection
                conn.Open();
                adapter.Fill(dt);
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
            finally
            {
                conn.Close();
            }

            return dt;
        }
        #endregion
        #region Return status of invoice
        public int getStatus(int invoice)
        {
            transactionsTable p = new transactionsTable();
            //Create SQL Connection
            SQLiteConnection conn = new SQLiteConnection(gParams.myConnection);

            //Create a DataTable
            DataTable dt = new DataTable();

            try
            {
                //Write SQL Query
                string sql = "SELECT * FROM tbl_transactions WHERE invoice_number=@invoice";

                //SQL Command to Execute Query
                SQLiteCommand cmd = new SQLiteCommand(sql, conn);
                //SQLiteDataAdapter to hold the data from database
                SQLiteDataAdapter adapter = new SQLiteDataAdapter(cmd);
                adapter.SelectCommand.Parameters.AddWithValue("@invoice", invoice);

                //Open DAtabase Connection
                conn.Open();
                adapter.Fill(dt);
                if (dt.Rows.Count > 0)
                {
                    p.id = int.Parse(dt.Rows[0]["id"].ToString());
                    p.invoice_number = int.Parse(dt.Rows[0]["invoice_number"].ToString());
                    p.customer_id = int.Parse(dt.Rows[0]["customer_id"].ToString());
                    p.transaction_date = long.Parse(dt.Rows[0]["transaction_date"].ToString());
                    // p.due_date = long.Parse(dt.Rows[0]["due_date"].ToString());
                    p.terms = dt.Rows[0]["terms"].ToString();
                    p.nonTaxableSub = decimal.Parse(dt.Rows[0]["nonTaxableSub"].ToString());
                    p.taxableSub = decimal.Parse(dt.Rows[0]["taxableSub"].ToString());
                    p.tax = decimal.Parse(dt.Rows[0]["tax"].ToString());
                    p.grandTotal = decimal.Parse(dt.Rows[0]["grandTotal"].ToString());
                    p.amtPaid = decimal.Parse(dt.Rows[0]["amtPaid"].ToString());
                    p.datePaid = long.Parse(dt.Rows[0]["datePaid"].ToString());
                    p.status = int.Parse(dt.Rows[0]["status"].ToString());
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
            finally
            {
                conn.Close();
            }

            return p.status;
        }
        #endregion
        #region Find highest used invoice number
        public int MaxInvoiceNumber()
        {
            //Create SQL Connection
            SQLiteConnection conn = new SQLiteConnection(gParams.myConnection);

            //Create a DataTable
            DataTable dt = new DataTable();
            int i_num = 0;

            try
            {
                //Write SQL Query
                string sql = "SELECT MAX(invoice_number) from tbl_transactions where status != 1;";

                //SQL Command to Execute Query
                SQLiteCommand cmd = new SQLiteCommand(sql, conn);
                //SQLiteDataAdapter to hold the data from database
                SQLiteDataAdapter adapter = new SQLiteDataAdapter(cmd);

                //Open DAtabase Connection
                conn.Open();
                adapter.Fill(dt);
                if (dt.Rows.Count > 0)
                {
                    foreach (DataRow dr in dt.Rows)
                    {
                        i_num = int.Parse(dr[0].ToString());
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
            finally
            {
                conn.Close();
            }

            return i_num;
        }

        #endregion
        public void CopytransBLL(transactionsTable p, transactionsTable dest)
        {
            // p.id = dest.id;
            // p.invoice_number = dest.invoice_number;
            // p.customer_id = dest.customer_id;
            // p.transaction_date = dest.transaction_date;
            // p.due_date = dest.due_date;
            // p.terms = dest.terms;
            p.nonTaxableSub = dest.nonTaxableSub;
            p.taxableSub = dest.taxableSub;
            p.tax = dest.tax;
            // p.grandTotal = dest.grandTotal;
            p.amtPaid = dest.amtPaid;
            p.datePaid = dest.datePaid;
            p.status = dest.status;
        }
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
    }
*/
