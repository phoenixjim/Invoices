#ifndef _Invoices_profitloss_h_
#define _Invoices_profitloss_h_
struct ProfitLossWindow : WithProfitLossWindowLayout<TopWindow> {
public:
	ProfitLossWindow() { CtrlLayout(*this, "Profit / Loss"); }
	void Paint(Draw& w) {
        w.DrawRect(GetSize(), Color(204, 255, 255)); // <= enter your background color here
     }
};


#endif
