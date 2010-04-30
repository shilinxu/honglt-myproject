namespace DataCenter
{
    partial class frmProvision
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
            this.PropertyGrid = new System.Windows.Forms.PropertyGrid();
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.listView1 = new System.Windows.Forms.ListView();
            this.ColumnTime = new System.Windows.Forms.ColumnHeader();
            this.ColumnCaller = new System.Windows.Forms.ColumnHeader();
            this.ColumnCalled = new System.Windows.Forms.ColumnHeader();
            this.ColumnContent = new System.Windows.Forms.ColumnHeader();
            this.ColumnService = new System.Windows.Forms.ColumnHeader();
            this.ColumnError = new System.Windows.Forms.ColumnHeader();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            this.SuspendLayout();
            // 
            // PropertyGrid
            // 
            this.PropertyGrid.Dock = System.Windows.Forms.DockStyle.Fill;
            this.PropertyGrid.Location = new System.Drawing.Point(0, 0);
            this.PropertyGrid.Name = "PropertyGrid";
            this.PropertyGrid.Size = new System.Drawing.Size(183, 209);
            this.PropertyGrid.TabIndex = 0;
            this.PropertyGrid.ToolbarVisible = false;
            // 
            // splitContainer1
            // 
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.FixedPanel = System.Windows.Forms.FixedPanel.Panel1;
            this.splitContainer1.Location = new System.Drawing.Point(0, 0);
            this.splitContainer1.Name = "splitContainer1";
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.PropertyGrid);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.listView1);
            this.splitContainer1.Size = new System.Drawing.Size(407, 209);
            this.splitContainer1.SplitterDistance = 183;
            this.splitContainer1.TabIndex = 1;
            // 
            // listView1
            // 
            this.listView1.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.ColumnTime,
            this.ColumnCaller,
            this.ColumnCalled,
            this.ColumnContent,
            this.ColumnService,
            this.ColumnError});
            this.listView1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.listView1.Location = new System.Drawing.Point(0, 0);
            this.listView1.Name = "listView1";
            this.listView1.Size = new System.Drawing.Size(220, 209);
            this.listView1.TabIndex = 0;
            this.listView1.UseCompatibleStateImageBehavior = false;
            this.listView1.View = System.Windows.Forms.View.Details;
            // 
            // ColumnTime
            // 
            this.ColumnTime.Text = "发生时间";
            this.ColumnTime.Width = 120;
            // 
            // ColumnCaller
            // 
            this.ColumnCaller.Text = "主叫号码";
            this.ColumnCaller.Width = 108;
            // 
            // ColumnCalled
            // 
            this.ColumnCalled.Text = "被叫号码";
            this.ColumnCalled.Width = 108;
            // 
            // ColumnContent
            // 
            this.ColumnContent.Text = "事件内容";
            this.ColumnContent.Width = 108;
            // 
            // ColumnService
            // 
            this.ColumnService.Text = "业务名称";
            this.ColumnService.Width = 120;
            // 
            // ColumnError
            // 
            this.ColumnError.Text = "错误代码";
            this.ColumnError.Width = 68;
            // 
            // frmProvision
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(250)))), ((int)(((byte)(250)))), ((int)(((byte)(246)))));
            this.ClientSize = new System.Drawing.Size(407, 209);
            this.Controls.Add(this.splitContainer1);
            this.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Name = "frmProvision";
            this.Text = "同步订购";
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.frmProvision_FormClosed);
            this.Load += new System.EventHandler(this.frmProvision_Load);
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel2.ResumeLayout(false);
            this.splitContainer1.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.PropertyGrid PropertyGrid;
        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.ListView listView1;
        private System.Windows.Forms.ColumnHeader ColumnTime;
        private System.Windows.Forms.ColumnHeader ColumnCaller;
        private System.Windows.Forms.ColumnHeader ColumnCalled;
        private System.Windows.Forms.ColumnHeader ColumnContent;
        private System.Windows.Forms.ColumnHeader ColumnService;
        private System.Windows.Forms.ColumnHeader ColumnError;


    }
}