using System;
using System.Net.Sockets;

using Global.Network;

namespace DataCenter.Network
{
    class Transfer : Connection
    {
        //////////////////////////////////////////////////////////////////////
        // Transfer construction

        public Transfer(ref Socket socket) : base(ref socket)
        {
        }

        //////////////////////////////////////////////////////////////////////
        // Transfer attributes

        public event EventHandler Connected;
//        public event EventHandler Disconnected;
 
        //////////////////////////////////////////////////////////////////////
        // Transfer socket event handlers

        protected virtual void OnHandshake()
        {
        }
   }
}
