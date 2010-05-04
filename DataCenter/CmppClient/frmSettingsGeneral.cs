using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Configuration;
using System.Xml;

namespace DataCenter
{
    public partial class frmSettingsGeneral : SettingsPage
    {
        public frmSettingsGeneral()
        {
            InitializeComponent();
        }

        private void frmSettingsGeneral_Load(object sender, EventArgs e)
        {
            ComboHost.Text = Properties.Settings.Database.Host;
            ComboDatabase.Text = Properties.Settings.Database.Database;
            TextUserId.Text = Properties.Settings.Database.UserId;
            TextPassword.Text = Properties.Settings.Database.Password;
        }

        public override void OnApply()
        {
            Properties.Settings.Database.Host = ComboHost.Text;
            Properties.Settings.Database.Database = ComboDatabase.Text;
            Properties.Settings.Database.UserId = TextUserId.Text;
            Properties.Settings.Database.Password = TextPassword.Text;

            base.OnOK();
        }
    }
}