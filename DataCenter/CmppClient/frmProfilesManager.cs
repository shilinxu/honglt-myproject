using System;
using System.Xml;
using System.Configuration;
using DataCenter.Properties;
using System.Net;

namespace DataCenter
{
    class frmProfilesManager : frmSettingsSheet
    {
        protected Properties.Profiles Profile;

        public frmProfilesManager() : base("业务规则")
        {
            this.Profile = Profiles.FromFile("Profiles.xml");
            if (this.Profile == null) this.Profile = Profiles.Create("Profiles.xml", true);
        }

        override protected void OnFormCreate(object sender, EventArgs e)
        {
            SettingsPage group = AddPage(new frmSettingsMobile());

            if (this.Profile.OrderRelation == null) this.Profile.AddElement(Profiles.OrderRelationType);
            AddPage(new frmProfilesMobile(this.Profile.OrderRelation), group);

            if (this.Profile.Blacklist == null) this.Profile.AddElement(Profiles.BlacklstType);
            AddPage(new frmProfilesSecure(this.Profile.Blacklist));
        }

        protected override void OnOK(object sender, EventArgs e)
        {
            base.OnOK(sender, e);

            Profile.Save();

            bool dddd = IPAddress.Parse("127.0.0.1").Equals(IPAddress.Parse("127.0.0.1"));
            bool aaa=Profile.Blacklist.IsDenied(IPAddress.Parse("127.0.0.10"));
        }
    }
}
