#pragma once

namespace XFilesHack {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for FormUpdate
	/// </summary>
	public ref class FormUpdate : public System::Windows::Forms::Form
	{
	public:
		FormUpdate(void)
		{
			InitializeComponent();
			InitializeMyTimer();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~FormUpdate()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Label^  label1;
	protected: 
	private: System::Windows::Forms::ProgressBar^  progressBar1;
	private: System::Windows::Forms::Timer^  timer1;
	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::Label^  label2;
	private: System::ComponentModel::IContainer^  components;

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
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->progressBar1 = (gcnew System::Windows::Forms::ProgressBar());
			this->timer1 = (gcnew System::Windows::Forms::Timer(this->components));
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->groupBox1->SuspendLayout();
			this->SuspendLayout();
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Font = (gcnew System::Drawing::Font(L"Calibri", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label1->Location = System::Drawing::Point(6, 16);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(108, 19);
			this->label1->TabIndex = 3;
			this->label1->Text = L"Updating DLL...";
			this->label1->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// progressBar1
			// 
			this->progressBar1->Location = System::Drawing::Point(10, 38);
			this->progressBar1->Name = L"progressBar1";
			this->progressBar1->Size = System::Drawing::Size(370, 26);
			this->progressBar1->TabIndex = 2;
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->label2);
			this->groupBox1->Controls->Add(this->label1);
			this->groupBox1->Controls->Add(this->progressBar1);
			this->groupBox1->Location = System::Drawing::Point(8, 2);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(386, 91);
			this->groupBox1->TabIndex = 4;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(7, 67);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(0, 13);
			this->label2->TabIndex = 4;
			// 
			// FormUpdate
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(406, 105);
			this->ControlBox = false;
			this->Controls->Add(this->groupBox1);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->Name = L"FormUpdate";
			this->Text = L"DLL Updater";
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			this->ResumeLayout(false);

		}

		 // Call this method from the constructor of the form. 
   void InitializeMyTimer()
   {
	  this->timer1->Interval = 100;

      // Connect the Tick event of the timer to its event handler.
      this->timer1->Tick += gcnew EventHandler( this, &FormUpdate::UpdateFormLayout );

      // Start the timer.
      this->timer1->Start();
   }

   void UpdateFormLayout(Object^ /*sender*/, EventArgs^ /*e*/)
   {
      this->progressBar1->Value = GetProgressBarUpdateValue();
	   this->label2->Text = gcnew String(GetLabel2UpdaterText());
	   if(this->progressBar1->Value == 100)this->Close();
   }
	
#pragma endregion
};
}
