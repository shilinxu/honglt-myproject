using System;
using System.Net;
using System.Net.Sockets;

namespace Global.Network
{
    using System.Diagnostics;
    using System.Threading;

    class Connection : IDisposable
    {
        //////////////////////////////////////////////////////////////////////
        // Connection construction

        protected Connection()
        {
        }

        protected Connection(ref Socket socket) : this()
        {
            AcceptFrom(socket);
        }

        ~Connection()
        {
            ((IDisposable)this).Dispose();
        }

        void IDisposable.Dispose()
        {
            this.Disconnect();
            GC.SuppressFinalize(this);
        }

        //////////////////////////////////////////////////////////////////////
        // Connection attributes

        protected Socket Socket;

        public virtual bool IsConnected
        {
            get { return Socket != null && Socket.Connected; }
        }

        private IAsyncResult ReadIoOver;
        private IAsyncResult WriteIoOver;

        protected bool HasOverlappedIoCompleted(IAsyncResult ar)
        {
            return (ar != null) ? ar.IsCompleted : true;
        }

        //////////////////////////////////////////////////////////////////////
        // Connection connect

        public virtual SocketError Connect(IPAddress address, int port)
        {
            try
            {
                this.Socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

                this.Socket.Bind(new IPEndPoint(address, port));
                this.Socket.Listen(256);

                return SocketError.Success;
            }
            catch (SocketException exception)
            {
                return (SocketError)exception.ErrorCode;
            }
        }

        //////////////////////////////////////////////////////////////////////
        // Connection connect to

        public virtual SocketError ConnectTo(IPAddress address, int port)
        {
            try
            {
                this.Socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

                this.Socket.Connect(address, port);

                this.Input = new Global.Buffer();
                this.Output = new Global.Buffer();

                return SocketError.Success;
            }
            catch (SocketException exception)
            {
                return (SocketError)exception.ErrorCode;
            }
            finally
            {
                this.Input = this.Output = null;
            }
        }

        protected Global.Buffer Input;
        protected Global.Buffer Output;

        //////////////////////////////////////////////////////////////////////
        // Connection close

        public virtual void Disconnect()
        {
            lock (this)
            {
                if (this.Socket != null) this.Socket.Close();
                this.Socket = null;
            }

            if (ReadIoOver != null) ReadIoOver.AsyncWaitHandle.WaitOne();
            if (WriteIoOver != null) WriteIoOver.AsyncWaitHandle.WaitOne();
 
            this.Input = this.Output = null;
        }

        //////////////////////////////////////////////////////////////////////
        // Connection accept an incoming connection

        protected virtual void AcceptFrom(Socket socket)
        {
            this.Input = new Global.Buffer();
            this.Output = new Global.Buffer();
            this.Socket = socket;

            OnConnected();
        }

        //////////////////////////////////////////////////////////////////////
        // Connection run function

        public virtual bool OnRun()
        {
            if (!IsConnected) return true;

            if (HasOverlappedIoCompleted(ReadIoOver))
            {
                if (OnRead() != SocketError.Success) return false;
            }

            if (HasOverlappedIoCompleted(WriteIoOver))
            {
                if (OnWrite() != SocketError.Success) return false;
            }

            return true;
        }
        
        //////////////////////////////////////////////////////////////////////
        // Connection socket event handlers

        public static readonly int MaxBuffer = 1024;
        private byte[] ReadBuffer, WriteBuffer;

        protected virtual bool OnConnected()
        {
            this.ReadBuffer = new byte[MaxBuffer];
            this.WriteBuffer = new byte[MaxBuffer];

            this.Received += new DataEventHandler(OnReceived);
            this.Written += new DataEventHandler(OnWritten);

            this.Disconnected += new ErrorEventHandler(OnDropped);

            return true;
        }

