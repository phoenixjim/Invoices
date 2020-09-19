#ifndef _Invoices_incomebycustomer_h_
#define _Invoices_incomebycustomer_h_
struct IncomeByCustomerWindow : WithIncomeByCustomerWindowLayout<TopWindow> {
public:
	IncomeByCustomerWindow() { CtrlLayout(*this, "Income by Customer"); }
	void Paint(Draw& w) {
        w.DrawRect(GetSize(), Color(204, 255, 255)); // <= enter your background color here
     }
};


#endif
