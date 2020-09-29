TABLE_ (CUSTOMERS)
	INT_	(CUST_ID) NOT_NULL PRIMARY_KEY AUTO_INCREMENT
	STRING_ (CUSTNAME, 150) NOT_NULL
	STRING_ (EMAIL, 150)
	STRING_ (CONTACT, 150)
	STRING_ (ADDRESS, 150)
	STRING_ (CITY, 50)
	STRING_ (STATE, 20)
	STRING_ (ZIP, 10)
	INT_ (TAXABLE) NOT_NULL
END_TABLE

TABLE_ (INVOICES)
	INT_ (INVOICE_ID)    NOT_NULL PRIMARY_KEY AUTO_INCREMENT
	INT_ (INVOICENUMBER)    NOT_NULL
	INT_ (CUSTOMERID)
	DATE_ (TRANSACTIONDATE)
	STRING_ (TERMS, 20) NOT_NULL
	DOUBLE_ (NONTAXABLESUB)
	DOUBLE_ (TAXABLESUB) 	
	DOUBLE_ (TAX)
	DOUBLE_ (GRANDTOTAL)
	DOUBLE_ (AMTPAID)
	DATE_ (DATEPAID)
	INT_ (STATUS)
END_TABLE

TABLE_ (PRODUCTS)
	INT_ (PROD_ID)	NOT_NULL PRIMARY_KEY AUTO_INCREMENT
	STRING_ (PRODNAME, 150)	NOT_NULL
	STRING_ (PRODDESCRIPTION, 200)
	DATE_ (DATEPURCHASED)
	DOUBLE_ (COST)	NOT_NULL
	INT_ (INVOICEID)	NOT_NULL
END_TABLE

TABLE_ (LINEITEMS)
	INT_ (LINEITEM_ID)	NOT_NULL PRIMARY_KEY AUTO_INCREMENT
	STRING_ (PRODUCTNAME, 200) NOT_NULL
	STRING_ (DESCRIPTION, 200)
	DOUBLE_ (PRICE)	 NOT_NULL
	DOUBLE_ (QTY)	 NOT_NULL
	DOUBLE_ (TOTAL)	 NOT_NULL
	INT64_ (INVOICEIDNUMBER)	 NOT_NULL
	INT_ (ISTAXABLE)	 NOT_NULL
END_TABLE
