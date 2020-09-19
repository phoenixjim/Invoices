#ifndef _Invoices_taxbycustomer_h_
#define _Invoices_taxbycustomer_h_
struct TaxByCustomerWindow : WithTaxByCustomerWindowLayout<TopWindow> {
public:
	TaxByCustomerWindow() { CtrlLayout(*this, "Income / Sales Tax by Customer"); }
	void Paint(Draw& w) {
        w.DrawRect(GetSize(), Color(204, 255, 255)); // <= enter your background color here
     }
};


#endif