        protected virtual void OnDropped(int error)
        {
            if (this.Socket != null) this.Disconnect();

            this.ReadBuffer = this.WriteBuffer = null;

            Debug.Assert(this.Disconnected != null);
            Disconnected(this, new ErrorEventArgs(error));

            this.Received = this.Written = null;
            this.Disconnected = null;
        }

        public event ErrorEventHandler Disconnected;

        protected virtual void OnDropped(object sender, EventArgs e)
        {
        }

        //////////////////////////////////////////////////////////////////////
        // Connection read event handler

        protected virtual SocketError OnRead()
        {
            try
            {
                if (this.Socket == null) return SocketError.NotSocket;

                if (!HasOverlappedIoCompleted(ReadIoOver)) return SocketError.Success;

                this.ReadIoOver = Socket.BeginReceive(
                    ReadBuffer, 0, MaxBuffer, SocketFlags.None,
                    new AsyncCallback(OnReadCompleted), null);

                return SocketError.Success;
            }
            catch (SocketException exception)
            {
                return (SocketError)exception.ErrorCode;
            }
        }

        protected virtual void OnReadCompleted(IAsyncResult ar)
        {
            lock (this)
            {
                if (this.Socket == null) return;

                int length = Socket.EndReceive(ar);
                length = Math.Min((length & 0xFFFFF), MaxBuffer);

                if (length > 0 && length <= MaxBuffer)
                {
                    this.Input.Write(ReadBuffer, length);
                    DoPrint(ReadBuffer, length, "RX:");

                    this.Received(this, new DataEventArgs(ref ReadBuffer, length));
                }

                SocketError error = (length > 0) ? OnRead() : SocketError.NotConnected;
                if (error != SocketError.Success) OnDropped(0);
            }
        }

        public event DataEventHandler Received;

        protected virtual void OnReceived(object sender, EventArgs e)
        {
        }

        //////////////////////////////////////////////////////////////////////
        // Connection write event handler

        protected virtual SocketError OnWrite()
        {
            try
            {
                if (this.Socket == null) return SocketError.NotSocket;

                if (this.Output.Length == 0) return SocketError.Success;
                if (!HasOverlappedIoCompleted(WriteIoOver)) return SocketError.Success;

                int length = this.Output.Read(WriteBuffer, MaxBuffer);

                this.WriteIoOver = Socket.BeginSend(
                    WriteBuffer, 0, length, SocketFlags.None,
                    new AsyncCallback(OnWriteCompleted), null);

                return SocketError.Success;
            }
            catch (SocketException exception)
            {
                return (SocketError)exception.ErrorCode;
            }
        }

        protected virtual void OnWriteCompleted(IAsyncResult ar)
        {
            lock (this)
            {
                if (this.Socket == null) return;

                int length = Socket.EndSend(ar);
                SocketError error = OnWrite();

                if ( error != SocketError.Success) OnDropped((int)error);
                else this.Written(this, new DataEventArgs(ref WriteBuffer, length));
            }
        }

        public event DataEventHandler Written;

        protected virtual void OnWritten(object sender, EventArgs e)
        {
        }

        //////////////////////////////////////////////////////////////////////
        // Connection helper

        protected virtual void DoPrint(byte[] buffer, int length, string prefix)
        {
#if DEBUG
            System.Diagnostics.Trace.WriteLine(prefix + Buffer.ToHex(buffer, length));
#endif
        }
    }

    public delegate void ErrorEventHandler(object sender, ErrorEventArgs e);

    class ErrorEventArgs : EventArgs
    {
        public ErrorEventArgs(int code)
        {
            this.Code = code;
        }

        public readonly int Code;
    }

    public delegate void DataEventHandler(object sender, DataEventArgs e);

    class DataEventArgs : EventArgs
    {
        public DataEventArgs(ref byte[] data) : this(ref data, data.Length)
        {
        }

        public DataEventArgs(ref byte[] data, int length)
        {
            this.Buffer = data;
            this.Length = length;
        }

        public readonly int Length;
        public readonly byte[] Buffer;
    }
}
