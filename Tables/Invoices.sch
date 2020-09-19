TABLE (tbl_customers)
	INT	(id) NOT_NULL PRIMARY_KEY AUTO_INCREMENT
	STRING (name, 150) NOT_NULL
	STRING (email, 150)
	STRING (contact, 150)
	STRING (address, 150)
	STRING (city, 50)
	STRING (state, 20)
	STRING (zip, 10)
	INT (taxable) NOT_NULL
END_TABLE

TABLE (tbl_transactions)
	INT (id)    NOT_NULL PRIMARY_KEY AUTO_INCREMENT
	INT (invoice_number)    NOT_NULL
	INT (customer_id)
	LONGRAW (transaction_date)
	STRING (terms, 20) NOT_NULL
	DOUBLE (nonTaxableSub)
	DOUBLE (taxableSub) 	
	DOUBLE (tax)
	DOUBLE (grandTotal)
	DOUBLE (amtPaid)
	LONGRAW (datePaid)
	INT (status)
END_TABLE

TABLE(tbl_products)
	INT (id)	NOT_NULL PRIMARY_KEY AUTO_INCREMENT
	STRING (name, 150)	NOT_NULL
	STRING (description, 200)
	LONGRAW (datePurchased)
	DOUBLE (cost)	NOT_NULL
	INT (invoice_number)	NOT_NULL
END_TABLE

TABLE (tbl_transaction_detail)
	INT (id)	NOT_NULL PRIMARY_KEY AUTO_INCREMENT
	STRING (product_name, 200) NOT_NULL
	STRING (description, 200)
	DOUBLE (price)	 NOT_NULL
	DOUBLE (qty)	 NOT_NULL
	DOUBLE (total)	 NOT_NULL
	INT64 (invoice_id)	 NOT_NULL
	INT (isTaxable)	 NOT_NULL
END_TABLE
