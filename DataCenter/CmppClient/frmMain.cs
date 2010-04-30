using System;
using System.Data;
using System.Drawing;
using System.Windows.Forms;

using DataCenter.Log;

namespace DataCenter
{
    public partial class frmMain : Form
    {
        public frmMain()
        {
            InitializeComponent();
            CustomComponent();
        }

        private void frmMain_Load(object sender, EventArgs e)
        {
            if (Database.SQLConnection != null && Database.SQLConnection.State == ConnectionState.Open)
                this.stripStatusDatabase.Text = Properties.Settings.Database.Host + '(' + Properties.Settings.Database.Database + ')';
            else
                this.stripStatusDatabase.Text = "数据库断开";
#if false
            System.Windows.Forms.MdiClient mc = GetMdiClient(this);
            mc.Paint += new PaintEventHandler(frmMain_Paint);
            mc.BackColor = this.BackColor;
#endif
            TreeNode tNode = AddView(new frmMobile(), 32);
            AddView(new frmProvision(), tNode, 29);
        }

        private void frmMain_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (MessageBox.Show("确定要退出本系统吗？", "警告", MessageBoxButtons.OKCancel,
                MessageBoxIcon.Warning, MessageBoxDefaultButton.Button2) == DialogResult.Cancel)
            {
                e.Cancel = true; return;
            }

            RemoveView(LeftView.TopNode);
        }

        private void ItemMenuStripExit_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void frmMain_Paint(object sender, PaintEventArgs e)
        {
            int nX = this.Width - Properties.Resources.Background.Width;
            int nY = this.Height - Properties.Resources.Background.Height;
            
            e.Graphics.DrawImage(Properties.Resources.Background, nX, nY);
        }
#if false
        protected System.Windows.Forms.MdiClient GetMdiClient(System.Windows.Forms.Form f)
        {
            foreach (System.Windows.Forms.Control c in f.Controls)
            {
                if (c is System.Windows.Forms.MdiClient) return (System.Windows.Forms.MdiClient)c;
            }

            return null;
        }
#endif
        private void ItemMenuStripSettings_Click(object sender, EventArgs e)
        {
            frmSettingsManager frmSettings = new frmSettingsManager();
            frmSettings.ShowDialog();
        }

        private void ItemMenuStripProfiles_Click(object sender, EventArgs e)
        {
            frmProfilesManager frmProfiles = new frmProfilesManager();
            frmProfiles.ShowDialog();
        }

        private void LeftView_AfterSelect(object sender, TreeViewEventArgs e)
        {
            Form frmPage = (Form)RightPanel.Tag;
            if (frmPage != null) frmPage.Hide();

            RightPanel.Tag = e.Node.Tag;
            ((Form)RightPanel.Tag).Show();
        }

        private void OnLogMessage(object sender, LogService.LogEventArgs e)
        {
            ListViewItem lvi = LogMessage.Items.Add(e.Text);

            switch (e.MsgType)
            {
                case    MsgType.Debug:
                    lvi.ForeColor = Color.FromArgb(192, 192, 192);
                    break;
            }
         //   lvi.
        }
    }
}