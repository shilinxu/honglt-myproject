using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Collections;

namespace DataCenter
{
    abstract partial class frmSettingsSheet : Form
    {
        protected ArrayList m_pPages = new ArrayList();

        public frmSettingsSheet(string caption)
        {
            InitializeComponent();
            this.Text = caption;
        }

        #region operations

        public virtual void SetImage(Image pImage)
        {
            PictureTile.Image = pImage;
        }

        protected Panel Body
        {
            get { return PanelBody; }
        }

        public virtual SettingsPage AddPage(SettingsPage pPage)
        {
            TreeNode tNode = TreeView.Nodes.Add(pPage.Text);
            return AddPage(pPage, tNode);
        }

        public virtual SettingsPage AddPage(SettingsPage pPage, SettingsPage pParent)
        {
            TreeNode tNode = FindNode(pParent);
            if (tNode != null) tNode.NodeFont = new Font(TreeView.Font, FontStyle.Bold);

            tNode = tNode.Nodes.Add(pPage.Text);
            return AddPage(pPage, tNode);
        }

        protected virtual SettingsPage AddPage(SettingsPage pPage, TreeNode tNode)
        {
            tNode.Tag = (Object)pPage;
            if (tNode.Parent != null) tNode.Parent.Expand();

            pPage.TopLevel = false;
            this.Body.Controls.Add(pPage);

            if (pPage.Height > this.Body.Height) this.Height += pPage.Height - this.Body.Height;
            if (pPage.Width > this.Body.Width) this.Width += pPage.Width - this.Body.Width;

            m_pPages.Add(pPage);
            return pPage;
        }
        
        #endregion

        private TreeNode FindNode(object page)
        {
            foreach (TreeNode node in TreeView.Nodes)
            {
                if (node.Tag == page) return node;
            }

            return null;
        }

        abstract protected void OnFormCreate(object sender, EventArgs e);

        private void TreeView_AfterSelect(object sender, TreeViewEventArgs e)
        {
            SettingsPage pPage = (SettingsPage)PanelBody.Tag;
            if (pPage != null) pPage.Hide();

            PanelBody.Tag = e.Node.Tag;
            ((SettingsPage)PanelBody.Tag).Show();
        }

        protected virtual void OnOK(object sender, EventArgs e)
        {
            foreach (SettingsPage pPage in m_pPages)
            {
                if (pPage.Created) pPage.OnApply();
            }
        }

        protected virtual void OnCancel(object sender, EventArgs e)
        {
            foreach (SettingsPage pPage in m_pPages)
            {
                pPage.OnCancel();
            }
        }
    }

    public partial class SettingsPage : Form
    {
        public virtual void OnOK() { }
        public virtual void OnCancel() { }

        public virtual void OnApply() { }
    }
}