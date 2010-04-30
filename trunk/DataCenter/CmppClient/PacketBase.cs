using System.Xml;

namespace DataCenter.Provision
{
    public class PacketBase : XmlDocument
    {
        protected static readonly string xmlnsdsmp = "http://www.monternet.com/dsmp/schemas/";

        // 定制关系
        public static readonly string SyncOrderRelationReq = "SyncOrderRelationReq";
        public static readonly string SyncOrderRelationResp = "SyncOrderRelationResp";

        protected PacketBase(string msgType) : this(msgType, null)
        {
        }

        protected PacketBase(string msgType, string version)
        {
            XmlElement body = AddElement(this, msgType);
            AddAttribute(body, "xmlns", xmlnsdsmp);

            AddElement(body, "Version", version);
        }

        public struct Addr_Schema
        {
            int DeviceType;
            string DeviceID;
        };

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
            XmlAttribute attribute = this.CreateAttribute(name);
            if (value != null) attribute.Value = value.ToString();

            return (XmlAttribute)parent.Attributes.Append(attribute);
        }
    }

    class SyncOrderRelation : PacketBase
    {
        public Addr_Schema Send_Address;

        public SyncOrderRelation() : base(SyncOrderRelationReq)
        {
        }
    }
}
