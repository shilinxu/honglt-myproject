using System;
using System.Windows.Forms;
using DataCenter.Properties;
using DataCenter.Windows.Forms;
using System.ComponentModel;
using System.Xml;
using System.IO;

namespace DataCenter
{
    partial class frmProvision : Form
    {
        protected History History;
        protected PropertyBag PropertyBag = new PropertyBag();

        public frmProvision()
        {
            InitializeComponent();
            this.Visible = true;
        }

        #region history operations

        protected void LoadProfile(Profiles Profile)
        {
            foreach (Profiles.OrderRelationElement element in Profile.OrderRelation)
            {
                AddElement(element, PropertyBag);
            }

            PropertyGrid.SelectedObject = PropertyBag;
        }

        protected void ResetElement(PropertyBag bag, XmlElement parent)
        {
            foreach (XmlElement element in parent.ChildNodes)
            {
                XmlAttribute attri = element.Attributes["ID"];
                if (attri == null) continue;

                PropertySpec item = FindElement(bag, attri.Value);
                if (item == null) continue;

                int value = 0;

                attri = element.Attributes["Subscribe"];
                if (attri != null) value += Int32.Parse(attri.Value);

                attri = element.Attributes["UnSubscribe"];
                if (attri != null) value += Int32.Parse(attri.Value);

                attri = element.Attributes["Other"];
                if (attri != null) value += Int32.Parse(attri.Value);

                item.Value = value;
            }

            PropertyGrid.SelectedObject = bag;
        }

        protected void AddElement(Profiles.OrderRelationElement element, PropertyBag bag)
        {
            const string category = "Provision¶©¹º";

            string desc = String.Format("ID := {{{0}}}", element.GetAttribute("Code"));
            PropertySpec item = new PropertySpec(element.GetAttribute("Name"), 0, category, desc);

            item.Key = element.GetAttribute("Code");
#if false
            item.Attributes = new Attribute[] { ReadOnlyAttribute.Yes };
#endif

            bag.Add(item);
        }

        protected bool IncElement(string key, string type, PropertyBag bag)
        {
            PropertySpec item = FindElement(bag, key);
            if (item == null) return false;

            item.Value = (int)item.Value + 1;
            PropertyGrid.SelectedObject = bag;

            return true;
        }

        private PropertySpec FindElement(PropertyBag bag, object key)
        {
            for (int i = 0; i < PropertyBag.Count; i++)
            {
                if (bag[i].Key == key) return bag[i];
            }

            return null;
        }

        #endregion

        private void frmProvision_Load(object sender, EventArgs e)
        {
            LoadProfile(Profiles.FromFile("Profiles.xml"));

            try
            {
                Directory.CreateDirectory("Provision");

                string file = String.Format("{0}Äê{1:D2}ÔÂ.xml", DateTime.Now.Year, DateTime.Now.Month);

                if (!File.Exists(@"Provision\" + file)) this.History = new History();
                else this.History = new History(@"Provision\" + file);

                XmlElement element = History.Location(DateTime.Now.ToLongDateString());
                if (element != null) ResetElement(this.PropertyBag, element);
            }
            catch(Exception)
            {
            }

            this.HttpServer = new Network.HttpServer();
        }

        private void frmProvision_FormClosed(object sender, FormClosedEventArgs e)
        {
            if (this.HttpServer != null) this.HttpServer.Disconnect();
        }

        Network.HttpServer HttpServer;
    }
}