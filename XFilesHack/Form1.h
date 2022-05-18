#pragma once

namespace XFilesHack {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Threading;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for Form1
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{
		
	public:
		Form1(void)
		{
			InitializeComponent();
			InitializeMyTimer();
			InitializeInjectionCore("");
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~Form1()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::ProgressBar^  progressBar1;
	private: System::Windows::Forms::Label^  label1;

	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::Timer^  timer1;
	private: System::Windows::Forms::WebBrowser^  webBrowser1;





	private: System::ComponentModel::IContainer^  components;
	protected: 

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			this->progressBar1 = (gcnew System::Windows::Forms::ProgressBar());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->webBrowser1 = (gcnew System::Windows::Forms::WebBrowser());
			this->timer1 = (gcnew System::Windows::Forms::Timer(this->components));
			this->groupBox1->SuspendLayout();
			this->SuspendLayout();
			// 
			// progressBar1
			// 
			this->progressBar1->Location = System::Drawing::Point(11, 25);
			this->progressBar1->Name = L"progressBar1";
			this->progressBar1->Size = System::Drawing::Size(552, 17);
			this->progressBar1->TabIndex = 0;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Font = (gcnew System::Drawing::Font(L"Calibri", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label1->Location = System::Drawing::Point(7, 3);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(111, 19);
			this->label1->TabIndex = 1;
			this->label1->Text = L"Harap Tunggu...";
			this->label1->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->webBrowser1);
			this->groupBox1->Location = System::Drawing::Point(12, 48);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(552, 231);
			this->groupBox1->TabIndex = 3;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"Informasi Server";
			// 
			// webBrowser1
			// 
			this->webBrowser1->Location = System::Drawing::Point(6, 14);
			this->webBrowser1->MinimumSize = System::Drawing::Size(20, 20);
			this->webBrowser1->Name = L"webBrowser1";
			this->webBrowser1->Size = System::Drawing::Size(540, 211);
			this->webBrowser1->TabIndex = 0;
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(576, 291);
			this->Controls->Add(this->groupBox1);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->progressBar1);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"Form1";
			this->ShowIcon = false;
			this->Text = L"XFilesHack Injector";
			this->groupBox1->ResumeLayout(false);
			this->ResumeLayout(false);
			this->PerformLayout();

		}


   // Call this method from the constructor of the form. 
   void InitializeMyTimer()
   {
	   this->webBrowser1->Navigate("http://vvip-enjoyers.net/vvip/");
	  this->timer1->Interval = 100;

      // Connect the Tick event of the timer to its event handler.
      this->timer1->Tick += gcnew EventHandler( this, &Form1::UpdateFormLayout );

      // Start the timer.
      this->timer1->Start();
   }

   void UpdateFormLayout(Object^ /*sender*/, EventArgs^ /*e*/)
   {
      this->progressBar1->Value = GetProgressBarMainValue();
      this->label1->Text = gcnew String(GetLabel1MainText());
	  if(GetFormLoginState()){
		    SetFormLoginState(false);
		    FormLogin^ LoginForm = gcnew FormLogin;
            LoginForm->ShowDialog( this );
	  }
	  if(GetFormSelectGameState()){
		    SetFormSelectGameState(false);
		    FormSelectGame^ SelectGameForm = gcnew FormSelectGame;
		    SelectGameForm->ShowDialog( this );
	  }
	  if(GetFormUpdateState()){
			SetFormUpdateState(false);
  			FormUpdate^ UpdateForm = gcnew FormUpdate;
			UpdateForm->ShowDialog( this );
	  }

	  if(GetFormMainState() == false)this->Close();
	  
   }

#pragma endregion
};
}



