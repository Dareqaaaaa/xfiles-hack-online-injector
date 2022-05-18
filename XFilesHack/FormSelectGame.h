#pragma once

namespace XFilesHack {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for FormSelectGame
	/// </summary>
	public ref class FormSelectGame : public System::Windows::Forms::Form
	{
	public:
		FormSelectGame(void)
		{
			InitializeComponent();
			AddItemToList();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~FormSelectGame()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::GroupBox^  groupBox1;
	protected: 
	private: System::Windows::Forms::ListBox^  listBox1;
	private: System::Windows::Forms::Button^  button1;

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
			this->listBox1 = (gcnew System::Windows::Forms::ListBox());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->groupBox1->SuspendLayout();
			this->SuspendLayout();
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->listBox1);
			this->groupBox1->Location = System::Drawing::Point(5, 6);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(275, 94);
			this->groupBox1->TabIndex = 0;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"Select Game";
			// 
			// listBox1
			// 
			this->listBox1->FormattingEnabled = true;
			this->listBox1->Location = System::Drawing::Point(7, 19);
			this->listBox1->Name = L"listBox1";
			this->listBox1->Size = System::Drawing::Size(260, 69);
			this->listBox1->TabIndex = 0;
			// 
			// button1
			// 
			this->button1->Location = System::Drawing::Point(104, 100);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(75, 24);
			this->button1->TabIndex = 1;
			this->button1->Text = L"Select";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &FormSelectGame::button1_Click);
			// 
			// FormSelectGame
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(284, 131);
			this->Controls->Add(this->button1);
			this->Controls->Add(this->groupBox1);
			this->ControlBox = false;
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->Name = L"FormSelectGame";
			this->Text = L"Select Game to Play";
			this->groupBox1->ResumeLayout(false);
			this->ResumeLayout(false);

		}

		void AddItemToList(void)
		{
			// Shutdown the painting of the ListBox as items are added.
			listBox1->BeginUpdate();
			for(int i = 0; i < GetNumberOfGames(); i++)
			{
				listBox1->Items->Add(gcnew String(GetGameNamebyIdx(i)));
			}
			listBox1->EndUpdate();
		}
#pragma endregion
	private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e) {
				 int selected_item = listBox1->SelectedIndex; 
				 SetGameCode(GetGameCodebyIdx(selected_item));
				 SetGameSelectedState(true);
				 this->Close();
			 }
	};
}
