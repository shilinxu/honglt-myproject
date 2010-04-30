using System;
using System.Text;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Diagnostics;

namespace Global
{
    unsafe class Buffer
    {
        public static Int16 SwapShort(Int16 x)
        {
            return (Int16)((x & 0xFF00) >> 8 + (x & 0x00FF) << 8);
        }

        public static Int32 SwapLong(Int32 x)
        {
            return SwapShort((Int16)(x << 16)) | SwapShort((Int16)(x >> 16));
        }

        public static Int64 SwapQWord(Int64 x)
        {
            return SwapLong((Int32)(x << 32)) | SwapLong((Int32)(x >> 32));
        }

        [DllImport("Kernel32.dll"/*, EntryPoint = "RtlCopyMemory"*/)]
//        public static extern void CopyMemory(IntPtr dest, IntPtr source, int size);
        public static extern void CopyMemory(void* dest, void* source, int size);

        [DllImport("Kernel32.dll"/*, EntryPoint = "RtlMoveMemory"*/)]
      //  public static extern void MoveMemory(IntPtr dest, IntPtr source, int size);
        public static extern void MoveMemory(void* dest, void* source, int size);

        ///////////////////////////////////////////////////////////////////////////////
        // Buffer construction

        ~Buffer()
        {
            if (BufferPtr != null) Marshal.FreeHGlobal(BufferPtr);
        }

        //////////////////////////////////////////////////////////////////////
        // Buffer attributes

        protected IntPtr BufferPtr;
        protected Int32 BufferLength, BufferSize;

        private const UInt32 BLOCK_SIZE = 2048;
        private const UInt32 BLOCK_MASK = 0xFFFFFC00;

        public int Length
        {
            get { return BufferLength; }
            private set { BufferLength = value; }
        }

        ///////////////////////////////////////////////////////////////////////////////
        // Buffer read

        public virtual int Peek(byte[] buffer, int size)
        {
            int length = Math.Min(this.Length, size);

            Marshal.Copy(BufferPtr, buffer, 0, length);

            return length;
        }

        public virtual int Read(byte[] buffer, int size)
        {
            int length = Peek(buffer, size);
            Remove(length);

            return length;
        }

        ///////////////////////////////////////////////////////////////////////////////
        // Buffer write

        public virtual int Write(byte[] buffer, int length)
        {
            return Write(buffer, length, 0);
        }

        public virtual int Write(byte[] buffer, int length, int offset)
        {
            EnsureBuffer(length);

            IntPtr source = (IntPtr)(BufferPtr.ToInt32() + offset);
            MoveMemory((IntPtr)(source.ToInt32() + length), source, Length - offset);

            Marshal.Copy(buffer, 0, (IntPtr)(BufferPtr.ToInt32() + offset), length);

            this.Length += length;
            return length;
        }

        protected void MoveMemory(IntPtr dest, IntPtr source, int length)
        {
            byte[] temp = new byte[length];

            Marshal.Copy(source, temp, 0, length);
            Marshal.Copy(temp, 0, dest, length);
        }

        ///////////////////////////////////////////////////////////////////////////////
        // Buffer remove

        public void Remove(int length)
        {
            length = Math.Min(BufferLength, length);

            this.Length -= length;
            MoveMemory(BufferPtr, (IntPtr)(BufferPtr.ToInt32() + length), Length);
        }

        public void Clear()
        {
            this.BufferLength = 0;
        }

        ///////////////////////////////////////////////////////////////////////////////
        // Buffer add utilities

        public void Print(string text)
        {
            Write(Encoding.Default.GetBytes(text), Encoding.Default.GetByteCount(text));
        }

        public int AddBuffer(Buffer buffer, int length, bool peek)
        {
            length = Math.Min(buffer.BufferLength, length);
  //          Add((void*)buffer.BufferPtr, length);

            if (peek) buffer.Remove(length);
            return length;
        }

        public void AddReversed(IntPtr data, int length)
        {
            EnsureBuffer(length);

            ReverseBuffer(data, (IntPtr)(BufferPtr.ToInt32() + BufferLength), length);
            this.BufferLength += length;

            DoPrint();
        }

        protected void EnsureBuffer(int length)
        {
            if (this.BufferSize - this.BufferLength >= length) return;

            this.BufferSize = this.BufferLength + length;
            this.BufferSize = (Int32)((this.BufferSize + BLOCK_SIZE - 1) & BLOCK_MASK);

            if (this.BufferPtr == IntPtr.Zero) this.BufferPtr = Marshal.AllocHGlobal(this.BufferSize);
            if (this.BufferSize > 0x80000 && this.BufferLength + length < 0x40000) this.BufferSize = 0x40000;

            this.BufferPtr = Marshal.ReAllocHGlobal(BufferPtr, (IntPtr)BufferSize);
        }

        //////////////////////////////////////////////////////////////////////
        // Buffer reverse buffer

        public static void ReverseBuffer(IntPtr input, IntPtr output, int length)
        {
            int offset = 0;
            for (; length >= 4; length -= 4, offset += 4)
            {
                int temp = Marshal.ReadInt32(input, length - 4);
                Marshal.WriteInt32(output, offset, SwapLong(temp));
            }

            for (; length > 0; length--, offset++)
            {
                byte temp = Marshal.ReadByte(input, length - 1);
                Marshal.WriteByte(output, offset, temp);
            }
        }

        //////////////////////////////////////////////////////////////////////
        // Buffer string conversion

        #region string conversion

        unsafe public virtual string ToHex()
        {
            return ToHex((Byte*)BufferPtr, BufferLength);
        }

        unsafe public static string ToHex(byte* buffer, int length)
        {
            string dump = "";
            const string hex = ("0123456789ABCDEF");

            for (int i = 0; i < length; i++)
            {
                if (dump.Length > 0) dump += ' ';

                dump += hex[buffer[i] >> 4]; dump += hex[buffer[i] & 0x0f];
            }

            return dump;
        }

        public static string ToHex(byte[] buffer, int length)
        {
            unsafe
            {
                fixed (byte* p = buffer)
                {
                    return ToHex(p, length);
                }
            }
        }

        unsafe public virtual string ToASCII()
        {
            return ToASCII((byte*)BufferPtr, BufferLength);
        }

        unsafe public static string ToASCII(byte* buffer, int length)
        {
            char[] chars = new char[length];
            fixed (char* p = chars) length = Encoding.Default.GetChars(buffer, length, p, length);

            return Encoding.Default.GetString(Encoding.Default.GetBytes(chars, 0, length));
        }

        public static string ToASCII(byte[] buffer, int length)
        {
            unsafe
            {
                fixed (byte* p = buffer)
                {
                    return ToASCII(p, length);
                }
            }
        }

        #endregion

        protected void DoPrint()
        {
#if DEBUG
            Trace.WriteLine(ToHex());
            //Debug.WriteLine(ToASCII());
#endif
        }
    }
}
