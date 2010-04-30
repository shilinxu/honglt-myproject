using System;
using System.Windows.Forms;
using DataCenter.Properties;
using System.Diagnostics;

namespace DataCenter
{
    partial class frmProfilesSecure : SettingsPage
    {
        private Profiles.BlacklstElement Profile;

        public frmProfilesSecure(Profiles.BlacklstElement profile)
        {
            this.Profile = profile;
            InitializeComponent();
        }

        private void frmProfilesSecure_Load(object sender, EventArgs e)
        {
            Debug.Assert(this.Profile != null);
            this.ListAction.SelectedIndex = (int)this.Profile.Action;

            foreach (Profiles.BlacklstElement element in Profile.ChildNodes) AddItem(element);

            if (ListView.Items.Count > 0) ListView.Items[0].Selected = true;
        }

        private void ListView_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (ListView.SelectedItems.Count == 0) return;
            ListViewItem lvi = ListView.SelectedItems[0];

            Profiles.BlacklstElement element = (Profiles.BlacklstElement)lvi.Tag;

            this.ListHost.Text = element.GetAttribute("Host");
            this.TextDesc.Text = element.GetAttribute("Desc");

            ButtonRemove.Enabled = true;
        }

        private ListViewItem AddItem(Profiles.BlacklstElement element)
        {
            ListViewItem lvi = this.ListView.Items.Add(element.GetAttribute("Host"));
            lvi.Tag = (object)element;

            lvi.SubItems.Add(element.GetAttribute("Date"));
            lvi.SubItems.Add(element.GetAttribute("Desc"));

            return lvi;
        }

        public override void OnApply()
        {
            this.Profile.Action = (Profiles.BlacklstElement.ActionType)this.ListAction.SelectedIndex;

            base.OnOK();
        }

        private void ButtonAdd_Click(object sender, EventArgs e)
        {
            Debug.Assert(this.Profile != null);
            Profiles.BlacklstElement element = this.Profile.CreateElement(Profiles.BlacklstType);

            element.SetAttribute("Host", ListHost.Text);
            element.SetAttribute("Desc", TextDesc.Text);

            element.SetAttribute("Date", DateTime.Now.ToLongDateString());

            AddItem(element);
        }

        private void ButtonRemove_Click(object sender, EventArgs e)
        {
            Debug.Assert(this.Profile != null);

            if (ListView.SelectedItems.Count == 0) return;
            ListViewItem lvi = ListView.SelectedItems[0];

            Profiles.BlacklstElement element = (Profiles.BlacklstElement)lvi.Tag;
            this.Profile.RemoveChild(element);

            ListView.SelectedItems[0].Remove();
        }
    }
}