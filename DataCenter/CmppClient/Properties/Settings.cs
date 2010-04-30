using System;
using System.Configuration;

namespace DataCenter.Properties
{
    internal sealed class Settings : System.Configuration.SettingsBase
    {
        public static DatabaseSection Database;
        public static HeadProvisionSection HeadProvision;
        public static BackProvisionSection BackProvision;

        private static Configuration configFile;

        public Settings()
        {
            configFile = ConfigurationManager.OpenExeConfiguration(ConfigurationUserLevel.None);

            Settings.Database = (DatabaseSection)ApplySection("Database", Settings.Database, typeof(DatabaseSection));

            Settings.HeadProvision = (HeadProvisionSection)ApplySection("HeadProvision", Settings.HeadProvision, typeof(HeadProvisionSection));
            Settings.BackProvision = (BackProvisionSection)ApplySection("BackProvision", Settings.HeadProvision, typeof(BackProvisionSection));
        }

        public static void Save(ConfigurationSaveMode saveMode)
        {
            if (configFile == null) return;
            configFile.Save(saveMode);

            ConfigurationManager.RefreshSection("Database");
        }

        private ConfigurationSection ApplySection(string name, ConfigurationSection section, Type type)
        {
            section = configFile.GetSection(name); if (section == null)
            {
                section = (ConfigurationSection)System.Activator.CreateInstance(type);
                configFile.Sections.Add(name, section);
            }

            return section;
        }

        internal class DatabaseSection : ConfigurationSection
        {
            [ConfigurationProperty("Host", DefaultValue="localhost")]
            public string Host
            {
                get { return (string)this["Host"]; }
                set { this["Host"] = value; }
            }

            [ConfigurationProperty("Database", DefaultValue = "datacenter")]
            public string Database
            {
                get { return (string)this["Database"]; }
                set { this["Database"] = value; }
            }

            [ConfigurationProperty("UserId", DefaultValue = "sa")]
            public string UserId
            {
                get { return (string)this["UserId"]; }
                set { this["UserId"] = value; }
            }

            [ConfigurationProperty("Password")]
            public string Password
            {
                get { return (string)this["Password"]; }
                set { this["Password"] = value; }
            }
        }

        internal class HeadProvisionSection : ConfigurationSection
        {
            [ConfigurationProperty("Host", DefaultValue = "127.0.0.1")]
            public string Host
            {
                get { return (string)this["Host"]; }
                set { this["Host"] = value; }
            }

            [ConfigurationProperty("Port", DefaultValue = 8080)]
            public Int32 Port
            {
                get { return (Int32)this["Port"]; }
                set { this["Port"] = value; }
            }

            [ConfigurationProperty("Version", DefaultValue = "1.5.0")]
            public string Version
            {
                get { return (string)this["Version"]; }
                set { this["Version"] = value; }
            }
        }

        internal class BackProvisionSection : ConfigurationSection
        {
            [ConfigurationProperty("Host", DefaultValue = "211.140.11.43")]
            public string Host
            {
                get { return (string)this["Host"]; }
                set { this["Host"] = value; }
            }

            [ConfigurationProperty("Port", DefaultValue = 80)]
            public Int32 Port
            {
                get { return (Int32)this["Port"]; }
                set { this["Port"] = value; }
            }

            [ConfigurationProperty("Version", DefaultValue = "1.5.0")]
            public string Version
            {
                get { return (string)this["Version"]; }
                set { this["Version"] = value; }
            }

            [ConfigurationProperty("URL", DefaultValue = "/dsmp/dsmp.wsdl")]
            public string URL
            {
                get { return (string)this["URL"]; }
                set { this["URL"] = value; }
            }
        }
    }
}
