using System;
using System.Xml;
using System.Reflection;

namespace DataCenter.Properties
{
    class History : XmlDocument
    {
        public History()
        {
            this.AppendChild(CreateXmlDeclaration("1.0", "utf-8", null));
            
            XmlElement parent = AddElement(this, "DataCenter");
            AddAttribute(parent, "Author", "Honglt");

            AddAttribute(parent, "Version", Assembly.GetExecutingAssembly().GetName().Version);
        }

        public History(string file)
        {
            this.Load(file);
        }

        public XmlElement AddElement(XmlNode parent, string name)
        {
            XmlElement element = this.CreateElement(name);

            parent.AppendChild(element);
            return element;
        }

        public XmlAttribute AddAttribute(XmlElement parent, string name, object value)
        {
            XmlAttribute attribute = this.CreateAttribute(name);
            attribute.Value = value.ToString();

            parent.Attributes.Append(attribute);
            return attribute;
        }

        public XmlElement Location(string date)
        {
            foreach (XmlElement element in this.DocumentElement.ChildNodes)
            {
                XmlAttribute attribute = element.Attributes["Date"];
                if ( attribute.Value == date ) return element;
            }

            return null;
        }

        public object IncAttribute(XmlElement parent, string key, string type)
        {
            foreach (XmlElement element in parent.ChildNodes)
            {
                XmlAttribute attri = element.Attributes["ID"];
                if (attri == null || attri.Value == key) continue;

                attri = element.Attributes[type];
                if (attri == null) attri = AddAttribute(element, "Map", 0);

                int value = attri != null ? Int32.Parse(attri.Value) : 0;
                attri.Value = value.ToString();

                return value;
            }

            return 0;
        }
    }
}
