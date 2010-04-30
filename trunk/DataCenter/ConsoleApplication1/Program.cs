using System;
using System.Collections.Generic;
using System.Text;
namespace Microshaoft
{
    using System;
    using System.Collections.Generic;
    using System.Runtime.InteropServices;
    class Program
    {
        static int[] bufsizes = 
                            {
                                256,
                                320 * 240 * 3,
                                1024 * 768 * 3
                            };
        static int bufsize;
        const long duration = 10 * 1000 * 1000 * 10;    // 10 seconds
        static void Main(string[] args)
        {
            //foreach (var b in bufsizes)
            {
               // bufsize = b;
               // Console.WriteLine("allocs of size " + bufsize);
                newbyte();
                marshallallochglobal();
                marshallAllocCoTaskMem();
                stackallocate();
                sharedmem();
                Console.WriteLine("memcopies of size " + bufsize);
                arraycopy();
                BufferBlockCopy();
                marshalcopy();
                kernelcopy();
                OwnMemCopyInt();
                OwnMemCopyLong();
                Console.WriteLine();
            }
            Console.WriteLine("done");
            Console.ReadLine();
        }
        private static void BufferBlockCopy()
        {
            byte[] buf1 = new byte[bufsize];
            byte[] buf2 = new byte[bufsize];
            long start = DateTime.UtcNow.Ticks;
            int i = 0;
            while ((start + duration) > DateTime.UtcNow.Ticks)
            {
                Buffer.BlockCopy(buf1, 0, buf2, 0, bufsize);
                i++;
            }
            Console.WriteLine("Buffer.BlockCopy: " + i);
        }
        unsafe private static void marshalcopy()
        {
            byte[] buf1 = new byte[bufsize];
            byte[] buf2 = new byte[bufsize];
            long start = DateTime.UtcNow.Ticks;
            int i = 0;
            fixed (byte* b1 = &buf1[0])
            fixed (byte* b2 = &buf2[0])
            {
                IntPtr ab2 = new IntPtr(b2);
                while ((start + duration) > DateTime.UtcNow.Ticks)
                {
                    Marshal.Copy(buf1, 0, ab2, buf1.Length);
                    i++;
                }
            }
            Console.WriteLine("Marshal.Copy: " + i);
        }
        [DllImport("Kernel32.dll", EntryPoint = "RtlMoveMemory")]
        public unsafe static extern void CopyMemory(byte* Destination, byte* Source, [MarshalAs(UnmanagedType.U4)] uint Length);
        unsafe private static void kernelcopy()
        {
            Console.Write("Kernel32NativeMethods.CopyMemory: ");
            byte[] buf1 = new byte[bufsize];
            byte[] buf2 = new byte[bufsize];
            long start = DateTime.UtcNow.Ticks;
            int i = 0;
            fixed (byte* b1 = &buf1[0])
            fixed (byte* b2 = &buf2[0])
            {
                while ((start + duration) > DateTime.UtcNow.Ticks)
                {
                    CopyMemory(b2, b1, (uint)buf1.Length);
                    i++;
                }
            }
            Console.WriteLine(i);
        }
        unsafe private static void OwnMemCopyInt()
        {
            Console.Write("OwnMemCopyInt: ");
            byte[] buf1 = new byte[bufsize];
            byte[] buf2 = new byte[bufsize];
            long start = DateTime.UtcNow.Ticks;
            int i = 0;
            fixed (byte* b1 = &buf1[0])
            fixed (byte* b2 = &buf2[0])
            {
                IntPtr ab1 = new IntPtr(b1);
                IntPtr ab2 = new IntPtr(b2);
                while ((start + duration) > DateTime.UtcNow.Ticks)
                {
                    MemCopyInt(ab1, ab2, buf1.Length);
                    i++;
                }
            }
            Console.WriteLine(i);
        }
        unsafe private static void OwnMemCopyLong()
        {
            Console.Write("OwnMemCopyLong: ");
            byte[] buf1 = new byte[bufsize];
            byte[] buf2 = new byte[bufsize];
            long start = DateTime.UtcNow.Ticks;
            int i = 0;
            fixed (byte* b1 = &buf1[0])
            fixed (byte* b2 = &buf2[0])
            {
                IntPtr ab1 = new IntPtr(b1);
                IntPtr ab2 = new IntPtr(b2);
                while ((start + duration) > DateTime.UtcNow.Ticks)
                {
                    MemCopyLong(ab1, ab2, buf1.Length);
                    i++;
                }
            }
            Console.WriteLine(i);
        }
        private static void arraycopy()
        {
            Console.Write("Array.Copy: ");
            byte[] buf1 = new byte[bufsize];
            byte[] buf2 = new byte[bufsize];
            long start = DateTime.UtcNow.Ticks;
            int i = 0;
            while ((start + duration) > DateTime.UtcNow.Ticks)
            {
                Array.Copy(buf1, buf2, buf1.Length);
                i++;
            }
            Console.WriteLine(i);
        }
        private static void newbyte()
        {
            Console.Write("new byte[]: ");
            long start = DateTime.UtcNow.Ticks;
            int i = 0;
            while ((start + duration) > DateTime.UtcNow.Ticks)
            {
                byte[] buf = new byte[bufsize];
                i++;
            }
            //GC.Collect();
            Console.WriteLine(i);
        }
        private static void marshallallochglobal()
        {
            Console.Write("Marshal.AllocHGlobal: ");
            long start = DateTime.UtcNow.Ticks;
            int i = 0;
            while ((start + duration) > DateTime.UtcNow.Ticks)
            {
                IntPtr p = Marshal.AllocHGlobal(bufsize);
                Marshal.FreeHGlobal(p);
                i++;
            }
            Console.WriteLine(i);
        }
        private static void marshallAllocCoTaskMem()
        {
            Console.Write("Marshal.AllocCoTaskMem: ");
            long start = DateTime.UtcNow.Ticks;
            int i = 0;
            while ((start + duration) > DateTime.UtcNow.Ticks)
            {
                IntPtr p = Marshal.AllocCoTaskMem(bufsize);
                Marshal.FreeCoTaskMem(p);
                i++;
            }
            Console.WriteLine(i);
        }
        private static void sharedmem()
        {
            try
            {
                Console.Write("SharedMemory: ");
                long start = DateTime.UtcNow.Ticks;
                int i = 0;
                while ((start + duration) > DateTime.UtcNow.Ticks)
                {
                    using (SharedMemory m = new SharedMemory("abc", bufsize, true))
                    {
                        i++;
                    }
                }
                Console.WriteLine(i);
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
            }
        }
        private static void stackallocate()
        {
            Console.Write("stackalloc: ");
            long start = DateTime.UtcNow.Ticks;
            int i = 0;
            while ((start + duration) > DateTime.UtcNow.Ticks)
            {
                stack();
                i++;
            }
            Console.WriteLine(i);
        }
        unsafe static void stack()
        {
            byte* buf = stackalloc byte[bufsize];
        }
        static public unsafe void MemCopyInt(IntPtr pSource, IntPtr pDest, int Len)
        {
            unchecked
            {
                int count = Len / Marshal.SizeOf(typeof(int));
                int rest = Len % count;
                int* ps = (int*)pSource.ToPointer(), pd = (int*)pDest.ToPointer();
                // Loop over the cnt in blocks of 4 bytes, copying an integer (4 bytes) at a time:
                for (int n = 0; n < count; n++)
                {
                    *pd++ = *ps++;
                }
                // Complete the copy by moving any bytes that weren't moved in blocks of 4:
                if (rest > 0)
                {
                    byte* ps1 = (byte*)ps;
                    byte* pd1 = (byte*)pd;
                    for (int n = 0; n < rest; n++)
                    {
                        *pd1++ = *ps1++;
                    }
                }
            }
        }
        static public unsafe void MemCopyLong(IntPtr pSource, IntPtr pDest, int Len)
        {
            unchecked
            {
                int count = Len / Marshal.SizeOf(typeof(long));
                int rest = Len % count;
                long* ps = (long*)pSource.ToPointer(), pd = (long*)pDest.ToPointer();
                // Loop over the cnt in blocks of n bytes, copying an long (n bytes) at a time:
                for (int n = 0; n < count; n++)
                {
                    *pd++ = *ps++;
                }
                // Complete the copy by moving any bytes that weren't moved in blocks of n:
                if (rest > 0)
                {
                    byte* ps1 = (byte*)ps;
                    byte* pd1 = (byte*)pd;
                    for (int n = 0; n < rest; n++)
                    {
                        *pd1 = *ps1;
                        pd1++;
                        ps1++;
                    }
                }
            }
        }
    }
    unsafe public class SharedMemory : IDisposable
    {
        [DllImport("kernel32.dll", SetLastError = true)]
        static extern IntPtr CreateFileMapping(IntPtr hFile, IntPtr lpFileMappingAttributes, PageProtection flProtect, uint dwMaximumSizeHigh, uint dwMaximumSizeLow, string lpName);
        [DllImport("kernel32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        static extern bool CloseHandle(IntPtr hObject);
        [DllImport("kernel32.dll", SetLastError = true)]
        static extern bool UnmapViewOfFile(IntPtr lpBaseAddress);
        [DllImport("kernel32.dll", SetLastError = true)]
        static extern IntPtr MapViewOfFile(IntPtr hFileMappingObject, uint dwDesiredAccess, uint dwFileOffsetHigh, uint dwFileOffsetLow, IntPtr dwNumberOfBytesToMap);
        [DllImport("kernel32.dll", SetLastError = true)]
        static extern IntPtr OpenFileMapping(uint dwDesiredAccess, bool bInheritHandle, string lpName);
        const UInt32 STANDARD_RIGHTS_REQUIRED = 0x000F0000;
        const UInt32 SECTION_QUERY = 0x0001;
        const UInt32 SECTION_MAP_WRITE = 0x0002;
        const UInt32 SECTION_MAP_READ = 0x0004;
        const UInt32 SECTION_MAP_EXECUTE = 0x0008;
        const UInt32 SECTION_EXTEND_SIZE = 0x0010;
        const UInt32 SECTION_ALL_ACCESS = (STANDARD_RIGHTS_REQUIRED | SECTION_QUERY | SECTION_MAP_WRITE | SECTION_MAP_READ | SECTION_MAP_EXECUTE | SECTION_EXTEND_SIZE);
        const UInt32 FILE_MAP_ALL_ACCESS = SECTION_ALL_ACCESS;
        [Flags]
        enum PageProtection : uint
        {
            NoAccess = 0x01,
            Readonly = 0x02,
            ReadWrite = 0x04,
            WriteCopy = 0x08,
            Execute = 0x10,
            ExecuteRead = 0x20,
            ExecuteReadWrite = 0x40,
            ExecuteWriteCopy = 0x80,
            Guard = 0x100,
            NoCache = 0x200,
            WriteCombine = 0x400,
        }
        private IntPtr hHandle;
/*        public void* Buffer
        {
            get;
            private set;
        }
 * */
//        public int Length { get; private set; }
        public SharedMemory(string name, int size, bool create)
        {
            if (create)
            {
                Create(name, size);
            }
            else
            {
                Attach(name);
            }
        }
        ~SharedMemory()
        {
            Detach();
        }
        private void Create(string SharedMemoryName, int NumBytes)
        {
            hHandle = CreateFileMapping(new IntPtr(-1), IntPtr.Zero, PageProtection.ReadWrite, 0, (uint)NumBytes, SharedMemoryName);
            if (IntPtr.Zero == hHandle)
            {
                throw new InvalidOperationException(String.Format("Failed to create FileMapping with given name '{0}'.", SharedMemoryName));
            }
            //Buffer = MapViewOfFile(hHandle, FILE_MAP_ALL_ACCESS, 0, 0, IntPtr.Zero).ToPointer();
//            Length = NumBytes;
        }
        private void Attach(string SharedMemoryName)
        {
            hHandle = OpenFileMapping(FILE_MAP_ALL_ACCESS, false, SharedMemoryName);
            if (IntPtr.Zero == hHandle)
            {
                throw new InvalidOperationException(String.Format("Failed to open FileMapping with given name '{0}'.", SharedMemoryName));
            }
            //Buffer = MapViewOfFile(hHandle, FILE_MAP_ALL_ACCESS, 0, 0, IntPtr.Zero).ToPointer();
        }
        private void Detach()
        {
            if (IntPtr.Zero != hHandle)
            {
//                UnmapViewOfFile(new IntPtr(Buffer));
                CloseHandle(hHandle); //fair to leak if can't close
                hHandle = IntPtr.Zero;
            }
//            Buffer = IntPtr.Zero.ToPointer();
        }
        #region IDisposable Members
        public void Dispose()
        {
            Detach();
        }
        #endregion
    }
}

