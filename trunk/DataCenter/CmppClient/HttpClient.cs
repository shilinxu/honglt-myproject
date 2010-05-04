using System;
using System.Net;
using System.Net.Sockets;
using System.Runtime.InteropServices;

namespace DataCenter.Network
{
    class HttpClient : Transfer
    {
        public HttpClient(ref Socket socket) : base(ref socket)
        {
            this.OnRun();
        }
    }
}
