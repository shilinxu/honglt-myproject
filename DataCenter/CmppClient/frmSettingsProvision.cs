using System;
using System.Windows.Forms;
using DataCenter.Properties;

namespace DataCenter
{
    partial class frmSettingsProvision : SettingsPage
    {
        public frmSettingsProvision()
        {
            InitializeComponent();
        }

        public override void OnApply()
        {
            Settings.HeadProvision.Host = this.lstHeadHost.Text;
            Settings.HeadProvision.Port = Int32.Parse(this.txtHeadPort.Text);
            Settings.HeadProvision.Version = this.lstHeadVersion.Text;

            Settings.BackProvision.Host = this.lstBackHost.Text;
            Settings.BackProvision.Port = Int32.Parse(this.txtBackPort.Text);
            Settings.BackProvision.Version = this.lstBackVersion.Text;
            Settings.BackProvision.URL = this.txtBackUrl.Text;
        }

        private void frmSettingsProvision_Load(object sender, EventArgs e)
        {
            this.lstHeadHost.Text = Settings.HeadProvision.Host;
            this.txtHeadPort.Text = Settings.HeadProvision.Port.ToString();
            this.lstHeadVersion.Text = Settings.HeadProvision.Version;

            this.lstBackHost.Text = Settings.BackProvision.Host;
            this.txtBackPort.Text = Settings.BackProvision.Port.ToString();
            this.lstBackVersion.Text = Settings.BackProvision.Version;
            this.txtBackUrl.Text = Settings.BackProvision.URL;
        }
    }
}