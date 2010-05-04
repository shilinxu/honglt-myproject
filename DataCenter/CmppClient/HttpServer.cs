using System;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using System.Collections.Generic;

using System.Diagnostics;
using DataCenter.Properties;
using Global.Network;
using DataCenter.Log;

namespace DataCenter.Network
{
    class HttpServer : Connection
    {
        protected const int PortDefault = 8080;

        //////////////////////////////////////////////////////////////////////
        // HttpServer construction

        public HttpServer() : this(PortDefault)
        {
            LogService.Default.Message(MsgType.Debug, "{0}", DateTime.Now);
            LogService.Default.Message(MsgType.Debug, "{0}", DateTime.Now);
        }

        public HttpServer(int port) : this(IPAddress.Any, port)
        {
        }

        public HttpServer(IPAddress address, int port)
        {
            Connect(address, port);
        }

        //////////////////////////////////////////////////////////////////////
        // HttpServer attributes

        private IAsyncResult AcceptIoOver;
        private List<HttpClient> Clients = new List<HttpClient>();

        //////////////////////////////////////////////////////////////////////
        // HttpServer connect

        public override SocketError Connect(IPAddress address, int port)
        {
            SocketError error = base.Connect(address, port);
            if (error != SocketError.Success) return error;

            AcceptNeighbour(); //StartThread();

            return SocketError.Success;
        }

        //////////////////////////////////////////////////////////////////////
        // HttpServer disconnect

        public override void Disconnect()
        {
            StopThread();

            foreach (HttpClient client in this.Clients)
            {
                client.Disconnect();
            }

            this.Clients.Clear();

            lock (this.SyncRoot) base.Disconnect();

            if (this.AcceptIoOver != null) AcceptIoOver.AsyncWaitHandle.WaitOne();
        }

        //////////////////////////////////////////////////////////////////////
        // HttpServer thread run

        protected Thread Thread = null;
        protected volatile bool Threading = false;

        protected object SyncRoot = new object();
        public AutoResetEvent WaitEvent = new AutoResetEvent(false);
        private ManualResetEvent ExitEvent = new ManualResetEvent(true);

        private void StartThread()
        {
            this.Thread = new Thread(new ThreadStart(this.ThreadStart));
            this.Threading = true; this.Thread.Start();
        }

        private void StopThread()
        {
            this.Threading = false;
            this.WaitEvent.Set();

            this.ExitEvent.WaitOne();
        }

        protected void ThreadStart()
        {
            this.ExitEvent.Reset();

            while (this.Threading)
            {
                lock (this.SyncRoot)
                {
                    RunNeighbours();
                }

                this.WaitEvent.WaitOne(50, false);
            }

            this.ExitEvent.Set();
        }

        //////////////////////////////////////////////////////////////////////
        // HttpServer accept a Neighbour

        protected SocketError AcceptNeighbour()
        {
            this.AcceptIoOver = Socket.BeginAccept(
                new AsyncCallback(OnAcceptCompleted), null);

            return SocketError.Success;
        }

        protected SocketError AcceptNeighbour(IAsyncResult ar)
        {
            if (this.Socket == null) return SocketError.NotSocket;

            Socket socket = this.Socket.EndAccept(ar);

            using (Profiles profile = Profiles.FromFile("Profiles.xml"))
            {
                IPEndPoint ep = (IPEndPoint)socket.RemoteEndPoint;
                if (profile.Blacklist == null || !profile.Blacklist.IsDenied(ep.Address))
                {
                    this.Clients.Add(new HttpClient(ref socket));
                }
                else
                {
                    socket.Close();
                }
            }

            return AcceptNeighbour();
        }

        private void OnAcceptCompleted(IAsyncResult ar)
        {
            lock (this.SyncRoot)
            {
                if (AcceptNeighbour(ar) != SocketError.Success) this.ExitEvent.Set();
            }
            Trace.WriteLine("OnAcceptCompleted ok.");
        }

        protected void RunNeighbours()
        {
            foreach (HttpClient client in this.Clients)
            {
                client.OnRun();
            }
        }
    }
}
