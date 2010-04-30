namespace DataCenter
{
    partial class frmSettingsGeneral
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
            this.LabelHost = new System.Windows.Forms.Label();
            this.GroupDatabase = new System.Windows.Forms.GroupBox();
            this.TextPassword = new System.Windows.Forms.TextBox();
            this.LabelPassword = new System.Windows.Forms.Label();
            this.TextUserId = new System.Windows.Forms.TextBox();
            this.LabelUserId = new System.Windows.Forms.Label();
            this.ComboDatabase = new System.Windows.Forms.ComboBox();
            this.LabelDatabase = new System.Windows.Forms.Label();
            this.ComboHost = new System.Windows.Forms.ComboBox();
            this.GroupDatabase.SuspendLayout();
            this.SuspendLayout();
            // 
            // LabelHost
            // 
            this.LabelHost.AutoSize = true;
            this.LabelHost.Location = new System.Drawing.Point(20, 22);
            this.LabelHost.Name = "LabelHost";
            this.LabelHost.Size = new System.Drawing.Size(47, 13);
            this.LabelHost.TabIndex = 0;
            this.LabelHost.Text = "服务器:";
            // 
            // GroupDatabase
            // 
            this.GroupDatabase.Controls.Add(this.TextPassword);
            this.GroupDatabase.Controls.Add(this.LabelPassword);
            this.GroupDatabase.Controls.Add(this.TextUserId);
            this.GroupDatabase.Controls.Add(this.LabelUserId);
            this.GroupDatabase.Controls.Add(this.ComboDatabase);
            this.GroupDatabase.Controls.Add(this.LabelDatabase);
            this.GroupDatabase.Controls.Add(this.ComboHost);
            this.GroupDatabase.Controls.Add(this.LabelHost);
            this.GroupDatabase.Location = new System.Drawing.Point(12, 1);
            this.GroupDatabase.Name = "GroupDatabase";
            this.GroupDatabase.Size = new System.Drawing.Size(376, 77);
            this.GroupDatabase.TabIndex = 1;
            this.GroupDatabase.TabStop = false;
            this.GroupDatabase.Text = "数据库";
            // 
            // TextPassword
            // 
            this.TextPassword.Location = new System.Drawing.Point(265, 45);
            this.TextPassword.Name = "TextPassword";
            this.TextPassword.PasswordChar = '*';
            this.TextPassword.Size = new System.Drawing.Size(99, 21);
            this.TextPassword.TabIndex = 7;
            // 
            // LabelPassword
            // 
            this.LabelPassword.AutoSize = true;
            this.LabelPassword.Location = new System.Drawing.Point(212, 48);
            this.LabelPassword.Name = "LabelPassword";
            this.LabelPassword.Size = new System.Drawing.Size(35, 13);
            this.LabelPassword.TabIndex = 6;
            this.LabelPassword.Text = "密码:";
            // 
            // TextUserId
            // 
            this.TextUserId.Location = new System.Drawing.Point(73, 45);
            this.TextUserId.Name = "TextUserId";
            this.TextUserId.Size = new System.Drawing.Size(121, 21);
            this.TextUserId.TabIndex = 5;
            // 
            // LabelUserId
            // 
            this.LabelUserId.AutoSize = true;
            this.LabelUserId.Location = new System.Drawing.Point(20, 48);
            this.LabelUserId.Name = "LabelUserId";
            this.LabelUserId.Size = new System.Drawing.Size(47, 13);
            this.LabelUserId.TabIndex = 4;
            this.LabelUserId.Text = "用户名:";
            // 
            // ComboDatabase
            // 
            this.ComboDatabase.FormattingEnabled = true;
            this.ComboDatabase.Location = new System.Drawing.Point(265, 18);
            this.ComboDatabase.Name = "ComboDatabase";
            this.ComboDatabase.Size = new System.Drawing.Size(99, 21);
            this.ComboDatabase.TabIndex = 3;
            // 
            // LabelDatabase
            // 
            this.LabelDatabase.AutoSize = true;
            this.LabelDatabase.Location = new System.Drawing.Point(212, 22);
            this.LabelDatabase.Name = "LabelDatabase";
            this.LabelDatabase.Size = new System.Drawing.Size(47, 13);
            this.LabelDatabase.TabIndex = 2;
            this.LabelDatabase.Text = "数据库:";
            // 
            // ComboHost
            // 
            this.ComboHost.FormattingEnabled = true;
            this.ComboHost.Location = new System.Drawing.Point(73, 18);
            this.ComboHost.Name = "ComboHost";
            this.ComboHost.Size = new System.Drawing.Size(121, 21);
            this.ComboHost.TabIndex = 1;
            // 
            // frmSettingsGeneral
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(250)))), ((int)(((byte)(250)))), ((int)(((byte)(246)))));
            this.ClientSize = new System.Drawing.Size(394, 113);
            this.Controls.Add(this.GroupDatabase);
            this.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Name = "frmSettingsGeneral";
            this.Text = "常规";
            this.Load += new System.EventHandler(this.frmSettingsGeneral_Load);
            this.GroupDatabase.ResumeLayout(false);
            this.GroupDatabase.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Label LabelHost;
        private System.Windows.Forms.GroupBox GroupDatabase;
        private System.Windows.Forms.ComboBox ComboHost;
        private System.Windows.Forms.ComboBox ComboDatabase;
        private System.Windows.Forms.Label LabelDatabase;
        private System.Windows.Forms.Label LabelUserId;
        private System.Windows.Forms.TextBox TextPassword;
        private System.Windows.Forms.Label LabelPassword;
        private System.Windows.Forms.TextBox TextUserId;
    }
}