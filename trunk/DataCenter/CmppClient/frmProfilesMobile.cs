using System;
using System.Windows.Forms;
using DataCenter.Properties;
using System.Diagnostics;

namespace DataCenter
{
    partial class frmProfilesMobile : SettingsPage
    {
        private Profiles.OrderRelationElement Profile;

        public frmProfilesMobile(Profiles.OrderRelationElement profile)
        {
            this.Profile = profile;
            InitializeComponent();
        }

        private void frmProfilesMobile_Load(object sender, EventArgs e)
        {
            Debug.Assert(this.Profile != null);

            foreach (Profiles.OrderRelationElement element in Profile.ChildNodes) AddItem(element);

            if (listView.Items.Count > 0) listView.Items[0].Selected = true;
        }

        private void listView_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (listView.SelectedItems.Count == 0) return;
            ListViewItem lvi = listView.SelectedItems[0];

            Profiles.OrderRelationElement element = (Profiles.OrderRelationElement)lvi.Tag;

            this.lstIndex.Text = element.GetAttribute("ID");
            this.txtCode.Text = element.GetAttribute("Code");

            this.lstFeeUser.SelectedIndex = int.Parse(element.GetAttribute("FeeUser"));
            this.lstFeeType.SelectedIndex = int.Parse(element.GetAttribute("FeeType"));

            this.txtFeeCode.Text = element.GetAttribute("FeeCode");
            this.txtName.Text = element.GetAttribute("Name");
            
            btnRemove.Enabled = true;
        }

        private ListViewItem AddItem(Profiles.OrderRelationElement element)
        {
            ListViewItem lvi = listView.Items.Add(element.GetAttribute("ID"));
            lvi.Tag = (object)element;

            lvi.SubItems.Add(element.GetAttribute("Code"));
            lvi.SubItems.Add(element.GetAttribute("Name"));

            return lvi;
        }

        private void btnAdd_Click(object sender, EventArgs e)
        {
            Debug.Assert(this.Profile != null);
            Profiles.OrderRelationElement element = this.Profile.CreateElement(Profiles.OrderRelationType);

            element.SetAttribute("ID", lstIndex.Text);
            element.SetAttribute("Code", txtCode.Text);

            element.SetAttribute("FeeUser", lstFeeUser.SelectedIndex.ToString());
            element.SetAttribute("FeeType", lstFeeType.SelectedIndex.ToString());

            element.SetAttribute("FeeCode", txtFeeCode.Text);
            element.SetAttribute("Name", txtName.Text);

            AddItem(element);
        }

        private void btnRemove_Click(object sender, EventArgs e)
        {
            Debug.Assert(this.Profile != null);

            if (listView.SelectedItems.Count == 0) return;
            ListViewItem lvi = listView.SelectedItems[0];

            Profiles.OrderRelationElement element = (Profiles.OrderRelationElement)lvi.Tag;
            this.Profile.RemoveChild(element);

            listView.SelectedItems[0].Remove();
        }
    }
}