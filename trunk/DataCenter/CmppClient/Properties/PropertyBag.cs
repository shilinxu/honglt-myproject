using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing.Design;
using System.Collections.Generic;

namespace DataCenter.Windows.Forms
{
	public class PropertySpec
	{
        private Attribute[] attributes;
        private string category;
		private string description;
        private object value;
        private string name;
        private object key;

        public PropertySpec(string name, object value) : this(name, value, null, null)
        {
        }

        public PropertySpec(string name, object value, string category) : this(name, value, category, null)
        {
        }

        public PropertySpec(string name, object value, string category, string description)
        {
            this.name = name; this.value = value;
            this.category = category; this.description = description;

            this.key = this.name;
        }

        public Attribute[] Attributes
        {
            get { return attributes; }
            set { attributes = value; }
        }

		public string Category
		{
			get { return category; }
			set { category = value; }
		}

		public string Description
		{
			get { return description; }
			set { description = value; }
		}

        public object Value
        {
            get { return this.value; }
            set { this.value = value; }
        }

		public string Name
		{
			get { return name; }
			set { name = value; }
		}

        public object Key
        {
            get { return key; }
            set { this.key = value; }
        }
	}

	public class PropertyBag : List<PropertySpec>, ICustomTypeDescriptor
	{
		#region PropertyDescriptor class definition

		private class PropertySpecDescriptor : PropertyDescriptor
		{
			private PropertySpec item;

            public PropertySpecDescriptor(PropertySpec item, Attribute[] attributes)
                : base(item.Name, attributes)
            {
                this.item = item;
            }

			public override Type ComponentType
			{
				get { return this.GetType(); }
			}

			public override bool IsReadOnly
			{
				get { return (Attributes.Matches(ReadOnlyAttribute.Yes)); }
			}

			public override Type PropertyType
			{
				get { return item.Value.GetType(); }
			}

			public override bool CanResetValue(object component)
			{
                return !this.GetValue(component).Equals(item.Value);
			}

			public override object GetValue(object component)
			{
                return item.Value;
			}

			public override void ResetValue(object component)
			{
			}

			public override void SetValue(object component, object value)
			{
                item.Value = value;
			}

			public override bool ShouldSerializeValue(object component)
			{
                return false;
			}

            public override string Category
            {
                get { return item.Category; }
            }
		}

		#endregion

		#region ICustomTypeDescriptor explicit interface definitions

        AttributeCollection ICustomTypeDescriptor.GetAttributes()
		{
			return TypeDescriptor.GetAttributes(this, true);
		}

		string ICustomTypeDescriptor.GetClassName()
		{
			return TypeDescriptor.GetClassName(this, true);
		}

		string ICustomTypeDescriptor.GetComponentName()
		{
			return TypeDescriptor.GetComponentName(this, true);
		}

		TypeConverter ICustomTypeDescriptor.GetConverter()
		{
			return TypeDescriptor.GetConverter(this, true);
		}

		EventDescriptor ICustomTypeDescriptor.GetDefaultEvent()
		{
			return TypeDescriptor.GetDefaultEvent(this, true);
		}

		PropertyDescriptor ICustomTypeDescriptor.GetDefaultProperty()
		{
            return TypeDescriptor.GetDefaultProperty(this, true);
		}

		object ICustomTypeDescriptor.GetEditor(Type editorBaseType)
		{
			return TypeDescriptor.GetEditor(this, editorBaseType, true);
		}

		EventDescriptorCollection ICustomTypeDescriptor.GetEvents()
		{
			return TypeDescriptor.GetEvents(this, true);
		}

		EventDescriptorCollection ICustomTypeDescriptor.GetEvents(Attribute[] attributes)
		{
			return TypeDescriptor.GetEvents(this, attributes, true);
		}

		PropertyDescriptorCollection ICustomTypeDescriptor.GetProperties()
		{
			return ((ICustomTypeDescriptor)this).GetProperties(new Attribute[0]);
		}

		PropertyDescriptorCollection ICustomTypeDescriptor.GetProperties(Attribute[] attributes)
		{
            PropertyDescriptor[] newProps = new PropertyDescriptor[Count];

            for (int i = 0; i < Count; i++)
            {
                ArrayList attrs = new ArrayList(attributes);
                attrs.Add(new DescriptionAttribute(this[i].Description));

                if (this[i].Attributes != null) attrs.AddRange(this[i].Attributes);

                Attribute[] attrArray = (Attribute[])attrs.ToArray(typeof(Attribute));
                newProps[i] = new PropertySpecDescriptor(this[i], attrArray);

            }
            return new PropertyDescriptorCollection(newProps);
		}

		object ICustomTypeDescriptor.GetPropertyOwner(PropertyDescriptor pd)
		{
			return this;
		}

		#endregion
	}
}
