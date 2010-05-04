namespace DataCenter
{
    partial class frmProfilesSecure
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.ListView = new System.Windows.Forms.ListView();
            this.ColumnHost = new System.Windows.Forms.ColumnHeader();
            this.ColumnDate = new System.Windows.Forms.ColumnHeader();
            this.groupEdit = new System.Windows.Forms.GroupBox();
            this.TextDesc = new System.Windows.Forms.TextBox();
            this.LabelDesc = new System.Windows.Forms.Label();
            this.LabelHost = new System.Windows.Forms.Label();
            this.ListHost = new System.Windows.Forms.ComboBox();
            this.ButtonRemove = new System.Windows.Forms.Button();
            this.ButtonAdd = new System.Windows.Forms.Button();
            this.LabelAction = new System.Windows.Forms.Label();
            this.ListAction = new System.Windows.Forms.ComboBox();
            this.groupEdit.SuspendLayout();
            this.SuspendLayout();
            // 
            // ListView
            // 
            this.ListView.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.ColumnHost,
            this.ColumnDate});
            this.ListView.FullRowSelect = true;
            this.ListView.HideSelection = false;
            this.ListView.Location = new System.Drawing.Point(12, 12);
            this.ListView.Name = "ListView";
            this.ListView.Size = new System.Drawing.Size(350, 134);
            this.ListView.TabIndex = 0;
            this.ListView.UseCompatibleStateImageBehavior = false;
            this.ListView.View = System.Windows.Forms.View.Details;
            this.ListView.SelectedIndexChanged += new System.EventHandler(this.ListView_SelectedIndexChanged);
            // 
            // ColumnHost
            // 
            this.ColumnHost.Text = "主机";
            this.ColumnHost.Width = 143;
            // 
            // ColumnDate
            // 
            this.ColumnDate.Text = "日期";
            this.ColumnDate.Width = 107;
            // 
            // groupEdit
            // 
            this.groupEdit.Controls.Add(this.TextDesc);
            this.groupEdit.Controls.Add(this.LabelDesc);
            this.groupEdit.Controls.Add(this.LabelHost);
            this.groupEdit.Controls.Add(this.ListHost);
            this.groupEdit.Location = new System.Drawing.Point(12, 152);
            this.groupEdit.Name = "groupEdit";
            this.groupEdit.Size = new System.Drawing.Size(350, 114);
            this.groupEdit.TabIndex = 1;
            this.groupEdit.TabStop = false;
            this.groupEdit.Text = "编辑";
            // 
            // TextDesc
            // 
            this.TextDesc.Location = new System.Drawing.Point(82, 53);
            this.TextDesc.Multiline = true;
            this.TextDesc.Name = "TextDesc";
            this.TextDesc.Size = new System.Drawing.Size(262, 52);
            this.TextDesc.TabIndex = 3;
            // 
            // LabelDesc
            // 
            this.LabelDesc.AutoSize = true;
            this.LabelDesc.Location = new System.Drawing.Point(20, 56);
            this.LabelDesc.Name = "LabelDesc";
            this.LabelDesc.Size = new System.Drawing.Size(35, 13);
            this.LabelDesc.TabIndex = 2;
            this.LabelDesc.Text = "说明:";
            // 
            // LabelHost
            // 
            this.LabelHost.AutoSize = true;
            this.LabelHost.Location = new System.Drawing.Point(20, 26);
            this.LabelHost.Name = "LabelHost";
            this.LabelHost.Size = new System.Drawing.Size(35, 13);
            this.LabelHost.TabIndex = 1;
            this.LabelHost.Text = "地址:";
            // 
            // ListHost
            // 
            this.ListHost.FormattingEnabled = true;
            this.ListHost.Location = new System.Drawing.Point(82, 26);
            this.ListHost.Name = "ListHost";
            this.ListHost.Size = new System.Drawing.Size(144, 21);
            this.ListHost.TabIndex = 0;
            // 
            // ButtonRemove
            // 
            this.ButtonRemove.Enabled = false;
            this.ButtonRemove.Location = new System.Drawing.Point(290, 278);
            this.ButtonRemove.Name = "ButtonRemove";
            this.ButtonRemove.Size = new System.Drawing.Size(64, 23);
            this.ButtonRemove.TabIndex = 11;
            this.ButtonRemove.Text = "移除";
            this.ButtonRemove.UseVisualStyleBackColor = true;
            this.ButtonRemove.Click += new System.EventHandler(this.ButtonRemove_Click);
            // 
            // ButtonAdd
            // 
            this.ButtonAdd.Location = new System.Drawing.Point(220, 278);
            this.ButtonAdd.Name = "ButtonAdd";
            this.ButtonAdd.Size = new System.Drawing.Size(64, 23);
            this.ButtonAdd.TabIndex = 10;
            this.ButtonAdd.Text = "新增";
            this.ButtonAdd.UseVisualStyleBackColor = true;
            this.ButtonAdd.Click += new System.EventHandler(this.ButtonAdd_Click);
            // 
            // LabelAction
            // 
            this.LabelAction.AutoSize = true;
            this.LabelAction.Location = new System.Drawing.Point(32, 281);
            this.LabelAction.Name = "LabelAction";
            this.LabelAction.Size = new System.Drawing.Size(59, 13);
            this.LabelAction.TabIndex = 13;
            this.LabelAction.Text = "默认策略:";
            // 
            // ListAction
            // 
            this.ListAction.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.ListAction.FormattingEnabled = true;
            this.ListAction.Items.AddRange(new object[] {
            "接受",
            "拒绝"});
            this.ListAction.Location = new System.Drawing.Point(94, 278);
            this.ListAction.Name = "ListAction";
            this.ListAction.Size = new System.Drawing.Size(112, 21);
            this.ListAction.TabIndex = 12;
            // 
            // frmProfilesSecure
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(250)))), ((int)(((byte)(250)))), ((int)(((byte)(246)))));
            this.ClientSize = new System.Drawing.Size(370, 313);
            this.Controls.Add(this.ButtonRemove);
            this.Controls.Add(this.LabelAction);
            this.Controls.Add(this.ButtonAdd);
            this.Controls.Add(this.groupEdit);
            this.Controls.Add(this.ListAction);
            this.Controls.Add(this.ListView);
            this.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Name = "frmProfilesSecure";
            this.Text = "网络安全";
            this.Load += new System.EventHandler(this.frmProfilesSecure_Load);
            this.groupEdit.ResumeLayout(false);
            this.groupEdit.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ListView ListView;
        private System.Windows.Forms.ColumnHeader ColumnHost;
        private System.Windows.Forms.ColumnHeader ColumnDate;
        private System.Windows.Forms.GroupBox groupEdit;
        private System.Windows.Forms.Label LabelDesc;
        private System.Windows.Forms.Label LabelHost;
        private System.Windows.Forms.ComboBox ListHost;
        private System.Windows.Forms.TextBox TextDesc;
        private System.Windows.Forms.Button ButtonRemove;
        private System.Windows.Forms.Button ButtonAdd;
        private System.Windows.Forms.Label LabelAction;
        private System.Windows.Forms.ComboBox ListAction;
    }
}