namespace DataCenter
{
    partial class frmSettingsSheet
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
            this.TableLayoutPanel = new System.Windows.Forms.TableLayoutPanel();
            this.PictureTile = new System.Windows.Forms.PictureBox();
            this.TreeView = new System.Windows.Forms.TreeView();
            this.PanelFoot = new System.Windows.Forms.Panel();
            this.ButtonCancel = new System.Windows.Forms.Button();
            this.ButtonOK = new System.Windows.Forms.Button();
            this.ButtonHelp = new System.Windows.Forms.Button();
            this.PanelBody = new System.Windows.Forms.Panel();
            this.TableLayoutPanel.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.PictureTile)).BeginInit();
            this.PanelFoot.SuspendLayout();
            this.SuspendLayout();
            // 
            // TableLayoutPanel
            // 
            this.TableLayoutPanel.ColumnCount = 2;
            this.TableLayoutPanel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle());
            this.TableLayoutPanel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.TableLayoutPanel.Controls.Add(this.PictureTile, 0, 0);
            this.TableLayoutPanel.Controls.Add(this.TreeView, 0, 1);
            this.TableLayoutPanel.Controls.Add(this.PanelFoot, 1, 2);
            this.TableLayoutPanel.Controls.Add(this.ButtonHelp, 0, 2);
            this.TableLayoutPanel.Controls.Add(this.PanelBody, 1, 1);
            this.TableLayoutPanel.Dock = System.Windows.Forms.DockStyle.Fill;
            this.TableLayoutPanel.Location = new System.Drawing.Point(0, 0);
            this.TableLayoutPanel.Name = "TableLayoutPanel";
            this.TableLayoutPanel.RowCount = 3;
            this.TableLayoutPanel.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 57F));
            this.TableLayoutPanel.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.TableLayoutPanel.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 30F));
            this.TableLayoutPanel.Size = new System.Drawing.Size(419, 303);
            this.TableLayoutPanel.TabIndex = 2;
            // 
            // PictureTile
            // 
            this.PictureTile.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.TableLayoutPanel.SetColumnSpan(this.PictureTile, 2);
            this.PictureTile.Dock = System.Windows.Forms.DockStyle.Fill;
            this.PictureTile.Image = global::DataCenter.Properties.Resources.Products;
            this.PictureTile.Location = new System.Drawing.Point(0, 0);
            this.PictureTile.Margin = new System.Windows.Forms.Padding(0);
            this.PictureTile.Name = "PictureTile";
            this.PictureTile.Size = new System.Drawing.Size(419, 57);
            this.PictureTile.TabIndex = 0;
            this.PictureTile.TabStop = false;
            // 
            // TreeView
            // 
            this.TreeView.Dock = System.Windows.Forms.DockStyle.Fill;
            this.TreeView.Location = new System.Drawing.Point(0, 60);
            this.TreeView.Margin = new System.Windows.Forms.Padding(0, 3, 3, 3);
            this.TreeView.Name = "TreeView";
            this.TreeView.ShowRootLines = false;
            this.TreeView.Size = new System.Drawing.Size(131, 210);
            this.TreeView.TabIndex = 1;
            this.TreeView.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.TreeView_AfterSelect);
            // 
            // PanelFoot
            // 
            this.PanelFoot.Controls.Add(this.ButtonCancel);
            this.PanelFoot.Controls.Add(this.ButtonOK);
            this.PanelFoot.Dock = System.Windows.Forms.DockStyle.Fill;
            this.PanelFoot.Location = new System.Drawing.Point(137, 276);
            this.PanelFoot.Name = "PanelFoot";
            this.PanelFoot.Size = new System.Drawing.Size(279, 24);
            this.PanelFoot.TabIndex = 4;
            // 
            // ButtonCancel
            // 
            this.ButtonCancel.Anchor = System.Windows.Forms.AnchorStyles.Right;
            this.ButtonCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.ButtonCancel.Location = new System.Drawing.Point(194, 0);
            this.ButtonCancel.Name = "ButtonCancel";
            this.ButtonCancel.Size = new System.Drawing.Size(75, 25);
            this.ButtonCancel.TabIndex = 3;
            this.ButtonCancel.Text = "取消";
            this.ButtonCancel.UseVisualStyleBackColor = true;
            this.ButtonCancel.Click += new System.EventHandler(this.OnCancel);
            // 
            // ButtonOK
            // 
            this.ButtonOK.Anchor = System.Windows.Forms.AnchorStyles.Right;
            this.ButtonOK.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.ButtonOK.Location = new System.Drawing.Point(104, 0);
            this.ButtonOK.Name = "ButtonOK";
            this.ButtonOK.Size = new System.Drawing.Size(75, 25);
            this.ButtonOK.TabIndex = 2;
            this.ButtonOK.Text = "确定";
            this.ButtonOK.UseVisualStyleBackColor = true;
            this.ButtonOK.Click += new System.EventHandler(this.OnOK);
            // 
            // ButtonHelp
            // 
            this.ButtonHelp.Anchor = System.Windows.Forms.AnchorStyles.Right;
            this.ButtonHelp.Enabled = false;
            this.ButtonHelp.Location = new System.Drawing.Point(56, 276);
            this.ButtonHelp.Name = "ButtonHelp";
            this.ButtonHelp.Size = new System.Drawing.Size(75, 24);
            this.ButtonHelp.TabIndex = 5;
            this.ButtonHelp.Text = "帮助";
            this.ButtonHelp.UseVisualStyleBackColor = true;
            // 
            // PanelBody
            // 
            this.PanelBody.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(250)))), ((int)(((byte)(250)))), ((int)(((byte)(246)))));
            this.PanelBody.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.PanelBody.Dock = System.Windows.Forms.DockStyle.Fill;
            this.PanelBody.Location = new System.Drawing.Point(137, 60);
            this.PanelBody.Name = "PanelBody";
            this.PanelBody.Size = new System.Drawing.Size(279, 210);
            this.PanelBody.TabIndex = 6;
            // 
            // frmSettingsSheet
            // 
            this.AcceptButton = this.ButtonOK;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.ButtonCancel;
            this.ClientSize = new System.Drawing.Size(419, 303);
            this.Controls.Add(this.TableLayoutPanel);
            this.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "frmSettingsSheet";
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "设置";
            this.Load += new System.EventHandler(this.OnFormCreate);
            this.TableLayoutPanel.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.PictureTile)).EndInit();
            this.PanelFoot.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TableLayoutPanel TableLayoutPanel;
        private System.Windows.Forms.PictureBox PictureTile;
        private System.Windows.Forms.TreeView TreeView;
        private System.Windows.Forms.Button ButtonOK;
        private System.Windows.Forms.Button ButtonCancel;
        private System.Windows.Forms.Panel PanelFoot;
        private System.Windows.Forms.Button ButtonHelp;
        private System.Windows.Forms.Panel PanelBody;
    }
}