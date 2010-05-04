namespace DataCenter
{
    partial class frmProfilesMobile
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
            this.listView = new System.Windows.Forms.ListView();
            this.columnIndex = new System.Windows.Forms.ColumnHeader();
            this.columnCode = new System.Windows.Forms.ColumnHeader();
            this.columnName = new System.Windows.Forms.ColumnHeader();
            this.groupEdit = new System.Windows.Forms.GroupBox();
            this.txtName = new System.Windows.Forms.TextBox();
            this.labelMail = new System.Windows.Forms.Label();
            this.btnRemove = new System.Windows.Forms.Button();
            this.btnAdd = new System.Windows.Forms.Button();
            this.txtFeeCode = new System.Windows.Forms.TextBox();
            this.labelValue = new System.Windows.Forms.Label();
            this.lstFeeType = new System.Windows.Forms.ComboBox();
            this.labelType = new System.Windows.Forms.Label();
            this.lstFeeUser = new System.Windows.Forms.ComboBox();
            this.labelUser = new System.Windows.Forms.Label();
            this.txtCode = new System.Windows.Forms.TextBox();
            this.labelcode = new System.Windows.Forms.Label();
            this.labelIndex = new System.Windows.Forms.Label();
            this.lstIndex = new System.Windows.Forms.ComboBox();
            this.groupEdit.SuspendLayout();
            this.SuspendLayout();
            // 
            // listView
            // 
            this.listView.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnIndex,
            this.columnCode,
            this.columnName});
            this.listView.FullRowSelect = true;
            this.listView.HideSelection = false;
            this.listView.Location = new System.Drawing.Point(12, 12);
            this.listView.Name = "listView";
            this.listView.Size = new System.Drawing.Size(350, 97);
            this.listView.TabIndex = 0;
            this.listView.UseCompatibleStateImageBehavior = false;
            this.listView.View = System.Windows.Forms.View.Details;
            this.listView.SelectedIndexChanged += new System.EventHandler(this.listView_SelectedIndexChanged);
            // 
            // columnIndex
            // 
            this.columnIndex.Text = "索引";
            // 
            // columnCode
            // 
            this.columnCode.Text = "代码";
            this.columnCode.Width = 83;
            // 
            // columnName
            // 
            this.columnName.Text = "名称";
            this.columnName.Width = 96;
            // 
            // groupEdit
            // 
            this.groupEdit.Controls.Add(this.txtName);
            this.groupEdit.Controls.Add(this.labelMail);
            this.groupEdit.Controls.Add(this.btnRemove);
            this.groupEdit.Controls.Add(this.btnAdd);
            this.groupEdit.Controls.Add(this.txtFeeCode);
            this.groupEdit.Controls.Add(this.labelValue);
            this.groupEdit.Controls.Add(this.lstFeeType);
            this.groupEdit.Controls.Add(this.labelType);
            this.groupEdit.Controls.Add(this.lstFeeUser);
            this.groupEdit.Controls.Add(this.labelUser);
            this.groupEdit.Controls.Add(this.txtCode);
            this.groupEdit.Controls.Add(this.labelcode);
            this.groupEdit.Controls.Add(this.labelIndex);
            this.groupEdit.Controls.Add(this.lstIndex);
            this.groupEdit.Location = new System.Drawing.Point(12, 116);
            this.groupEdit.Name = "groupEdit";
            this.groupEdit.Size = new System.Drawing.Size(350, 191);
            this.groupEdit.TabIndex = 1;
            this.groupEdit.TabStop = false;
            this.groupEdit.Text = "编辑";
            // 
            // txtName
            // 
            this.txtName.Location = new System.Drawing.Point(109, 160);
            this.txtName.Name = "txtName";
            this.txtName.Size = new System.Drawing.Size(231, 21);
            this.txtName.TabIndex = 13;
            // 
            // labelMail
            // 
            this.labelMail.AutoSize = true;
            this.labelMail.Location = new System.Drawing.Point(20, 163);
            this.labelMail.Name = "labelMail";
            this.labelMail.Size = new System.Drawing.Size(59, 13);
            this.labelMail.TabIndex = 12;
            this.labelMail.Text = "业务名称:";
            // 
            // btnRemove
            // 
            this.btnRemove.Enabled = false;
            this.btnRemove.Location = new System.Drawing.Point(256, 47);
            this.btnRemove.Name = "btnRemove";
            this.btnRemove.Size = new System.Drawing.Size(75, 23);
            this.btnRemove.TabIndex = 11;
            this.btnRemove.Text = "移除";
            this.btnRemove.UseVisualStyleBackColor = true;
            this.btnRemove.Click += new System.EventHandler(this.btnRemove_Click);
            // 
            // btnAdd
            // 
            this.btnAdd.Location = new System.Drawing.Point(256, 20);
            this.btnAdd.Name = "btnAdd";
            this.btnAdd.Size = new System.Drawing.Size(75, 23);
            this.btnAdd.TabIndex = 10;
            this.btnAdd.Text = "新增";
            this.btnAdd.UseVisualStyleBackColor = true;
            this.btnAdd.Click += new System.EventHandler(this.btnAdd_Click);
            // 
            // txtFeeCode
            // 
            this.txtFeeCode.Location = new System.Drawing.Point(109, 133);
            this.txtFeeCode.Name = "txtFeeCode";
            this.txtFeeCode.Size = new System.Drawing.Size(231, 21);
            this.txtFeeCode.TabIndex = 9;
            // 
            // labelValue
            // 
            this.labelValue.AutoSize = true;
            this.labelValue.Location = new System.Drawing.Point(20, 136);
            this.labelValue.Name = "labelValue";
            this.labelValue.Size = new System.Drawing.Size(59, 13);
            this.labelValue.TabIndex = 8;
            this.labelValue.Text = "资费代码:";
            // 
            // lstFeeType
            // 
            this.lstFeeType.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.lstFeeType.FormattingEnabled = true;
            this.lstFeeType.Items.AddRange(new object[] {
            "免费",
            "按条收费",
            "包月"});
            this.lstFeeType.Location = new System.Drawing.Point(109, 106);
            this.lstFeeType.Name = "lstFeeType";
            this.lstFeeType.Size = new System.Drawing.Size(231, 21);
            this.lstFeeType.TabIndex = 7;
            // 
            // labelType
            // 
            this.labelType.AutoSize = true;
            this.labelType.Location = new System.Drawing.Point(20, 109);
            this.labelType.Name = "labelType";
            this.labelType.Size = new System.Drawing.Size(59, 13);
            this.labelType.TabIndex = 6;
            this.labelType.Text = "计费类型:";
            // 
            // lstFeeUser
            // 
            this.lstFeeUser.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.lstFeeUser.FormattingEnabled = true;
            this.lstFeeUser.Items.AddRange(new object[] {
            "对目的终端计费",
            "对源终端计费",
            "对SP计费",
            "无效"});
            this.lstFeeUser.Location = new System.Drawing.Point(109, 79);
            this.lstFeeUser.Name = "lstFeeUser";
            this.lstFeeUser.Size = new System.Drawing.Size(231, 21);
            this.lstFeeUser.TabIndex = 5;
            // 
            // labelUser
            // 
            this.labelUser.AutoSize = true;
            this.labelUser.Location = new System.Drawing.Point(20, 82);
            this.labelUser.Name = "labelUser";
            this.labelUser.Size = new System.Drawing.Size(83, 13);
            this.labelUser.TabIndex = 4;
            this.labelUser.Text = "计费用户类型:";
            // 
            // txtCode
            // 
            this.txtCode.Location = new System.Drawing.Point(109, 50);
            this.txtCode.Name = "txtCode";
            this.txtCode.Size = new System.Drawing.Size(120, 21);
            this.txtCode.TabIndex = 3;
            // 
            // labelcode
            // 
            this.labelcode.AutoSize = true;
            this.labelcode.Location = new System.Drawing.Point(20, 53);
            this.labelcode.Name = "labelcode";
            this.labelcode.Size = new System.Drawing.Size(59, 13);
            this.labelcode.TabIndex = 2;
            this.labelcode.Text = "业务代码:";
            // 
            // labelIndex
            // 
            this.labelIndex.AutoSize = true;
            this.labelIndex.Location = new System.Drawing.Point(20, 26);
            this.labelIndex.Name = "labelIndex";
            this.labelIndex.Size = new System.Drawing.Size(35, 13);
            this.labelIndex.TabIndex = 1;
            this.labelIndex.Text = "索引:";
            // 
            // lstIndex
            // 
            this.lstIndex.FormattingEnabled = true;
            this.lstIndex.Location = new System.Drawing.Point(109, 23);
            this.lstIndex.Name = "lstIndex";
            this.lstIndex.Size = new System.Drawing.Size(120, 21);
            this.lstIndex.TabIndex = 0;
            // 
            // frmProfilesMobile
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(250)))), ((int)(((byte)(250)))), ((int)(((byte)(246)))));
            this.ClientSize = new System.Drawing.Size(370, 313);
            this.Controls.Add(this.groupEdit);
            this.Controls.Add(this.listView);
            this.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Name = "frmProfilesMobile";
            this.Text = "业务定制";
            this.Load += new System.EventHandler(this.frmProfilesMobile_Load);
            this.groupEdit.ResumeLayout(false);
            this.groupEdit.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.ListView listView;
        private System.Windows.Forms.ColumnHeader columnIndex;
        private System.Windows.Forms.ColumnHeader columnCode;
        private System.Windows.Forms.ColumnHeader columnName;
        private System.Windows.Forms.GroupBox groupEdit;
        private System.Windows.Forms.Label labelcode;
        private System.Windows.Forms.Label labelIndex;
        private System.Windows.Forms.ComboBox lstIndex;
        private System.Windows.Forms.Label labelType;
        private System.Windows.Forms.ComboBox lstFeeUser;
        private System.Windows.Forms.Label labelUser;
        private System.Windows.Forms.TextBox txtCode;
        private System.Windows.Forms.ComboBox lstFeeType;
        private System.Windows.Forms.TextBox txtFeeCode;
        private System.Windows.Forms.Label labelValue;
        private System.Windows.Forms.Button btnRemove;
        private System.Windows.Forms.Button btnAdd;
        private System.Windows.Forms.TextBox txtName;
        private System.Windows.Forms.Label labelMail;
    }
}