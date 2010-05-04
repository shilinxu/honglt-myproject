using System;
using System.IO;

namespace DataCenter.Log
{
    class LogService : IDisposable
    {
        //////////////////////////////////////////////////////////////////////
        // LogService construction

        public LogService(string file)
        {
            this.FileName = file;
        }

        void IDisposable.Dispose()
        {
            if (this.Writer != null) Writer.Dispose();
        }

        //////////////////////////////////////////////////////////////////////
        // LogService attributes

        private StreamWriter Writer;
        private readonly string FileName;

        public static readonly LogService Default = new LogService("CmppClient.log");

        /////////////////////////////////////////////////////////////////////////////
        // LogService operations

        public void Message(MsgType type, string format, params object[] arg)
        {
            string text = string.Format(format, arg);

            this.Writer = new StreamWriter(FileName, true);
            this.Writer.WriteLine(DateTime.Now.ToString("[HH:mm:ss]") + ' ' + text);
            this.Writer.Close();

            if (Log != null) Log(this, new LogEventArgs(ref text, type));
        }

        public event LogEventHandler Log;

        //////////////////////////////////////////////////////////////////////
        // Connection write event handler

        public delegate void LogEventHandler(object sender, LogEventArgs e);

        public class LogEventArgs : EventArgs
        {
            public LogEventArgs(ref string text, MsgType type)
            {
                this.Text = text; this.MsgType = type;
            }

            public readonly MsgType MsgType;
            public readonly string Text;
        }
    }

    enum MsgType
    {
        Default = 0,
        System = 1,
        Error = 2,
        Debug = 3,
        Temp = 4,
    }
}
