namespace DataCenter
{
    partial class frmSettingsMobile
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
            this.lblHeadHost = new System.Windows.Forms.Label();
            this.groupHead = new System.Windows.Forms.GroupBox();
            this.txtHeadPort = new System.Windows.Forms.TextBox();
            this.lblHeadVersion = new System.Windows.Forms.Label();
            this.lstHeadVersion = new System.Windows.Forms.ComboBox();
            this.lblHeadPort = new System.Windows.Forms.Label();
            this.lstHeadHost = new System.Windows.Forms.ComboBox();
            this.txtBackPort = new System.Windows.Forms.TextBox();
            this.lblBackVersion = new System.Windows.Forms.Label();
            this.lstBackVersion = new System.Windows.Forms.ComboBox();
            this.lblBackPort = new System.Windows.Forms.Label();
            this.groupBack = new System.Windows.Forms.GroupBox();
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.lblBackUrl = new System.Windows.Forms.Label();
            this.lstBackHost = new System.Windows.Forms.ComboBox();
            this.lblBackHost = new System.Windows.Forms.Label();
            this.groupHead.SuspendLayout();
            this.groupBack.SuspendLayout();
            this.SuspendLayout();
            // 
            // lblHeadHost
            // 
            this.lblHeadHost.AutoSize = true;
            this.lblHeadHost.Location = new System.Drawing.Point(20, 22);
            this.lblHeadHost.Name = "lblHeadHost";
            this.lblHeadHost.Size = new System.Drawing.Size(47, 13);
            this.lblHeadHost.TabIndex = 0;
            this.lblHeadHost.Text = "服务器:";
            // 
            // groupHead
            // 
            this.groupHead.Controls.Add(this.txtHeadPort);
            this.groupHead.Controls.Add(this.lblHeadVersion);
            this.groupHead.Controls.Add(this.lstHeadVersion);
            this.groupHead.Controls.Add(this.lblHeadPort);
            this.groupHead.Controls.Add(this.lstHeadHost);
            this.groupHead.Controls.Add(this.lblHeadHost);
            this.groupHead.Location = new System.Drawing.Point(12, 1);
            this.groupHead.Name = "groupHead";
            this.groupHead.Size = new System.Drawing.Size(376, 77);
            this.groupHead.TabIndex = 1;
            this.groupHead.TabStop = false;
            this.groupHead.Text = "正向同步";
            // 
            // txtHeadPort
            // 
            this.txtHeadPort.Location = new System.Drawing.Point(265, 18);
            this.txtHeadPort.Name = "txtHeadPort";
            this.txtHeadPort.Size = new System.Drawing.Size(99, 21);
            this.txtHeadPort.TabIndex = 7;
            // 
            // lblHeadVersion
            // 
            this.lblHeadVersion.AutoSize = true;
            this.lblHeadVersion.Location = new System.Drawing.Point(200, 48);
            this.lblHeadVersion.Name = "lblHeadVersion";
            this.lblHeadVersion.Size = new System.Drawing.Size(59, 13);
            this.lblHeadVersion.TabIndex = 6;
            this.lblHeadVersion.Text = "消息版本:";
            // 
            // lstHeadVersion
            // 
            this.lstHeadVersion.FormattingEnabled = true;
            this.lstHeadVersion.Location = new System.Drawing.Point(265, 45);
            this.lstHeadVersion.Name = "lstHeadVersion";
            this.lstHeadVersion.Size = new System.Drawing.Size(99, 21);
            this.lstHeadVersion.TabIndex = 3;
            // 
            // lblHeadPort
            // 
            this.lblHeadPort.AutoSize = true;
            this.lblHeadPort.Location = new System.Drawing.Point(212, 22);
            this.lblHeadPort.Name = "lblHeadPort";
            this.lblHeadPort.Size = new System.Drawing.Size(35, 13);
            this.lblHeadPort.TabIndex = 2;
            this.lblHeadPort.Text = "端口:";
            // 
            // lstHeadHost
            // 
            this.lstHeadHost.FormattingEnabled = true;
            this.lstHeadHost.Location = new System.Drawing.Point(73, 18);
            this.lstHeadHost.Name = "lstHeadHost";
            this.lstHeadHost.Size = new System.Drawing.Size(121, 21);
            this.lstHeadHost.TabIndex = 1;
            // 
            // txtBackPort
            // 
            this.txtBackPort.Location = new System.Drawing.Point(265, 18);
            this.txtBackPort.Name = "txtBackPort";
            this.txtBackPort.Size = new System.Drawing.Size(99, 21);
            this.txtBackPort.TabIndex = 7;
            // 
            // lblBackVersion
            // 
            this.lblBackVersion.AutoSize = true;
            this.lblBackVersion.Location = new System.Drawing.Point(200, 48);
            this.lblBackVersion.Name = "lblBackVersion";
            this.lblBackVersion.Size = new System.Drawing.Size(59, 13);
            this.lblBackVersion.TabIndex = 6;
            this.lblBackVersion.Text = "消息版本:";
            // 
            // lstBackVersion
            // 
            this.lstBackVersion.FormattingEnabled = true;
            this.lstBackVersion.Location = new System.Drawing.Point(265, 45);
            this.lstBackVersion.Name = "lstBackVersion";
            this.lstBackVersion.Size = new System.Drawing.Size(99, 21);
            this.lstBackVersion.TabIndex = 3;
            // 
            // lblBackPort
            // 
            this.lblBackPort.AutoSize = true;
            this.lblBackPort.Location = new System.Drawing.Point(212, 22);
            this.lblBackPort.Name = "lblBackPort";
            this.lblBackPort.Size = new System.Drawing.Size(35, 13);
            this.lblBackPort.TabIndex = 2;
            this.lblBackPort.Text = "端口:";
            // 
            // groupBack
            // 
            this.groupBack.Controls.Add(this.textBox1);
            this.groupBack.Controls.Add(this.lblBackUrl);
            this.groupBack.Controls.Add(this.txtBackPort);
            this.groupBack.Controls.Add(this.lblBackVersion);
            this.groupBack.Controls.Add(this.lstBackVersion);
            this.groupBack.Controls.Add(this.lblBackPort);
            this.groupBack.Controls.Add(this.lstBackHost);
            this.groupBack.Controls.Add(this.lblBackHost);
            this.groupBack.Location = new System.Drawing.Point(12, 81);
            this.groupBack.Name = "groupBack";
            this.groupBack.Size = new System.Drawing.Size(376, 104);
            this.groupBack.TabIndex = 8;
            this.groupBack.TabStop = false;
            this.groupBack.Text = "反向同步";
            // 
            // textBox1
            // 
            this.textBox1.Location = new System.Drawing.Point(73, 73);
            this.textBox1.Name = "textBox1";
            this.textBox1.Size = new System.Drawing.Size(292, 21);
            this.textBox1.TabIndex = 9;
            // 
            // lblBackUrl
            // 
            this.lblBackUrl.AutoSize = true;
            this.lblBackUrl.Location = new System.Drawing.Point(20, 76);
            this.lblBackUrl.Name = "lblBackUrl";
            this.lblBackUrl.Size = new System.Drawing.Size(30, 13);
            this.lblBackUrl.TabIndex = 8;
            this.lblBackUrl.Text = "URL:";
            // 
            // lstBackHost
            // 
            this.lstBackHost.FormattingEnabled = true;
            this.lstBackHost.Location = new System.Drawing.Point(73, 18);
            this.lstBackHost.Name = "lstBackHost";
            this.lstBackHost.Size = new System.Drawing.Size(121, 21);
            this.lstBackHost.TabIndex = 1;
            // 
            // lblBackHost
            // 
            this.lblBackHost.AutoSize = true;
            this.lblBackHost.Location = new System.Drawing.Point(20, 22);
            this.lblBackHost.Name = "lblBackHost";
            this.lblBackHost.Size = new System.Drawing.Size(47, 13);
            this.lblBackHost.TabIndex = 0;
            this.lblBackHost.Text = "服务器:";
            // 
            // frmSettingsMobile
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(250)))), ((int)(((byte)(250)))), ((int)(((byte)(246)))));
            this.ClientSize = new System.Drawing.Size(394, 201);
            this.Controls.Add(this.groupBack);
            this.Controls.Add(this.groupHead);
            this.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Name = "frmSettingsMobile";
            this.Text = "中国移动";
            this.groupHead.ResumeLayout(false);
            this.groupHead.PerformLayout();
            this.groupBack.ResumeLayout(false);
            this.groupBack.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Label lblHeadHost;
        private System.Windows.Forms.GroupBox groupHead;
        private System.Windows.Forms.ComboBox lstHeadHost;
        private System.Windows.Forms.ComboBox lstHeadVersion;
        private System.Windows.Forms.Label lblHeadPort;
        private System.Windows.Forms.TextBox txtHeadPort;
        private System.Windows.Forms.Label lblHeadVersion;
        private System.Windows.Forms.TextBox txtBackPort;
        private System.Windows.Forms.Label lblBackVersion;
        private System.Windows.Forms.ComboBox lstBackVersion;
        private System.Windows.Forms.Label lblBackPort;
        private System.Windows.Forms.GroupBox groupBack;
        private System.Windows.Forms.ComboBox lstBackHost;
        private System.Windows.Forms.Label lblBackHost;
        private System.Windows.Forms.TextBox textBox1;
        private System.Windows.Forms.Label lblBackUrl;
    }
}