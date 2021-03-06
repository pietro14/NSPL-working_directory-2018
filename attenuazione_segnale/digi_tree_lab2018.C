#define digi_tree_cxx
#include "digi_tree_lab2018.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TH1F.h>

// open the file when running the macro:
//      > root -l 
//      Root > .L digi_tree_lab2017.C
//      Root > digi_tree * t =new digi_tree("xx") (to open file runxx_wave.root)
//      Root > t->Loop()  // Loop on all entries

void digi_tree::Loop()
{

   // definizione di alcuni parametri utili
   int time_range_low=0;  //limite inferiore della finestra temporale da considerare (NB: in bin di tempo, non in ns)
   int time_range_hi=1024;  //limite superiore della finestra temporale da considerare (NB: in bin di tempo, non in ns)
   bool draw_signal=true;  //flag per decidere se visualizzare segnale per segnale
   bool fit_signal=true;  //flag per decidere se effettuare o meno il fit per determinare il tempo di salita
   int fit_range_low=0;   //limite inferiore intervallo di fit
   int fit_range_hi=250;    //limite superiore intervallo di fit

   double integrale; 
   // fine definizione parametri

   
   //definire una canvas
   TCanvas * draw_canvas=new TCanvas("draw_canvas","draw_canvas");
   
   //definizione di un frame per visualizzare il segnale
   TH2F * h_signal_frame=new TH2F("","",(time_range_hi-time_range_low)/1000,time_range_low,time_range_hi,4096,0,4906);

   //definizione della funzione di fit per i tempi  
   //   TF1 landau_d("landau_d","[4]+[0]*TMath::Landau([3]+x,[1],[2])",fit_range_low,fit_range_hi);
   // landau_d.SetParameters(-1,0.7,5,100,4000);

   TF1 retta("retta","[0]",fit_range_low,fit_range_hi);
   retta.SetParameter(0,3800);


   
   //histogrammi delle cariche misurate
   TH1F* h_charge=new TH1F("h_charge","time; signal time [ns]; entries",100,50,100);

   //apertura del file dei dati nel formato root
   if (fChain == 0) return;
   Long64_t nentries = fChain->GetEntriesFast();
   cout<<"opening file "<<RootFile<<" with "<<nentries<< " entries"<<endl;
   
   // loop su tutti gli eventi
   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0){
	cout<<"problem with event "<<jentry<<endl;
	break;
      }      
      bool test=true;
      nb = fChain->GetEntry(jentry);   nbytes += nb;


      if(jentry%100==0) cout<< "    .... event "<<ev_no<< " jentry= "<<jentry<<endl;
      TGraph * h_signal = new TGraph();  //grafico per visualizzare il segnale

      integrale = 0;
      
      //canale 0
      //loop su tutti i bin del segnale
      for(int i=0; i<nbins-1; i++){	
	// qui dentro tutto quello che si vuole

	// ad esempio, riempire un grafico col segnale del canale 0
	h_signal->SetPoint(i,(float)data_bin_ch1->at(i),(float)data_value_ch1->at(i));

	//cout << data_value_ch1->size() <<" " << data_bin_ch1->size()  << endl;
		integrale = integrale + (float)data_value_ch1->at(i);

      }
     
      // fare un fit al segnale del canale 0
      retta.SetParameters(0,3800);
       h_signal->Fit("retta","Q"," ",fit_range_low,fit_range_hi);

    
      //integrale

      /*
      ROOT::Math::WrappedTF1 wf1(landau_d);
      ROOT::Math::GaussIntegrator ig;
      ig.SetFunction(wf1);
      ig.SetRelTolerance(0.001);
      */
      integrale = retta.GetParameter(0)*1024-integrale;
      h_charge->Fill(integrale);

      //cout << fixed << ev_no <<" "<<integrale << endl;
      
      
      

      //Disegnare il segnale del canale 0
      if(draw_signal){
	h_signal->SetMarkerStyle(20); 
	h_signal->SetMarkerSize(0.4);
	TString tit_string="event - ";
	tit_string+=ev_no;
	h_signal_frame->SetTitle(tit_string); 
	h_signal_frame->GetXaxis()->SetTitle("time bin");
	h_signal_frame->GetYaxis()->SetTitle("ADC counts");
	h_signal_frame->Draw();
	h_signal->Draw("P");
	draw_canvas->WaitPrimitive();
      }      

   } // fine loop sugli eventi



   
   TFile * histograms = new TFile("histograms.root", "RECREATE");
   
   
   h_charge->Write();
   
   histograms->Close();

}
