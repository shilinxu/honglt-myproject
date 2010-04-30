using System;
using System.Configuration;

namespace DataCenter
{
    class frmSettingsManager : frmSettingsSheet
    {
        public frmSettingsManager() : base("…Ë÷√")
        {
        }

        override protected void OnFormCreate(object sender, EventArgs e)
        {
            AddPage(new frmSettingsGeneral());
           
            SettingsPage pGroup = AddPage(new frmSettingsMobile());
            AddPage(new frmSettingsProvision(), pGroup);
        }

        protected override void OnOK(object sender, EventArgs e)
        {
            base.OnOK(sender, e);
            Properties.Settings.Save(ConfigurationSaveMode.Modified);
        }
    }
}
