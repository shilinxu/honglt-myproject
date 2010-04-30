using System;
using System.Xml;
using System.Configuration;
using System.Collections;

#if false
namespace DataCenter.Properties
{
    internal sealed class Profiles : System.Configuration.SettingsBase
    {
        public OrderRelationSection OrderRelation;
        private Configuration configFile;

        public static Profiles Synchronized(string file)
        {
            return new Profiles(file);
        }

        public Profiles() : this("Profiles.xml")
        {
        }

        public Profiles(string fileName)
        {
            ExeConfigurationFileMap fileMap = new ExeConfigurationFileMap();
            fileMap.ExeConfigFilename = fileName;

            configFile = ConfigurationManager.OpenMappedExeConfiguration(fileMap, ConfigurationUserLevel.None);

            OrderRelation = (OrderRelationSection)ApplySection("OrderRelation", OrderRelation, typeof(OrderRelationSection));
        }

        public void Save(ConfigurationSaveMode saveMode)
        {
            if (configFile == null) return;
            configFile.Save(saveMode);
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

        internal class OrderRelationSection : ConfigurationSection
        {
            [ConfigurationProperty("", IsDefaultCollection = true)]
            public OrderRelationCollection OrderRelations
            {
                get
                {
                    return (OrderRelationCollection)base[""];
                }
            }
        }

        internal class OrderRelationCollection : ConfigurationElementCollection 
        {
            protected override ConfigurationElement CreateNewElement() 
            {
                return new OrderRelationElement();
            }

            protected override object GetElementKey(ConfigurationElement element)
            {
                return ((OrderRelationElement)element).ID;
            }

            public override ConfigurationElementCollectionType CollectionType
            {
                get
                {
                    return ConfigurationElementCollectionType.BasicMap;
                }
            }

            protected override string ElementName
            {
                get
                {
                    return "OrderRelation";
                }
            }

            public OrderRelationElement this[object key]
            {
                get
                {
                    return (OrderRelationElement)BaseGet(key);
                }
                set
                {
                    //if (BaseGet(key) != null) BaseRemove(key);
                    BaseAdd(value);
                }
            }

            public void RemoveKey(object key)
            {
                BaseRemove(key);
            }
        }

        internal class OrderRelationElement : ConfigurationElement
        {
            [ConfigurationProperty("ID")]
            public Int32 ID
            {
                get { return (Int32)this["ID"]; }
                set { this["ID"] = value; }
            }

            [ConfigurationProperty("Code")]
            public string Code
            {
                get { return (string)this["Code"]; }
                set { this["Code"] = value; }
            }

            [ConfigurationProperty("Name")]
            public string Name
            {
                get { return (string)this["Name"]; }
                set { this["Name"] = value; }
            }

            [ConfigurationProperty("FeeUser")]
            public Int32 FeeUser
            {
                get { return (Int32)this["FeeUser"]; }
                set { this["FeeUser"] = value; }
            }

            [ConfigurationProperty("FeeType")]
            public Int32 FeeType
            {
                get { return (Int32)this["FeeType"]; }
                set { this["FeeType"] = value; }
            }

            [ConfigurationProperty("FeeCode")]
            public string FeeCode
            {
                get { return (string)this["FeeCode"]; }
                set { this["FeeCode"] = value; }
            }
        }
    }
}
#endif

namespace DataCenter.Properties
{
    using System.Net;

    internal class Profiles : XmlDocument, IDisposable
    {
        protected string DefaultFile = "Profiles.xml";

        void IDisposable.Dispose()
        {
        }

        protected Profiles(string fileName)
        {
            this.Load(fileName);

            this.DefaultFile = fileName;
        }

        protected Profiles(string fileName, bool header)
        {
            if ( header ) AppendChild(CreateXmlDeclaration("1.0", "utf-8", null));

            XmlElement parent = AddElement(this, "DataCenter");
            AddAttribute(parent, "Author", "Honglt");

            this.DefaultFile = fileName;
        }

