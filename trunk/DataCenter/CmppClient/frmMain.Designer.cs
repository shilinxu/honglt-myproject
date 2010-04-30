using System.Drawing;
using System.Windows.Forms;
using DataCenter.Log;

namespace DataCenter
{
    partial class frmMain
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
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(frmMain));
            this.menuStrip = new System.Windows.Forms.MenuStrip();
            this.ItemMenuStripFile = new System.Windows.Forms.ToolStripMenuItem();
            this.ItemMenuStripExit = new System.Windows.Forms.ToolStripMenuItem();
            this.ItemMenuStripTools = new System.Windows.Forms.ToolStripMenuItem();
            this.ItemMenuStripProfiles = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.ItemMenuStripSettings = new System.Windows.Forms.ToolStripMenuItem();
            this.statusStrip = new System.Windows.Forms.StatusStrip();
            this.stripStatusDatabase = new System.Windows.Forms.ToolStripStatusLabel();
            this.ImageList = new System.Windows.Forms.ImageList(this.components);
            this.LeftView = new System.Windows.Forms.TreeView();
            this.SplitterLeft = new System.Windows.Forms.Splitter();
            this.LogMessage = new System.Windows.Forms.ListView();
            this.Message = new System.Windows.Forms.ColumnHeader();
            this.SplitterRight = new System.Windows.Forms.Splitter();
            this.RightPanel = new System.Windows.Forms.Panel();
            this.menuStrip.SuspendLayout();
            this.statusStrip.SuspendLayout();
            this.SuspendLayout();
            // 
            // menuStrip
            // 
            this.menuStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ItemMenuStripFile,
            this.ItemMenuStripTools});
            this.menuStrip.Location = new System.Drawing.Point(0, 0);
            this.menuStrip.Name = "menuStrip";
            this.menuStrip.Size = new System.Drawing.Size(577, 24);
            this.menuStrip.TabIndex = 0;
            this.menuStrip.Text = "menuStrip";
            // 
            // ItemMenuStripFile
            // 
            this.ItemMenuStripFile.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ItemMenuStripExit});
            this.ItemMenuStripFile.Name = "ItemMenuStripFile";
            this.ItemMenuStripFile.Size = new System.Drawing.Size(59, 20);
            this.ItemMenuStripFile.Text = "文件(&F)";
            // 
            // ItemMenuStripExit
            // 
            this.ItemMenuStripExit.Name = "ItemMenuStripExit";
            this.ItemMenuStripExit.Size = new System.Drawing.Size(112, 22);
            this.ItemMenuStripExit.Text = "退出(&X)";
            this.ItemMenuStripExit.Click += new System.EventHandler(this.ItemMenuStripExit_Click);
            // 
            // ItemMenuStripTools
            // 
            this.ItemMenuStripTools.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ItemMenuStripProfiles,
            this.toolStripSeparator1,
            this.ItemMenuStripSettings});
            this.ItemMenuStripTools.Name = "ItemMenuStripTools";
            this.ItemMenuStripTools.Size = new System.Drawing.Size(59, 20);
            this.ItemMenuStripTools.Text = "工具(&T)";
            // 
            // ItemMenuStripProfiles
            // 
            this.ItemMenuStripProfiles.Name = "ItemMenuStripProfiles";
            this.ItemMenuStripProfiles.Size = new System.Drawing.Size(154, 22);
            this.ItemMenuStripProfiles.Text = "业务定制";
            this.ItemMenuStripProfiles.Click += new System.EventHandler(this.ItemMenuStripProfiles_Click);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(151, 6);
            // 
            // ItemMenuStripSettings
            // 
            this.ItemMenuStripSettings.Name = "ItemMenuStripSettings";
            this.ItemMenuStripSettings.Size = new System.Drawing.Size(154, 22);
            this.ItemMenuStripSettings.Text = "系统设置(&O)...";
            this.ItemMenuStripSettings.Click += new System.EventHandler(this.ItemMenuStripSettings_Click);
            // 
            // statusStrip
            // 
            this.statusStrip.BackColor = System.Drawing.SystemColors.Control;
            this.statusStrip.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.statusStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.stripStatusDatabase});
            this.statusStrip.Location = new System.Drawing.Point(0, 334);
            this.statusStrip.Name = "statusStrip";
            this.statusStrip.Size = new System.Drawing.Size(577, 22);
            this.statusStrip.TabIndex = 2;
            this.statusStrip.Text = "statusStrip1";
            // 
            // stripStatusDatabase
            // 
            this.stripStatusDatabase.Name = "stripStatusDatabase";
            this.stripStatusDatabase.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.stripStatusDatabase.Size = new System.Drawing.Size(43, 17);
            this.stripStatusDatabase.Text = "连接...";
            this.stripStatusDatabase.TextAlign = System.Drawing.ContentAlignment.BottomRight;
            // 
            // ImageList
            // 
            this.ImageList.ColorDepth = System.Windows.Forms.ColorDepth.Depth8Bit;
            this.ImageList.ImageSize = new System.Drawing.Size(16, 16);
            this.ImageList.TransparentColor = System.Drawing.Color.Lime;
            // 
            // LeftView
            // 
            this.LeftView.Dock = System.Windows.Forms.DockStyle.Left;
            this.LeftView.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.LeftView.ImageIndex = 0;
            this.LeftView.ImageList = this.ImageList;
            this.LeftView.Location = new System.Drawing.Point(0, 24);
            this.LeftView.Name = "LeftView";
            this.LeftView.SelectedImageIndex = 0;
            this.LeftView.ShowLines = false;
            this.LeftView.ShowRootLines = false;
            this.LeftView.Size = new System.Drawing.Size(156, 310);
            this.LeftView.TabIndex = 0;
            this.LeftView.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.LeftView_AfterSelect);
            // 
            // SplitterLeft
            // 
            this.SplitterLeft.BackColor = System.Drawing.SystemColors.Control;
            this.SplitterLeft.Location = new System.Drawing.Point(156, 24);
            this.SplitterLeft.Name = "SplitterLeft";
            this.SplitterLeft.Size = new System.Drawing.Size(3, 310);
            this.SplitterLeft.TabIndex = 3;
            this.SplitterLeft.TabStop = false;
            // 
            // LogMessage
            // 
            this.LogMessage.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.Message});
            this.LogMessage.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.LogMessage.Font = new System.Drawing.Font("Verdana", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.LogMessage.ForeColor = System.Drawing.Color.RoyalBlue;
            this.LogMessage.FullRowSelect = true;
            this.LogMessage.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.None;
            this.LogMessage.Location = new System.Drawing.Point(159, 231);
            this.LogMessage.MultiSelect = false;
            this.LogMessage.Name = "LogMessage";
            this.LogMessage.Size = new System.Drawing.Size(418, 103);
            this.LogMessage.TabIndex = 10;
            this.LogMessage.UseCompatibleStateImageBehavior = false;
            this.LogMessage.View = System.Windows.Forms.View.Details;
            // 
            // Message
            // 
            this.Message.Text = "Messages";
            this.Message.Width = 700;
            // 
            // SplitterRight
            // 
            this.SplitterRight.BackColor = System.Drawing.SystemColors.Control;
            this.SplitterRight.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.SplitterRight.Location = new System.Drawing.Point(159, 228);
            this.SplitterRight.Name = "SplitterRight";
            this.SplitterRight.Size = new System.Drawing.Size(418, 3);
            this.SplitterRight.TabIndex = 11;
            this.SplitterRight.TabStop = false;
            // 
            // RightPanel
            // 
            this.RightPanel.BackColor = System.Drawing.Color.Transparent;
            this.RightPanel.Dock = System.Windows.Forms.DockStyle.Fill;
            this.RightPanel.Location = new System.Drawing.Point(159, 24);
            this.RightPanel.Name = "RightPanel";
            this.RightPanel.Size = new System.Drawing.Size(418, 204);
            this.RightPanel.TabIndex = 12;
            // 
            // frmMain
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.Window;
            this.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.ClientSize = new System.Drawing.Size(577, 356);
            this.Controls.Add(this.RightPanel);
            this.Controls.Add(this.SplitterRight);
            this.Controls.Add(this.LogMessage);
            this.Controls.Add(this.SplitterLeft);
            this.Controls.Add(this.LeftView);
            this.Controls.Add(this.statusStrip);
            this.Controls.Add(this.menuStrip);
            this.DoubleBuffered = true;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MainMenuStrip = this.menuStrip;
            this.Name = "frmMain";
            this.Text = "中国移动短信客户端";
            this.WindowState = System.Windows.Forms.FormWindowState.Maximized;
            this.Load += new System.EventHandler(this.frmMain_Load);
            this.Paint += new System.Windows.Forms.PaintEventHandler(this.frmMain_Paint);
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.frmMain_FormClosing);
            this.menuStrip.ResumeLayout(false);
            this.menuStrip.PerformLayout();
            this.statusStrip.ResumeLayout(false);
            this.statusStrip.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        #region Windows Form Designer custom code

        private void CustomComponent()
        {
            Bitmap bmp = Properties.Resources.MenuBar;

            for (int nPos = 0; nPos < bmp.Width / bmp.Height; nPos++)
            {
                ImageList.Images.Add(bmp.Clone(new Rectangle(bmp.Height * nPos, 0, bmp.Height, bmp.Height), bmp.PixelFormat));
            }

            ItemMenuStripSettings.Image = ImageList.Images[18];
            ItemMenuStripProfiles.Image = ImageList.Images[29];

            LogService.Default.Log += new LogService.LogEventHandler(OnLogMessage);
        }

        public TreeNode AddView(Form frmPage, int nImage)
        {
            TreeNode tNode = LeftView.Nodes.Add(frmPage.Text);
            tNode.ImageIndex = tNode.SelectedImageIndex = nImage;

            frmPage.TopLevel = false;
            frmPage.Dock = System.Windows.Forms.DockStyle.Fill;

            RightPanel.Controls.Add(frmPage);
            tNode.Tag = (object)frmPage;
 
            return tNode;
        }

        public TreeNode AddView(Form frmPage, TreeNode tParent, int nImage)
        {
            TreeNode tNode = tParent.Nodes.Add(frmPage.Text);
            tNode.ImageIndex = tNode.SelectedImageIndex = nImage;

            frmPage.TopLevel = false;
            frmPage.Dock = System.Windows.Forms.DockStyle.Fill;

            RightPanel.Controls.Add(frmPage);
            tNode.Tag = (object)frmPage;

            tParent.NodeFont = new Font(LeftView.Font, FontStyle.Bold);
            tParent.Expand();
            return tNode;
        }

        protected void RemoveView(TreeNode tNode)
        {
            for (TreeNode tChild = tNode.LastNode; tChild != null; tChild = tNode.LastNode)
            {
                RemoveView(tChild); tNode.Nodes.Remove(tChild);
            }

            Form frmPage = (Form)tNode.Tag;
            frmPage.Close();
        }

        #endregion

        private System.Windows.Forms.MenuStrip menuStrip;
        private System.Windows.Forms.ToolStripMenuItem ItemMenuStripFile;
        private System.Windows.Forms.StatusStrip statusStrip;
        private System.Windows.Forms.ToolStripStatusLabel stripStatusDatabase;
        private System.Windows.Forms.ToolStripMenuItem ItemMenuStripTools;
        private System.Windows.Forms.ToolStripMenuItem ItemMenuStripSettings;
        private System.Windows.Forms.ImageList ImageList;
        private System.Windows.Forms.ToolStripMenuItem ItemMenuStripExit;
        private System.Windows.Forms.ToolStripMenuItem ItemMenuStripProfiles;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.TreeView LeftView;
        private Splitter SplitterLeft;
        private ListView LogMessage;
        private ColumnHeader Message;
        private Splitter SplitterRight;
        private Panel RightPanel;
    }
}

