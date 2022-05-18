#pragma once

namespace XFilesHack {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Runtime::InteropServices;

	/// <summary>
	/// Summary for FormLogin
	/// </summary>
	public ref class FormLogin : public System::Windows::Forms::Form
	{
	public:
		FormLogin(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~FormLogin()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::GroupBox^  groupBox1;
	protected: 
	private: System::Windows::Forms::Button^  buttoncancel;
	private: System::Windows::Forms::Button^  buttonlogin;

	private: System::Windows::Forms::TextBox^  textBox1;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::TextBox^  textBox2;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->buttoncancel = (gcnew System::Windows::Forms::Button());
			this->buttonlogin = (gcnew System::Windows::Forms::Button());
			this->textBox1 = (gcnew System::Windows::Forms::TextBox());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->textBox2 = (gcnew System::Windows::Forms::TextBox());
			this->groupBox1->SuspendLayout();
			this->SuspendLayout();
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->textBox2);
			this->groupBox1->Controls->Add(this->buttoncancel);
			this->groupBox1->Controls->Add(this->buttonlogin);
			this->groupBox1->Controls->Add(this->textBox1);
			this->groupBox1->Controls->Add(this->label2);
			this->groupBox1->Controls->Add(this->label1);
			this->groupBox1->Location = System::Drawing::Point(12, 12);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(260, 98);
			this->groupBox1->TabIndex = 0;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"Login Information";
			// 
			// buttoncancel
			// 
			this->buttoncancel->Location = System::Drawing::Point(182, 66);
			this->buttoncancel->Name = L"buttoncancel";
			this->buttoncancel->Size = System::Drawing::Size(72, 26);
			this->buttoncancel->TabIndex = 5;
			this->buttoncancel->Text = L"Cancel";
			this->buttoncancel->UseVisualStyleBackColor = true;
			this->buttoncancel->Click += gcnew System::EventHandler(this, &FormLogin::buttoncancel_Click);
			// 
			// buttonlogin
			// 
			this->buttonlogin->Location = System::Drawing::Point(104, 66);
			this->buttonlogin->Name = L"buttonlogin";
			this->buttonlogin->Size = System::Drawing::Size(72, 26);
			this->buttonlogin->TabIndex = 4;
			this->buttonlogin->Text = L"Login";
			this->buttonlogin->UseVisualStyleBackColor = true;
			this->buttonlogin->Click += gcnew System::EventHandler(this, &FormLogin::buttonlogin_Click);
			// 
			// textBox1
			// 
			this->textBox1->Location = System::Drawing::Point(104, 17);
			this->textBox1->Name = L"textBox1";
			this->textBox1->Size = System::Drawing::Size(150, 20);
			this->textBox1->TabIndex = 2;
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(40, 45);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(65, 13);
			this->label2->TabIndex = 1;
			this->label2->Text = L"Kata Sandi :";
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(12, 20);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(93, 13);
			this->label1->TabIndex = 0;
			this->label1->Text = L"Nama Pengguna :";
			// 
			// textBox2
			// 
			this->textBox2->Location = System::Drawing::Point(104, 43);
			this->textBox2->Name = L"textBox2";
			this->textBox2->Size = System::Drawing::Size(150, 20);
			this->textBox2->TabIndex = 6;
			this->textBox2->PasswordChar = '*';
			// 
			// FormLogin
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(284, 134);
			this->Controls->Add(this->groupBox1);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"FormLogin";
			this->ShowIcon = false;
			this->Text = L"XFilesHack Member Login";
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void buttoncancel_Click(System::Object^  sender, System::EventArgs^  e) {
				 SetLoggedInState(true);
				 this->Close();
			 }
private: System::Void buttonlogin_Click(System::Object^  sender, System::EventArgs^  e) {
			 const char* username = (const char*)(void*)Marshal::StringToHGlobalAnsi(this->textBox1->Text);
			 const char* password = (const char*)(void*)Marshal::StringToHGlobalAnsi(this->textBox2->Text);
			 char LoginInformation[200] = {0};
			 sprintf(LoginInformation,"user=%s&pass=%s",username,password);
			 SetMemberLoginInformation(LoginInformation);
			 SetLoggedInState(true);
			 this->Close();
		 }
};
}