        public static Profiles Create(string fileName, bool header)
        {
            try
            {
                return new Profiles(fileName, header);
            }
            catch (Exception)
            {
                return null;
            }
        }

        public static Profiles FromFile(string fileName)
        {
            try
            {
                return new Profiles(fileName);
            }
            catch (Exception)
            {
                return null;
            }
        }

        public virtual bool Save()
        {
            try
            {
                base.Save(this.DefaultFile);
                return true;
            }
            catch (XmlException)
            {
                return false;
            }
        }

        public override XmlElement CreateElement(string prefix, string localName, string namespaceURI)
        {
            if (localName == BlacklstType) return new BlacklstElement(prefix, localName, namespaceURI, this);
            if (localName == OrderRelationType) return new OrderRelationElement(prefix, localName, namespaceURI, this);

            return base.CreateElement(prefix, localName, namespaceURI);
        }

        public override XmlElement this[string name]
        {
            get { return this.DocumentElement[name]; }
        }

        public BlacklstElement Blacklist
        {
            get { return (BlacklstElement)this[BlacklstType];}
        }

        public OrderRelationElement OrderRelation
        {
            get { return (OrderRelationElement)this[OrderRelationType]; }
        }

        #region XML operations definition

        public XmlElement AddElement(string name)
        {
            return AddElement(this.DocumentElement, name);
        }

        public XmlElement AddElement(XmlNode parent, string name)
        {
            return AddElement(parent, name, null);
        }

        public XmlElement AddElement(XmlNode parent, string name, object value)
        {
            XmlElement element = this.CreateElement(name);
            if (value != null) element.Value = value.ToString();

            return (XmlElement)parent.AppendChild(element);
        }

        public XmlAttribute AddAttribute(XmlElement parent, string name)
        {
            return AddAttribute(parent, name, null);
        }

        public XmlAttribute AddAttribute(XmlElement parent, string name, object value)
        {
            XmlAttribute attribute = parent.Attributes[name]; if (attribute == null)
            {
                parent.Attributes.Append(attribute = CreateAttribute(name));
            }

            attribute.Value = value.ToString(); return attribute;
        }

        #endregion

        public const string BlacklstType = "Blacklist";

        internal class BlacklstElement : XmlElement
        {
            public enum ActionType
            {
                Accept = 0, Reject = 1,
            }

            protected internal BlacklstElement(string prefix, string localName, string namespaceURI, XmlDocument doc)
                : base(prefix, localName, namespaceURI, doc)
            {
            }

            protected internal BlacklstElement CreateElement(string name)
            {
                return (BlacklstElement)this.AppendChild(new BlacklstElement(null, name, null, this.OwnerDocument));
            }

            public ActionType Action
            {
                get { return (ActionType)Enum.Parse(typeof(ActionType), GetAttribute("Action"), true); }
                set { SetAttribute("Action", value.ToString()); }
            }

            public bool IsDenied(IPAddress address)
            {
                IPAddress addr2; IPAddress.TryParse(GetAttribute("Host"), out addr2);
                bool found = addr2 != null && addr2.Equals(address);

                XmlAttribute attri = Attributes["Action"];
                if (attri == null) return found;
                
                foreach (XmlElement element in this.ChildNodes)
                {
                    if (((BlacklstElement)element).IsDenied(address))
                    {
                        found = true; break;
                    }
                }

                // 拒绝条件: 拒绝 && bFound ==> TRUE
                //         : !拒绝 && !Found ==> TRUE
                // 受理方法: 异或 = 0 ==> TRUE, 否则 FALSE

                if (this.Action == ActionType.Reject ^ found) return false;
                return true;
            }
        }

        public const string OrderRelationType = "OrderRelation";

        internal class OrderRelationElement : XmlElement
        {
            protected internal OrderRelationElement(string prefix, string localName, string namespaceURI, XmlDocument doc)
                : base(prefix, localName, namespaceURI, doc)
            {
            }

            protected internal OrderRelationElement CreateElement(string name)
            {
                return (OrderRelationElement)this.AppendChild(new OrderRelationElement(null, name, null, this.OwnerDocument));
            }
        }
    }
}
